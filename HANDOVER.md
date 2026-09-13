# tscc development handover

This is the entry point for developers and coding agents working on tscc. Read
`README.md` for the evolving public checkpoint; read this file and
`docs/handover/` before changing compiler behavior.

## Authority and current identity

- Product/executable: `tscc`.
- Current CLI version: `0.15.0`.
- CP53 uses vendored Jsonic++ commit `88e4736` for typed JSONC configuration;
  regex extraction must not return.
- Current positioning: experimental standalone TypeScript-to-JavaScript compiler
  written in C++17.
- Current default target: `es2022`; default module mode: `preserve`.
- Build: Make.
- A deliberately bounded semantic checker slice is active: durable primitive facts
  flow through expressions, assignment and ordinary-function signatures/calls,
  with const, argument, arity and annotated-return checks. This is not yet a
  general TypeScript type checker.

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
- `Semantic`/`Binder`/`Type`/`Checker`: bounded structural spans, lexical symbol
  identity (including static value imports), durable primitive types, and the
  currently protected semantic slice.
- `Transpiler`: parser/token transforms and JavaScript emission.
- `Project`/`ProgramGraph`: canonical roots/files, relative dependency discovery,
  dependency edges, durable compilation-unit ownership, and
  graph/project work.
- `Config`: current `tsconfig.json` subset.
- `Compiler`: file-local preparation, program-wide emit policy, and staged output
  orchestration. Default emit-on-error and opt-in `noEmitOnError` are intentional.
- `main.cpp`: CLI.

Read `docs/handover/ARCHITECTURE.md` and current headers/implementation before
assuming pass boundaries. Some stages intentionally cooperate rather than forming
a full traditional typed compiler pipeline.

## Current capability shape

The README records checkpoints through 0.15.0, including broad type-syntax
erasure, generics, enums, namespaces, parameter properties, project/relative
module discovery, TSX preserve mode, CommonJS lowering with live imported reads,
module grammar hardening, scope/shadowing work, and import attributes.

The independent corpus currently contains 579 cases: 555 pass, zero fail, and 24
semantic-checker-only skips at the retained checkpoint. Counts are evidence, not
the product definition. The checked subset now includes bounded callable,
structural object, index/call signature, array, tuple and cross-module named-import
families; the project is not implicitly a drop-in replacement for `tsc`.

CP14/TC7A adds canonical literal, union, `null` and `undefined` identities,
normalized union ownership and bounded assignability. Narrowing and flow joins
remain CP17 work.

CP17/TC7B adds branch-local facts for strict `typeof` and literal/nullish
equality guards. Facts apply only inside the guarded brace region and restore on
exit. Else complements, mutation invalidation, joins, loops and definite
assignment remain future flow-graph work.

CP19/TC8A establishes canonical structural object shapes in the durable type
store. Properties retain name, type, optionality and readonly metadata and can
be looked up by name. This is infrastructure only; source-facing object checks
belong to CP20.

CP20/TC8B activates a bounded structural slice: flat object type annotations,
object-literal inference in variable initializers, named property reads,
required/optional structural assignment, and readonly property-write errors.
Nested/index/call properties, excess-property freshness, interfaces/classes and
general contextual object typing remain future work.

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

`make test` is the complete aggregate correctness gate and currently runs the
smoke, parser, binder, type-model, source-edit, checker, runtime, project,
independent regression, TSX, and CommonJS targets. Focused targets remain useful
for iteration. `make test-sanitize` builds and smoke-runs the ASan/UBSan candidate;
the retained memory-safety checkpoint targets provide the deeper lifetime,
project-pressure, RSS, and Valgrind evidence. Benchmarks live in `benchmarks/`.

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

The sibling standalone `tscc-regression-suite` is the canonical external contract
owner. This repo contains a release/convenience mirror in `regression/`.
`make check-regression-sync` compares the executable corpus and runner; update the
standalone suite first and synchronize the mirror in the same checkpoint. Notes
and handovers remain repository-owned and are not required to be byte-identical.

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
- `COMPILER-JS-ROADMAP.md`: coordinated checkpoint plan for developing tscc as
  a compiler, building JS++, and integrating them through explicit boundaries.
