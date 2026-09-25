#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2026 FoBE Studio
# SPDX-License-Identifier: Apache-2.0
"""Check font records and notices and optionally export an inventory offline."""

import argparse
import ast
import csv
import hashlib
import io
import json
from pathlib import Path
import re


HERE = Path(__file__).resolve().parent
ROOT = HERE.parent
SOURCE = ROOT / "src"
FONTS = ROOT / "fonts"
ARRAY = re.compile(
    r"const\s+uint8_t\s+(u8g2_font_\w+)\s*\[(\d+)\][^=]*=\s*"
    r'((?:"(?:\\.|[^"\\])*"\s*)+);',
    re.DOTALL,
)
STRING = re.compile(r'"(?:\\.|[^"\\])*"')
# Excluded from the distributable SDK, including public declarations and options.
EXCLUDED_PREFIXES = ("u8g2_font_wqy", "u8g2_font_unifont")


def load_records():
    catalog = json.loads((FONTS / "catalog.json").read_text())
    manifest = json.loads((FONTS / "sources.json").read_text())
    expected = [entry["id"] for entry in manifest["notices"]]
    if any(not re.fullmatch(r"[a-zA-Z0-9_-]+", name) for name in expected):
        raise ValueError("Invalid notice identifier")
    actual = [path.stem for path in (FONTS / "notices").glob("*.txt")]
    if len(expected) != len(set(expected)) or sorted(actual) != sorted(expected):
        raise ValueError("Notice files differ from the source manifest")
    for entry in manifest["notices"]:
        data = (FONTS / "notices" / (entry["id"] + ".txt")).read_bytes()
        if hashlib.sha256(data).hexdigest() != entry["sha256"]:
            raise ValueError(f"Upstream notice changed: {entry['id']}")
    for group in catalog["groups"].values():
        for field in ("notice", "baseline_notice"):
            if field in group and group[field] not in expected:
                raise ValueError(f"Missing group notice: {group[field]}")
    return catalog, manifest


def excluded(symbol):
    return symbol.lower().startswith(EXCLUDED_PREFIXES)


def semantic_sha256(fonts):
    encoded = json.dumps(fonts, sort_keys=True, separators=(",", ":")).encode()
    return hashlib.sha256(encoded).hexdigest()


def reference_fonts(path, manifest):
    """Verify the immutable import input before applying the recorded exclusion."""
    upstream = path.read_bytes()
    reference = manifest["font_reference"]
    if hashlib.sha256(upstream).hexdigest() != reference["source_sha256"]:
        raise ValueError("Reference source does not match the recorded upstream file hash")
    fonts = parse_fonts(upstream.decode("utf-8", errors="replace"))
    if (len(fonts) != reference["font_count"] or
            semantic_sha256(fonts) != reference["semantic_sha256"]):
        raise ValueError("Reference arrays differ from the recorded upstream baseline")
    return {symbol: details for symbol, details in fonts.items() if not excluded(symbol)}


def parse_fonts(text):
    """Read data separately from the preceding per-font attribution comment."""
    fonts = {}
    for match in ARRAY.finditer(text):
        symbol, size, literals = match.groups()
        start = text.rfind("/*", 0, match.start())
        end = text.find("*/", start)
        header = text[start:end]
        fields = {}
        for line in header.splitlines():
            key, separator, value = line.strip().partition(":")
            if separator:
                fields[key] = value.strip()
        if "Fontname" not in fields or "Copyright" not in fields:
            raise ValueError(f"Missing attribution header: {symbol}")
        if symbol in fonts:
            raise ValueError(f"Duplicate definition: {symbol}")
        data = b"".join(ast.literal_eval("b" + item) for item in STRING.findall(literals))
        # Each C string initializer has a trailing NUL included in its array size.
        if len(data) + 1 != int(size):
            raise ValueError(f"Unexpected array size: {symbol}")
        fonts[symbol] = {
            "fontname": fields["Fontname"],
            "copyright": fields["Copyright"],
            "sha256": hashlib.sha256(data + b"\0").hexdigest(),
        }
    # Separate inventories catch omissions in the full initializer parser.
    declarations = re.findall(r"const\s+uint8_t\s+(u8g2_font_\w+)\s*\[", text)
    headers = re.findall(r"^  Fontname:", text, re.MULTILINE)
    if len(declarations) != len(fonts) or set(declarations) != fonts.keys():
        raise ValueError("Font initializer parser does not cover all declarations")
    if len(headers) != len(fonts):
        raise ValueError("Font headers and array definitions do not have equal coverage")
    return fonts


