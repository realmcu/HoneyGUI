#!/usr/bin/env python3
#
# Copyright (c) 2026, Realtek Semiconductor Corporation
# SPDX-License-Identifier: MIT
#
# Generate the HoneyGUI BMP emoji property table from Unicode emoji-data.txt.
# Only U+2000..U+2FFF is included because supplementary-plane emoji use the
# existing direct resource lookup path.

import argparse
import re
from pathlib import Path

MIN_CODEPOINT = 0x2000
MAX_CODEPOINT = 0x2FFF
PROPERTIES = ("Emoji", "Emoji_Presentation")
LINE_RE = re.compile(
    r"^\s*([0-9A-Fa-f]+)(?:\.\.([0-9A-Fa-f]+))?\s*;\s*([A-Za-z_]+)"
)


def parse_version(lines):
    for line in lines:
        if line.startswith("# Version:"):
            return line.split(":", 1)[1].strip()
    return "unknown"


def merge_ranges(ranges):
    merged = []
    for first, last in sorted(ranges):
        if merged and first <= merged[-1][1] + 1:
            merged[-1] = (merged[-1][0], max(merged[-1][1], last))
        else:
            merged.append((first, last))
    return merged


def parse_properties(source):
    lines = source.read_text(encoding="utf-8").splitlines()
    ranges = {prop: [] for prop in PROPERTIES}

    for line in lines:
        match = LINE_RE.match(line)
        if match is None:
            continue
        first_text, last_text, prop = match.groups()
        if prop not in ranges:
            continue

        first = int(first_text, 16)
        last = int(last_text, 16) if last_text else first
        first = max(first, MIN_CODEPOINT)
        last = min(last, MAX_CODEPOINT)
        if first <= last:
            ranges[prop].append((first, last))

    return parse_version(lines), {
        prop: merge_ranges(prop_ranges) for prop, prop_ranges in ranges.items()
    }


def format_ranges(name, ranges):
    body = "\n".join(
        "    {0x%04X, 0x%04X}," % (first, last) for first, last in ranges
    )
    return (
        "static const gui_emoji_unicode_range_t %s[] =\n"
        "{\n%s\n};\n" % (name, body)
    )


def generate_header(version, properties):
    emoji_ranges = format_ranges(
        "gui_emoji_property_ranges", properties["Emoji"]
    )
    presentation_ranges = format_ranges(
        "gui_emoji_presentation_ranges", properties["Emoji_Presentation"]
    )

    return """/*
 * Copyright (c) 2026, Realtek Semiconductor Corporation
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef EMOJI_UNICODE_PROPERTY_H
#define EMOJI_UNICODE_PROPERTY_H

#include <stdbool.h>
#include <stdint.h>

/* Generated from Unicode emoji-data.txt version %s. Do not edit manually. */
typedef struct
{
    uint16_t first;
    uint16_t last;
} gui_emoji_unicode_range_t;

%s
%s
static bool gui_emoji_unicode_in_ranges(
    uint16_t unicode, const gui_emoji_unicode_range_t *ranges, uint32_t range_count)
{
    int32_t left = 0;
    int32_t right = (int32_t)range_count - 1;

    while (left <= right)
    {
        int32_t middle = left + (right - left) / 2;

        if (unicode < ranges[middle].first)
        {
            right = middle - 1;
        }
        else if (unicode > ranges[middle].last)
        {
            left = middle + 1;
        }
        else
        {
            return true;
        }
    }

    return false;
}

static bool gui_emoji_unicode_has_property(uint32_t unicode)
{
    if (unicode < 0x2000 || unicode > 0x2FFF)
    {
        return false;
    }

    return gui_emoji_unicode_in_ranges(
               (uint16_t)unicode, gui_emoji_property_ranges,
               sizeof(gui_emoji_property_ranges) / sizeof(gui_emoji_property_ranges[0]));
}

static bool gui_emoji_unicode_has_default_presentation(uint32_t unicode)
{
    if (unicode < 0x2000 || unicode > 0x2FFF)
    {
        return false;
    }

    return gui_emoji_unicode_in_ranges(
               (uint16_t)unicode, gui_emoji_presentation_ranges,
               sizeof(gui_emoji_presentation_ranges) /
               sizeof(gui_emoji_presentation_ranges[0]));
}

#endif /* EMOJI_UNICODE_PROPERTY_H */
""" % (version, emoji_ranges, presentation_ranges)


def main():
    here = Path(__file__).resolve().parent
    parser = argparse.ArgumentParser(
        description="Generate the U+2000..U+2FFF emoji property header"
    )
    parser.add_argument(
        "--input", type=Path, default=here / "emoji-data.txt"
    )
    parser.add_argument(
        "--output",
        type=Path,
        default=here.parent.parent / "realgui" / "engine" / "draw" /
        "emoji_unicode_property.h",
    )
    parser.add_argument(
        "--check", action="store_true",
        help="fail if the existing output differs from generated content",
    )
    args = parser.parse_args()

    version, properties = parse_properties(args.input)
    content = generate_header(version, properties)

    if args.check:
        if not args.output.exists() or args.output.read_text(encoding="ascii") != content:
            raise SystemExit("ERROR: generated emoji property header is out of date")
        print("Emoji property header is up to date")
        return

    args.output.write_text(content, encoding="ascii", newline="\n")
    print(
        "Generated %s: %d Emoji ranges, %d Emoji_Presentation ranges"
        % (
            args.output,
            len(properties["Emoji"]),
            len(properties["Emoji_Presentation"]),
        )
    )


if __name__ == "__main__":
    main()
