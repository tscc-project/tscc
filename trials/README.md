# TCP6A real-project trials

This directory owns the reproducible evidence contract used to choose compiler
work after the preview milestone. `manifest.json` pins every trial by a stable
local revision and records its licence, entry points, compiler options, oracle,
timeout, and expected current status. The initial projects are deliberately
small, auditable stand-ins for common package shapes; externally acquired
projects can be added only with an immutable revision and an offline cache rule.

Run the corpus with:

```sh
python3 trials/run.py --tscc ./tscc --manifest trials/manifest.json \
  --json .build/trials/latest.json
```

The runner never treats unsupported behavior as success. Each trial is reported
as `pass`, `fail`, or `unsupported`, and failures carry one of: `acquisition`,
`configuration`, `parser`, `binder`, `checker`, `resolution`, `lowering`,
`emission`, `runtime`, `diagnostic`, `crash`, or `timeout`. A manifest expectation
only makes a known unsupported result non-fatal to the harness; it does not turn
that result into a compiler pass.
