# tscc architecture handover

This is the living implementation-oriented architecture map for the native tscc
compiler. It combines the current high-level source map with inherited compiler
design knowledge, semantic hazards, testing seams, and the concrete reconciliation
work required before substantial changes.

Source, tests, the independent regression suite, Git history, benchmarks, debug
artifacts, and current documentation remain authoritative. Exact names and current
capabilities must be verified against the live repository. Never reshape the
compiler merely to make it resemble this handover.

## Reconciled implementation snapshot (2026-08-16)

The inherited model has now been checked against the compiler and the standalone
suite. The executable pipeline is concretely:

~~~text
main.cpp CLI parsing / optional tsconfig loading
→ compile_files queue of absolute root paths
→ SourceFile load and line map
→ byte-oriented Lexer producing owned token strings plus byte ranges
→ optional relative import/export discovery
→ Parser producing a small Program of syntax nodes, declaration facts, erasures, and replacements
→ Checker validating the currently supported semantic slice
→ Transpiler applying erasure and runtime-lowering/module passes
→ direct binary-truncating write to .js or .jsx
~~~

This remains primarily an ES2022-oriented native transpiler and is not a
conventional bound/typed AST compiler. `Program::root` is a deliberately small
structural tree; most transformation intent is represented by byte-based
`EraseRange` and `Replacement` records. A lightweight `SemanticModel` now retains
source/token spans for variable and parameter declarations, function declarations,
brace regions, and returns without taking ownership of emission. The first checker
consumes its variable-declaration nodes, so the overlay is connected to production
compilation rather than being a parallel unused tree. The first semantic seam retains simple
`VariableDeclaration` token ranges before erasure, then `Checker` compares direct
primitive literal initializers with explicit `number`, `string`, or `boolean`
annotations. There is still no binder, symbol table, general type model, inference,
or expression checker. Transpiler-owned scope/shadow analysis remains local
transform machinery and must not be described as a general binding phase.

`compile_files` owns the shipped dependency traversal using a queue and de-duplicated
absolute path strings. `ModuleGraph` is real, tested infrastructure, but the current
driver does not instantiate it. Relative resolution is deliberately small: `.ts`,
`.tsx`, `index.ts`, `index.tsx`, and `.js`/`.jsx` source substitution; packages,
path aliases, declaration files, and Node resolution are outside this implementation.

Output writing currently occurs per file immediately after successful transpilation.
Consequently a later error in a multi-file invocation can leave earlier outputs,
and each destination is opened with truncation rather than staged/renamed. Treat
transactional multi-file emission as an explicit future policy decision, not an
existing guarantee.

The implementation-local `regression/` corpus and standalone suite have identical
`README.md`, `TSCC_BUG_LOG.md`, `cases.json`, and `run.py` files at this checkpoint;
only `REGRESSION_NOTES.md` differs. This byte equality is useful evidence but is not
automatically enforced. The stale case count and TypeScript 7 oracle drift were
resolved on 2026-08-16 by documenting 511 cases and making semantic CommonJS mode
explicit. After the first checker slice the result is 484 pass, zero fail, and 27
semantic skips under TypeScript 7.0.2. The runner still invokes `tsc` from `PATH`,
so a pinned release oracle or documented supported range remains required for
fully reproducible classifications.

## Architectural identity and support standard

tscc is a semantics-preserving source transformation pipeline:

~~~text
TypeScript/JavaScript source
→ lexical analysis
→ structural parsing
→ compact program representation
→ binding/scope work where implemented
→ supported semantic work
→ erasure and runtime lowering
→ JavaScript emission
→ executable output
~~~

The current implementation may combine stages. What matters is that every claimed
feature has a coherent path from source syntax to correct observable JavaScript.

Syntax acceptance is not feature completion. Depending on the feature, support
requires lexing, parsing, representation, binding, transformation, precedence-safe
emission, runtime equivalence, and appropriate rejection of invalid neighbors.
Develop vertically rather than adding broad grammar whose semantics will arrive
later.

