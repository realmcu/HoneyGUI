#!/usr/bin/env python3
#
# Copyright (c) 2026, Realtek Semiconductor Corporation
# SPDX-License-Identifier: MIT
#
# Generate HoneyGUI emoji resources from the noto-emoji repository, tiered by
# global usage frequency (see emoji_freq.json, the authoritative tier table
# checked into tool/emoji/).
#
# Non-destructive: reads noto-emoji sources, writes into --out-dir, and never
# deletes anything (unlike the legacy noto-emoji/remove_emoji.py, which pruned
# in place).
#
#   PNG mode: convert noto png/<size>/emoji_u*.png -> HoneyGUI .bin using
#             tool/image-convert-tool/converter.py.
#   SVG mode: copy noto svg/emoji_u*.svg -> --out-dir, renamed to the runtime
#             lookup name.
#
# Output file names are normalized to EXACTLY match what the firmware builds at
# runtime in draw_font.c gui_unicode_append_emoji_file_name():
#   fe0f stripped, lowercase, min-4-digit hex, '_'-joined, prefixed 'emoji_u'.
# Some noto source names keep fe0f (e.g. long ZWJ sequences); this script
# strips it so gui_vfs_get_file_address() finds the file. Enumerate-and-
# normalize on the source side makes the match robust regardless of fe0f.
#
# Usage examples:
#   # smoke set: top-2 tiers, 72px, ARGB8888, RLE, dry-run first
#   python gen_emoji_res.py --max-tier 1 --png-size 72 --format argb8888 \
#       --compress rle --dry-run
#   # full common set (tier 0..9) both formats for the flash comparison
#   python gen_emoji_res.py --mode both --max-tier 9 --png-size 72

import argparse
import json
import os
import sys
import shutil
from pathlib import Path

HERE = os.path.dirname(os.path.abspath(__file__))

# Same drop-set as the runtime rule / extractor.
DROP = {0x20, 0x09, 0x0a, 0x0d, 0xfe0f}


def normalize_basename(basename):
    """'emoji_u1f469_200d_2764_fe0f_...' -> 'emoji_u1f469_200d_2764_...'
    Returns None if the name is not an emoji_u<hex>[_<hex>...] name."""
    if not basename.startswith("emoji_u"):
        return None
    body = basename[len("emoji_u"):]
    if not body:
        return None
    cps = []
    for tok in body.split("_"):
        if tok == "":
            continue
        try:
            cps.append(int(tok, 16))
        except ValueError:
            return None
    cps = [c for c in cps if c not in DROP]
    if not cps:
        return None
    return "emoji_u" + "_".join("%04x" % c for c in cps)


def codepoint_count(prefix):
    """Number of codepoints in a normalized 'emoji_u...' name.
    Equals prefix.count('_') because 'emoji_u' contributes one '_' and each
    extra codepoint contributes one more."""
    return prefix.count("_")


def load_selected(freq_path, max_tier, include_whitelist):
    with open(freq_path, encoding="ascii") as f:
        data = json.load(f)
    tiers = data["tiers"]
    sel = {}
    for lvl in range(0, max_tier + 1):
        for name in tiers.get(str(lvl), []):
            n = normalize_basename(name)
            if n and n not in sel:
                sel[n] = lvl
    if include_whitelist:
        for name in data.get("whitelist", []):
            n = normalize_basename(name)
            if n and n not in sel:
                sel[n] = -1  # -1 marks "whitelist extra"
    return sel


def index_sources(src_dir, ext):
    """Map normalized-name -> actual source path for every *ext file in
    src_dir. ext includes the dot, e.g. '.png'."""
    m = {}
    collisions = 0
    if not os.path.isdir(src_dir):
        return m, collisions
    for fn in os.listdir(src_dir):
        low = fn.lower()
        if not low.endswith(ext):
            continue
        base = fn[:-len(ext)]
        n = normalize_basename(base)
        if not n:
            continue
        if n in m:
            collisions += 1
            continue
        m[n] = os.path.join(src_dir, fn)
    return m, collisions


