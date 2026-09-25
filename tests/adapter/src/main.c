/* SPDX-FileCopyrightText: 2026 FoBE Studio */
/* SPDX-License-Identifier: Apache-2.0 */
#include <string.h>
#include <display/u8g2.h>
#include <display/u8g2_snapshot.h>
#include <zephyr/ztest.h>

#define WIDTH 128
#define HEIGHT 64
#define REEL_WIDTH 250
#define REEL_HEIGHT 120
static uint8_t pixels[REEL_WIDTH * REEL_HEIGHT / 8];
static uint16_t panel_width = WIDTH;
static uint16_t panel_height = HEIGHT;
static bool reel_panel;
static unsigned int writes;
static bool fail_write;
static u8g2_t renderer;

static int blank(const struct device *dev)
{
	ARG_UNUSED(dev);
	return 0;
}

static int write_frame(const struct device *dev, uint16_t x, uint16_t y,
		       const struct display_buffer_descriptor *desc, const void *buf)
{
	ARG_UNUSED(dev);
	zassert_true(desc->width > 0);
	zassert_true(x + desc->width <= panel_width);
	zassert_true(y + desc->height <= panel_height);
	zassert_equal(desc->height, 8);
	zassert_equal(desc->pitch, desc->width);
	zassert_equal(desc->buf_size, desc->width);
	if (fail_write) {
		return -EIO;
	}
	memcpy(pixels + y / 8 * panel_width + x, buf, desc->width);
	writes++;
	return 0;
}

static void caps(const struct device *dev, struct display_capabilities *out)
{
	ARG_UNUSED(dev);
	*out = (struct display_capabilities){
		.x_resolution = panel_width, .y_resolution = panel_height,
		.supported_pixel_formats = reel_panel ? PIXEL_FORMAT_MONO10 : PIXEL_FORMAT_MONO01,
		.current_pixel_format = reel_panel ? PIXEL_FORMAT_MONO10 : PIXEL_FORMAT_MONO01,
		.screen_info = SCREEN_INFO_MONO_VTILED |
			(reel_panel ? SCREEN_INFO_MONO_MSB_FIRST | SCREEN_INFO_EPD : 0),
	};
}

static int format(const struct device *dev, enum display_pixel_format fmt)
{
	ARG_UNUSED(dev);
	return fmt == (reel_panel ? PIXEL_FORMAT_MONO10 : PIXEL_FORMAT_MONO01) ?
		0 : -ENOTSUP;
}

static DEVICE_API(display, panel_api) = {
	.blanking_on = blank, .blanking_off = blank, .write = write_frame,
	.get_capabilities = caps, .set_pixel_format = format,
};
DEVICE_DEFINE(test_display, "test_display", NULL, NULL, NULL, NULL,
	      POST_KERNEL, CONFIG_DISPLAY_INIT_PRIORITY, &panel_api);

static void render(void)
{
	u8g2_FirstPage(&renderer);
	do {
		u8g2_DrawBox(&renderer, 0, 0, WIDTH, HEIGHT);
	} while (u8g2_NextPage(&renderer));
}

static void before(void *fixture)
{
	ARG_UNUSED(fixture);
	fail_write = false;
	u8g2_deinit();
	panel_width = WIDTH;
	panel_height = HEIGHT;
	reel_panel = false;
	zassert_ok(u8g2_set_output_invert(false));
	zassert_ok(u8g2_init(&renderer, DEVICE_GET(test_display), U8G2_R0));
	writes = 0;
}

static void after(void *fixture)
{
	ARG_UNUSED(fixture);
	u8g2_deinit();
}

ZTEST(adapter, test_frame_and_inversion)
{
	render();
	zassert_true(writes > 0);
	for (size_t i = 0; i < WIDTH * HEIGHT / 8; ++i) {
		zassert_equal(pixels[i], 0xff);
	}
	zassert_ok(u8g2_set_output_invert(true));
	render();
	for (size_t i = 0; i < WIDTH * HEIGHT / 8; ++i) {
		zassert_equal(pixels[i], 0);
	}
}

ZTEST(adapter, test_selected_font)
{
	u8g2_SetFont(&renderer, u8g2_font_6x10_tf);
	zassert_true(u8g2_GetStrWidth(&renderer, "sdk-u8g2") > 0);
}