The product contract must remain explicit. A drop-in tsc replacement, practical
TypeScript subset, transpiler, and JavaScript compiler with selected TS syntax have
very different architectural obligations. Documentation, implementation, and
tests must describe the same scope. A smaller complete subset is preferable to
silent corruption behind broad compatibility language.

## Source and diagnostics

`SourceFile` owns source text and position mapping. Diagnostics should preserve
file, line, column, source context/caret, severity/category, and stable non-zero
failure behavior to the degree current APIs support them. Do not pass anonymous
strings through the compiler if doing so destroys useful source attribution.

## Lexer

`Lexer` produces tokens consumed by parser/project logic. Lexer changes should
test comments, strings/templates, numbers, punctuators, Unicode/current supported
identifiers, ambiguous operators, and EOF. Token correctness is foundational but
does not itself establish language-feature support.

## Syntax and parser

`Parser` consumes a `SourceFile`, token vector, and `Diagnostics`, producing a
`Program`. Current parsing functions explicitly recognize declarations,
statements, classes, enums, namespaces, variables, parameters, annotations,
generics, object/runtime ranges, and replacement ranges.

The representation is intentionally compact rather than a full TypeScript typed
AST. Parser-owned erasure/replacement ranges prevent broad heuristics such as
“every colon means type annotation.” Do not move runtime semantics into token
guessing when a structural parser decision is required.

## Transpiler

`transpile`/`transpile_tokens` coordinate parser/token information and emission.
Transform ownership must be explicit enough that overlapping passes do not rewrite
one another's syntax. Historical module/import bugs demonstrate that individually
valid transforms can corrupt output when their ranges overlap.

Preserve evaluation order, once-only side effects, lexical shadowing, and
short-circuit behavior. Compiler-generated names need hygiene against user names
and nested transforms.

## Project and module graph

`Project` contains `ModuleFile`, dependency discovery, relative resolution, and a
`ModuleGraph`. Project/config/compiler code supports current root/out directory,
module discovery, cycle de-duplication, and output orchestration. Avoid expanding
module resolution into package-manager emulation without a deliberate contract.

## Configuration and compiler orchestration

`Config` loads the supported `tsconfig.json`/JSONC subset into `CompilerOptions`
and roots. `Compiler` coordinates files, options, project behavior, and output.
`main.cpp` owns CLI parsing/help/version and should not duplicate compiler
semantics.

## Semantic checker status

There is now a distinct checker pass, but only for explicit primitive annotations
against direct variable literal initializers. The regression suite promotes a
semantic case only when tscc owns that contract; all other cases accepted by
`tsc --noCheck` but rejected by full `tsc` remain semantic skips. Do not “fix”
those through ad hoc parser rejection. A production-ready type-checking compiler
is the explicit long-term destination.
Reaching it requires deliberate syntax, binder, symbol/module, type-system,
checker, diagnostic, and incremental-program checkpoints. Preserve the current
transpiler as working infrastructure while those layers are introduced; do not
mistake its range-erasure representation for the eventual semantic program model,
and do not replace it in one unvalidated big-bang rewrite.

### Intended hybrid evolution

The range-edit emitter is an architectural asset, not merely scaffolding. Much
TypeScript output is original source plus selective erasure, and semantic
understanding does not require reconstructing every unchanged JavaScript byte.
The likely direction is therefore hybrid:

~~~text
source and durable structural syntax
        ├── binder → symbols → progressively richer types/checking
        └── validated source edits → JavaScript emission
~~~

Begin with the minimum structural representation that can establish correct
lexical scopes and binding identity for concrete supported features. Do not design
a TypeScript-sized checker object model speculatively. Allow each vertical feature
to teach the next required abstraction. A future transpile-only operation may be
able to omit expensive semantic layers, but the user-facing mode/API is not yet
decided.

Current `Replacement` already represents erase, replacement, and insertion:
`begin == end` is an insertion. Parser erasures are sorted ascending; replacements
from parser and CommonJS passes are applied through `SourceEdit`. Original source
offsets remain usable because edits are ordered from the end of the source.
Non-empty replacements may be adjacent but not overlap, insertions may share a
boundary, and same-position insertion text retains producer order. Invalid ranges,
overlaps, and insertions inside replaced spans are diagnosed before mutation.
Parser erasures remain composable blanking ranges and may overlap safely.