def make_converter(args):
    """Import tool/image-convert-tool in-process and build a converter plus a
    compressor instance matching the CLI options. In-process avoids spawning a
    Python interpreter per file (hundreds of files -> very slow on Windows).
    Returns (ImageConverter_instance, compressor_or_None)."""
    if args.tool_dir not in sys.path:
        sys.path.insert(0, args.tool_dir)
    from converter import ImageConverter
    from compress import (RLECompression, FastLzCompression,
                          YUVCompression, AdaptiveCompression)
    c = None
    if args.compress == "rle":
        c = RLECompression(run_length_1=args.rle_run1, run_length_2=0, level=1)
    elif args.compress == "fastlz":
        c = FastLzCompression()
    elif args.compress == "yuv":
        c = YUVCompression(sample_mode="yuv444", blur_bits=0, use_fastlz=False)
    elif args.compress == "yuv-fastlz":
        c = YUVCompression(sample_mode="yuv444", blur_bits=0, use_fastlz=True)
    elif args.compress == "adaptive":
        c = AdaptiveCompression(rle_run_length_1=args.rle_run1, rle_run_length_2=0)
    return ImageConverter(), c


def process(sel, src_index, out_dir, ext, is_png, args, missing_out,
            conv=None, compressor=None):
    made = 0
    failed = 0
    for name in sorted(sel):
        src = src_index.get(name)
        if src is None:
            missing_out.append(name)
            continue
        dst = os.path.join(out_dir, name + (".bin" if is_png else ext))
        if args.dry_run:
            made += 1
            continue
        if is_png:
            try:
                conv.convert(Path(src), Path(dst), args.format, compressor)
                if not os.path.exists(dst):
                    raise RuntimeError("no output produced")
            except Exception as e:  # isolate per-file failures
                failed += 1
                if failed <= 10:
                    print("[FAIL] %s: %s" % (name, str(e)[:200]))
                continue
        else:
            shutil.copyfile(src, dst)
        made += 1
    return made, failed