- `PRODUCT-BOUNDARY.md`: settled ownership and dependency contract between tscc
  and JS++, protected by `make test-product-boundary`.
- `../feature-matrix.json`: machine-checked support dimensions and evidence cases,
  protected by `make test-feature-matrix`.
- `PROJECT-HISTORY.md`: detailed tscc history and institutional context,
  including compiler semantics, production definition, and roadmap evolution.

## Maintaining this handover

These documents are living infrastructure. Review them whenever architecture,
language scope, compatibility, tests/corpus, build/release workflow, website
claims, or durable compiler lessons change. Consolidate rather than append a
diary. Every substantial checkpoint must review handover and roadmap impact.

## Coordinated tscc and JS++ direction (2026-08-29)

The next development campaign is defined in
`docs/handover/COMPILER-JS-ROADMAP.md`. Its central boundary is settled unless
new evidence justifies reopening it:

- tscc owns TypeScript syntax, binding, types, diagnostics, project/module
  compilation, lowering, and JavaScript emission;
- JS++ owns executable ECMAScript parsing, bytecode, runtime semantics, values,
  objects, environments, garbage collection, exceptions, modules, and embedding;
- normal tscc compilation does not execute compiler input through JS++;
- the first integration is test-only: eligible emitted JavaScript is parsed and
  executed in both Node and JS++ as JS++ gains the necessary coverage;
- no existing tscc component migrates merely because it is JavaScript-facing.
  Sharing is considered only after both implementations exist and a checkpoint
  proves a stable common contract.

CP1 froze this relationship as an executable repository invariant. Normal tscc
targets must not include `js.h`, link `libjs`, or compile JS++ private sources.
Future test-only or optional integration must use an explicit target and a public
JS++ surface rather than silently entering the compiler core.

CP9/INT0 implements that first test-only adapter. Run
`make test-js-interop JS_RUNTIME=/path/to/js` to compile six eligible TypeScript
fixtures, then compare emitted completion values under Node, JS++ and explicit
fixture expectations. It is deliberately absent from `make test` and
`make test-core`; normal compiler development still requires no JS++ artifact.

CP10 expands binder-owned identity across arrow scopes, identifier defaults/rest,
flat destructuring and class names while retaining explicit limits for nested
patterns, member tables and merging. Do not reintroduce emitter guesses for
binding families that remain outside this contract.

## Memory-safety Checkpoint 5A (2026-08-18)

- Maintained compiler lifetime corpus added at `tests/memory_lifetime.cpp`, with Make targets for sanitizer, native RSS/project pressure and independent Valgrind confirmation.
- At commit `a05d3d8`, 80 ASan/LSan/UBSan in-process lifetime iterations passed with no finding across parser/AST, semantic, binder, type, checker, emitter, CommonJS, TSX, malformed input and success-after-failure paths.
- Native lifetime evidence used 5 warm-ups plus 40 measured × 20-iteration runs; RSS samples stayed within 4,908–5,108 KiB (5,108 KiB peak) rather than increasing monotonically.
- Project/module pressure passed 8 sanitizer rounds and 20 native rounds over a 120-file graph with a cycle, missing-module failure and immediate repaired compile.
- Full `make test` remains green (independent corpus 492 pass / 0 fail / 27 intentional semantic skips) and `make check-regression-sync` passes.
- Checkpoint 5B remains external Valgrind confirmation: run `make valgrind-memory-safety-checkpoint-5` on Linux with Valgrind and return `.build/memory-safety/checkpoint-5-valgrind.json`.

## Memory-safety Checkpoint 5 complete (2026-08-18)

- Checkpoint 5B passed independently on Linux 7.0.0-29 x86-64 at tscc commit `d96419e`.
- Valgrind 3.26.0 completed 40 maintained compiler-lifetime iterations with 0 errors, 0 bytes in use at exit, and 25,003 allocations matched by 25,003 frees; peak Valgrind process RSS was 166,512 KiB.
- Exact evidence is retained at `docs/evidence/memory-safety/checkpoint-5-valgrind.json`.
- Combined with Checkpoint 5A's sanitizer lifetime corpus, settled native RSS, repeated 120-file graph/cycle/failure/recovery workloads, full regression pass and mirror sync, the tscc compiler-lifetime baseline is complete.
- The cross-project campaign now advances to Checkpoint 6; do not treat this baseline as a claim of complete TypeScript semantic compatibility.
## CP22 / TC8C nested structural objects (2026-08-30)

