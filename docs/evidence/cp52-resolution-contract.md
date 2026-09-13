# CP51-CP52 resolution contract

Date: 13 September 2026

- Relative resolution returns `not applicable`, `resolved`, `missing`, or
  `ambiguous`; callers no longer infer all outcomes from a boolean.
- Supported candidates include `.ts`, `.tsx`, `.mts`, `.cts`, `.d.ts`,
  `.d.mts`, `.d.cts`, matching directory indexes, JavaScript-specifier mapping,
  and relative package metadata.
- Conflicting `types` and `typings` entries that both exist are diagnosed rather
  than silently selected.
- `--moduleResolution node` is explicit and versioned by name. It ascends
  `node_modules`, respects package boundaries, handles scoped packages and
  subpaths, and selects the bounded `types`, `typings`, `exports`-condition,
  `default`, `main`, or index contract.
- Default `relative` mode continues to preserve bare specifiers without
  acquiring ambient machine packages. `--noResolve` still constructs only the
  requested roots.
- Focused graph tests prove package callable facts reach consumers and reject a
  wrong argument. TCP6A remains 3/3 and the independent wall remains
  555 pass, 0 fail, 24 explicit semantic skips across 579 cases.

CP53 must not extend `Config.cpp`'s current regex extraction. Nested `paths`,
inheritance and cycle-attributed diagnostics require a proper JSONC value tree;
that parser replacement is the entry condition for the next checkpoint.
