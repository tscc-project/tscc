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

TCP0-TCP5 completed the first bounded compiler-preview contract. Current work is
therefore evidence-led expansion rather than redefining that already-qualified
finish line. PC0V is complete at TSCC `1bc3047` and JS++ `d35999f`; native
lifetime uncertainty is no longer the next compiler blocker:

1. Keep the 579-case external contract, 7/7 runtime intersection, machine-readable
   preview contract and repository mirrors synchronized.
2. Keep the pinned TypeScript 7.0.2/Node 22.22.1 oracle environment and complete
   PC0V evidence as retained regression gates.
3. Build TCP6A around pinned, licence-compatible real small projects and classify
   failures by compiler stage before implementing fixes.
4. Choose TCP7 from measured blockers. Broader import-type propagation and
   practical package resolution are candidates, not predetermined commitments.
5. Continue using complete semantic slices, sanitizer coverage and performance/RSS
   comparison for every architecture or compatibility expansion.

PC0P remains a parallel JS++ packaging gate and EP6A remains a parallel JS++
conformance campaign. Neither should keep TSCC idle or select TSCC features in
the absence of TSCC project evidence.

## Post-PC0V compiler program: CP43-CP72

Status: CP43-CP65 completed on 13 September 2026. TCP6A now passes all three
initial project shapes. Declaration inputs join the canonical graph without
emitting JavaScript, their callable facts reach consumers, default/namespace
imports carry types, and re-export aliases/cycles converge deterministically.
The 579-case wall remains 555 pass, 0 fail and 24 explicit semantic skips. The
first reassessment selected practical resolution (CP51), now complete with a
structured resolution result, modern/declaration extensions, directory/package
entries and explicit ambiguity. CP52 adds the deliberately named `node` mode,
ascending `node_modules` boundaries, scoped/subpath packages and selected
`types`/`typings`/`exports`/`default`/`main` entries while preserving the default
relative-only boundary. CP53 replaces the regex configuration reader with
Jsonic++, maps the selected JSONC configuration surface, detects inheritance
cycles, and feeds base URL/path/root mappings into the canonical graph. CP54-55
add declaration inputs and a hermetic standard-library contract; CP56-57 add
generic identity, substitution and deterministic inference. CP58 now preserves
ordered public overload sets, resolves the most specific applicable function
signature and keeps implementation signatures private. CP59-60 establish
canonical two-sided class symbols and deterministic class relationships,
including inheritance, interfaces, access, readonly, abstract/override rules,
`super`, and method/constructor overloads. CP61 replaces residual expression
discovery with durable statement/expression identities and completes the
selected expression grammar. CP62 adds stable per-function CFG blocks, typed
edges, predecessor joins and reachability. CP63 moves truthiness, equality,
discriminant, `typeof`, `instanceof`, property-presence, assignment and
definite-assignment facts onto that foundation. CP64 adds canonical optional and
rest tuple metadata, readonly collection identity, indexed writes, key operators,
numeric/symbol index domains and an explicit excess-property policy. CP65 adds
bounded advanced operators and their utility-type foundation. CP66 is next.
See `docs/evidence/cp45-tcp7-decision.md` and
`docs/evidence/cp50-reassessment.md` for the retained decisions.

These 30 checkpoints are the current route from the bounded compiler preview
toward a proper TypeScript compiler. CP43-CP45 are ordered commitments. CP46-CP72
are a dependency-aware backlog whose order must be revised at each decision gate
using real-project evidence. A checkpoint is complete only when its stated
behavior is on the production compilation path, focused and independent tests
pass, the 579-case wall remains classified, performance/RSS are measured where
relevant, and handover/feature-matrix claims are reconciled.

### Evidence and architectural closure

43. **TCP6A trial contract and runner.** Define a manifest for pinned,
    licence-compatible small TypeScript projects; record source revision,
    install/build commands, supported configuration, expected oracle result,
    timeouts and offline cache rules. Classify every outcome as acquisition,
    configuration, parser, binder, checker, resolution, lowering, emission,
    runtime, diagnostic, crash or timeout. The runner must not turn unsupported
    behavior into a pass.
44. **TCP6A baseline and reduced blockers.** Run the unmodified compiler against
    the trial set, retain machine-readable results, minimize at least the leading
    failures into independent regression fixtures, and publish a capability map.
    This checkpoint adds evidence, not opportunistic fixes.
45. **TCP7 evidence decision.** Rank blockers by projects unlocked, dependency
    depth, semantic risk and architectural leverage. Select the next two bounded
    vertical slices and explicitly defer the rest. Re-run this decision after
    CP50, CP56, CP62 and CP68.
