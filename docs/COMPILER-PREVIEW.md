# tscc compiler preview contract plan

The compiler preview is a bounded, useful TypeScript compiler milestone. It does
not mean drop-in `tsc` compatibility. It means tscc independently parses, binds,
checks and emits a versioned subset with deterministic project, diagnostic and
output behavior, while rejecting or documenting everything outside it.

## Contract to freeze

### Compilation behavior

- One `ProgramGraph` owns source identity, imports, semantic facts, diagnostics
  and output decisions for single-file and multi-file compilation.
- Production compilation never invokes `tsc`, Node or JS++; those are independent
  test oracles/runtimes only.
- The preview pins target, module and JSX modes; relative module discovery;
  `rootDir`/`outDir`; `noEmit`/`noEmitOnError`; JSONC project configuration; exit
  status; deterministic output paths; and unsupported-option failure.

TCP2 pins ES2022 as the only preview target, preserve/esnext/commonjs module
modes, JSX preserve, sorted project roots, deterministic repeated output and
visible rejection of unknown `compilerOptions`. Default mode emits successfully
prepared siblings while returning failure; `noEmitOnError` commits none.

### Checked language subset

The first contract should promote only behavior already backed end to end:
primitive/literal/union expressions, assignments and bounded narrowing;
canonical callable variables and contextual callbacks; structural object types,
methods, inheritance, index and call signatures; and bounded arrays and tuples.
Every promoted family needs positive runtime output and negative diagnostic
cases. Generics, overloads, class semantics, declaration libraries, broad
control-flow analysis and cross-module type propagation remain excluded until
separately earned.

Runtime-bearing enum, namespace and parameter-property transforms, TS erasure,
TSX preserve and current ESM/CommonJS behavior belong to a separate emitted-code
matrix. A syntax feature being emitted does not imply it is semantically checked.

### Diagnostics, output and operation

Supported errors carry stable file/line/column attribution, stable `TSCC` codes
and deterministic ordering. CLI errors use `TSCC000x`, ordinary compiler errors
currently use `TSCC1000`, and warnings use `TSCC2000`. Valid projects produce deterministic
JavaScript; invalid projects obey the documented output policy. The preview
ships a reproducible native binary with documented platforms and a versioned
feature matrix, and passes clean-build, sanitizer, malformed-input,
representative-project and performance-regression gates.

## Executable exit demonstration

A frozen multi-file project exercises relative modules, callable variables,
contextual callbacks, structural objects and arrays/tuples. tscc compiles it
without Node or `tsc`; its JavaScript has the same observable result in the
declared reference runtime and supported JS++ intersection. A sibling invalid
project fails with pinned diagnostics under both emit policies. A clean
environment reproduces the build and artifacts.

## Definition checkpoints

1. **TCP0 - contract inventory:** turn matrices into implemented, preview,
   deferred and excluded sets; freeze CLI, project and demonstration inputs.
2. **TCP1 - diagnostic contract:** stabilize locations, categories, ordering,
   exit codes and unsupported-input behavior for the promoted subset.
3. **TCP2 - project/output contract:** pin resolution, configuration, module
   modes, deterministic paths/content and emit-on-error semantics.
4. **TCP3 - semantic closure:** close only gaps exposed by the frozen preview
   corpus; do not expand into unrelated TypeScript families.
5. **TCP4 - runtime intersection:** execute the emitted corpus in the declared
   reference runtime and JS++, recording intentional differences.
6. **TCP5 - preview candidate:** representative projects, sanitizer/fuzz gates,
   performance guardrails, reproducible packages and exact public limitations.

TCP0 is frozen. Its main output is a reviewable contract manifest, not another
feature. Later checkpoints must point to a failed preview acceptance item or
remain outside this milestone.

## Reassessment after TCP0-TCP2

Project/output mechanics are no longer the primary preview blocker. TCP3 should
close semantic gaps exposed by the frozen projects, led by cross-module exported
and imported type identity; imported values currently compile conservatively as
`unknown`, so the positive project executes but is not fully checked across its
module boundary. TCP4 then freezes a deliberately module-free emitted-JavaScript
intersection for JS++ rather than making ECMAScript/CommonJS module loading a
runtime dependency. TCP5 is candidate evidence, not open-ended feature growth.
