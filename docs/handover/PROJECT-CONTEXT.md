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

The current README calls tscc experimental and explicitly states that it lacks a
semantic type checker. That makes its current strongest contract a fast native
transpilation/compiler subset with selected runtime-bearing TypeScript transforms,
project handling, TSX preservation, and CommonJS module behavior.

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

The project has focused C++/shell integration tests, runtime comparisons, project
and CommonJS differential fixtures, and a 519-case independent corpus classified
against TypeScript. The retained baseline is 492 pass, zero fail, and 27
intentional semantic-only skips under TypeScript 7.0.2. Historical performance JSON tracks many checkpoints and
workloads. Current website/README ratios are local transpile-oriented comparisons,
not equivalent-work claims because `tsc` performs a much broader job.

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
