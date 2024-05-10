# SPDX-License-Identifier: Apache-2.0

zephyr_include_directories(
	.
)

target_include_directories(app PRIVATE ${ZEPHYR_BASE}/include/portability)

target_sources(app PRIVATE
	lx_platform_memory.c
	lx_platform_time.c
	lx_platform_log.c
	lx_platform_new.cpp
)