46. **Unknown-expression accounting.** Give every checker-owned expression a
    durable supported/unknown classification and reason. Reject or conservatively
    type unsupported checked syntax instead of silently treating it as a sound
    fact. Add corpus telemetry so later grammar work targets measured unknowns.
47. **Binding completion for selected projects.** Complete nested/computed
    destructuring, arrow/class member bindings, hoisting and default-parameter
    temporal regions needed by the selected trials. Retire matching CommonJS
    shadow heuristics only after identity-parity tests pass.

### Modules, resolution and declaration inputs

48. **Canonical export tables.** Make value/type namespaces, aliases and exported
    symbol identity explicit per compilation unit. Replace source scans used for
    named-import type propagation with graph-owned export facts.
49. **Default and namespace imports.** Propagate callable, object, array, tuple,
    union and literal types through default and namespace imports, including
    shadowing and missing-member diagnostics.
50. **Re-exports, type-only edges and cycles.** Support named/star re-exports,
    import/export type, aliases and cyclic module graphs with deterministic
    fixed-point behavior and no unit-local `TypeId` leakage.
51. **Practical relative resolution.** Resolve supported extensions, directory
    indexes and package entry metadata with canonical identities, ambiguity
    diagnostics and deterministic search order. Preserve `--noResolve` exactly.
52. **Versioned package resolution.** Add a deliberately named Node-style module
    resolution mode covering `node_modules`, package boundaries, `types` and the
    selected `exports` conditions. Do not claim all `tsc` resolution modes from
    one implementation.
53. **Configuration mapping.** Implement the trial-selected `baseUrl`, `paths`,
    `rootDirs`, include/exclude/files and module-resolution settings with config
    inheritance, cycle detection and path-attributed diagnostics.
54. **Declaration-file frontend.** Parse and bind the `.d.ts` declarations needed
    by the trial corpus: ambient modules, exported values/types, interfaces,
    aliases, functions and classes. Declaration files emit no JavaScript.
55. **Standard library contract.** Introduce explicit, versioned library bundles
    and `lib`/`types` selection. Resolve built-in global types without ambient
    machine state and test compilation with and without default libraries.

### Core type-system expansion

56. **Generic identity and explicit substitution.** Add type parameters,
    constraints, defaults, explicit type arguments and recursion guards across
    functions, aliases, interfaces and classes before attempting broad inference.
57. **Generic inference.** Infer type arguments from calls and contextual return
    positions with deterministic candidate combination, constraint checking and
    diagnostics for unresolved or conflicting inference.
58. **Overload sets and resolution.** Preserve ordered overload declarations,
    select applicable signatures, report no-match/ambiguity cleanly and keep the
    implementation signature out of the public callable surface.
59. **Class symbols and two-sided types.** Model constructor/static and instance
    sides, fields, methods, constructors, parameter properties and `this` using
    canonical symbols and types rather than erasure-only class knowledge.
60. **Class relationships.** Add inheritance, `super`, implemented interfaces,
    visibility, readonly, abstract members and override compatibility with
    deterministic base-cycle diagnostics.
61. **Complete expression ownership for selected grammar.** Replace remaining
    checker discovery ranges with durable expression/statement child identities;
    add selected logical, comparison, conditional, update, `new`, optional-chain,
    non-null and assertion/satisfies nodes without creating a parallel typer.
62. **Control-flow graph foundation.** Build per-function basic blocks and edges
    for branches, loops, short circuiting, returns, throws, try/catch/finally and
    reachability. Prove stable graph identity and conservative joins before
    widening diagnostics.
63. **Flow-sensitive narrowing and assignment.** Add truthiness, equality,
    discriminants, `typeof`, `instanceof`, property-presence, assignment and
    definite-assignment facts with loop fixed points and lexical restoration.
64. **Richer object and collection types.** Add optional/rest tuple elements,
    readonly arrays/tuples, indexed writes, excess-property policy, `keyof`,
    indexed access and symbol/numeric keys on the canonical structural model.
    **Completed 2026-09-13:** collection mutability and tuple element flags are
    part of canonical type identity and survive import/substitution; variadic
    tuple substitution flattens concrete tuple arguments. Indexed reads and
    writes share one key-domain operation. Fresh object literals remain
    structurally permissive under ordinary assignment for retained compatibility,
    while `satisfies` performs excess-property checking. See
    `docs/evidence/cp64-richer-collections.md`.
65. **Advanced type operators.** Add intersections, conditional and mapped types,
    template-literal types and utility-type prerequisites as separately bounded
    slices with instantiation-depth and complexity limits.
    **Completed 2026-09-13:** canonical/deferred operator nodes now cover
    intersections, `keyof`/indexed access, homomorphic and finite-key mapped
    types, distributive conditional substitution with bounded `infer`, and
    finite template-literal expansion. Core utility transforms share those
    primitives. Substitution stops after 64 levels and combinatorial operator
    surfaces stop or widen at 256 members. Key remapping, recursive conditional
    aliases and complete intrinsic utility parity remain outside this bounded
    slice. See `docs/evidence/cp65-advanced-types.md`.

