# tscc testing handover

## Test layers

- Focused C++ parser tests for internal syntax ownership and diagnostics.
- Shell integration tests for smoke, runtime features/edges, project modules,
  TSX preserve, and CommonJS modules.
- Internal `regression/` and standalone `tscc-regression-suite` black-box corpus.
- Benchmarks and retained checkpoint JSON, separate from correctness.

Use the strongest appropriate combination rather than relying on compile success.

## External corpus model

Cases distinguish runtime, emit, syntax-negative, and semantic-only behavior.
The runner batches reference TypeScript classification, then compiles each case
with a candidate executable. Runtime cases execute emitted JavaScript under Node;
emit cases assert required/forbidden structure; syntax negatives require
controlled rejection; semantic-only cases remain skips until a checker exists.

The 2026-08-16 baseline contains 511 cases: 483 pass, zero fail, and 28
semantic-only skips with Node v22.22.1 and TypeScript 7.0.2. TypeScript 7 changed
its implicit module default, which initially reclassified
`semantic-import-meta-commonjs`; the runner now explicitly uses CommonJS for its
semantic oracle. The smoke differential uses the supported `preserve` mode in
place of TypeScript 7's removed `none` mode.

`make test` is the complete local correctness entry point and runs smoke, parser,
runtime/generics, project/module, regression, TSX-preserve, and CommonJS targets.
Keep focused targets available for iteration, but require the aggregate target at
checkpoints.

## Feature test dimensions

Choose relevant dimensions from:

```text
basic valid
nested/parenthesized
inside function/block/loop/class/module
shadowed/captured binding
side-effect operand
combined transforms
malformed delimiter/grammar
unexpected EOF
diagnostic attribution
project/module/runtime form
```

Avoid meaningless Cartesian products, but never accept one top-level literal as
semantic proof.

## Runtime and evaluation order

Use counters, calls, getters, computed properties, update expressions,
destructuring defaults, and short-circuit branches. Assert result and evaluation
count/order. Compiler output can parse and look plausible while evaluating twice
or observing the wrong binding.

## Scope

Test global, function, nested function, block, loop, catch, class, module,
destructured parameter, rest/default parameter, and closure contexts where the
feature applies. Import live-binding rewrites must respect local shadowing and
syntax-owned positions such as labels, property keys, and later import clauses.

## Parser/error behavior

Malformed input must not crash, hang, loop, consume arbitrary following
declarations, or emit nonsense silently. Timeouts may guard infinite-loop cases.
Test comments/newlines, adjacency, ambiguous prefixes, and EOF.

## Differential evidence

Use TypeScript as oracle only for intended compatibility. Separate no-check
syntax/emit behavior from semantic checking. Exact output spelling is useful for
transform stability but is not necessarily semantic equivalence.

A differential failure is evidence of a difference requiring classification, not
automatic evidence that tscc is wrong. Possible causes include a tscc regression,
reference-TypeScript change, stale fixture, option mismatch, harness defect, or an
intentional documented divergence. Classify first, then fix the responsible layer.
Maintain one explicit canonical TypeScript oracle for release evidence; upgrade it
as a recorded checkpoint that reruns and classifies the complete suite. Additional
versions may be informative but must not make the canonical result depend on the
ambient `PATH`.

## Suite synchronization

The local and standalone corpora currently match byte-for-byte for README, cases,
and runner; only repository-specific notes differ. Establish canonical ownership
and an automated comparison.
Do not keep two manually diverging “canonical” suites.

## Environment and determinism

Control working/temp directories, module paths, compiler options, environment
colors, Node/TypeScript versions, and timeouts. Generated outputs in fixtures
should be classified as expected evidence or regenerated artifacts, not silently
trusted.

## Native safety and performance

Run ASan/UBSan for parser/range/string/AST changes. Fuzzing/generation should seek
crashes, hangs, recovery failures, and differential semantic mismatches, then
minimize findings. Benchmarks run only after semantic gates and record workload,
versions, machine, cold/warm conditions, and comparable work.
