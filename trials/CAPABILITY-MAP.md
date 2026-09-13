# TCP6A baseline capability map

Baseline compiler: `aa73e48` (CP43). Results are retained in
`evidence/cp44-baseline.json`.

| Project shape | Result | Proven capability or leading blocker |
|---|---|---|
| Typed single-file CLI | pass | typed function erasure/checking, emit, Node execution |
| Default-import library | pass | relative traversal, default import/export CommonJS lowering, callable use |
| Declaration consumer | unsupported | relative resolution does not select `.d.ts`; declaration inputs are not part of the compilation graph |

The highest-leverage observed blocker is declaration-file acquisition and its
frontend contract. It blocks packages that ship JavaScript plus declarations
before their exported type facts can reach the checker. The reduced case in
`tests/trial_blockers.sh` freezes that result without pretending the unsupported
project passes.