The original `SourceFile` remains authoritative for unchanged bytes; the compact
semantic overlay is becoming authoritative for the bounded meanings it represents;
the validated edit
set should be authoritative for changed bytes. This can reduce pressure to store
formatting trivia or duplicate token text in every semantic node. It is a current
architectural hypothesis—not yet a benchmark-proven final design—that this compact
semantic overlay can remain materially lighter and faster than full AST re-emission.

`Binder` adds the first bounded identity layer over that overlay. It models a root
scope, nested brace scopes, function-body scopes, ordinary function/parameter and
simple variable symbols, `var` promotion to the nearest function scope, lexical
shadowing, and ordinary identifier references. Property names, type positions,
destructuring, arrows, class/member namespaces, imports, and complete JavaScript
hoisting remain outside this first binding contract. Neutral brace regions may
create empty scopes around object literals; because lookup walks outward and no
object-property declarations are introduced, this is conservative for the current
subset rather than a claim that every brace is semantically a block.

CommonJS live-import reference rewriting is the first production consumer of
bound identity. Resolved ordinary local identifiers are excluded from imported
binding rewrites; this fixed function-local `var` shadowing that the previous
range heuristic missed. The range heuristic remains an explicit migration bridge
for arrows, destructuring, catch/loop/class bindings, templates, and other forms
outside the binder's current contract.

The checker is the second bound-identity consumer. Primitive annotation facts are
attached to bound variable symbols, allowing simple identifier initializers and
direct assignments to compare source and target types without textual-name
ambiguity. `TypeModel` now owns canonical program-lifetime type IDs and
symbol-to-type facts. Its initial `TypeStore` deliberately contains only unknown,
number, string, boolean, and bigint identities. This removes checker-private type
identity without pretending to establish unions, literals, objects, functions,
or inference.

The Makefile emits and includes compiler dependency files (`-MMD -MP`). This is a
correctness invariant for incremental native development: semantic-header layout
changes must rebuild every consumer rather than linking ABI-incompatible stale
objects. Clean builds remain part of checkpoints, but ordinary incremental builds
must also be trustworthy.

Avoid two independent grammar implementations. A staged migration may temporarily
leave some constructs represented only by current parser/range machinery and a
bounded subset represented by durable semantic nodes, but both modes should
converge on shared lexical/parsing truth. Unsupported semantic subsets must be
explicit rather than silently producing incorrect binding.

### Architectural pressures and migration bridges

Use these labels deliberately:

- **Proven current invariant:** behavior established directly by source/tests.
- **Architectural pressure:** a limitation likely to require change, but not an
  instruction to refactor immediately.
- **Migration bridge:** temporary or lasting connective code that lets proven
  transpiler behavior consume new semantic information safely.
- **Hypothesis to test:** a promising direction that still requires measurements
  and implementation evidence.

Current pressures include anonymous edit provenance beyond conflict diagnostics,
transform-local shadow reasoning, parallel dependency traversal paths, regex-based
configuration extraction, and per-file opportunistic output writes. A useful
migration bridge will be semantic nodes retaining original source spans so an
existing transformation can consult binder symbol identity while continuing to
produce the proven `Replacement` form. Capture the old heuristic in regressions,
prove equivalence or improvement, then remove it; do not keep an unused semantic
architecture beside the real compiler indefinitely.

Program-wide ownership will eventually need one coherent compilation identity,
but `Program` must coordinate `ModuleGraph`, binder, checker, diagnostics, options,
and emission rather than becoming a monolithic implementation of all of them.

## Compiler driver and source ownership

The top-level driver should validate CLI/options, load sources, invoke compilation,
write completed outputs, report diagnostics, and return truthful status. Feature
semantics should not be split into unrelated CLI special cases.

