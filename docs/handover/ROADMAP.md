# tscc production-readiness roadmap

## Destination and development principle

The long-term product goal is explicit: **tscc is intended to become a
production-ready TypeScript type-checking compiler**, not remain permanently a
syntax-erasing transpiler. That destination includes parsing, binding, module and
symbol resolution, type checking, diagnostics, JavaScript emission, project
compilation, and dependable real-world operation for a clearly versioned
TypeScript compatibility contract.

The destination does not justify pretending the missing middle already exists.
The current fast transpiler is a valuable, working foundation and each checkpoint
on the path must have an honest contract, preserve proven behavior, and add a
complete vertical slice with tests. Build the compiler in stages; do not attempt
to teleport from range-based erasure to complete TypeScript compatibility through
a single rewrite, a collection of parser special cases, or ad hoc semantic
rejections.

This is therefore a living staged roadmap, reviewed after every substantial
compiler checkpoint. Intermediate releases may accurately describe themselves as
transpiler-only or as supporting a deliberately bounded checking subset while the
architecture advances toward the full goal.

## Production-ready meaning

The chosen mission is eventually broad TypeScript compatibility with genuine
type checking. A production claim must name the supported TypeScript language and
configuration version/range, and that documented scope must work end-to-end with
strong semantic, runtime, negative, scope, module, and project evidence.
Unsupported behavior must be explicit; failures controlled; representative real
projects reliable; diagnostics and performance usable; and releases reproducible.

“Production-ready” can be reached progressively for explicitly bounded subsets,
but the final destination is not satisfied by `--noCheck`-equivalent transpilation
alone.

## Staged architectural path

1. **Harden the current transpiler.** Preserve its regression corpus, define
   supported syntax/module/project behavior, fix oracle reproducibility and output
   transaction policy, and stop silent corruption at unsupported boundaries.
2. **Establish a durable syntax foundation.** Introduce a lossless or sufficiently
   complete syntax representation with stable source spans and recovery, while
   retaining the proven range emitter as a compatibility path during migration.
3. **Introduce names and scopes.** Add interned names/symbol identity, declarations,
   lexical scopes, imports/exports, merging rules, and a binder with focused tests.
4. **Introduce a type model and checker incrementally.** Begin with primitive,
   literal, union, object/function and generic relationships; add complete vertical
   semantic slices rather than syntax-shaped diagnostic heuristics.
5. **Unify project and module semantics.** Replace parallel traversal machinery
   with one program graph that owns source files, resolution, dependency ordering,
   symbols, diagnostics, and future incremental invalidation.
6. **Broaden TypeScript compatibility deliberately.** Add declarations, inference,
   narrowing, classes, overloads, conditional/mapped/indexed types, JSX typing,
   libraries, package resolution, configuration, and other families according to
   evidence and explicit checkpoints.
7. **Production hardening.** Validate large real projects, incremental/watch
   behavior, diagnostic quality/compatibility, memory and speed, fuzzing,
   sanitizers, platforms, packaging, and repeatable releases.

These stages may overlap where a vertical feature needs them, but later-stage
surface area must not outrun the foundations it depends on.

This ordering expresses dependencies, not a fixed seven-phase waterfall. Binding
may expose missing syntax structure; a file-local binder may require module
identity earlier than expected; a checker slice may send work back to the parser.
Reorder or revisit checkpoints when evidence requires it while preserving the
direction from syntax identity through binding, semantic reasoning, program-wide
reasoning, compatibility, and production confidence.

## Distinct confidence milestones

Treat these as separate engineering milestones even if product naming and public
versioning are decided later:

1. **Production transpiler:** a defined TypeScript syntax/module subset, correct
   JavaScript and runtime behavior, controlled rejection, project reliability,
   native safety, real-world corpus evidence, and stable performance/memory.
2. **Production type-checking compiler:** the above plus durable syntax, binding,
   symbols, type relationships and inference, semantic diagnostics, cross-module
   type behavior, libraries/configuration, and a large semantic compatibility
   corpus.

The first milestone must not be held indefinitely hostage to the second. Equally,
reaching it must not obscure the stated type-checking destination. Whether users
eventually select a fast transpile-only path and a checked path through flags,
commands, configuration, or separate modes remains a product decision; preserve
the architectural option without inventing the interface prematurely.

## Transpiler regression wall

Before semantic layers begin changing hot paths, retain a dated baseline for:

- the full parser/transpiler/runtime and independent regression corpus;
- representative emitted structure where byte shape is meaningfully contractual;
- single-file, many-file, TSX, CommonJS, and project throughput;
- peak RSS and representation/lifetime cost;
- malformed-input behavior and sanitizer evidence.

