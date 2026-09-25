/* SPDX-FileCopyrightText: 2026 FoBE Studio */
/* SPDX-License-Identifier: Apache-2.0 */
#include <display/u8g2_snapshot.h>
#include <zephyr/ztest.h>
BUILD_ASSERT(!IS_ENABLED(CONFIG_U8G2));
ZTEST(disabled, test_disabled_snapshot)
{
	zassert_equal(u8g2_snapshot_copy(NULL, 0, NULL), -ENOTSUP);
}
ZTEST_SUITE(disabled, NULL, NULL, NULL, NULL, NULL);