Map whether compilation is single-file, multi-root, or a shared Program/Project.
That decision controls module graphs, cross-file binding, diagnostics, lifetime,
memory, and future incremental work. Source text may back tokens, ranges, or string
views, so SourceFile ownership must outlive all consumers.

CLI behavior is part of the compiler contract: no arguments, unknown flags,
missing files, unsupported options, multiple files, output paths, diagnostics, and
write failures need deliberate non-zero behavior. Compile fully before writing a
destination. Multi-file partial-output policy must be documented rather than
accidental.

## Lexer model and hazards

The lexer identifies token kind, source range, literal payload, and lexical context
needed by parsing. It should not own high-level semantics, but it must preserve
line-terminator information required by JavaScript grammar.

The supported lexical inventory may include identifiers and keywords, numbers,
strings, templates, comments, operators, punctuation, regex literals, Unicode
escapes, private identifiers, and JSX/TSX modes. Current source defines the actual
set.

High-risk families are:

- ASI-sensitive newlines after return, throw, break, continue, async, yield, and
  around postfix increment/decrement;
- parser-context-sensitive regex versus division;
- escaped and unterminated strings;
- nested template raw/expression state;
- numeric bases, exponents, separators, and bigint;
- Unicode identifier policy and private fields;
- JSX/TSX transitions between TypeScript, markup, and expressions;
- every lexical state at EOF.

Avoid duplicated regex/division heuristics in lexer and parser. If the parser owns
the context decision, make that seam explicit. Trivia can be discarded only after
preserving newlines and location data needed for grammar and diagnostics.

Arbitrary malformed input must produce a result or diagnostic, never an assertion,
hang, out-of-bounds access, or memory corruption. Recursive nesting depth and
scanner-mode restoration deserve adversarial tests.

## Parser and program representation

The parser converts tokens/source into statements, declarations, expressions,
types, patterns, and modules within the supported subset. A hand-written
recursive-descent design is appropriate when grammar ownership and context are
clear.

Map node kinds, ownership, child storage, source ranges, pointer/address stability,
and whether parent pointers exist. Binder or transform references can make a
seemingly local vector/ownership refactor compiler-wide. Location data should not
be discarded without measured justification.

Operator precedence and associativity must be centralized. New assignment,
conditional, comma, or binary syntax cannot be added through isolated branches.
TypeScript ambiguities involving generics, assertions, arrows, async, and JSX
require an understood disambiguation strategy rather than example-driven patches.

Parser context—function, async, generator, loop, switch, class, module, await, and
yield where relevant—must restore on every success and error path. Scoped/RAII
state is safer than scattered save/set/restore code.

Malformed constructs need controlled EOF behavior. Recovery, if present, must have
defined synchronization boundaries; fail-fast can be more trustworthy than a
recovery mechanism that emits cascades.

The current compact range-based representation is not a conventional full typed
AST. Erasure and replacement ranges are valuable precisely because they are
structurally identified. Naive source surgery such as deleting every colon-looking
range would be fragile around comments, nesting, commas, parentheses, and runtime
syntax.

## Type syntax, runtime syntax, and semantic scope

Maintain an explicit distinction:

~~~text
compile-time-only syntax
    annotations, interfaces, aliases, type parameters, type-only imports
    where supported → erase after required analysis

runtime-bearing syntax
    enums, namespaces, parameter properties, decorators, or other constructs
    where supported → real lowering and runtime evidence
~~~

Not every TypeScript construct is “delete it.” The support matrix should classify
each feature as erasure-only, preserved for the target, runtime-lowered,
semantic-checked, parser-only/experimental, or unsupported.

Current tscc has no full TypeScript semantic checker. Validity accepted by
tsc --noCheck but rejected by full tsc is a semantic-only difference, not
automatically a parser defect. Unsupported-valid source should ideally be
distinguished from invalid source, but no ad hoc parser checks should impersonate
a type system.

If a checker is introduced later, type/value namespaces, structural type identity,
inference, assignability, canonical type ownership, and cross-file symbols become
major architecture. That requires an explicit project decision, not gradual
accumulation.

## Binding, scopes, and generated names