Every syntax/binder/checker checkpoint should report whether existing correctness,
runtime semantics, compile performance, and memory changed. Regressions are not
automatically forbidden, but they must be measured, explained, and accepted as
deliberate trade-offs rather than hidden inside semantic progress.

## Current priorities

1. Define the first bounded compatibility checkpoint on the path to the stated
   production type-checking goal: TypeScript range, checked language subset,
   ECMAScript targets, module/project resolution, diagnostics, and unsupported
   behavior.
2. Reconcile/document the actual parser/syntax/transpiler/project architecture.
3. Build an evidence-backed feature matrix distinguishing parse, transform/emit,
   runtime, negative diagnostics, and semantic-only gaps.
4. Prioritize incomplete semantic slices and architecture weaknesses over adding
   more syntax.
5. Maintain the standalone regression suite as canonical external contract owner
   and enforce parity of its executable corpus with the implementation mirror.
6. Expand runtime/differential, side-effect, scope, malformed/recovery, and
   real-world project coverage.
7. Run native safety validation and introduce targeted fuzzing/stress work.
8. Reproduce and contextualize performance across current TS, TSX, project, and
   CommonJS workloads; monitor RSS and semantic comparability.
9. Reconcile README and website claims/examples/support matrix with current tests.
10. Validate a clean release-like candidate for the explicitly promised scope and
    decide production status with Nick.

## Reconciliation findings to resolve

- Pin or define the supported TypeScript oracle range. The 2026-08-16 baseline is
  green on TypeScript 7.0.2 after making module modes explicit; upgrades still
  require complete-suite classification rather than relying on ambient defaults.
- Keep the automated parity check between the local mirror and canonical
  independent corpus green; the stale case count and TypeScript 7 oracle drift
  are resolved.
- Decide whether `ModuleGraph` should become the driver implementation or remain
  separately tested infrastructure; avoid maintaining two traversal paths without
  an explicit reason.
- Continue hardening multi-file output transactions. Current compilation prepares
  all successful sources before emission and stages each file beside its target.
  `noEmitOnError` suppresses the whole prepared set after any compile error, while
  default emit-on-error commits only successful sources. A failure during the
  final sequence of filesystem renames is still not an atomic project-wide commit.
- Replace broad architectural wording about binding/semantic phases with the exact
  compact syntax/range model unless those phases are genuinely introduced.

## Tentative next architecture checkpoints

These are bounded current directions and must be revised after each checkpoint:

1. **Completed — edit invariants:** replacement composition now has deterministic
   boundary/insertion ordering and rejects conflicts before source mutation.
   Erasures remain idempotent source blanking; add provenance only when diagnostics
   or a newly observed conflict family justifies its cost.
2. **Completed first structural foundation:** the lightweight semantic overlay now
   retains declaration, function, parameter, brace-region, and return spans and is
   consumed by the checker without changing output. Identifier references and
   semantic scope classification belong to the binder checkpoint rather than being
   guessed by this neutral structural layer.
3. **Completed bounded binder foundation:** ordinary function declarations,
   parameters, simple variables, root/function/brace scopes, `var` promotion,
   nested functions, closure references, and lexical shadowing now have stable
   symbol identity. Destructuring, arrows, imports, classes/members, full hoisting,
   and default-parameter temporal semantics remain explicit future slices.
4. **Completed first production migration bridge:** CommonJS live-import rewriting
   now consults bound identity for ordinary local references and retains legacy
   shadow ranges for unsupported binding forms. The migration found and fixed a
   function-local `var` shadowing bug. Retire each remaining heuristic branch only
   as its binding form enters the protected binder contract.
5. **Completed first experiment — minimal literal types:** the first checker owns
   primitive annotations and direct literal initializer compatibility. Move these
   type facts onto bound declarations rather than broadening expression syntax yet.
6. **Completed bound primitive checking slice:** annotated primitive facts flow
   through bound identifier initializers and direct assignments with shadow-aware
   positive and negative diagnostics. Unions, structural object types, calls,
   inference, compound/destructuring assignment, and narrowing remain outside it.

7. **Completed durable type ownership:** `TypeModel` owns canonical primitive and
   special type IDs plus bound-symbol facts for the program lifetime. The store is
   intentionally small; add composite types only with a complete semantic slice.
8. **Completed primitive expression slice:** a bounded precedence grammar covers
   primitive unary, additive and multiplicative expressions over literals and
   bound identifiers. Logical/comparison/conditional/call/property expressions,
   inference and control flow remain outside it.
9. **Completed bounded assignment semantics:** symbols retain `var`/`let`/`const`
   identity; const reassignment and primitive direct/compound assignment are
   checked. Updates, destructuring, properties, definite assignment and
   control-flow state remain outside this slice.
10. **Completed catch/loop binder migration:** simple catch identifiers and simple
    `for` variable declarations have explicit lexical regions and binder-owned
    CommonJS shadowing. Destructuring and arrow/class binding forms remain on the
    legacy bridge.
