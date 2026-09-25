# SPDX-FileCopyrightText: 2026 FoBE Studio
# SPDX-License-Identifier: Apache-2.0

zephyr_include_directories(include)
add_subdirectory_ifdef(CONFIG_U8G2 src)
