# CP45 / TCP7 evidence decision

Date: 13 September 2026

## Evidence considered

- TCP6A baseline: two project shapes pass and one is explicitly unsupported.
- The leading reduced failure is a JavaScript implementation accompanied by a
  relative `.d.ts` declaration input. Resolution stops before the declaration
  frontend can contribute exports to the program graph.
- The 579-case independent wall remains 555 pass, 0 fail, 24 semantic skips.
- Default import/export CommonJS runtime behavior already passes the selected
  trial, so broad default-import rewriting is not the first blocker.

## Ranked blockers

Scores use 1 (low) to 5 (high). Risk is inverted when totaling so a lower-risk
slice ranks higher: `projects + depth + leverage + (6 - risk)`.

| Rank | Blocker | Projects | Depth | Leverage | Risk | Score |
|---:|---|---:|---:|---:|---:|---:|
| 1 | Declaration input acquisition and no-emit ownership | 5 | 5 | 5 | 3 | 18 |
| 2 | Declaration export facts reaching imports | 5 | 5 | 5 | 4 | 17 |
| 3 | Unknown-expression accounting | 3 | 4 | 5 | 2 | 16 |
| 4 | Package resolution/config mapping | 4 | 4 | 4 | 4 | 14 |
| 5 | Broad generic/class semantics | 3 | 5 | 4 | 5 | 13 |

## Selected vertical slices

1. **Declaration graph slice:** relative resolver selects `.d.ts`, compilation
   units identify declaration inputs, and declaration files produce no JavaScript.
2. **Declaration semantic slice:** bind the supported ambient/export surface and
   propagate canonical exported callable/object facts into consuming imports.

These slices bring the observed failing project through acquisition, resolution,
binding and checking. Work stops rather than accepting the project if any stage
cannot prove its behavior.

## Deferred work

Package resolution, config mapping, generic inference, overloads, class/flow
semantics, JSX checking and production output services remain queued. CP46's
unknown-expression accounting stays immediately after the two selected slices,
because it is the guardrail for safely broadening semantic coverage.

## Decision gates

- Slice 1 exits only when declaration inputs join the canonical graph, never emit,
  and `--noResolve` behavior remains exact.
- Slice 2 exits only when the TCP6A declaration consumer passes its runtime oracle
  and a wrong argument/member use is rejected by the checker.
- Reassess the ranking after CP50, CP56, CP62 and CP68 using retained trial data.