Nested object annotations and literals, chained property reads and path-specific
missing/incompatible-property diagnostics are active. Structural assignment
intentionally permits extra source properties; freshness/excess-property errors
remain deferred. The external corpus is 529 cases: 505 pass, 0 fail, 24 skips.
## CP24 durable expression identity (2026-08-30)

Every expression range requested by checking is now interned once into the
CompilationUnit-owned `ExpressionModel`, with a stable ID, token span and
canonical significant-token sequence retained through emission. The typer
consumes that owned representation rather than rescanning raw ranges. This is
the migration seam for richer operator/property nodes; it is not yet a complete
TypeScript expression AST.

## CP26 structured expression nodes (2026-08-30)

The compilation-unit expression model retains explicit literal, identifier,
parenthesized, unary, binary, call, property, object-literal and assignment
nodes with stable child identities and operator tokens. The bounded external
contract remains 505/0/24; unknown grammar is retained as an unknown node.

## CP28 reusable object declarations (2026-08-30)

Basic object aliases and interfaces now resolve to canonical shapes. Nested named
properties, optional/readonly metadata and compatible interface merging feed the
existing structural checks. Indexed/call signatures, inheritance, generics,
classes and conflicting merges remain outside this slice. The corpus is 533 cases:
509 pass, 0 fail and 24 skips.

## CP30 expression-typing migration (2026-08-30)

The checker now walks durable expression child IDs directly for literals,
identifiers, parentheses, unary/binary operators, calls, property access, object
literals and assignments. The compatibility significant-token view has been
removed from `ExpressionNode`; precedence parsing exists only in the expression
builder. This is an ownership change and retains the 509/0/24 contract.

## CP31 canonical callable types (2026-08-30)

Function types are now canonical `TypeStore` values carrying parameter, result,
required-arity and rest metadata. Basic callable aliases and variable annotations
resolve to those identities, and function declarations publish the same shape.
Assignability uses contravariant parameters and covariant results for this bounded
slice. Calls through variables and contextual function-expression typing remain
CP32 work; the external contract remains 509/0/24.

## CP32 callable variables and contextual functions (2026-08-30)

Annotated callable variables now participate in argument and arity checking, and
their expected type contextually types simple arrow and function expressions,
including nested expression-bodied arrows. Direct block `return` expressions are
checked against the contextual result. Destructured declaration parameters retain
bounded unknown parameter slots rather than producing false zero-arity errors.
The external corpus is 539 cases: 515 pass, 0 fail and 24 deliberate skips.
Calls in arbitrary standalone expression statements and full contextual inference,
overloads and generics remain outside this slice.

## CP33 whole-program callable checking (2026-08-30)

Callable references are now discovered and checked in every source context, not
only variable initializers and ordinary function declarations. Standalone,
branch-nested and throw-expression calls use the retained call-expression node
for argument and arity typing. Broader non-call operator checking in every
statement form remains incremental work. The corpus is 543 cases at 519/0/24.

## CP34 callable inference and parameter forms (2026-08-30)

Unannotated variables initialized by arrows or function expressions now acquire
canonical callable types when parameter annotations and body results provide a
bounded inference result. Calls use those inferred identities. Contextual
callables accept optional/default/rest parameter forms and nested callable
results. Typed function-expression parameters are erased during emission.
The corpus is 549 cases at 525/0/24. Inline callback arrows nested directly in
call arguments remain blocked on nested-arrow expression grammar; overloads,
generics and broad return-flow inference remain deferred.

## CP35 nested contextual callbacks (2026-08-29)

Call arguments now own bounded recursive expression subtrees, so inline arrow
and function-expression callbacks receive the callable parameter context at the
actual argument site. Named callable annotations on ordinary function parameters
resolve through the canonical type store, and checker-owned initializer/return
ranges suppress duplicate whole-program call diagnostics. The corpus is 553
cases at 529/0/24. This is intentionally a call-argument slice; general nested
expression ownership in every parenthesized, array and object context remains
future architecture work.