def make_inventory(fonts, catalog):
    output = io.StringIO(newline="")
    columns = ["symbol", "family", "group", "fontname", "copyright", "license", "status", "sha256"]
    writer = csv.DictWriter(output, columns, delimiter="\t", lineterminator="\n")
    writer.writeheader()
    families = catalog["families"]
    for symbol, details in sorted(fonts.items()):
        candidates = [name for name in families if symbol.startswith(f"u8g2_font_{name}_")]
        if not candidates:
            raise ValueError(f"Unmapped font family: {symbol}")
        family = max(candidates, key=len)
        entry = families[family]
        group = catalog["groups"][entry["group"]]
        writer.writerow({
            "symbol": symbol,
            "family": family,
            "group": entry["group"],
            **details,
            "license": entry.get("license", group["license"]),
            "status": entry.get("status", group["status"]),
        })
    return output.getvalue()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    mode = parser.add_mutually_exclusive_group()
    mode.add_argument("--check", action="store_true", help="validate without writing files (default)")
    mode.add_argument("--output", type=Path, help="export the validated per-symbol inventory as TSV")
    parser.add_argument("--reference-fonts", type=Path,
                        help="also compare a downloaded copy of the recorded upstream font source")
    args = parser.parse_args()
    fonts = parse_fonts((SOURCE / "u8g2_fonts.c").read_text())
    if any(excluded(symbol) for symbol in fonts):
        raise ValueError("Excluded WenQuanYi or Unifont font data was reintroduced")
    catalog, manifest = load_records()
    configs = set(re.findall(
        r"^config U8G2_FONT_(U8G2_FONT_\w+)$",
        (ROOT / "Kconfig.fonts").read_text(), re.MULTILINE,
    ))
    if configs != {symbol.upper() for symbol in fonts}:
        raise ValueError("Kconfig font inventory differs from C definitions")
    header = (ROOT / "include/display/u8g2.h").read_text()
    declared = set(re.findall(r"extern\s+const\s+uint8_t\s+(u8g2_font_\w+)\s*\[", header))
    if not fonts.keys() <= declared:
        raise ValueError("Some C font definitions are absent from the public header")
    if any(excluded(symbol) for symbol in declared) or re.search(
            r"\bu8g(?:2)?_font_(?:wqy|unifont)\w*", header):
        raise ValueError("Excluded WenQuanYi or Unifont public declaration was reintroduced")
    reference = manifest["font_reference"]
    selection = manifest["font_selection"]
    if tuple(selection["excluded_symbol_prefixes"]) != EXCLUDED_PREFIXES:
        raise ValueError("Recorded font exclusions differ from the import policy")
    if (len(fonts) != selection["font_count"] or
            semantic_sha256(fonts) != selection["semantic_sha256"]):
        raise ValueError("Font data or attribution differs from the recorded retained selection")
    if args.reference_fonts:
        if reference_fonts(args.reference_fonts, manifest) != fonts:
            raise ValueError("Reference font arrays or attribution differ from local source")
        print(f"All {len(fonts)} arrays and attribution headers match upstream "
              f"{reference['revision']} after excluding WenQuanYi and Unifont")
    data = make_inventory(fonts, catalog)
    if args.output:
        args.output.write_text(data)
    rows = list(csv.DictReader(io.StringIO(data), delimiter="\t"))
    statuses = {status: sum(row["status"] == status for row in rows) for status in sorted({r["status"] for r in rows})}
    print(f"{len(fonts)} fonts; {len({r['family'] for r in rows})} families; "
          f"{len({r['group'] for r in rows})} groups; {statuses}; notices verified")


if __name__ == "__main__":
    main()
