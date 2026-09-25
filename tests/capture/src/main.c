/* SPDX-FileCopyrightText: 2026 FoBE Studio */
/* SPDX-License-Identifier: Apache-2.0 */
#include <string.h>
#include <display/u8g2.h>
#include <zephyr/ztest.h>

static const u8x8_display_info_t display_info = {
	.tile_width = 1, .tile_height = 1, .pixel_width = 8, .pixel_height = 8,
};
static char output[256];
static size_t output_len;

static uint8_t display_cb(u8x8_t *u8x8, uint8_t msg, uint8_t arg, void *ptr)
{
	ARG_UNUSED(arg);
	ARG_UNUSED(ptr);
	if (msg == U8X8_MSG_DISPLAY_SETUP_MEMORY) {
		u8x8_d_helper_display_setup_memory(u8x8, &display_info);
	}
	return 1;
}

static void capture(const char *text)
{
	size_t len = strlen(text);

	zassert_true(output_len + len < sizeof(output));
	memcpy(output + output_len, text, len + 1);
	output_len += len;
}

static void verify_export(bool horizontal)
{
	u8g2_t renderer;
	uint8_t buffer[8];

	u8g2_SetupDisplay(&renderer, display_cb, u8x8_cad_empty, u8x8_byte_empty,
			 display_cb);
	u8g2_SetupBuffer(&renderer, buffer, 1,
		horizontal ? u8g2_ll_hvline_horizontal_right_lsb :
			u8g2_ll_hvline_vertical_top_lsb, U8G2_R0);
	u8g2_ClearBuffer(&renderer);
	u8g2_DrawPixel(&renderer, 0, 0);
	u8g2_DrawPixel(&renderer, 7, 1);
	u8g2_DrawPixel(&renderer, 3, 5);

	output_len = 0;
	if (horizontal) {
		u8g2_WriteBufferPBM2(&renderer, capture);
	} else {
		u8g2_WriteBufferPBM(&renderer, capture);
	}
	zassert_mem_equal(output,
		"P1\n8\n8\n10000000\n00000001\n00000000\n00000000\n"
		"00000000\n00010000\n00000000\n00000000\n", 80);
	zassert_equal(output_len, 79);

	output_len = 0;
	if (horizontal) {
		u8g2_WriteBufferXBM2(&renderer, capture);
	} else {
		u8g2_WriteBufferXBM(&renderer, capture);
	}
	zassert_true(strcmp(output,
		"#define xbm_width 8\n#define xbm_height 8\n"
		"static unsigned char xbm_bits[] = {\n"
		"0x01,\n0x80,\n0x00,\n0x00,\n0x00,\n0x08,\n0x00,\n0x00};\n") == 0);
}

ZTEST(capture, test_vertical_exports)
{
	verify_export(false);
}

ZTEST(capture, test_horizontal_exports)
{
	verify_export(true);
}

ZTEST_SUITE(capture, NULL, NULL, NULL, NULL, NULL);