## CP36 callable object members and interface inheritance (2026-08-29)

Object annotations now canonicalize method signatures as the same callable type
used by function-valued properties. Property-call nodes therefore share argument,
arity and contextual result checking with ordinary calls. Interfaces may extend
already-declared object interfaces and merge inherited required properties. The
corpus is 559 cases at 535/0/24. Call signatures, index signatures, generic
methods, forward inheritance and classes remain deferred.

## CP37 nested expression ownership (2026-08-29)

Parenthesized, object-property and array-element expressions now build bounded
recursive subtrees instead of sharing the outer builder cursor. Context flows
through parentheses and structural object properties, enabling contextual
callbacks in both positions. The corpus is 563 cases at 539/0/24. Statement
call-root discovery is still retained for constructs without durable statement
expression children; replacing that final discovery layer remains future work.

## CP38 index and callable object signatures (2026-08-29)

Structural object identities now retain bounded string/number index signatures
and one call signature. String-indexed properties participate in lookup and
object-literal assignability; callable objects reuse canonical function
variance, contextual typing, argument, arity and result checks. The corpus is
569 cases at 545/0/24. Computed element-access typing, multiple overload
signatures, mapped types and generic signatures remain deferred.

## CP39 annotation parser decomposition (2026-08-30)

Type annotation grammar now lives behind `TypeParser.h` in a dedicated recursive
parser implementation. Parameter lists, function types, object members, methods,
index signatures, call signatures and unions are parsed through small productions
instead of the former monolithic routine. Existing 569-case evidence remains
545/0/24; this architecture checkpoint intentionally adds no support claim.

## CP40 computed element-access typing (2026-08-30)

Computed property nodes now retain both receiver and key expression children.
String literals prefer exact declared properties; dynamic string and number keys
select CP38's canonical index identities, with number keys falling back to a
string index. Four cases raise the corpus to 573 and 549/0/24. General array,
tuple, symbol-key and indexed-write checking remain deferred.

## CP41 durable statement expression ownership (2026-08-30)

Standalone, branch/loop-header and throw call roots are retained as semantic
nodes with durable token ranges. The checker consumes those nodes alongside
initializer and return owners; its binding-reference discovery scan is gone.
Program-graph evidence pins retained roots and the 573-case corpus remains
549/0/24. Full statement AST children beyond callable roots remain incremental.

## CP42 canonical array and tuple types (2026-08-30)

`T[]` and tuple annotations now receive canonical structural identities rather
than collapsing to a primitive. Contextual array literals retain element types;
tuples retain ordered positional types, exact numeric reads and literal length.
Six cases raise the corpus to 579 and 555/0/24. Readonly arrays, optional/rest
tuple elements, array methods and indexed-write checking remain deferred.

## Compiler preview contract gate (2026-08-30)

`docs/COMPILER-PREVIEW.md` defines a bounded compiler finish line. Proceed through
TCP0 inventory, TCP1 diagnostics, TCP2 project/output behavior, TCP3 semantic
closure, TCP4 runtime intersection and TCP5 candidate evidence. The frozen
multi-file positive/negative demonstration decides readiness; this is not a
drop-in `tsc` compatibility claim.

## TCP0 contract inventory (2026-08-30)

`docs/compiler-preview-contract.json` freezes the CLI, checked subset,
emitted-only families, exclusions and candidate gates. The positive and negative
multi-file projects live under `examples/compiler-preview/`. Production remains
free of Node, `tsc` and JS++ dependencies.

## TCP1 diagnostic contract (2026-08-30)

Non-pretty diagnostics are deterministically ordered by file/location/severity/
code/message and carry stable `TSCC` codes. CLI contract errors use TSCC0001-3,
ordinary compiler errors use TSCC1000 and warnings use TSCC2000. Missing option
values fail explicitly. `tests/diagnostic_contract.sh` pins the surface.

## TCP2 project and output contract (2026-08-30)

