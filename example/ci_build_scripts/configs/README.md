# CI Configurations

These directories match the demo source layout under `example/`. Each file is
a complete minimal build input. CI copies one `.config_*` file to
`win32_sim/.config` and builds it directly without parsing `Kconfig.gui`.

- `application/` contains the configs for `example/application/`.
- `widget/` contains the configs for `example/widget/`.

When a build option changes, update the affected files explicitly so the
configuration diff is visible in review.
