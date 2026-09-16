# tool/emoji - Emoji resource pipeline for HoneyGUI

Generates the color-emoji resources used by the TTF vector text path
(`gui_text_emoji_set()`, see `realgui/engine/draw/draw_font.c`
`gui_unicode_append_emoji_file_name()` and `realgui/widget/gui_text.c`).

## Files

| File | Purpose |
| --- | --- |
| `gen_emoji_res.py` | Main generator: reads `emoji_freq.json` to pick which emoji to keep, then converts source images from noto-emoji into HoneyGUI `.bin` resources (or copies `.svg`). Output file names are normalized to exactly match the runtime lookup name. |
| `emoji_freq.json` | Authoritative usage-frequency tier table (`tiers`: 0 = most used .. 17 = least, plus a `whitelist` of force-keep ZWJ sequences). This is the checked-in source of truth for *which* emoji are generated. ASCII-only. |
| `emoji-data.txt` | Unicode Emoji Data (UCD), used by `gen_emoji_property_header.py` to build the BMP emoji property table. Raw Unicode data, not ASCII-safe. |
| `gen_emoji_property_header.py` | Regenerates `realgui/engine/draw/emoji_unicode_property.h` (U+2000..U+2FFF Emoji / Emoji_Presentation ranges) from `emoji-data.txt`. |
| `bench_pack_size.py` | (internal) Flash-size evaluation: sweeps tier / size / format / compress, packs results with `tool/mkromfs`. Not part of the product build. Kept in the dev workspace, not tracked here. |

## External dependencies

- **noto-emoji** (https://github.com/googlefonts/noto-emoji) - source of the
  emoji images. Point `--noto-dir` (or the `NOTO_EMOJI_DIR` environment
  variable) at a checkout. This is an input to the tools, not bundled here.

## Usage

### 1. Generate emoji `.bin` resources (PNG path)

```sh
# dry-run first: see how many emoji are selected and which sources are missing
python tool/emoji/gen_emoji_res.py --max-tier 9 --png-size 72 --dry-run

# generate tier 0..9, 72px, ARGB8888, RLE
python tool/emoji/gen_emoji_res.py --noto-dir /path/to/noto-emoji \
    --max-tier 9 --png-size 72 --format argb8888 --compress rle --out-dir out
```

Key options: `--max-tier N` (0=most used, 17=least), `--max-codepoints N`
(drop over-complex composite emoji, -1 = keep all), `--format`
(argb8888/argb8565/...), `--compress` (none/rle/fastlz/yuv/adaptive),
`--png-size` (32/72/128/512), `--mode` (png/svg/both), `--include-whitelist`
(also include the ZWJ whitelist extras), `--dry-run`.

> `--rle-run1` must be 1 for IDU-compatible RLE (the script refuses otherwise).

### 2. Refresh the BMP emoji property table

```sh
# check the in-tree header matches the data
python tool/emoji/gen_emoji_property_header.py --check

# regenerate realgui/engine/draw/emoji_unicode_property.h from emoji-data.txt
python tool/emoji/gen_emoji_property_header.py
```

### 3. Deliver

Copy the generated `emoji_u*.bin` files into your ROMFS `root/font/emoji/`,
re-pack, mount, and call `gui_text_emoji_set(text, "/rom/font/emoji/emoji_u", size)`.

## Notes for contributors

- `emoji_freq.json` is the authoritative tier table. To keep emoji that are not
  in the tiers/whitelist, add them to `whitelist` and rerun with
  `--include-whitelist`.
- Do not hand-edit generated artifacts (`emoji_u*.bin`); regenerate from
  noto-emoji sources.
- The tools are ASCII-only Python; `emoji-data.txt` is the only non-ASCII file
  here (raw UCD). `tool/` is covered by the CI `ignore_file_encoding` list.