The preview target is exactly ES2022; module modes are preserve/esnext/commonjs
and JSX is preserve-only. Project roots are sorted, unknown compiler options fail
with TSCC3001, and repeated builds are byte-identical. Default error policy emits
prepared valid siblings; `noEmitOnError` commits none. The focused gate executes
the frozen positive project and pins both policies.

## Post-TCP2 order decision (2026-08-30)

After JS++ EP3, do TCP3 semantic closure with cross-module type propagation as
the leading failed acceptance item. Follow JS++ EP4 with TCP4's module-free
runtime intersection, then run EP5 and TCP5 candidate gates. Do not broaden into
classes/generics/overloads unless the frozen preview corpus demonstrates a need.

## TCP3 cross-module semantic closure (2026-08-30)

`ProgramGraph` now imports exported named value types after every unit is owned
and checked. Structural object, array, tuple, union, literal and callable types
are cloned into the importing unit's `TypeStore`, so no unit-local `TypeId`
crosses the file boundary. A graph-aware checker pass now accepts the frozen
positive project and rejects its cross-module callable/object mismatch. This
slice covers relative named value imports only; default/namespace/re-export type
surfaces remain deferred because the preview does not require them.

## TCP4 module-free JS++ intersection (2026-08-30)

`examples/compiler-preview/jspp-intersection.ts` freezes the runtime boundary:
typed interface/tuple use, an ordinary typed function, an object/array literal
and numeric completion, with no modules or host APIs. `make
test-preview-interop JS_RUNTIME=...` compiles it through production TSCC and
requires both Node and JS++ to return `42`. The independent intersection is now
7/7. This is emitted-subset evidence, not a JS++ module-loading claim.

## TCP5 compiler preview candidate (2026-08-30)

The bounded Linux compiler preview is qualified at 555/0/24 plus the 7/7
Node/JS++ intersection. The candidate gate covers the frozen positive and
cross-module negative projects, a representative existing project, deterministic
diagnostics/output and emit policies, ASan/UBSan, 400 deterministic malformed
mutations, a loose 100-process startup guardrail and byte-identical preview
archives. The archive installs the binary, license, contract, preview docs and
frozen examples. LeakSanitizer/Valgrind remains external under the ptrace-managed
runner. Exact evidence and limitations are in
`docs/evidence/compiler-preview-candidate.json`.

## Post-preview reassessment (2026-08-30, historical)

This assessment set the evidence-first order: PC0 external lifetime/platform
work, TCP6 real-project trials, EP6 selected conformance, then a TCP7 decision.
PC0V was later completed at TSCC `1bc3047` and JS++ `d35999f`; PC0P remains a
parallel JS++ platform gate. TCP6A is now the active TSCC step. Default/namespace/
re-export type propagation and practical package resolution remain candidates,
not commitments, until project evidence ranks them. Do not begin broad class,
generic or package-resolution work merely because PC0V passed.

## Current game-plan handoff (2026-08-30)

The original preview campaigns are complete rather than open-ended:

- TSCC TCP0-TCP5 is qualified at 555 pass, 0 fail and 24 intentional
  semantic-only skips, with a 7/7 Node/JS++ emitted-runtime intersection.
- JS++ EP0-EP5 is qualified at 171/171 with ABI 0.1 packaging, the frozen C
  embedding demonstration, containment, recovery, ASan/UBSan, deterministic
  fuzzing and soak evidence.
- TSCC remains a bounded TypeScript compiler preview, not a drop-in `tsc`
  replacement. JS++ remains a bounded synchronous embedded preview, not a
  general ECMAScript implementation.
- Ordinary TSCC compilation still has no JS++ production dependency. JS++ is an
  independent engine and test runtime at this stage.

PC0 is now split so unavailable tooling does not turn into a false pass or block
all useful work:

1. **PC0V - external native-memory confirmation:** complete at TSCC `1bc3047`
   and JS++ `d35999f` (2026-09-13). TSCC's retained Valgrind lifetime gate and
   the JS++ frozen host, lifecycle, VM and heap gates all pass under Valgrind
   3.26.0 with zero errors and zero bytes in use at exit. The JS++ run exposed
   a standalone-execution lifetime defect that was corrected after ownership
   review (JS++ `d35999f`); discovery and post-fix evidence are retained in both
   repositories. See the PC0V section below. Do not weaken suppressions or
   classify environment failures as product passes.
