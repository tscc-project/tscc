# Coordinated tscc and JS++ roadmap

## Purpose

This living checkpoint plan coordinates three goals:

1. move tscc from its protected transpiler foundation toward a genuine
   TypeScript type-checking compiler;
2. build JS++ as an independent embeddable JavaScript engine;
3. integrate JS++ where it materially strengthens tscc without turning the
   compiler into a runtime wrapper or forcing premature shared architecture.

Near checkpoints are deliberately more specific than distant ones. Later work
must be reassessed as implementation evidence exposes the real constraints.

## Current campaign status and next gate

The bounded finish lines have been reached. TSCC TCP0-TCP5 is a qualified Linux
compiler preview at 555/0/24, and JS++ EP0-EP5 is a qualified Linux embedded
preview at 171/171. Their module-free runtime intersection is 7/7. These are
evidence-backed preview contracts, not broad TypeScript or ECMAScript
compatibility claims.

The next campaign is evidence acquisition rather than immediate feature breadth:

1. PC0V external Valgrind confirmation is now complete at TSCC `1bc3047` and
   JS++ `d35999f` (2026-09-13); it required a JS++ standalone local-heap
   lifetime correction that was reviewed and finalized at `d35999f`, and all
   declared workloads pass under Valgrind.
2. PC0P second-platform packaging, noting TSCC already has multi-platform CI
   while JS++ currently has only the qualified Linux package;
3. EP6A pinned selected-Test262 infrastructure and classifications;
4. TCP6A a pinned, license-compatible real-project trial manifest and failure
   classifier;
5. a decision gate that selects TCP7 and the next JS++ vertical slice from those
   results.

PC0V is complete at the tested heads; PC0P remains pending. Work may prepare
EP6A and TCP6A, but no broader release claim should bypass the remaining
platform confirmation.

## Product boundary

### tscc owns

- TypeScript and supported JavaScript compiler input;
- compiler-quality source locations, syntax recovery, declarations, scopes,
  symbols, binding, TypeScript merging, types, inference, narrowing, checking,
  and diagnostics;
- project graphs, module resolution, configuration, and incremental invalidation;
- TypeScript lowering, target-specific JavaScript emission, source maps, and
  declaration output.

### JS++ owns

- executable ECMAScript parsing and runtime semantics;
- bytecode and virtual-machine execution;
- JavaScript values, objects, prototypes, environments, closures, exceptions,
  garbage collection, jobs, modules, and built-ins;
- native embedding plus the `js`, `js.h`, `libjs.a`, and `libjs.so` artifacts.

### Dependency rule

tscc compiles ordinary projects without executing their source. JS++ begins as
a test/runtime oracle beside Node. Optional production integration is allowed
later only for a concrete, deterministic, resource-bounded compiler feature.
Execution is never a substitute for implementing TypeScript semantics.

## Checkpoint discipline

Every checkpoint requires relevant focused tests, independent contract cases,
a clean optimized build, sanitizer coverage for native ownership changes,
unchanged accepted behaviour unless deliberately revised, reconciled handovers
and roadmaps, and an exact accepted repository head. Checkpoints do not imply a
release or public compatibility claim.

## Foundation checkpoints

### F0 - Reconcile the retained baseline - completed 2026-08-30

Correct stale statements about aggregate tests, sanitizers, the bounded checker,
corpus counts, and prepared/staged output. Record the verified 519-case baseline
without changing behaviour.

Completed at the retained 519-case baseline: 492 pass, zero fail, and 27
intentional semantic-only skips under TypeScript 7.0.2. The aggregate test,
sanitizer, bounded-checker, corpus, and staged-output descriptions were reconciled.

### F1 - Freeze the product boundary - completed 2026-08-30

Record the ownership and dependency rules in tscc, JS++, both regression suites,
and their websites. Reject private cross-repository includes and an implicit
required JS++ runtime dependency.

Completed with `PRODUCT-BOUNDARY.md` and an aggregate repository check that
rejects accidental JS++ headers, sources, or linkage in the normal compiler.

### F2 - Establish executable feature matrices - completed 2026-08-30

