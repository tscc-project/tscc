# tscc development handover

This is the entry point for developers and coding agents working on tscc. Read
`README.md` for the evolving public checkpoint; read this file and
`docs/handover/` before changing compiler behavior.

## Authority and current identity

- Product/executable: `tscc`.
- Current CLI version: `0.15.0`.
- Current positioning: experimental standalone TypeScript-to-JavaScript compiler
  written in C++17.
- Current default target: `es2022`; default module mode: `preserve`.
- Build: Make.
- There is no semantic TypeScript type checker at this checkpoint.

Source and tests define current behavior. The internal and standalone regression
corpora define executable compatibility evidence. README/website claims must stay
within that evidence. Git and benchmark results record historical checkpoints.
These handovers preserve architecture, development practice, decisions, and the
living production-risk model.

## The central correctness rule

```text
parser accepts syntax
    ≠ feature is supported
```

Support can require lexing, parsing, AST/replacement ownership, binding/scope,
lowering, module behavior, emission, runtime semantics, error behavior, and
diagnostics. Implement narrow complete semantic slices rather than accumulating
grammar productions without end-to-end meaning.

## Current architecture map

- `Source`: source text and offset→line/column ownership.
- `Diagnostic`: attributed diagnostics and presentation.
- `Lexer`: tokens.
- `Syntax`/`Parser`: a deliberately compact `Program` representation and
  parser-owned ranges/replacements for TypeScript-only/runtime-bearing syntax.
- `Transpiler`: parser/token transforms and JavaScript emission.
- `Project`/`ModuleGraph`: source loading, relative dependency discovery, and
  graph/project work.
- `Config`: current `tsconfig.json` subset.
- `Compiler`: compiler options and multi-file/output orchestration.
- `main.cpp`: CLI.

Read `docs/handover/ARCHITECTURE.md` and current headers/implementation before
assuming pass boundaries. Some stages intentionally cooperate rather than forming
a full traditional typed compiler pipeline.

## Current capability shape

The README records checkpoints through 0.15.0, including broad type-syntax
erasure, generics, enums, namespaces, parameter properties, project/relative
module discovery, TSX preserve mode, CommonJS lowering with live imported reads,
module grammar hardening, scope/shadowing work, and import attributes.

The independent corpus currently contains 511 cases: 483 pass, zero fail, and 28
semantic-checker-only skips at the retained checkpoint. Counts are evidence, not
the product definition. There is still no semantic checker and the project is not
implicitly a drop-in replacement for all `tsc` behavior.

## Build and validation

```bash
make
make test
make test-parser
make test-runtime
make test-project
make test-regression
make test-tsx
make test-commonjs
```

Inspect the current Makefile because `make test` currently runs only the smoke
target; the additional targets must be invoked deliberately for a complete
checkpoint. Benchmarks live in `benchmarks/`; their README and JSON files retain
local checkpoint evidence. The Makefile has no named sanitizer target.

The regression runner requires the candidate `tscc`, Node, and TypeScript `tsc`.
It uses `tsc --noCheck` as syntax/emission reference and normal `tsc` to classify
semantic-only cases. Reference versions/environment affect reproducibility and
must be recorded for serious checkpoints.

## Development standard

```text
define exact compatibility/semantic slice
→ ask reference behavior where intended
→ add external regression and focused internal test
→ implement through every required stage
→ inspect emitted JS
→ execute it
→ test side effects, scope, malformed neighbors
→ full regression and integrations
→ sanitizers/performance if relevant
→ website/support/handover/roadmap reconciliation
```

Transforms must preserve evaluation count/order, short-circuiting, and binding.
Use counters, calls, getters, computed properties, shadowing, and nested contexts
rather than literals alone.

## Regression repositories

This repo contains `regression/`; a standalone `tscc-regression-suite` mirrors the
corpus/runner. Current comparison shows the substantive corpus files match while
standalone `REGRESSION_NOTES.md` includes newer checkpoint notes. The canonical
sync direction is not yet sufficiently encoded. Do not guess or allow drift:
establish and automate ownership as a dedicated checkpoint.

## Unknown artifacts

Historical benchmark JSON, hidden probes such as `.shadow-debug`, generated
project outputs, and temporary-looking files must be classified through Git,
scripts, references, and intent before cleanup. Do not delete unfamiliar evidence
merely because its name looks temporary.

## Checkpoints and public actions

A compiler checkpoint is an evidence-backed baseline, not a commit, tag, release,
website claim, or version bump by implication. Report exact compatibility scope,
tests/runtime/differential results, reference versions, safety/performance
evidence, docs/site changes, repository residue, and known unsupported behavior.

Do not commit, push, tag, release, deploy, or make destructive repository changes
without explicit approval.

## Deeper handovers

- `PROJECT-CONTEXT.md`: identity, compatibility, and compiler-risk model.
- `ARCHITECTURE.md`: compiler pipeline, ownership and semantic invariants,
  production hazards, and source-reconciliation exercise.
- `DEVELOPMENT.md`: vertical-slice/checkpoint workflow.
- `TESTING.md`: runtime, differential, scope, and malformed-input evidence.
- `DECISIONS.md`: current scope and rejected assumptions.
- `ROADMAP.md`: living production-readiness risk assessment.
- `PROJECT-HISTORY.md`: detailed tscc history and institutional context,
  including compiler semantics, production definition, and roadmap evolution.

## Maintaining this handover

These documents are living infrastructure. Review them whenever architecture,
language scope, compatibility, tests/corpus, build/release workflow, website
claims, or durable compiler lessons change. Consolidate rather than append a
diary. Every substantial checkpoint must review handover and roadmap impact.