2. **PC0P - second-platform packaging:** TSCC already has Ubuntu/macOS/Windows CI
   and multi-platform release workflows. JS++ still needs CI/package coverage
   beyond the locally qualified Linux ABI. Adding or running remote workflows is
   a separate authorized task; no second-platform JS++ result is currently
   claimed.

After the current commits are pushed, the next development sequence is:

1. **TCP6A:** define a curated small-project trial manifest and reproducible
   runner. Projects must be real, pinned and license-compatible. Classify failures
   by parser, binder, checker, resolution, configuration, emit and runtime rather
   than immediately fixing every incompatibility.
2. **TCP6A baseline:** run the unmodified compiler, retain machine-readable
   results and reduce the leading failures into independent regression cases.
3. **TCP7 decision gate:** current candidates are default/namespace/re-export
   type propagation and practical package resolution. Select one only when the
   trial corpus demonstrates it is the leading compiler blocker.
4. **Bounded compiler expansion:** follow the dependency-aware CP43-CP72 program
   in `docs/handover/ROADMAP.md`, reassessing after each evidence gate.

PC0P second-platform packaging and EP6A selected-Test262 remain independent JS++
campaigns. They do not block TSCC real-project trials. JS++ remains a test-only
runtime for the selected TSCC output intersection unless a later decision changes
that boundary explicitly.

Do not push, tag, release or broaden compatibility claims merely because the
preview candidates are qualified. Update this section when PC0V, PC0P, EP6A or
TCP6A produces evidence.

## CI aggregate-suite provisioning repair (2026-08-30)

The first post-preview push exposed an undeclared workspace assumption: Unix CI
checked out only TSCC, while `make test` expected
`../tscc-regression-suite/validate_preview_contract.py`. Windows passed its
self-contained `test-core`; Linux/macOS failed before compiler testing.

`TSCC_REGRESSION_SUITE_DIR` now owns the suite location, defaulting to the
existing sibling layout for local aggregate work. Unix CI checks out regression
suite commit `8a6ecd9a4f6ea9658cd7011286e575bd33c57d9f` into the workspace and
exports its absolute path. Keep the commit pinned and update it deliberately
whenever compiler/suite contracts advance together. Do not switch this checkout
to an unpinned moving branch or remove the aggregate gate merely to keep CI
green.

## CI aggregate topology repair (2026-08-30)

The first provisioning repair checked out the pinned suite, but nested it inside
the default TSCC checkout. The suite validator deliberately derives the compiler
repository as its sibling, so that layout produced `tscc/tscc/docs/...` and
failed before compiler tests with `NotADirectoryError`.

Unix CI now checks out TSCC at `${GITHUB_WORKSPACE}/tscc` and the pinned suite at
`${GITHUB_WORKSPACE}/tscc-regression-suite`, then runs build, test and sanitizer
steps with `working-directory: tscc`. This matches the aggregate workspace model
used locally. Windows retains its self-contained default checkout and
`test-core`. Keep the two Unix checkouts as siblings unless the suite validator
is explicitly redesigned around a passed compiler-repository path.

## Post-preview workspace hygiene (2026-09-09)

The tracked `benchmarks/.tmp100` JavaScript files were generated output retained
from the initial repository commit, not benchmark inputs or historical evidence.
No current benchmark, test, documentation page or release path reads them; active
benchmark runners create disposable output under `benchmarks/.tmp`,
`benchmarks/.cjs-tmp` and `benchmarks/.tsx-tmp`. The obsolete files are removed
and `benchmarks/.tmp*/` is ignored so later generated output cannot re-enter the
repository accidentally.

The coordinated workspace contains Nift source repositories on `stage` and
generated deployment repositories at their `public/main` gitlinks. At this
checkpoint the TSCC website source and generated repositories are aligned with
their recorded remotes. The JS++ generated site is aligned with `origin/main`,
while outer `js-website/stage` intentionally retains local commit `cd1aa6c`
pending an explicit push; do not mistake that known local commit for an
unclassified working-tree change.

## Post-preview evidence-tool repair (2026-09-09)