Identifier spelling is not binding identity. Shadowed x declarations in outer,
block, function, class, catch, loop, type-parameter, and module scopes may represent
different symbols.

A binder, or whatever current mechanism substitutes for one, must answer which
scope owns a declaration, which declaration each use resolves to, and whether the
use is legal. String-based replacement without binding identity is dangerous for
shadowing, closures, imports, and transforms.

Map:

~~~text
scope hierarchy and ownership
symbol/declaration representation
name-table structure
insertion and lookup
var versus let/const/function/class behavior
closure references
type versus value namespace handling
import/export aliases
~~~

Generated temporaries must be deterministic, correctly scoped, unique against user
bindings, and introduced at the narrowest semantically valid point. A global
counter without scope awareness is both a collision and concurrency hazard.

Binding confidence is foundational: a weak model can poison transforms, closures,
modules, diagnostics, and future type analysis simultaneously.

## Transform and lowering pipeline

Establish whether current transforms mutate ranges/nodes, construct new
representations, lower directly during emission, or use a hybrid. Neither separate
passes nor emitter-driven lowering is inherently superior, but ownership and pass
order must be explicit.

The central invariant is evaluation preservation:

~~~text
same meaningful order
same number of evaluations
same short-circuit behavior
same receiver/this behavior
same exceptions and side effects
~~~

Optional chains, nullish/logical assignment, destructuring, defaults, rest/spread,
classes, async/generators, enums, and namespaces each require feature-specific
semantic slices if supported.

Any lowered subexpression must be tested with calls, counters, getters, computed
properties, assignments, and throwing functions. Optional method calls require
special receiver tests. Nested transforms must compose, and transform order must
not emerge accidentally from overlapping replacement ranges.

Do not duplicate expression text or introduce temporaries farther out than needed.
When transforms overlap, verify that one pass cannot rewrite another pass's
declaration as though it were a runtime reference.

## Emitter model

The emitter is the final semantic boundary, not merely a formatter. It must produce
valid deterministic JavaScript with correct precedence, associativity,
parenthesization, token separation, statement boundaries, strings, and generated
identifiers.

Examples such as multiplication containing addition, subtraction containing a
right subtraction child, and object/function/class expressions at statement start
need centralized precedence/context handling. Adjacent identifiers, numeric dots,
plus-plus, minus-minus, and slash-slash need separators where required.

An explicit semicolon strategy is generally safer than fragile minimal ASI while
the compiler matures. Readable deterministic output improves debugging and
snapshots. Minify++ owns optional final compression; tscc should not duplicate its
risky transformations.

Comment preservation and source-map support must be stated explicitly. If source
maps are absent, do not imply them. If introduced, every transform must preserve
mapping information.

## Modules and project graph

Parsing import/export syntax, resolving modules, and maintaining a multi-file
module graph are distinct capabilities. Determine support for ES modules,
CommonJS, default and aliased exports, re-exports, type-only imports, relative
resolution, extensions, index files, package metadata, tsconfig paths, and
ESM/CJS conditions.

A deliberately bounded resolver—such as relative local files only—can be a valid
contract. Do not accidentally commit tscc to reproducing all Node and TypeScript
resolution behavior. If modules are preserved rather than bundled, circular
runtime semantics can remain the target runtime's concern; bundling would require
a much deeper model.

## Diagnostics and failure behavior

Diagnostics are part of the product. Preserve file, source range or line/column,
message, category/severity where implemented, and non-zero status. Structured
diagnostics are preferable to printing anonymous strings deep inside scanner and
parser code, but architectural cleanup should be evidence-driven.

Distinguish syntax, unsupported feature, semantic, and type failures where the
current product can do so honestly. Exact tsc codes/messages are not a contract
unless deliberately chosen. Test at an appropriate level—status, category/code,
substring, or exact text—without making all wording brittle.

Invalid compilation should not create or partially replace output. Write failure,
missing directory, permissions, and multi-file partial success need explicit
behavior.

## Runtime and differential evidence

