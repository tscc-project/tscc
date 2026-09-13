# tscc project context

## Identity and ambition

tscc is an independent native TypeScript-to-JavaScript compiler project. It is
related to Nift and Minify++ by history and engineering values, not by required
architecture. Do not turn it into “Nift's compiler” or force the three projects
into a product suite.

Historically tscc was the longest-term/most ambitious project because compiler
semantics interact across many stages. The live repository has advanced through
0.15.0 and substantial parser/transpiler/module checkpoints; maturity must be
assessed from current evidence rather than the old label.

## Compatibility contract

The current compiler has a substantial but deliberately bounded semantic checker,
not a complete TypeScript type system. It now owns real compiler foundations for
scopes/symbols, cross-file program graphs, generics/inference, overloads, classes,
CFG-based narrowing, richer collections, advanced type operators, selected JSX
contracts and runtime-bearing TypeScript facts, while retaining the fast native
emitter and project/module pipeline.

Do not infer complete TypeScript/`tsc` replacement compatibility. Before
production positioning, define target TypeScript version/range, ECMAScript
targets, module modes, project resolution, type-analysis scope, diagnostic
compatibility, and explicit unsupported behavior.

`tsc` is an important oracle where compatibility is intended, but not every
surprising TypeScript behavior must be copied blindly. Distinguish intentional
compatibility, deliberate divergence, unsupported semantics, and tscc defects.

## Complete semantic slices

A feature can involve:

```text
lex → parse → own syntax/ranges → bind/scope → lower → emit → execute → diagnose
```

Not every feature uses every stage, but production claims must reflect the stages
that matter. Prefer one complete slice with runtime/negative evidence over twenty
new parser productions.

## High-risk compiler families

- Parser ambiguity, unexpected EOF, delimiter recovery, comments/newlines, and
  consuming following declarations.
- Scope/binding across globals, functions, blocks, loops, classes, modules,
  catches, destructuring, shadowing, and closures.
- Lowering that duplicates/reorders/skips side effects or changes short-circuiting.
- Temporary-variable hygiene and nested transform ownership.
- Module imports/exports/re-exports, live bindings, cycles, aliases, resolution,
  type-only forms, and CommonJS transformation.
- TSX versus generics/assertions/angle syntax.
- Diagnostics, source locations, controlled failure, and no hangs.
- Project/config behavior and output path/layout.

## Current evidence

The current pause baseline is CP75 (`e4c45dc`, 14 September 2026). The external
compatibility corpus remains the frozen 579-case preview wall at **555 pass, 0
fail, 24 explicit semantic skips** against pinned TypeScript 7.0.2 and Node
22.22.1. It is still an important regression gate, but it now under-represents
post-preview semantic breadth because CP43-CP75 added many capabilities through
focused compiler tests without growing the external corpus.

Compiler-owned evidence now also covers canonical module/export/declaration
facts, package/declaration resolution, selected JSONC configuration, hermetic
libraries, generic substitution/inference, overload resolution, class semantics,
complete selected expression ownership, CFG/narrowing/definite assignment,
richer collection/advanced types, bounded JSX semantics, local runtime-bearing
TypeScript facts, line source maps, declaration maps and incremental output
retention. CP71-73 add deterministic mutation/generated-project hardening,
ASan/UBSan, RSS/lifetime workloads and strict real-binary Valgrind runs across the
newer compiler paths. CP74-75 close the correctness defects uncovered by that
validation.

This means tscc is no longer accurately described as “a transpiler with a small
checker.” It is a real compiler preview with incomplete compatibility. The next
risk is not whether the architecture can support a checker; it is whether the
long TypeScript compatibility tail can be expanded without losing correctness,
speed or truthful support boundaries.

## Performance philosophy

Native startup and transpilation speed are important, but semantics lead.
Separate startup, lex/parse, transform, emit, whole project, many-file, module,
TSX, CommonJS, and RSS dimensions where instrumentation justifies it. Compare
`tsc --noCheck` separately from full `tsc` and record versions/configuration.

## Production-ready meaning

Production readiness need not mean 100% TypeScript if the promised subset is
explicit and useful. It means the public promise equals tested implementation:
complete semantic slices, reliable real projects, controlled failures, usable
diagnostics, stable performance, and repeatable validation/release processes.

The most important product question remains:

> What exact compiler scope is tscc promising at production?