def main():
    ap = argparse.ArgumentParser(
        description="Generate tiered HoneyGUI emoji resources from noto-emoji")
    ap.add_argument("--noto-dir",
                    default=os.environ.get("NOTO_EMOJI_DIR", ""))
    # Located in tool/emoji/, so the repo root is two levels up.
    ap.add_argument("--tool-dir",
                    default=os.path.abspath(os.path.join(HERE, "..", "..", "tool", "image-convert-tool")))
    ap.add_argument("--freq", default=os.path.join(HERE, "emoji_freq.json"))
    ap.add_argument("--out-dir", default=os.path.join(HERE, "out"))
    ap.add_argument("--mode", choices=["png", "svg", "both"], default="png")
    ap.add_argument("--png-size", type=int, default=72, choices=[32, 72, 128, 512])
    ap.add_argument("--max-tier", type=int, default=9,
                    help="keep frequency tiers 0..N (0=most used, 17=least)")
    ap.add_argument("--max-codepoints", type=int, default=-1,
                    help="drop composite emoji with more than N codepoints "
                         "(-1 = keep all); a single emoji has 1 codepoint, a "
                         "flag 2, a woman+ZWJ+girl 3")
    ap.add_argument("--include-whitelist", action="store_true",
                    help="also include the force-keep ZWJ whitelist extras")
    ap.add_argument("--format", default="argb8888",
                    choices=["auto", "rgb565", "rgb888", "argb8888", "argb8565", "a8"])
    ap.add_argument("--compress", default="rle",
                    choices=["none", "rle", "fastlz", "yuv", "yuv-fastlz", "adaptive"])
    # feature_1 = RLE run-length byte count. Must be 1: the IDU decoder
    # (acc_sw_idu.c decode_RLE_1stage) reads run length as that many bytes,
    # only run_len=1 matches the [len:1][pixel] nodes we emit. Default 3
    # (old value) desynced every node and made images undecodable on IDU.
    ap.add_argument("--rle-run1", type=int, default=1, choices=[0, 1, 2, 3])
    ap.add_argument("--dry-run", action="store_true")
    args = ap.parse_args()

    if not args.noto_dir:
        print("ERROR: --noto-dir is required (or set the NOTO_EMOJI_DIR "
              "environment variable to the noto-emoji checkout)")
        sys.exit(1)

    # IDU RLE decoding only supports run length = 1 byte (decode_RLE_1stage
    # takes run length byte count from feature_1). Any other value produces
    # bins that the firmware cannot decode, so refuse instead of emitting
    # silently-broken resources.
    if args.compress == "rle" and args.rle_run1 != 1:
        print("ERROR: --rle-run1 must be 1 for IDU-compatible RLE "
              "(feature_1 = run length byte count, see acc_sw_idu.c "
              "decode_RLE_1stage)")
        sys.exit(1)

    sel = load_selected(args.freq, args.max_tier, args.include_whitelist)
    n_before = len(sel)
    dropped = 0
    if args.max_codepoints >= 0:
        kept = {k: v for k, v in sel.items()
                if codepoint_count(k) <= args.max_codepoints}
        dropped = n_before - len(kept)
        sel = kept

    print("=== emoji resource generation ===")
    print("noto-dir : %s" % args.noto_dir)
    print("out-dir  : %s" % args.out_dir)
    print("mode=%s size=%d max-tier=%d format=%s compress=%s dry-run=%s"
          % (args.mode, args.png_size, args.max_tier, args.format,
             args.compress, args.dry_run))
    print("selected emoji: %d (tier 0..%d%s)"
          % (len(sel), args.max_tier,
             ", +whitelist" if args.include_whitelist else ""))
    if dropped:
        print("dropped %d composite emoji (> %d codepoints)"
              % (dropped, args.max_codepoints))

    if not args.dry_run:
        os.makedirs(args.out_dir, exist_ok=True)

    total_made = 0
    total_failed = 0
    missing = []

    if args.mode in ("png", "both"):
        png_dir = os.path.join(args.noto_dir, "png", str(args.png_size))
        idx, coll = index_sources(png_dir, ".png")
        print("png sources indexed: %d (%s)%s"
              % (len(idx), png_dir,
                 (" [%d name collisions skipped]" % coll) if coll else ""))
        conv = compressor = None
        if not args.dry_run:
            try:
                conv, compressor = make_converter(args)
            except Exception as e:
                print("ERROR: cannot load image-convert-tool from %s: %s"
                      % (args.tool_dir, e))
                sys.exit(1)
        made, failed = process(sel, idx, args.out_dir, ".bin", True, args,
                               missing, conv, compressor)
        print("PNG->bin: %d made, %d failed" % (made, failed))
        total_made += made
        total_failed += failed

    if args.mode in ("svg", "both"):
        svg_dir = os.path.join(args.noto_dir, "svg")
        idx, coll = index_sources(svg_dir, ".svg")
        print("svg sources indexed: %d (%s)%s"
              % (len(idx), svg_dir,
                 (" [%d name collisions skipped]" % coll) if coll else ""))
        # In 'both' mode 'missing' would double count; use a fresh list per fmt.
        miss_svg = []
        made, failed = process(sel, idx, args.out_dir, ".svg", False, args, miss_svg)
        print("SVG copy : %d made, %d missing-source" % (made, len(miss_svg)))
        total_made += made
        if args.mode == "svg":
            missing = miss_svg

    # Report missing sources explicitly -- never silently drop coverage.
    if args.mode in ("png", "both") and missing:
        print("missing PNG source for %d selected emoji (e.g. flags are not in "
              "noto png/): %s%s"
              % (len(missing), ", ".join(missing[:15]),
                 " ..." if len(missing) > 15 else ""))

    print("=== done: %d resources written, %d failed ==="
          % (total_made, total_failed))
    if total_failed:
        sys.exit(2)


if __name__ == "__main__":
    main()
