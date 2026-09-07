# Embedded Test Workspace

This directory contains the HoneyGUI embedded CI helpers.

## Initialize the firmware workspace

Run:

```sh
python setup_workspace.py
```

The default workspace is:

```text
<current-west-workspace>-honeygui-embedded
```

Use a custom location when needed:

```sh
python setup_workspace.py /path/to/workspace
```

The script runs the equivalent of:

```sh
mkdir <workspace>
cd <workspace>
west init -m ssh://cn4soc.rtkbf.com:29418/hmi/manifest \
    --mr master \
    --mf test/rtl8773gtp-eBadge-wh.yml \
    .
west update
```

An existing initialized workspace is updated without running `west init`
again. A non-empty directory without `.west/config` is rejected to avoid
overwriting unrelated files.

## Run the embedded test

Run the complete flow:

```sh
python build_test.py
```

The script performs these stages:

```text
1. Initialize or update the separate west workspace.
2. Build the current HoneyGUI checkout from <workspace>/applications.
3. Flash the runtime and bin/app.bin through /dev/ttyUSB0.
4. Capture the firmware log from /dev/ttyUSB1 for 60 seconds.
```

The build passes the current checkout through Zephyr's
`EXTRA_ZEPHYR_MODULES`, overriding the HoneyGUI revision downloaded by the
manifest. This ensures CI tests the source that contains these scripts.

Logs are stored under `embedded_test/log/`. The log reader starts before
flashing when the download and log ports are different, so the reboot log
is not lost.

Useful options:

```sh
# Print the complete flow without changing the workspace or hardware.
python build_test.py --dry-run

# Use an existing workspace and perform a pristine build.
python build_test.py /path/to/workspace --skip-setup --pristine

# Override the HoneyGUI checkout used by the embedded build.
python build_test.py --honeygui-source /path/to/honeygui

# Build only.
python build_test.py --skip-setup \
    --skip-runtime-flash --skip-app-flash --skip-log

# Override ports, capture time, and basic log checks.
python build_test.py \
    --flash-port /dev/ttyUSB0 \
    --log-port /dev/ttyUSB1 \
    --log-seconds 120 \
    --expect "Booting Zephyr OS" \
    --reject "ASSERTION FAIL"
```

Use `--log-seconds 0` to capture until `Ctrl+C`.