`make test-core` now uses `test-smoke-core`, which retains TSCC compilation,
Node syntax/runtime, version, help and diagnostic smoke coverage while explicitly
skipping only the TypeScript reference invocation. `make test` continues to use
the strict `test-smoke` target and therefore still requires `tsc`; do not replace
aggregate differential evidence with the core-only path.

## Pinned oracle environment (2026-09-09)

The canonical sibling regression suite owns a locked Node 22.22.1 and TypeScript
7.0.2 environment. The aggregate `make test` gate prepends its versioned wrappers
to `PATH` and runs `test-oracles` before differential work. CI provisions the
pinned suite, uses Node 22.22.1 and installs its lockfile with `npm ci`; global or
ambient TypeScript is no longer accepted as release-quality evidence.

## Post-preview retained baseline (2026-09-09)

Checkpoint 5 reran the complete aggregate against pinned Node 22.22.1 and
TypeScript 7.0.2 at TSCC `c6f453d`, suite `8a6ecd9` and JS++ `38603fb`.
The result is 555 pass, zero fail and 24 deliberate semantic skips, plus the 7/7
test-only runtime intersection. Reproducible and installed packages, a freshly
built ASan/UBSan candidate, 400 deterministic mutations and representative
projects passed. A fresh sibling `git archive` export independently rebuilt and
passed the same aggregate, package and sanitizer gates. Exact evidence is retained
in `docs/evidence/post-preview-checkpoint-5.json`; PC0V is now complete (see
below) and PC0P remains pending.

## PC0V external native-memory confirmation (2026-09-13)

PC0V is complete at TSCC `1bc3047` and JS++ `d35999f`, on Linux 7.0.0-29 x86_64
with c++ 15.2.0 and Valgrind 3.26.0.

- The retained aggregate gate `make test` passed at `1bc3047` against pinned
  Node 22.22.1 and TypeScript 7.0.2: 555 pass, zero fail, 24 deliberate skips.
- `make valgrind-memory-safety-checkpoint-5` passed: exit 0, zero errors,
  `in use at exit: 0 bytes`, 46,083 allocations matched by 46,083 frees, "All
  heap blocks were freed -- no leaks are possible". Evidence is preserved as
  distinct post-preview PC0V output at
  `docs/evidence/memory-safety/post-preview-pc0v-valgrind.json`; the historical
  Checkpoint 5 evidence under that directory is unchanged.
- JS++ PC0V required correcting a standalone local-heap lifetime defect found
  during the run. The first candidate repair was superseded after ownership
  review by JS++ `d35999f`, which defines one coherent heap/global ownership
  model, rejects the invalid caller-global-without-caller-heap combination, and
  makes self-referential returned values lifetime-safe. All JS++ Valgrind
  workloads now pass; see the JS++ handover for that model and contract.
- The machine-readable summary is `docs/evidence/post-preview-pc0v.json`.
  PC0V is not a general TypeScript compatibility or broad memory-safety claim;
  PC0P second-platform packaging remains pending.

## CP58 ordered overload resolution (2026-09-13)

Function declarations sharing a name and lexical scope now expose one ordered
overload set. Calls independently instantiate and filter candidates, prefer the
most specific applicable signature with stable declaration-order tie-breaking,
and emit one controlled no-match diagnostic. When declarations precede a
function body, the implementation signature is not callable from the public
surface. Non-overloaded functions preserve the earlier detailed diagnostics.

The focused evidence is `docs/evidence/cp58-overload-resolution.md` and
`tests/overload_resolution.sh`. The full pinned aggregate remains 555/0/24.
Method and constructor overloads intentionally remain for CP59-CP60 because they
require canonical class symbols and instance/static type ownership first.

## CP59 canonical class types (2026-09-13)

Classes now have canonical semantic records and genuinely separate value/static
and instance types. Fields, methods, constructors, constructor overloads,
parameter properties and modifiers are retained; construction checks arguments,
and `this` resolves to the owning instance shape. See
`docs/evidence/cp59-class-types.md` and `tests/class_types.sh`. The full pinned
aggregate remains 555/0/24 and ASan/UBSan passes. Inheritance and enforcement of
the retained relationship/modifier facts begin only in CP60.
