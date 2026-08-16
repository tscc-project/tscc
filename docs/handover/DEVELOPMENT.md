# tscc development and checkpoint workflow

## Establish the baseline

Record branch/commit/status, version, compiler/reference/Node versions, and
current focused/full results. Preserve unexplained files. `make test` is now the
aggregate correctness gate; add sanitizer, fuzz/stress, performance, packaging,
and platform evidence separately when the checkpoint's risk requires them.

## Define a bounded semantic slice

Avoid objectives such as “improve TypeScript support.” State exact syntax,
runtime, module, diagnostic, and compatibility behavior. Query `tsc`/`tsc
--noCheck` where compatibility applies and decide which behavior tscc promises.

## Evidence-first workflow

1. Add a minimal external regression and a focused internal/pass test where useful.
2. Confirm reference classification and expected candidate failure.
3. Trace lexer/parser/syntax/transpiler/project/config/compiler stages involved.
4. Implement the smallest complete slice.
5. Inspect emitted JavaScript.
6. Execute output and compare reference runtime when relevant.
7. Add side-effect, shadowing, nesting, malformed, and unexpected-EOF neighbors.
8. Run focused tests, all integration targets, and the independent corpus.
9. Run ASan/UBSan for native parser/range/lifetime risk.
10. Benchmark only if a hot path changed, against a comparable baseline.
11. Reconcile README, website support/benchmarks, handovers, and roadmap.

Do not patch one AST/token shape if the issue is a binding, transform-ownership,
or evaluation-order family. Conversely, do not rewrite the whole compiler because
a cleaner abstraction is imaginable without evidence.

## Reference discipline

Pin or record TypeScript and Node versions for checkpoint evidence. `tsc
--noCheck` answers syntax/emit-reference questions; normal `tsc` identifies
semantic restrictions. Reclassify incorrect test assumptions rather than treating
all TypeScript acceptance/rejection differences as tscc bugs.

## Checkpoint report

Include baseline, promised semantic slice, architecture stages changed, external
and internal cases, runtime/differential results, pass/fail/skip counts, reference
versions, malformed/scope/side-effect coverage, sanitizers, performance, docs/site,
handover/roadmap, repository artifacts, and known unsupported behavior.

A candidate becomes the trusted baseline only when another developer could start
from it without caveats such as unexplained failures or stale support docs.

## Living roadmap

Every feature checkpoint must reassess whether the next task is still correct. A
scope/binding architectural weakness outranks planned syntax accumulation. Real
project failures may reorder the roadmap. Items can shrink or disappear when
evidence shows them unnecessary.

The Minify++ 2026-08 fuzz-hardening checkpoint is the reference precedent: when
new evidence interrupts planned infrastructure or compiler work, minimize it,
identify the semantic family, retain exact and property-level protection, fix the
responsible architectural boundary, and then resume the original campaign. For
tscc, translate a single shadowing, malformed-input, or bad-emit failure into a
binding, recovery, or source-edit invariant question before considering a local
special case. Record what the checkpoint teaches in this workflow and the living
roadmap so the lesson survives the current development context.
