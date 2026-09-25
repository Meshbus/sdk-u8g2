# Font provenance and licenses

The U8G2 implementation's BSD license does **not** cover every bundled font.
The original third-party notices and license texts in `notices/` retain their
own terms.
Apache-2.0 applies to FoBE Studio-owned integration, tools and samples; it does
not relicense upstream code or fonts. Upstream copyright comments in C sources
and headers are preserved. This directory contains the catalog and source
records; `notices/` retains the complete supplemental font notices and license
texts unchanged.

The bundled selection contains **1,616 arrays**, **496 build-name prefixes**
and **37 upstream font groups**. Sizes, character subsets and rendering modes
are separate arrays, not necessarily distinct typeface designs. A source
release distributes all of them even when a firmware selects only a few.
WenQuanYi and Unifont data, options and public declarations are excluded.

### Evidence and scope

- All bundled array bytes (including final NUL), symbols, `Fontname` and
  `Copyright` fields match U8G2 revision
  [`4a3cadf68afbe3f4d2d415b2517d30c95e40b82c`](https://github.com/olikraus/u8g2/tree/4a3cadf68afbe3f4d2d415b2517d30c95e40b82c).
  Includes, formatting and local Kconfig guards are outside that comparison.
- Supplemental notices come from U8G2 revision
  [`d6c8499c5f2707cac8eccd09fd8f677d12b17977`](https://github.com/olikraus/u8g2/tree/d6c8499c5f2707cac8eccd09fd8f677d12b17977).
  Identical baseline/supplemental notices share one text block; changed
  notices retain both versions. The Pentacom notice was absent at the baseline
  path, so its later notice remains supplemental evidence.
- The [catalog](catalog.json) maps prefixes to source inputs and notices,
  with family-specific exceptions. A prefix can have several source inputs.
  The [source manifest](sources.json) records original URLs, revisions and
  SHA-256 hashes, including the upstream build tables used for the mapping.
- `documented` means evidence was found, not that every use is cleared.
  A generic license text does not fill in a missing font-specific grant or
  select an unspecified license version. Restrictions and unresolved grants
  remain recorded below.

### Excluded fonts

The bundled selection excludes all 30 WenQuanYi arrays and 43 Unifont arrays
from the pinned baseline, including non-Chinese Unifont subsets, along with
their Kconfig options, public font declarations and Unifont aliases. Generic
UTF-8 drawing support is available for fonts supplied by the consuming
application under their own terms.

### Restrictions and unresolved items

The inventory records **15 restricted arrays** and **87 arrays requiring further
review**. These are included in the source; recording their existence does not
grant additional rights. Neither a disabled Kconfig option nor an unused symbol
removes the corresponding font from a source release.

| Build-name prefix / array count | Recorded issue |
| --- | --- |
| `fancypixels` (2), `iconquadpix` (1), `lastapprenticebold` (1), `lastapprenticethin` (1) | The authors' pages say Creative Commons NonCommercial without a specific license variant or version. [Source notices](notices/u8g2-upstream-fntgrpbitfontmaker2-pre.txt) include the original author URLs. |
| `lucasarts_scumm_subtitle_o` (3), `lucasarts_scumm_subtitle_r` (3) | CC BY-NC-SA 3.0. [Attribution](notices/u8g2-upstream-fntgrpfontstruct-pre.txt), [license text](notices/u8g2-texts-cc-by-nc-sa-3-0-txt.txt). |
| `nokiafc22` (4) | Custom notice prohibits sale of the font and explicitly raises uncertainty about the original Nokia rights. [Exact notice](notices/u8g2-upstream-fntgrpdafont-pre.txt). |
| `logisoso*` (51) | The upstream group page says OFL, but reproduces the original package's GPLv2-with-font-exception COPYING. Both statements are retained; no license is selected by inference. [Evidence](notices/u8g2-upstream-fntgrplogisoso-pre.txt). |
| `Pixellari` (5), `VCR_OSD` (8), `pixelpoiiz` (1), `pearfont` (1) | A platform's “100% free” or generic category does not establish a complete distribution grant. [Evidence](notices/u8g2-upstream-fntgrpdafont-pre.txt). |
| `adventurer` (3), `bauhaus2015` (2), `finderskeepers` (3), `IPAandRUSLCD` (3), `jinxedwizards` (1), `lastpriestess` (2) | Attribution licenses are named without a version. The precise variant must be established before treating a particular standard text as their license. |
| `freedoomr10` (2), `freedoomr25` (2) | The group calls these public domain/free, but the local headers attribute FreeUniversal/SIL Sophia. [Group statement](notices/u8g2-upstream-fntgrpu8g-pre.txt), [FreeUniversal notice](notices/u8g2-upstream-fntgrpfreeuniversal-pre.txt). |
| `battery19` (1), `7Segments_26x42` (1) | The local headers identify the editor rather than a license grant; the upstream group does not resolve the grant. |
| `siji` (1) | GPLv2 is stated, with no font-specific only/or-later clarification. Its combination with the X11 6x10 font is recorded. [Source](notices/u8g2-upstream-fntgrpsiji-pre.txt), [original GPL text](notices/u8g2-texts-siji-license-txt.txt). |

Additional qualifications remain attached to documented groups. For example,
PC Senior permits bundling in software but prohibits standalone commercial
font collections; Lucida and Iranian Sans have their own naming and attribution
terms. OFL group notices that omit a version remain labeled that way. Consult
the original notice rather than applying the library's BSD license to them.

Open Iconic's original PNG icons and generated fonts have separate notices.
The [upstream build description](notices/u8g2-upstream-fntgrpiconic-pre.txt) records
PNG-to-BDF conversion and identifies the resulting fonts as OFL. Both the
[original icon MIT notice](notices/u8g2-texts-open-iconic-icon-license-txt.txt) and
[font OFL 1.1 notice](notices/u8g2-texts-open-iconic-font-license-txt.txt) are retained.

### Checking and exporting the inventory

From the repository root, check the source selection and retained notices:

```sh
python3 scripts/font_inventory.py --check
```

The checker reads the `fonts/catalog.json` and `fonts/sources.json`. It compares
initializer coverage with C declarations, attribution comments, Kconfig options
and public headers, verifies every retained notice hash, and rejects excluded
WenQuanYi or Unifont symbols. Classification remains a reviewed input, not an
automatic legal determination. No network is used and no tracked file is written.

To export a per-symbol TSV (including family, group, attribution, license,
review status and array hash), choose an output path:

```sh
python3 scripts/font_inventory.py --output /tmp/u8g2-font-inventory.tsv
```

To independently repeat the upstream comparison, obtain the exact
`font_reference.url` recorded in the source manifest, then run:

```sh
python3 scripts/font_inventory.py --check --reference-fonts /path/to/upstream/u8g2_fonts.c
python3 scripts/import_fonts.py --check --reference-fonts /path/to/upstream/u8g2_fonts.c
```

The baseline semantic hash covers all original arrays and attribution headers;
the selection hash covers only the bundled arrays. Both tools check the complete
upstream file hash. To regenerate C arrays and Kconfig options, run the importer
without `--check`; it preserves upstream comments and bytes, adds local guards,
and removes excluded public declarations and aliases.

When updating fonts, review the source and license together, update the
catalog, manifest, notice files and hashes, and reconcile the counts and
unresolved items. New families require dependency-license review. Removing the
explicit exclusions is a policy change, not an inventory refresh.

### U8G2 font group index

| Font group | Recorded license | Status | Notices |
| --- | --- | --- | --- |
| Academia Sinica | Academia Sinica permission notice | documented | [Supplemental](notices/u8g2-upstream-fntgrpacademiasinica-pre.txt) / [Baseline](notices/u8g2-upstream-fntgrpacademiasinica-pre.txt) |
| Adobe X11 | Adobe/DEC X11 permission notice | documented | [Supplemental](notices/u8g2-upstream-fntgrpadobex11-pre.txt) / [Baseline](notices/u8g2-upstream-fntgrpadobex11-pre.txt) |
| Angel | Public domain statement | documented | [Supplemental](notices/u8g2-upstream-fntgrpangel-pre.txt) / [Baseline](notices/u8g2-upstream-fntgrpangel-pre.txt) |
| bitfontmaker2 | Public domain statement | documented | [Supplemental](notices/u8g2-upstream-fntgrpbitfontmaker2-pre.txt) / [Baseline](notices/u8g2-baseline-fntgrpbitfontmaker2-pre.txt) |
| ChristinaAntoinetteNeofotistou | Creative Commons Attribution; version unspecified | review-required | [Supplemental](notices/u8g2-upstream-fntgrpchristinaneofotistou-pre.txt) / [Baseline](notices/u8g2-upstream-fntgrpchristinaneofotistou-pre.txt) |
| Codeman38 | OFL-1.1 | documented | [Supplemental](notices/u8g2-upstream-fntgrpcodeman38-pre.txt) / [Baseline](notices/u8g2-upstream-fntgrpcodeman38-pre.txt) |
| crox | EWT/Cronyx permission notice | documented | [Supplemental](notices/u8g2-upstream-fntgrpcrox-pre.txt) / [Baseline](notices/u8g2-upstream-fntgrpcrox-pre.txt) |
| cu12 | MIT permission notice (NMSU) | documented | [Supplemental](notices/u8g2-upstream-fntgrpcu12-pre.txt) / [Baseline](notices/u8g2-baseline-fntgrpcu12-pre.txt) |
| dafont | Distribution terms not established by platform free label | review-required | [Supplemental](notices/u8g2-upstream-fntgrpdafont-pre.txt) / [Baseline](notices/u8g2-upstream-fntgrpdafont-pre.txt) |
| efont | efont/Bitstream/Adobe permission notices | documented | [Supplemental](notices/u8g2-upstream-fntgrpefont-pre.txt) / [Baseline](notices/u8g2-upstream-fntgrpefont-pre.txt) |
| Extant | Public domain statement | documented | [Supplemental](notices/u8g2-upstream-fntgrpextant-pre.txt) / [Baseline](notices/u8g2-baseline-fntgrpextant-pre.txt) |
| Fontstruct | CC-BY-SA-3.0 | documented | [Supplemental](notices/u8g2-upstream-fntgrpfontstruct-pre.txt) / [Baseline](notices/u8g2-baseline-fntgrpfontstruct-pre.txt) |
| Free Universal | SIL Open Font License; version unspecified in group notice | documented | [Supplemental](notices/u8g2-upstream-fntgrpfreeuniversal-pre.txt) / [Baseline](notices/u8g2-upstream-fntgrpfreeuniversal-pre.txt) |
| Geoff | Public domain statement | documented | [Supplemental](notices/u8g2-upstream-fntgrpgeoff-pre.txt) / [Baseline](notices/u8g2-baseline-fntgrpgeoff-pre.txt) |
| GilesBooth | Public domain statement | documented | [Supplemental](notices/u8g2-upstream-fntgrpgilesbooth-pre.txt) / [Baseline](notices/u8g2-upstream-fntgrpgilesbooth-pre.txt) |
| Open Iconic | OFL-1.1 font; original PNG icons also carry MIT notice | documented | [Supplemental](notices/u8g2-upstream-fntgrpiconic-pre.txt) / [Baseline](notices/u8g2-upstream-fntgrpiconic-pre.txt) |
| Inconsolata | SIL Open Font License; version unspecified in group notice | documented | [Supplemental](notices/u8g2-upstream-fntgrpinconsolata-pre.txt) / [Baseline](notices/u8g2-upstream-fntgrpinconsolata-pre.txt) |
| JapanYoshi | Public domain statement | documented | [Supplemental](notices/u8g2-upstream-fntgrpjapanyoshi-pre.txt) / [Baseline](notices/u8g2-baseline-fntgrpjapanyoshi-pre.txt) |
| JayWright | Public domain statement | documented | [Supplemental](notices/u8g2-upstream-fntgrpjaywright-pre.txt) / [Baseline](notices/u8g2-upstream-fntgrpjaywright-pre.txt) |
| JosephKnightcom | Public domain statement | documented | [Supplemental](notices/u8g2-upstream-fntgrpjosephknightcom-pre.txt) / [Baseline](notices/u8g2-baseline-fntgrpjosephknightcom-pre.txt) |
| Logisoso | Conflicting OFL listing and GPLv2 with font exception COPYING | review-required | [Supplemental](notices/u8g2-upstream-fntgrplogisoso-pre.txt) / [Baseline](notices/u8g2-upstream-fntgrplogisoso-pre.txt) |
| lucida | Lucida/Bigelow-Holmes/Sun permission notice | documented | [Supplemental](notices/u8g2-upstream-fntgrplucida-pre.txt) / [Baseline](notices/u8g2-upstream-fntgrplucida-pre.txt) |
| MistressEllipsis | Public domain statement | documented | [Supplemental](notices/u8g2-upstream-fntgrpmistressellipsis-pre.txt) / [Baseline](notices/u8g2-baseline-fntgrpmistressellipsis-pre.txt) |
| NBP | CC-BY-SA-3.0 | documented | [Supplemental](notices/u8g2-upstream-fntgrpnbp-pre.txt) / [Baseline](notices/u8g2-upstream-fntgrpnbp-pre.txt) |
| Oldschool PC Fonts | CC-BY-SA-4.0 | documented | [Supplemental](notices/u8g2-upstream-fntgrpoldschoolpcfonts-pre.txt) / [Baseline](notices/u8g2-upstream-fntgrpoldschoolpcfonts-pre.txt) |
| Old Standard | SIL Open Font License; version unspecified in group notice | documented | [Supplemental](notices/u8g2-upstream-fntgrpoldstandard-pre.txt) / [Baseline](notices/u8g2-upstream-fntgrpoldstandard-pre.txt) |
| Open Game Art | CC0-1.0 | documented | [Supplemental](notices/u8g2-upstream-fntgrpopengameart-pre.txt) / [Baseline](notices/u8g2-upstream-fntgrpopengameart-pre.txt) |
| Pentacom | Public domain statement | documented | [Supplemental](notices/u8g2-upstream-fntgrppentacom-pre.txt) |
| Persian | See family-specific Persian font notices | documented | [Supplemental](notices/u8g2-upstream-fntgrppersian-pre.txt) / [Baseline](notices/u8g2-upstream-fntgrppersian-pre.txt) |
| Profont | MIT | documented | [Supplemental](notices/u8g2-upstream-fntgrpprofont-pre.txt) / [Baseline](notices/u8g2-upstream-fntgrpprofont-pre.txt) |
| Siji Icon Font | GPLv2; only/or-later not specified by font notice; plus X11 6x10 | review-required | [Supplemental](notices/u8g2-upstream-fntgrpsiji-pre.txt) / [Baseline](notices/u8g2-baseline-fntgrpsiji-pre.txt) |
| Tlwg (Thai-Fonts) | Public domain statement | documented | [Supplemental](notices/u8g2-upstream-fntgrptlwg-pre.txt) / [Baseline](notices/u8g2-upstream-fntgrptlwg-pre.txt) |
| Tom-Thumb | CC0-1.0 OR CC-BY-3.0 (upstream also records historical MIT) | documented | [Supplemental](notices/u8g2-upstream-fntgrptomthumb-pre.txt) / [Baseline](notices/u8g2-upstream-fntgrptomthumb-pre.txt) |
| UW ttyp0 | TTYP0 permission notice | documented | [Supplemental](notices/u8g2-upstream-fntgrpttyp0-pre.txt) / [Baseline](notices/u8g2-upstream-fntgrpttyp0-pre.txt) |
| Tulamide | Public domain statement | documented | [Supplemental](notices/u8g2-upstream-fntgrptulamide-pre.txt) / [Baseline](notices/u8g2-upstream-fntgrptulamide-pre.txt) |
| U8glib | Public domain statement | documented | [Supplemental](notices/u8g2-upstream-fntgrpu8g-pre.txt) / [Baseline](notices/u8g2-baseline-fntgrpu8g-pre.txt) |
| X11 | Public domain or unencumbered glyph statement | documented | [Supplemental](notices/u8g2-upstream-fntgrpx11-pre.txt) / [Baseline](notices/u8g2-upstream-fntgrpx11-pre.txt) |
