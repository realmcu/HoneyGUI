#!/usr/bin/env python3
#
# Copyright (c) 2026, Realtek Semiconductor Corporation
#
# SPDX-License-Identifier: MIT
#
"""Build every example/widget demo with SCons."""

import sys

import ci_common

if __name__ == "__main__":
    sys.exit(ci_common.main_for("widget", "scons"))