Classify JS++ features as tokenized, parsed, compiled, executed, error-tested,
embedding-tested, and conformance-tested. Classify tscc features as parsed,
bound, typed, lowered, emitted, runtime-tested, diagnostic-tested, and
project-tested. Use explicit versioned targets rather than broad language claims.

Completed with versioned JSON matrices and validators in both product and
independent-suite repositories. The tscc matrix checks its 519-case oracle totals
and every named external evidence case.

## tscc compiler checkpoints

### TC1 - Durable compilation unit - completed 2026-08-30

Create stable per-file ownership for source, tokens, syntax, semantic structure,
binding, type facts, diagnostics, options, and output while preserving behaviour.

Completed with `CompilationUnit`, an explicit source-to-emitted lifecycle, retained
front-end/semantic state after emission, per-unit diagnostics, and a focused
lifetime regression. The compiler driver retains every discovered unit through
the output commit phase; the source-edit emitter remains the compatibility path.

### TC2 - One program and module graph - completed 2026-08-30

Make one program graph own roots, discovered files, identities, dependencies,
compilation units, program diagnostics, and emission policy. Retire the duplicate
`Compiler.cpp` traversal after parity evidence.

Completed with `ProgramGraph` as the sole production traversal. It owns canonical
file identities, root indices, dependency edges, durable compilation units and
graph diagnostics; `Compiler.cpp` prepares and commits those same units.

### TC3 - Durable syntax identity - completed 2026-08-30

Grow stable node IDs, relationships, exact spans, token ownership, and recovery
nodes for the next semantic slices. Retain the range/replacement emitter as a
compatibility path during migration.

Completed with compilation-unit-local stable node IDs, parent relationships,
exact token/byte spans, lookup by identity and explicit parser recovery nodes.
Focused tests prove identity survives analysis and source-preserving emission.

### TC4 - Recovery and syntax diagnostics

Harden EOF, delimiters, generic/relational and TSX ambiguity, comments, semicolon
boundaries, and recovery without consuming following declarations. Add
deterministic fuzz-ready parser entry points.

Completed as CP8 on 2026-08-30. Failed top-level constructs now resynchronize at
explicit declaration boundaries after semicolons/braces, retaining subsequent
declarations and stable recovery identities. `ParserLimits` gives fuzzers and
adversarial callers a deterministic top-level work ceiling. Focused tests repeat
the same malformed parse, compare diagnostic/tree identity, exercise EOF and
delimiter failures, and prove the work limit fails closed. This is a recovery
foundation, not a claim that every TypeScript/TSX ambiguity is fully solved.

### TC5 - Declaration and scope completion

Bind arrows, destructuring, defaults/rest, classes and members, remaining
block/loop forms, imports/exports, hoisting regions, and merging foundations.
Retire CommonJS shadow heuristics one protected binding family at a time.

Completed as CP10 on 2026-08-30 for the bounded declaration families now used by
the checker and emitter: ordinary functions, expression/block arrows with typed,
default and rest identifier parameters, flat object/array destructuring, class
names, catch/loop bindings, value imports, block/function scope, captures,
shadowing and order-independent function lookup. Nested/computed destructuring,
full class-member symbol tables and declaration merging remain later explicit
slices rather than inferred from this checkpoint.

### TC6 - Function signatures and calls

Completed as CP13 on 2026-08-30 for typed ordinary declarations: durable
parameter/result signatures, optional/default/rest arity boundaries, direct
calls, argument compatibility, exact ordinary arity diagnostics and annotated
return-expression checking. Callable variable types, function expressions,
overloads, generics and contextual typing remain later TC6 expansions.

### TC7 - Literal, union, null, and undefined types

CP14/TC7A completes canonical literal, normalized union and nullish identities
with bounded assignability. CP17/TC7B completes strict equality/`typeof`
then-branch facts and lexical restoration; full flow joins remain later work.

### TC8 - Object and property types

CP19/TC8A is complete: the canonical type store interns structural object
shapes and retains optional/readonly property metadata with stable lookup.
CP20/TC8B activates the first bounded source-facing structural checks.

CP20/TC8B is complete for flat variable annotations and literals, property
reads, required/optional structural assignment and readonly writes. Nested and
indexed shapes, freshness/excess checks and class/interface integration remain.

Add object type literals, property lookup, optional/readonly properties, object
literal checking, property access, structural assignment, and an explicit
excess-property compatibility decision.