### TypeScript surface, output and production operation

66. **JSX/TSX semantics.** Type JSX elements, attributes, children, intrinsic
    elements and component calls against explicit library contracts; then add
    selected JSX emit modes without weakening preserve mode.
    **Completed as a bounded contract 2026-09-13:** an explicit
    `namespace JSX { interface IntrinsicElements { ... } }` opts a TSX unit into
    intrinsic-tag, attribute, required-property, primitive attribute-value and
    callable component-props checking. Preserve output is unchanged. Contextual
    expression typing, spread attributes, `ElementClass`, library augmentation
    across files and runtime JSX transforms remain outside this slice. See
    `docs/evidence/cp66-jsx-semantics.md`.
67. **Runtime-bearing TypeScript semantics.** Move enums, namespaces and parameter
    properties from tested lowering with partial binding to checked symbol/type
    semantics, including merging and cross-module behavior.
    **Completed as a bounded local-unit slice 2026-09-13:** enums now introduce
    value symbols, member-object types and literal unions, so member access and
    enum-typed assignment participate in the ordinary checker. Constructor
    parameter properties already use the CP59 class model and are now gated
    alongside enum and namespace runtime differentials. Namespace lowering and
    merging remain runtime-verified; exported namespace member typing and
    cross-module enum/namespace fact propagation remain follow-up compatibility
    work. See `docs/evidence/cp67-runtime-types.md`.
68. **Source maps and diagnostic polish.** Emit deterministic source maps with
    exact mappings through erasure and lowering; improve related locations,
    category/codes and recovery diagnostics against reduced real-project cases.
    **Completed as a bounded line-map slice 2026-09-13:** `--sourceMap` emits a
    deterministic v3 sidecar and trailer, staged with its JavaScript output.
    Whitespace-preserving erasure and single-line lowerings retain exact source
    lines; column segments inside rewritten enums/namespaces are intentionally
    not yet claimed. Existing stable diagnostic codes and recovery gates remain
    green. See `docs/evidence/cp68-source-maps.md`.
69. **Declaration emit and output integrity.** Emit `.d.ts` and declaration maps
    for the supported checked surface. Define collision, stale-output and
    project-wide commit/recovery behavior beyond best-effort sibling renames.
    **Completed as a bounded declaration surface 2026-09-13:** `--declaration`
    and `--declarationMap` emit interfaces, aliases, enums, annotated variables
    and function signatures. JavaScript, declarations and maps are staged as one
    project transaction; duplicate target paths fail before writes. Class and
    inferred declaration emit plus stale-output manifests remain unclaimed. See
    `docs/evidence/cp69-declaration-output.md`.
70. **Incremental compiler service.** Add reusable programs, dependency-aware
    invalidation, cancellation and watch mode with clean/incremental equivalence
    gates for output and diagnostics.
71. **Production hardening campaign.** Exercise larger graphs and real projects,
    adversarial/generated input, parser and semantic fuzzing, sanitizer/Valgrind,
    performance/RSS budgets, failure recovery and supported-platform packages.
72. **Compatibility and release decision.** Publish the exact supported syntax,
    checking, configuration, resolution, emit and platform matrices; compare
    separately with checked `tsc` and `tsc --noCheck`; decide from retained
    evidence whether production-transpiler and production-checker milestones are
    earned, and release only under a separately authorized release process.

This program intentionally does not promise that CP70 equals complete `tsc`
compatibility. Decorators, project references, every historical module-resolution
mode, plugins/transformers and newly added TypeScript syntax enter through the
same evidence and compatibility process. A proper compiler is one with a truthful,
useful and dependable supported contract—not one that silently accepts every
spelling it can erase.

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
    classes, and template interpolation.
13. **Completed preview cross-module type propagation:** relative named value
    imports receive cloned exported structural and callable types in the program
    graph. Type IDs remain unit-local. Default imports, namespaces, re-exports and
    package resolution remain outside this bounded preview slice.

Every checkpoint requires unchanged/equivalent transpiler output and runtime
evidence plus measured performance/RSS impact. Do not require the entire language
to migrate before a bounded binder slice can ship, and do not accept semantic
infrastructure that remains disconnected from production compilation.

This earlier assessment selected function signatures and call checking. CP31-CP38
subsequently delivered canonical callable identities, contextual callbacks,
nested structural expressions, interface inheritance, index signatures and call
signatures. It is no longer the current decision point. TCP6A real-project
evidence now selects the next semantic slice under CP43-CP45 above.

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