Emitted JavaScript that parses or matches a snapshot may still be wrong. Runtime
execution is a first-class oracle:

~~~text
fixture.ts
→ tscc
→ emitted JavaScript
→ Node/target runtime
→ stdout, status, stderr, or structured observable result
~~~

Where TypeScript compatibility is intended, compare the same fixture through a
pinned official tsc configuration and tscc, then compare acceptance and runtime
semantics. Exact emit need not match unless that is a stated goal.

The reference version, target, module mode, noCheck/full-check choice, and other
options must be recorded. The locally available tsc was version 7.0.2 when this
handover was integrated; the regression suite must still define and preferably pin
its own reference rather than inherit whichever global executable happens to be
installed.

Transforms require side-effect, exception, this, super, getter/setter, computed
property, and nested-composition cases as applicable. Runtime equivalence cannot
prove that an invalid program should have been rejected, so negative and
diagnostic tests remain separate.

## Test and suite architecture

Use complementary layers:

1. Focused lexer/parser and structural tests.
2. Binding, scope, semantic, and type tests where those stages exist.
3. Emission snapshots for localized transform visibility.
4. Runtime behavior tests.
5. Official-TypeScript differential tests.
6. Malformed/adversarial input.
7. Representative real projects.
8. ASan/UBSan, parser fuzzing, and later grammar/differential fuzzing.
9. Performance and RSS benchmarks.

The relationship between implementation-local regression and the independent
tscc-regression-suite must be mapped and machine-checked. A healthy split is fast
internal evidence plus an executable-only external contract, but current repository
reality decides canonical ownership.

Large corpus failures should be reduced to focused reproductions. Knowledge
checkpoints that add evidence without changing source are valuable.

## Ownership, determinism, and performance

Compiler memory may simultaneously include source, tokens, program
representation, symbols, types, transformed ranges/nodes, diagnostics, and output.
Map which representations remain live and whether addresses/views are stable
before container changes. Arenas and interning are useful only when present or
justified by profiling.

Prefer per-compilation/program state. Global parser flags, temp counters, symbol
maps, or diagnostics constrain future parallelism. Shared caches need documented
lifetime and thread safety.

Same source, options, and version should normally emit byte-identical output.
Unordered-map iteration, filesystem order, parallel completion, random temporary
names, and timestamps must not determine emitted content.

Correctness precedes optimization. Measure startup, I/O, lexing, parsing, binding,
checking, lowering, emission, total time, RSS, and output size as applicable.
Test both large files and many small files. Watch for repeated linear declaration
searches, parent-scope rescans, vector-front insertion, quadratic concatenation,
and repeated subtree emission.

Comparisons with tsc must disclose differences in checking, target, module mode,
process startup, and cache state. A native transpiler beating full tsc checking is
not automatically meaningful.

## Debug and benchmark artifacts

Dotfiles, shadow-debug output, probe files, and benchmark JSON must be classified
through source references, scripts, Git history, and documentation before removal.
They may be permanent diagnostic infrastructure, retained checkpoint evidence,
fixtures, or accidental investigative residue.

Useful debug infrastructure should be documented and made reproducible. Unused
session residue should not fossilize merely because its origin is uncertain.

## Architecture invariants and warning signs

- Product scope and compatibility claims are explicit.
- Syntax recognition is not feature completion.
- Binding identity differs from identifier spelling.
- Type-only erasure differs from runtime lowering.
- Evaluation count, order, short-circuiting, receiver, and exceptions are sacred.
- Generated names are deterministic and binding-safe.
- Emission is precedence- and token-boundary-aware.
- Unsupported source fails rather than being silently corrupted.
- Compiler output is deterministic.
- Runtime semantics outrank emitted spelling.
- Official TypeScript is a pinned reference, not behavior recalled from memory.
- Minify++ owns optional compression and Nift owns website generation.
- Performance work follows correctness evidence.

Immediately scrutinize parser-driven output surgery, textual-name transforms,
duplicated expression emission, scope-unaware temp counters, child emission without
precedence, duplicated feature rules across parser/emitter, naive type-range
deletion, unordered emitted order, assertions on malformed input, broad
all-TypeScript efforts, and website claims based on one parse test.