ZTEST(adapter, test_snapshot_contract)
{
	struct u8g2_snapshot_info info;
	uint8_t frame[WIDTH * HEIGHT / 8];

	render();
	if (IS_ENABLED(CONFIG_U8G2_SNAPSHOT)) {
		zassert_ok(u8g2_snapshot_copy(frame, sizeof(frame), &info));
		zassert_equal(info.len, sizeof(frame));
		zassert_equal(info.width, WIDTH);
		zassert_equal(info.height, HEIGHT);
		zassert_mem_equal(frame, pixels, sizeof(frame));
		zassert_equal(u8g2_snapshot_copy(frame, 1, &info), -ENOSPC);
		fail_write = true;
		u8g2_ClearBuffer(&renderer);
		u8g2_SendBuffer(&renderer);
		uint8_t retained[sizeof(frame)];

		zassert_ok(u8g2_snapshot_copy(retained, sizeof(retained), &info));
		zassert_mem_equal(retained, frame, sizeof(frame));
		u8g2_deinit();
		zassert_equal(u8g2_snapshot_copy(frame, sizeof(frame), &info), -ENODEV);
	} else {
		zassert_equal(u8g2_snapshot_copy(frame, sizeof(frame), &info), -ENOTSUP);
	}
}

static void render_reel_edge(void)
{
	u8g2_FirstPage(&renderer);
	do {
		for (uint16_t row = 0; row < REEL_HEIGHT / 8; row++) {
			u8g2_DrawPixel(&renderer, REEL_WIDTH - 1, row * 8 + row % 8);
		}
	} while (u8g2_NextPage(&renderer));
}

ZTEST(adapter, test_reel_width_and_snapshot_stride)
{
	static uint8_t frame[REEL_WIDTH * REEL_HEIGHT / 8];
	struct u8g2_snapshot_info info;

	u8g2_deinit();
	panel_width = REEL_WIDTH;
	panel_height = REEL_HEIGHT;
	reel_panel = true;
	zassert_ok(u8g2_init(&renderer, DEVICE_GET(test_display), U8G2_R0));
	zassert_equal(u8g2_GetDisplayWidth(&renderer), REEL_WIDTH);
	zassert_equal(u8g2_GetDisplayHeight(&renderer), REEL_HEIGHT);

	/* Exercise the repeated-tile clear path, including its final two columns. */
	u8x8_ClearDisplay(u8g2_GetU8x8(&renderer));
	for (size_t i = 0; i < sizeof(pixels); i++) {
		zassert_equal(pixels[i], 0xff);
	}

	writes = 0;
	render_reel_edge();
	zassert_equal(writes, REEL_HEIGHT / 8);
	for (uint16_t row = 0; row < REEL_HEIGHT / 8; row++) {
		for (uint16_t x = 0; x < REEL_WIDTH; x++) {
			uint8_t expected = x == REEL_WIDTH - 1 ?
				(uint8_t)~BIT(7 - row % 8) : 0xff;

			zassert_equal(pixels[row * REEL_WIDTH + x], expected);
		}
	}

	if (IS_ENABLED(CONFIG_U8G2_SNAPSHOT)) {
		zassert_ok(u8g2_snapshot_copy(frame, sizeof(frame), &info));
		zassert_equal(info.width, REEL_WIDTH);
		zassert_equal(info.height, REEL_HEIGHT);
		zassert_equal(info.len, sizeof(frame));
		zassert_false(info.inverted);
		for (uint16_t row = 0; row < REEL_HEIGHT / 8; row++) {
			for (uint16_t x = 0; x < REEL_WIDTH; x++) {
				uint8_t expected = x == REEL_WIDTH - 1 ? BIT(row % 8) : 0;

				zassert_equal(frame[row * REEL_WIDTH + x], expected);
			}
		}
	}

	if (IS_ENABLED(CONFIG_U8G2_BUFFER_MODE_FULL)) {
		/* Update just the final tile: the driver must receive two columns. */
		u8g2_ClearBuffer(&renderer);
		u8g2_DrawPixel(&renderer, REEL_WIDTH - 1, 7);
		writes = 0;
		u8g2_UpdateDisplayArea(&renderer, 31, 0, 1, 1);
		zassert_equal(writes, 1);
		zassert_equal(pixels[REEL_WIDTH - 2], 0xff);
		zassert_equal(pixels[REEL_WIDTH - 1], 0xfe);
	}

	zassert_ok(u8g2_set_output_invert(true));
	render_reel_edge();
	for (uint16_t row = 0; row < REEL_HEIGHT / 8; row++) {
		for (uint16_t x = 0; x < REEL_WIDTH; x++) {
			uint8_t expected = x == REEL_WIDTH - 1 ? BIT(7 - row % 8) : 0;

			zassert_equal(pixels[row * REEL_WIDTH + x], expected);
		}
	}
}

ZTEST_SUITE(adapter, NULL, NULL, before, after, NULL);