11. **Completed program preparation/output policy:** each source now prepares with
    file-local diagnostics before any output is committed. Default emit-on-error
    writes only successfully prepared sources; CLI and tsconfig `noEmitOnError`
    suppress the entire prepared set when any source fails. Staged sibling files
    reduce individual replacement risk, but filesystem rename failure is not a
    transactional multi-file commit and must not be described as one.
12. **Completed bounded import identity:** static default, namespace, and named
    value imports are root-scope binder symbols. Ordinary CommonJS live references
    now positively resolve to import identity while supported local shadows resolve
    elsewhere; the legacy bridge remains for unbound destructuring, arrows,
    classes, and template interpolation. Imported symbol types remain deliberately
    `unknown`: this checkpoint does not claim export tables or cross-module types.

Every checkpoint requires unchanged/equivalent transpiler output and runtime
evidence plus measured performance/RSS impact. Do not require the entire language
to migrate before a bounded binder slice can ship, and do not accept semantic
infrastructure that remains disconnected from production compilation.

Reassess the next semantic slice from final validation evidence. The likely next
direction is function signatures and call checking because it extends durable
types vertically without first requiring a complete module system. Module export
tables and cross-file type propagation remain valuable, but should follow a clear
file-local callable/object type contract rather than transporting only `unknown`
facts between files.

## 2026-08-17 campaign assessment

The twelve-checkpoint architecture campaign passes:

- clean optimized build and every focused/integration test;
- independent and mirrored 519-case corpus at 492 pass, zero fail, 27 deliberate
  semantic skips under TypeScript 7.0.2;
- ASan+UBSan build and full test suite with no sanitizer finding;
- a build/test from a clean `git archive` of committed `HEAD`;
- five-run performance medians of 1.575 ms small, 6.827 ms/100 basic files,
  25.529 ms/500 basic files, 8.480 ms/100 feature-heavy files, and
  9.391 ms/100 advanced files.

The candid result is not “production-ready TypeScript checker.” It is a stronger,
well-protected experimental transpiler and compiler foundation whose semantic
architecture is now real and used in production paths. The remaining gap is
fundamental language scope: callable/composite types, richer binding forms,
inference/control flow, module export/type graphs, libraries/packages, broad
diagnostics, fuzzing, representative projects, platforms, and packaging. Do not
convert green infrastructure evidence into a compatibility claim.

## Roadmap response to evidence

CP32 completes the first bounded callable-expression slice: annotated callable
variables, contextual arrow/function parameters and results, and calls inside
checker-owned initializer expressions. The next decision gate should compare
broader function inference/overloads with object methods, index signatures,
inheritance and classes; neither should bypass durable expression and type-model
ownership. Standalone expression-statement checking is also an explicit gap.

CP33 closes that callable-expression coverage gap by discovering callable roots
across the complete source token stream while retaining node-owned argument
typing. Future work should replace discovery scans as statement/control-flow
nodes gain durable expression-child identities; do not add a second typer.

CP34 publishes inferred callable identities for bounded typed arrows/function
expressions and completes optional/default/rest behavior for contextual callable
variables. The evidence exposed nested callback arrows inside call arguments as
the next expression-grammar seam. Solve that through recursive expression nodes,
not a callback-specific token parser, before claiming general contextual typing.

CP35 gives each call argument a delimiter-aware recursive expression subtree.
That closes direct inline callback contextual typing and removes duplicate scans
inside checker-owned initializer/return roots. Continue replacing token-range
discovery as durable statement and nested-expression ownership expands; do not
describe this bounded call-argument result as a complete expression grammar.

CP36 reuses canonical callable identities for object methods/function-valued
properties and adds bounded interface inheritance. Evidence supports member-call
argument and arity checks without a second method typer. The next object-model
work should add index/call signatures or classes only after selecting a durable
representation for those shapes.

CP37 extends recursive expression ownership through parentheses, object values
and array elements, and propagates object-property context into nested callable
expressions. This materially reduces cursor-coupled parsing, but statement forms
still need explicit expression-child identities before the global call-root
discovery pass can be removed safely.

CP38 stores index and call signatures directly on canonical structural object
identities. Callable objects deliberately delegate to the existing function
variance/checking path. Computed element access and overload sets should extend
this identity rather than create another checker-specific representation.

If a feature reveals a binding/transform-ownership deficiency, prioritize that
architecture before planned grammar features. If a real corpus demonstrates a
high-value missing slice, reorder accordingly. Remove speculative work when a
focused regression proves current architecture sufficient.

## After production

Track TypeScript evolution deliberately, expand supported scope through complete
slices, retain every production regression, improve diagnostics/platforms,
monitor performance, and keep public compatibility wording synchronized.