Good changes form a complete slice: grammar representation, explicit
binding/semantics, coherent lowering, safe emission, runtime evidence, and negative
neighbors. If a difficult feature remains unreliable, explicit rejection and a
truthful production subset may be the strongest decision.

## Required repository reconciliation

Before substantial development, produce a current map with files, key types and
functions, ownership/lifetimes, tests, hazards, and maturity for:

~~~text
CLI/compiler driver and source loading
SourceFile ownership and location mapping
Lexer tokens, trivia/newlines, templates, regex, Unicode and JSX modes
Parser entries, precedence, type grammar, contexts and errors
Program/AST/range representation and stable references
scopes, symbols, declarations, lookup and shadowing
current semantic/type work and explicit absence
erasure/lowering passes, order and overlap handling
temporary-name generation and hygiene
emitter precedence, tokens, semicolons and strings
module syntax, resolution and multi-file graph
diagnostic structure and status propagation
output commit and partial-failure semantics
local versus standalone regression ownership
runtime and tsc differential harnesses
benchmark commands and retained artifacts
debug/probe infrastructure
sanitizer/fuzz tooling
website compatibility claims
~~~

Trace these programs through the live compiler:

1. Simple JavaScript arithmetic from CLI to emitted file.
2. A simple type annotation and the exact point where it disappears.
3. Nested lexical shadowing and binding identity.
4. One implemented runtime-bearing transform.
5. Side-effectful and this-sensitive variants of that transform.
6. Invalid syntax through EOF diagnostic, status, and output behavior.
7. One supported module through parse, graph/resolution, and emission.
8. One fixture through pinned tsc and tscc runtime comparison.

Explicitly report where this handover is stale. Legitimate outcomes include no
binder, transpile-only semantics, emitter-driven transforms, broader feature
support, intentional debug artifacts, a canonical independent suite, or current
website claims ahead of evidence. Do not alter the compiler until the reason for a
disagreement is understood.

## Living production roadmap

Production readiness is scoped: publicly claimed features must work end-to-end,
preserve observable semantics under meaningful runtime testing, reject invalid or
unsupported inputs controllably, compile representative projects, pass native
safety checks, and have repeatable release validation. It need not clone every
TypeScript feature unless that mission is explicitly chosen.

The current inherited sequence is:

~~~text
repository and architecture reconciliation
→ exact compatibility contract
→ feature support matrix across parse/bind/semantic/emit/runtime/negative evidence
→ close existing semantic holes
→ binding and scope audit
→ transform side-effect/temp/this audit
→ emitter precedence/token/semicolon audit
→ independent regression strengthening
→ pinned TypeScript differential
→ real-world projects
→ malformed/adversarial audit
→ sanitizers and fuzzing
→ measured performance and memory work
→ CLI/project/output hardening
→ documentation and website reconciliation
→ clean release candidate
~~~

Revise this sequence whenever evidence changes risk. A generated-name collision,
binder weakness, transform semantic failure, important real-project gap, reference
divergence, sanitizer finding, or measured hotspot should reorder the work rather
than being deferred behind a stale feature plan.

The compressed mental model is: tscc builds enough structure to understand the
supported source semantics, binds names correctly, erases or lowers constructs
without changing behavior, emits deterministic precedence-safe JavaScript, and
diagnoses everything outside its contract rather than silently generating the
wrong program.

Keep this document living. If binding, modules, checking scope, transforms,
compatibility goals, suite ownership, or production evidence changes, update it
and the corresponding tests, decisions, roadmap, and public claims.

## Architectural review questions

- Which component owns this syntax and its source range?
- Is the behavior erasure-only or runtime-bearing?
- Can transforms overlap or rewrite declarations as runtime references?
- What binding identity applies under shadowing?
- Can an operand be evaluated more than once or in a different order?
- Is this parser validity, semantic validity, or unsupported behavior?
- Does project/module logic need the feature, or only single-file emission?
- What external test proves the whole slice?