### TC9 - Classes and constructors

Add instance/static sides, constructors, fields, methods, visibility,
inheritance, `this`, `super`, overrides, and abstract members.

### TC10 - Generic foundations

Add type parameters, constraints, explicit instantiation, substitution and
recursion guards, then bounded inference after explicit forms are sound.

### TC11 - Control flow and narrowing

Introduce flow facts for assignment, branches, returns, loops, truthiness,
discriminants, `typeof`, `instanceof`, and definite assignment.

### TC12 - Cross-module symbols and types

Add export tables, import aliases, type-only edges, re-exports, cycles, and
cross-file callable/object type propagation on the unified program graph.

### TC13 - Libraries and package resolution

Add declaration files, configurable standard libraries, package lookup,
`package.json` fields, path mappings, and versioned module-resolution modes.

### TC14 - Incremental compiler service

Add persistent programs, changed-file invalidation, reusable syntax/symbol/type
state, watch operation, and clean-versus-incremental equivalence gates.

### TC15 - Source maps and declarations

Add source maps, declaration emission, later declaration maps, atomic multi-output
policy, collision handling, and stale-output handling.

### TC16 - Production compiler campaign

Exercise representative projects, large graphs, diagnostics, malformed/generated
inputs, fuzzing, memory tools, supported platforms, release artifacts, and clearly
separated comparisons with `tsc --noCheck` and checked `tsc`. Assess production
transpiler and production checker milestones independently.

## JS++ checkpoints

### JS0 - Repository and build foundation

Create the C++17 layout, Make build, `include/js.h`, CLI/library targets, tests,
handovers, and independent regression runner.

### JS1 - Embedding lifecycle - completed 2026-08-30

Implement opaque runtime/value types, creation/destruction, evaluation, errors,
versioning, allocator policy, value lifetime, runtime affinity, and an explicit
thread-safety contract.

### JS2 - Source, diagnostics, lexer, and parser - completed 2026-08-30

Implement UTF-8 source ownership, locations, ECMAScript tokens, comments,
literals, semicolon rules, executable syntax, and controlled recovery. Parsing
is independently testable before execution.

### JS3 - Bytecode and primitive execution - completed 2026-08-30

Implement an inspectable unoptimized VM for primitive values, variables,
arithmetic, comparisons, expression completion, and bytecode disassembly.

JS0–JS3 now provide the C++17 build, opaque `js.h` lifecycle, attributed bounded
frontend, inspectable stack bytecode and a 17-case primitive execution/embedding
contract. This does not yet activate tscc dual-runtime testing; INT0 remains a
separate future checkpoint with an explicitly selected emitted-JavaScript subset.

### JS4 - Control flow

Add blocks, conditions, loops, break/continue, short-circuiting, conditional
expressions, and completion propagation with side-effect-order tests.

### JS5 - Functions and environments

Add functions, calls, parameters, returns, recursion, lexical environments,
closures, and then arrows.

### JS6 - Objects and arrays

Add ordinary objects, arrays, properties/descriptors, computed access, methods,
`this`, prototypes, constructors, and `new`.

### JS7 - Garbage collection

Introduce tracing collection after roots are explicit across the VM stack,
environments, objects, native handles, and exceptions. Add allocation pressure,
teardown, sanitizer, and Valgrind evidence.

### JS8 - Exceptions

Add throw/try/catch/finally, completion interaction, native error propagation,
uncaught reporting, and stack traces.

### JS9 - Conversions and built-ins

Centralize ECMAScript conversions and add Object, Array, String, Number, Boolean,
Math, JSON, and errors as tested families.

### JS10 - Native functions and handles

Extend `js.h` with persistent handles, native functions, arguments, property
construction, exceptions, user data, finalizers, and safe re-entry rules.

### JS11 - Promises and jobs

Add promises, reactions, a host-controlled job queue, then async functions.
Networking, filesystems, and timers remain host facilities.

### JS12 - Modules

Add module parsing, records, linking, cycles, live bindings, host resolver hooks,
and later dynamic import.

### JS13 - Broader ECMAScript slices

Add classes, destructuring, rest/spread, generators/iterators, symbols,
collections, regular expressions, typed arrays, dates, and other modern families
as complete parse/compile/runtime/error slices.

