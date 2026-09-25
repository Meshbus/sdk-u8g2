# SPDX-FileCopyrightText: 2026 FoBE Studio
# SPDX-License-Identifier: Apache-2.0
"""Exercise rejection of incomplete or modified font notice inventories."""
import hashlib
import json
from pathlib import Path
import sys
import tempfile
import unittest
from unittest.mock import patch

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import font_inventory


class NoticeInventoryTests(unittest.TestCase):
    def setUp(self):
        directory = tempfile.TemporaryDirectory()
        self.addCleanup(directory.cleanup)
        self.root = Path(directory.name)
        (self.root / "notices").mkdir()
        self.notice = self.root / "notices/font.txt"
        self.notice.write_bytes(b"Copyright and permission\r\n")
        self.catalog = {"groups": {"font": {"notice": "font"}}}
        self.sources = {"notices": [{
            "id": "font", "sha256": hashlib.sha256(self.notice.read_bytes()).hexdigest()
        }]}
        (self.root / "catalog.json").write_text(json.dumps(self.catalog))
        (self.root / "sources.json").write_text(json.dumps(self.sources))
        redirect = patch.object(font_inventory, "FONTS", self.root)
        redirect.start()
        self.addCleanup(redirect.stop)

    def test_preserves_original_line_endings(self):
        self.assertEqual(font_inventory.load_records(), (self.catalog, self.sources))
        self.notice.write_bytes(b"Copyright and permission\n")
        with self.assertRaisesRegex(ValueError, "Upstream notice changed"):
            font_inventory.load_records()

    def test_missing_or_unlisted_notice_rejected(self):
        extra = self.root / "notices/unlisted.txt"
        extra.write_text("Additional terms")
        with self.assertRaisesRegex(ValueError, "Notice files differ"):
            font_inventory.load_records()
        extra.unlink()
        self.notice.unlink()
        with self.assertRaisesRegex(ValueError, "Notice files differ"):
            font_inventory.load_records()

    def test_missing_group_notice_rejected(self):
        self.catalog["groups"]["font"]["baseline_notice"] = "missing"
        (self.root / "catalog.json").write_text(json.dumps(self.catalog))
        with self.assertRaisesRegex(ValueError, "Missing group notice"):
            font_inventory.load_records()

    def test_path_escape_rejected(self):
        self.sources["notices"][0]["id"] = "../outside"
        (self.root / "sources.json").write_text(json.dumps(self.sources))
        with self.assertRaisesRegex(ValueError, "Invalid notice identifier"):
            font_inventory.load_records()


if __name__ == "__main__":
    unittest.main()
