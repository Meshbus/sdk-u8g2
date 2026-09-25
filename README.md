# sdk-u8g2

U8g2 rendering and a Zephyr Display API adapter, provided as an independent
Zephyr module. The adapter supports vertically tiled monochrome MONO01/MONO10
displays, full or 8-row page buffers, output inversion and optional synchronized
software-frame snapshots. It supports one display instance.
Widths need not be multiples of eight: partial edge tiles are clipped to the
physical panel, including the 250-pixel-wide reel_board display. Snapshot bytes
are packed to the physical width without internal tile padding.

## Integration

Add the repository to a west manifest as project `u8g2`, using
`https://github.com/Meshbus/sdk-u8g2`, path `modules/lib/u8g2` and a reviewed commit
SHA. Alternatively pass `-DZEPHYR_EXTRA_MODULES=/absolute/path/to/u8g2` to CMake.
The module supplies its own CMake and Kconfig; no external adapter is required.

Enable `CONFIG_DISPLAY=y`, `CONFIG_U8G2=y`, an appropriate display driver, and a
heap large enough for the adapter's buffers (`CONFIG_HEAP_MEM_POOL_SIZE`).
Select individual `CONFIG_U8G2_FONT_*` options. The default is a full buffer;
`CONFIG_U8G2_BUFFER_MODE_PAGE8=y` selects paged drawing. Snapshots are disabled
by default and require full-buffer mode. Consumers may enable
`CONFIG_U8G2_SNAPSHOT=y`; the header documents copying and error semantics.

Public headers are `<display/u8g2.h>`, `<display/u8x8.h>` and
`<display/u8g2_snapshot.h>`.
Product font selection and snapshot defaults belong to the consuming application.

The public `u8g2_WriteBufferPBM()` / `u8g2_WriteBufferXBM()` functions and their
`2` variants export the current U8g2 buffer through a text callback. These
upstream buffer exports use tile dimensions, so their image width includes any
padding columns and page mode exports the current page. Use the snapshot API
for a complete frame packed to the physical display dimensions.

## Examples and tests

The [display sample](samples/display/README.rst) supports the targets and shield
recorded in its `sample.yaml`.
From the west workspace with an activated Zephyr environment:

```sh
west twister -T modules/lib/u8g2/tests -p qemu_x86 \
  -O build/u8g2-tests --inline-logs
west twister -T modules/lib/u8g2/samples -p frdm_k64f -p reel_board \
  --build-only -O build/u8g2-samples --inline-logs
python3 modules/lib/u8g2/scripts/font_inventory.py --check
python3 -m unittest discover -s modules/lib/u8g2/scripts/tests
```

The contract tests explicitly load only this module. Their fake display checks
full/page rendering, inversion, selected-font linkage, snapshot behavior and
disabled configuration. A 250x120 MONO10/MSB-first fake panel covers edge
clipping, page strides, inversion and packed snapshots. Capture tests check
PBM/XBM contents for both buffer layouts. QEMU and sample compilation do not
prove physical panel behavior.

## Source and licensing

This module is derived from U8g2 and includes Zephyr display I/O, Kconfig
font selection, snapshots and font exclusions maintained by FoBE Studio.

FoBE Studio-owned integration, tools and examples retain Apache-2.0 terms;
upstream source and headers retain their two-clause BSD notices. Fonts have
independent terms. [LICENSING.md](LICENSING.md) describes the license scope;
[fonts/README.md](fonts/README.md) indexes font terms and restrictions.
Complete font notices are in `fonts/notices/`; `fonts/catalog.json` and
`fonts/sources.json` record the catalog, source revisions and hashes.
The inventory contains 1,616 arrays and does not include WenQuanYi or Unifont.
Of the bundled arrays, **15 are restricted** and **87 require further license
review**; see [restrictions and unresolved items](fonts/README.md#restrictions-and-unresolved-items).
These arrays are distributed in the source even when their Kconfig options are
disabled. A successful inventory check establishes consistency, not permission
for every use. Review the terms of each selected font before using or
redistributing it.

To regenerate the bundled selection, supply the exact upstream file recorded
in `font_reference` in `fonts/sources.json`, then run:

```sh
python3 scripts/import_fonts.py --reference-fonts /path/to/upstream/u8g2_fonts.c
python3 scripts/import_fonts.py --check --reference-fonts /path/to/upstream/u8g2_fonts.c
```

Generated arrays and options are maintained through that importer. Original
font data, attribution and supplemental notice hashes are checked independently.
