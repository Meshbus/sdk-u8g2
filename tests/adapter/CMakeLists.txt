# SPDX-FileCopyrightText: 2026 FoBE Studio
# SPDX-License-Identifier: Apache-2.0
cmake_minimum_required(VERSION 3.20.0)
# Deliberately exclude Meshbus and all other west modules.
get_filename_component(U8G2_ROOT "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)
set(ZEPHYR_MODULES "${U8G2_ROOT}")
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(u8g2_adapter_contract)
target_sources(app PRIVATE src/main.c)
