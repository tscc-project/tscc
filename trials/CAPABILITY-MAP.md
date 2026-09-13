# TCP6A baseline capability map

Baseline compiler: `aa73e48` (CP43). Results are retained in
`evidence/cp44-baseline.json`.

| Project shape | Result | Proven capability or leading blocker |
|---|---|---|
| Typed single-file CLI | pass | typed function erasure/checking, emit, Node execution |
| Default-import library | pass | relative traversal, default import/export CommonJS lowering, callable use |
| Declaration consumer | unsupported | relative resolution does not select `.d.ts`; declaration inputs are not part of the compilation graph |

The CP44 baseline identified declaration-file acquisition as the
highest-leverage blocker. CP47-CP48 subsequently closed that initial slice: the
declaration consumer now passes, its `.d.ts` produces no JavaScript, and the
reduced negative case proves an imported declaration signature rejects a wrong
argument. CP49-CP50 extend those canonical graph facts through default,
namespace, aliased re-export and cyclic re-export paths.
