# Licensing

U8g2 core and upstream headers retain their two-clause BSD notices.
FoBE Studio integration, tools and samples retain Apache-2.0 notices.
Fonts have independent terms recorded in `fonts/`. No repository-wide default
relicenses third-party source or font data.

## Capture export helpers

`src/u8x8_capture.c` and `src/u8x8_u16toa.c` implement the public
PBM/XBM export functions. Both are imported from U8g2 revision
[`4a3cadf68afbe3f4d2d415b2517d30c95e40b82c`](https://github.com/olikraus/u8g2/tree/4a3cadf68afbe3f4d2d415b2517d30c95e40b82c/csrc).
Only the include changes from `"u8x8.h"` to `<display/u8x8.h>`; code and upstream
copyright/permission notices are retained. They are compiled by this module
when `CONFIG_U8G2=y`, with ordinary unused-function elimination at link time.
The selected license is BSD-2-Clause; its text is retained in the source files
and `LICENSES/BSD-2-Clause.txt`.

| Original upstream file | SHA-256 before the include-path change |
| --- | --- |
| `csrc/u8x8_capture.c` | `36f3b20027f4dec2a48053e1f83034b559cd62723c74f3cabc956567de83d776` |
| `csrc/u8x8_u16toa.c` | `5a6877ecb5b0665a48f952588e5bdd173407cf4dac399bcfa75ba64dad412b42` |

## Fonts

Fonts have independent terms. See [font licensing and restrictions](fonts/README.md),
[the catalog](fonts/catalog.json), [source records](fonts/sources.json), and the
[retained upstream notices and license texts](fonts/notices/).
The library's BSD license and the integration's Apache-2.0 license do not
relicense font data. Preserve the applicable notices when distributing sources
or compiled outputs under the relevant terms.