### JS14 - Conformance and hardening

Run selected Test262 with explicit supported, unsupported,
harness-inapplicable, failed, crash, and timeout classifications. Add parser,
bytecode, and execution fuzzing.

### JS15 - Packaging and releases

Ship supported platform libraries/CLI, pkg-config/CMake metadata, ABI versioning,
per-user and `--system` installation, release automation, examples, and
evidence-backed documentation.

## Integration checkpoints

### INT0 - Dual-runtime regression harness

Execute eligible tscc output under Node and JS++. Node remains the compatibility
oracle; JS++ failures remain engine gaps until the supported intersection is
explicitly promoted.

Completed as CP9 on 2026-08-30. The standalone tscc suite owns six explicitly
eligible fixtures spanning erasure, control flow, typed functions, generics and
closures. Each fixture compiles through the public `tscc` CLI, then the emitted
file executes independently through Node and the public JS++ CLI. Expected values
prevent two equally wrong runtimes from passing by agreement; an anti-agreement
self-test exercises all mismatch classifications. The opt-in
`make test-js-interop JS_RUNTIME=...` target is excluded from `test`/`test-core`,
and the normal compiler still has no JS++ header, source or library dependency.

### INT1 - Independent output parsing

When dependable, use the JS++ parser as an optional test gate before Node
execution. Keep it a test dependency initially.

### INT2 - Shared emitted-JavaScript corpus

Establish clearly owned cases from enums, namespaces, parameter properties,
CommonJS helpers, closures, classes, and evaluation-order defects. Avoid
manually divergent copies.

### INT3 - Syntax-sharing decision gate

Compare source ownership, tokens, diagnostics, executable syntax, losslessness,
TypeScript extension needs, and recovery. Share only a small public API if both
implementations prove a stable common contract. Default to shared concepts and
corpora, not private classes.

### INT4 - Optional compile-time evaluation

Consider embedding only for a concrete feature such as configuration,
plugins/transformers, scripting, or deliberately safe constant evaluation. It
must be opt-in, bounded, and isolated from ordinary compilation.

### INT5 - Developer runtime validation

If useful, expose an explicit developer/CI mode for validating or executing
tscc output through JS++. Never silently execute code during normal compilation.

### INT6 - Production dependency decision

Choose between test-only integration, optional linked integration, a public
syntax dependency, or a required runtime dependency. Default to test-only first
and optional embedding later. A required dependency needs strong evidence and
an explicit product decision.

## Initial execution order

Start with F0, F1, F2, TC1, JS0, JS1, TC2, JS2, JS3, then INT0. After INT0,
alternate bounded compiler and engine slices according to evidence. The likely
first major tscc semantic feature is callable types and call checking; the likely
first major JS++ path is primitives through closures and objects.

## Existing tscc migration policy

Nothing moves initially. The TypeScript lexer/parser, range transforms, binder,
checker, CommonJS lowering, and project machinery stay in tscc. Runtime harnesses
become multi-engine. Output validation may later use a public JS++ parser. Generic
source/diagnostic facilities are reconsidered only after comparative evidence.
CommonJS lowering remains tscc-owned because it is a compiler transform; JS++
separately owns ECMAScript module runtime semantics.
CP22/TC8C completes nested structural shapes, chained reads and path-specific
diagnostics. Extra source properties remain compatible by explicit policy.
CP24 establishes CompilationUnit-owned expression identity and routes checker
entry through interned expression nodes. Rich operator-node migration can now
proceed incrementally without creating parallel range identities.
CP26 gives that owned model explicit structural kinds, child IDs and operator
tokens for the bounded grammar. This is an architecture checkpoint rather than
a wider TypeScript compatibility claim.
CP28 adds reusable object aliases/interfaces and a deliberately compatible-only
interface merge policy. More expressive member signatures, inheritance,
generics and classes remain decision-gated follow-on work.
CP30 removes the checker compatibility token view: semantic expression typing
now walks retained node kinds and child identities directly. Broader callable
semantics can build on that graph without introducing another parser.
CP31 makes callable signatures canonical type identities shared by aliases,
annotated variables and declarations. The bounded variance policy is parameter
contravariance and result covariance; call-through/contextual behavior follows.
