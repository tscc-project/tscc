# tscc

`tscc` is an experimental standalone TypeScript-to-JavaScript compiler written in C++17. The goal is a small native compiler with a `tsc`-familiar command line, precise diagnostics, and no JavaScript runtime dependency in the compiler itself.

## Checkpoint 0.1

The first checkpoint establishes the architecture rather than claiming TypeScript completeness:

- `Source` owns file text and offset → line/column mapping.
- `Diagnostic` prints file/line/column errors with optional ANSI colour and source carets.
- `Lexer` tokenises identifiers/keywords, numbers, strings/templates, comments and punctuation.
- `Transpiler` performs the first syntax-erasure subset.
- `Compiler` handles multiple inputs and output paths.
- `main.cpp` implements a `tsc`-style CLI surface.

Currently exercised syntax includes `interface` and `type` erasure, common type annotations, return annotations, `as` assertions, postfix non-null assertions, `implements`, `public`/`private`/`protected`/`readonly`/`abstract`, and ordinary JavaScript preservation.

This historical first checkpoint was intentionally not a type checker. Later checkpoints added enums, namespaces, TSX preservation, relative module traversal, CommonJS lowering, and a supported `tsconfig.json` subset; consult the current capability sections and tests rather than treating this chronology as a support matrix.

## Bounded semantic checking slice

tscc now has a separate `Checker` pass and a deliberately narrow type contract. For simple variable declarations, explicit `number`, `string`, and `boolean` annotations are checked against direct primitive literal initializers (including signed numeric literals). Those annotated primitive facts also flow through bound identifier initializers and direct assignments, so `const text: string = "x"; const value: number = text` is rejected with a source-positioned diagnostic.

This is the beginning of type checking, not a general TypeScript type system. A bounded primitive expression grammar covers parentheses, unary operators, additive and multiplicative arithmetic over literals and bound identifiers. The checker also owns const/compound assignment plus typed ordinary-function arguments, bounded arity and annotated returns. Unions, object types, inference, callable variables/function expressions, overloads, generics, logical/conditional expressions and narrowing remain unchecked. The independent 521-case corpus records 495 passes, no failures, and 26 explicit semantic skips.

Static default, namespace, and named value imports now have binder-owned root
symbols. This identity protects ordinary CommonJS live reads from supported local
shadows; imported types intentionally remain `unknown` because tscc does not yet
own module export tables or cross-file type propagation. Compilation also separates
file preparation from output policy: default mode emits successfully prepared
files, `--noEmitOnError` suppresses the whole prepared set after any error, and
`--noEmit` suppresses output unconditionally.

The 17 August 2026 campaign checkpoint passed clean optimized, ASan+UBSan, full
521-case, and clean committed-archive builds. Five-run medians on this machine were
1.575 ms (small), 6.827 ms (100 basic files), 25.529 ms (500 basic files),
8.480 ms (100 feature-heavy files), and 9.391 ms (100 advanced files). These are
dated transpilation-oriented measurements, not equivalent-work or cross-machine
claims.

## CLI

```bash
tscc app.ts
tscc --outDir dist src/a.ts src/b.ts
tscc --target es2022 --removeComments app.ts
tscc --noEmit app.ts
tscc --pretty false app.ts
tscc --version
tscc --help
```

## Build and test

```bash
make
make test
```

The aggregate target runs smoke, parser, checker, runtime, project/module, independent regression, TSX, and CommonJS validation. Focused targets such as `make test-checker` remain available during iteration.


## Parser / AST checkpoint

`tscc` no longer relies on a single pass that erases anything that merely looks like a type annotation. The compiler now has a parser-owned `Program` syntax tree and source ranges for TypeScript-only syntax.

Current parser-owned constructs include:

- interface declarations;
- type aliases;
- function parameter and return annotations;
- local variable annotations inside executable blocks;
- arrow-function parameter/return annotations;
- class `implements` clauses;
- access/readonly/abstract/declare modifiers;
- class field and method annotations.

The syntax tree is intentionally still small. Expression-level `as`/`satisfies` assertions and postfix non-null assertions remain token-level transforms for now. Runtime transforms such as enums, namespaces and constructor parameter properties belong to later AST checkpoints.

An important architectural property is that an object literal such as:

```ts
const user = {name: "Ada", age: 3};
```

is no longer competing with a heuristic "`:` means type annotation" rule. Declaration context determines which colons belong to TypeScript syntax.

Run the focused parser test with:

```bash
make test-parser
```

## Benchmarks

Performance is treated as a regression-tested compiler property from the beginning.

```bash
python3 benchmarks/benchmark.py --runs 20 \
  --json benchmarks/results/latest.json
```

The benchmark corpus currently includes a small program, 100 independent files and 500 independent files. It reports separate rows for:

- `tscc` transpile + emit;
- `tsc --noCheck` emit-only comparison;
- normal `tsc` checking + emit.

The numbers are local-machine measurements, not universal performance claims. `tscc` also supports far less TypeScript today, so the meaningful goal is to watch how its own numbers move as compatibility grows.

A relative performance guard is included:

```bash
python3 benchmarks/check_regression.py \
  benchmarks/results/baseline.json \
  benchmarks/results/latest.json
```

By default it fails if a comparable `tscc` median slows by more than 25%. Benchmarks should grow with language coverage: new major features should add representative fixtures rather than being measured only on the original easy corpus.


## Current local checkpoint snapshot

5-run median on this machine:

| workload | tscc | tsc --noCheck | tsc full |
|---|---:|---:|---:|
| small | 7.146 ms | 869.260 ms | 839.994 ms |
| 100 files | 10.441 ms | 965.042 ms | 938.285 ms |
| 500 files | 25.071 ms | 1247.039 ms | 1403.016 ms |

That is roughly 121.7× / 92.4× / 49.7× faster than `tsc --noCheck` for these fixtures. These are checkpoint measurements, not a general performance claim: `tscc` currently supports a much smaller TypeScript surface than `tsc`.


## v0.3.0 runtime/generics checkpoint

This checkpoint extends the parser beyond pure type erasure into the first runtime-bearing TypeScript transforms.

Supported additions include:

- generic function declarations and calls;
- generic arrows;
- generic classes, methods and `extends` type arguments;
- nested generic closers such as `Promise<Array<string>>`;
- constructor parameter properties (`public` / `private` / `protected` / `readonly`);
- derived parameter-property assignment after `super(...)`;
- numeric, string and heterogeneous enums;
- numeric enum auto-increment after decimal, signed, hexadecimal and binary initializers;
- optional parameters/properties and definite-assignment class fields.

`tests/runtime_features.sh` compiles the same feature fixture with `tscc` and `tsc`, executes both JavaScript outputs with Node and compares their observable output.

### Current local benchmark snapshot

| workload | tscc | tsc --noCheck | tsc full |
|---|---:|---:|---:|
| small | 7.686 ms | 814.101 ms | 874.833 ms |
| 100 basic files | 11.172 ms | 953.004 ms | 1000.306 ms |
| 500 basic files | 24.807 ms | 1261.080 ms | 1395.247 ms |
| 100 feature-heavy files | 13.477 ms | 1112.149 ms | 1346.031 ms |

The feature-heavy fixture exercises generics, classes, parameter properties and enums. On this machine its checkpoint ratio is about **82.5× versus `tsc --noCheck`**. This is still not an equivalent-work claim: `tscc` does not yet implement TypeScript's semantic type checker, module graph, JSX/TSX, namespaces, decorators or downlevel transforms.

The benchmark harness supports `--case` so long-running cases can be measured independently:

```bash
python3 benchmarks/benchmark.py --runs 10 --case features-100
```

The v0.2-to-v0.3 regression guard remains green on comparable fixtures; no common benchmark slowed by the allowed 25% threshold.


## v0.4.0 project/module checkpoint

`tscc` now has an initial project/module layer rather than treating every CLI file as isolated.

Current additions:

- `-p` / `--project tsconfig.json`;
- JSONC-style comments and trailing commas for the currently supported tsconfig fields;
- `compilerOptions.rootDir`, `outDir`, `target`, `module`, `removeComments`, `noEmit`, and `noEmitOnError`;
- `files` arrays, with recursive `.ts` discovery when `files` is omitted;
- static relative `import` / `export ... from` dependency discovery;
- `.js` source specifiers resolving to sibling `.ts` inputs, so runtime-correct ESM specifiers can remain unchanged in emitted JS;
- directory-preserving output under `outDir`;
- cycle de-duplication;
- attributed unresolved-relative-module diagnostics;
- `--noResolve`;
- erasure of `import type`, `export type`, and exported interfaces/type aliases.

The module syntax itself is currently preserved; CommonJS rewriting is intentionally not being faked before a proper module transform exists.

Compilation now has separate preparation and output-commit phases. Every input is
processed independently before output policy is applied, so the order of valid and
invalid files cannot suppress later diagnostics or valid preparation. By default,
tscc follows an emit-on-error policy and writes the successfully prepared files
while still returning a failing status. Pass `--noEmitOnError`, or set
`compilerOptions.noEmitOnError` to `true`, to write no files when any input has an
error. `--noEmit` remains stronger and never writes output.

The compiler initially built the entire module graph and then lexed/transpiled in a second phase. Benchmarking showed that architecture hurt the many-file baseline, so v0.4 switched to a streaming work queue: each source is loaded and lexed once, relative dependencies are discovered from that token stream, the source is transpiled immediately, and unseen dependencies are queued.

### Project benchmark

A 200-module ESM chain has been added to the benchmark corpus.

Current local checkpoint measurements:

| workload | tscc | tsc --noCheck | tsc full |
|---|---:|---:|---:|
| small | 7.302 ms | 827.369 ms | 835.431 ms |
| 100 basic files | 13.163 ms | 915.944 ms | 1068.156 ms |
| 100 feature-heavy files | 22.227 ms | 1168.566 ms | 1237.028 ms |
| 200-module project | 14.496 ms | 1001.207 ms | 1647.545 ms |

The 200-module checkpoint is about **69× faster than `tsc --noCheck`** on this machine. As before, this is not an equivalent-work claim: `tscc` still lacks semantic type checking and substantial TypeScript/compiler functionality.

The project benchmark is available separately:

```bash
python3 benchmarks/project_benchmark.py --runs 10
```


## v0.4.1 regression-hardening checkpoint

The independent compiler-agnostic regression corpus from the parallel audit is now part of the normal workflow under `regression/`. Its 78 cases are checked against `tsc` as the reference parser/emitter contract.

The first v0.4.0 run exposed several genuine parser/transpiler bugs that the smaller source-tree tests had missed. After root-cause fixes, the corpus is **72 pass / 0 fail / 6 deliberate semantic-checker skips**. The older runtime differential suite then caught one regression introduced during those fixes (binary enum auto-increment), which was corrected before the checkpoint was packaged.

Use:

```bash
make test-regression
```

This target now runs alongside parser, runtime/module integration and performance benchmarks before new language features are accepted.


### v0.4.1 performance guard

The regression-hardening changes did not compromise the performance objective. Current local checkpoint medians from the same benchmark harness were approximately 2.17 ms for the small fixture, 4.63 ms for 100 basic files, 7.44 ms for the 100 feature-heavy files, and 6.67 ms for the 200-module project. The 200-module project remained about **68× faster than `tsc --noCheck`** on this machine. These remain checkpoint measurements rather than equivalent-work claims because semantic type checking is not implemented yet.


## v0.5.0 language-hardening checkpoint

The independent regression corpus has grown from **78 to 216 cases**. The harness now batches reference `tsc`/`tsc --noCheck` validation, so the entire corpus can keep growing without paying hundreds of TypeScript startup costs per checkpoint. Current result: **198 pass / 0 fail / 18 deliberate semantic-checker skips**.

This checkpoint adds or hardens:

- angle-bracket type assertions;
- chained postfix non-null assertions;
- function, method and constructor overload signatures;
- ambient `declare` declarations;
- `override` and compile-time `this` parameters;
- `const enum` handling;
- richer enum constant evaluation including parentheses, arithmetic, shifts, bitwise operators, fractions and exponentiation;
- basic/nested namespaces with exported variables, functions and enums;
- optional class methods and richer class declarations;
- contextual generic-close handling for source such as `B<number>=value`;
- stricter malformed interface/type/generic/assertion/enum recovery.

The suite also retained several cases that initially looked like parser errors but `tsc --noCheck` accepts and normal `tsc` rejects. Those stay classified as future semantic-checker regressions rather than being counted as `tscc` parser defects. No confirmed `tsc` correctness bug was found in this checkpoint.

### v0.5.0 performance snapshot

Two-run local medians for the comparable fixtures were:

| workload | tscc | tsc --noCheck | tsc full |
|---|---:|---:|---:|
| small | **1.20 ms** | 310.07 ms | 372.06 ms |
| 100 basic files | **4.97 ms** | 422.11 ms | 455.69 ms |
| 100 feature-heavy files | **6.85 ms** | 546.65 ms | 562.15 ms |
| 100 advanced files | **8.32 ms** | 575.94 ms | 671.04 ms |
| 200-module project | **6.36 ms** | 421.65 ms | 855.57 ms |

The new `advanced-100` fixture specifically exercises namespaces, overloads, abstract/override classes, angle assertions, `satisfies`, and fractional/exponent enum constants. It is about **69× faster than `tsc --noCheck`** on this machine at this checkpoint. The v0.4.1→v0.5 regression guard remains green on all comparable fixtures. These remain checkpoint measurements, not equivalent-work claims while semantic type checking and other major compiler features are still absent.


## v0.6.0 advanced grammar/project checkpoint

The independent regression gate grows from **216 to 265 cases** and is green at **246 pass / 0 fail / 19 semantic-only skips**. Added coverage includes mapped/conditional/template-literal/indexed/variadic types, `const` type parameters, destructured typed parameters, class static/accessor forms, ambient enums/classes/functions, namespace merging/dotted namespaces, instantiation expressions, generic expression references and more malformed advanced-type grammar.

Project mode now supports mixed type-only named specifiers (`import {type T, value}` / `export {type T, value}`), `export type *`, and practical `tsconfig.json` `include`/`exclude` glob discovery.

Local checkpoint medians: small 1.18 ms, 100 basic files 5.81 ms, 100 advanced files 8.52 ms, and the 200-module project 6.80 ms. The project remains about 65x faster than `tsc --noCheck` on this machine; these are checkpoint measurements, not equivalent-work claims while semantic checking remains unimplemented.


## v0.6.1 regression-hardening checkpoint

The independent differential corpus now contains 273 cases. This checkpoint added default generic parameters, readonly-array types, constructor types, type predicates/assertion signatures, and malformed counterparts. Three new malformed forms were initially accepted by `tscc`; parser validation was tightened and the final gate is **254 pass / 0 fail / 19 semantic-only skips**.

The regression runner also forces colour off for Node subprocesses so Node's evolving default console colouring cannot create false runtime mismatches.


## v0.7.0 TSX preserve checkpoint

`tscc` now has an initial TSX path with `--jsx preserve`, deliberately matching the TypeScript mode that strips TypeScript syntax while retaining JSX for a later JSX-aware tool.

```bash
tscc --jsx preserve src/App.tsx --outDir dist
```

A `.tsx` input emits `.jsx`. Angle-bracket type assertions are disabled in TSX context, because TypeScript reserves `<...>` for JSX/generic syntax there. The TSX structural validator checks nested opening/closing tags, fragments and `{...}` expression boundaries, and rejects malformed tag nesting instead of emitting broken JSX.

Project mode also understands:

```json
{
  "compilerOptions": {
    "rootDir": "src",
    "outDir": "dist",
    "jsx": "preserve"
  }
}
```

Relative runtime imports such as `./View.jsx` can resolve to `View.tsx` during compilation while the emitted import remains `./View.jsx`.

The independent regression corpus now contains **283 cases**:

```text
264 pass
0 fail
19 semantic-checker-only skips
```

Ten TSX cases cover basic preservation, fragments, typed arrow components, `as` expressions inside JSX, generic JSX components, spread attributes, nested ternaries and malformed TSX.

A dedicated 100-file TSX benchmark is included:

| workload | tscc | tsc --noCheck | full tsc |
|---|---:|---:|---:|
| 100 TSX files (`jsx: preserve`) | 6.134 ms | 547.762 ms | 579.377 ms |

That checkpoint is about **89× faster than `tsc --noCheck`** on this machine. As with the other benchmarks, this is not an equivalent-work claim while `tscc` has no semantic type checker.


## v0.8.0 regression/runtime-module checkpoint

The independent compiler gate now contains **331 cases**:

```text
309 pass
0 fail
22 semantic-checker-only skips
```

This checkpoint broadened TSX and advanced type grammar, then strengthened two TypeScript-specific module forms into executable runtime tests.

New/fixed areas include:

- nested JSX after `??`, `&&`, `||`, optional-call and expression-prefix boundaries;
- the TSX ambiguity between `<T>(...) => ...` and an opening JSX element;
- `declare global { ... }` erasure without swallowing following runtime code;
- `export as namespace` declaration-only erasure;
- `import name = require("pkg")` lowering to executable `const name = require("pkg")`;
- `export = value` lowering to `module.exports = value`;
- empty `keyof`/other type-operator operands in annotation positions;
- missing generic `extends` constraints;
- malformed `typeof import()` type queries;
- labelled tuples, mapped/conditional/import-query types, overloads and additional class/interface forms.

The regression suite continues to use `tsc --noCheck` as the syntax authority and full `tsc` only for semantic-only classification. Cases accepted by TypeScript are reclassified rather than counted as `tscc` bugs; for example `X[ ]` is valid TypeScript syntax.

TSX preserve remains intentionally a preservation mode, not a JSX runtime transform. A later compiler/tool can consume the emitted `.jsx`.


## v0.9.0 CommonJS checkpoint

`tscc` now has an initial executable CommonJS module transform:

```bash
tscc --module commonjs src/main.ts --outDir dist
```

or in `tsconfig.json`:

```json
{
  "compilerOptions": {
    "module": "commonjs"
  }
}
```

The current transform covers:

- side-effect imports;
- default imports;
- namespace imports;
- named imports and aliases;
- mixed named type/runtime imports;
- local `export const` / `let` / `var`;
- exported functions and classes;
- named export lists and aliases;
- named re-exports;
- `export *`;
- named and anonymous default function/class/expression exports;
- TypeScript `import name = require("...")`;
- TypeScript `export = value`.

Exported local variables use getters on `exports`, so mutation of the exporting module's local binding is visible through the exported property.

A dedicated multi-file runtime differential test compares observable output with `tsc --module commonjs`, including default/named/namespace imports, re-exports, `export *`, default declarations, `export =`, side-effect imports and a circular dependency.

### Current CommonJS boundary

This is intentionally called an **initial** CommonJS transform. Named/default ES imports are currently materialized as local values when the importing module executes. That means the most demanding ESM live-import case—where another module mutates an exported binding *after* it has been imported—does not yet have fully equivalent live-binding behavior. The transform is not presented as complete until that is addressed.

The compiler regression corpus now contains **367 cases**:

```text
345 pass
0 fail
22 semantic-checker-only skips
```

This checkpoint added CommonJS emit/runtime cases, deeper TSX generic-arrow ambiguity tests, regex literals containing JSX-looking angle brackets, ambient exported declarations, declaration grammar, namespace merges and further malformed counterparts.

### CommonJS performance

A new 200-module CommonJS project benchmark is included:

| workload | tscc | tsc --noCheck | full tsc |
|---|---:|---:|---:|
| 200-module CommonJS project | 8.983 ms | 667.427 ms | 618.347 ms |

That checkpoint is about **74× faster than `tsc --noCheck`** on this machine. As with every current performance table, this is not an equivalent-work claim while `tscc` still lacks TypeScript's semantic checker and full compiler surface.


## v0.10.0 live CommonJS checkpoint

The CommonJS transform now preserves live reads for default and named ES imports instead of snapshotting them at module initialization.

Conceptually:

```ts
import read, {value, bump} from "./state.js";

console.log(value, read());
bump();
console.log(value, read());
```

is emitted using module-object property reads, so the second `value` observes the mutation performed by `bump()`.

Imported bindings are also context-aware enough to preserve ordinary JavaScript shadowing:

```ts
import {value} from "./state.js";

function f(value: number) {
    return value + 1;      // local parameter
}

{
    const value = 20;      // block-local
    console.log(value);
}

console.log(value);        // live imported binding
```

Catch parameters and object-literal shorthand are covered as well. Shorthand such as `{value}` becomes an explicit key/value pair when the value is a live imported property, while ordinary function argument lists are not confused with object shorthand.

The live-binding differential fixture compiles and runs the same multi-module program with `tscc` and `tsc --module commonjs`, including mutation after import and shadowing.

This checkpoint also fixed a cross-feature parser bug exposed by the live-binding test: TypeScript annotations inside a typed arrow following a block or `try/catch` could escape erasure. Generic statement ranges now walk nested/sequential runtime syntax instead of only looking for expression type arguments.

The independent regression corpus now contains **385 cases**:

```text
363 pass
0 fail
22 semantic-checker-only skips
```

New cases cover live CommonJS import references, object shorthand, argument-list counter-cases, parameter/block/catch shadowing, block/try/switch/do runtime type erasure and malformed nested annotations.

### Performance

Current local medians:

| workload | tscc | tsc --noCheck | full tsc |
|---|---:|---:|---:|
| 100 TS files | 5.408 ms | 459.391 ms | 521.514 ms |
| 100 TSX files | 5.962 ms | 548.647 ms | 560.215 ms |
| 200 CommonJS modules | 8.924 ms | 547.999 ms | 660.260 ms |

The CommonJS fixture remains about **61× faster than `tsc --noCheck`** on this machine. This is still not an equivalent-work claim while semantic type checking and portions of the TypeScript compiler surface remain unimplemented.


## v0.11.0 CommonJS completeness checkpoint

This checkpoint pushes the CommonJS transform further toward normal TypeScript/ES-module behavior rather than adding another large subsystem.

The independent regression corpus now contains **400 cases**:

```text
377 pass
0 fail
23 semantic-checker-only skips
```

New CommonJS coverage includes:

- object and array destructured exports;
- `import { default as name }`;
- `export { default as name } from ...`;
- `export * as namespace from ...`;
- local `export {x as default}`;
- live imported references inside template-literal interpolations;
- computed property keys;
- object-literal shorthand versus function-argument counter-cases;
- destructuring property keys that must not be rewritten as imported bindings;
- live re-export chains;
- syntax validation for malformed namespace re-exports.

A multi-file runtime differential fixture now checks destructured exports, namespace re-exports, named-default specifiers and mutations observed through a re-export chain against `tsc --module commonjs`.

One useful bug involved an imported name being reused as the imported *specifier* in a later import declaration. The live-binding reference pass initially rewrote that later import clause and overlapped the module transform. Import declarations are now treated as transform-owned syntax rather than runtime references, while named local exports explicitly resolve imported live expressions when needed.

The transform also now handles template literal uses such as:

```ts
import {value} from "./state.js";
console.log(`${value}`);
```

without snapshotting or leaving an undeclared local identifier.

### Performance

Current local medians:

| workload | tscc | tsc --noCheck | full tsc |
|---|---:|---:|---:|
| 100 TS files | 4.995 ms | 442.776 ms | 433.288 ms |
| 100 TSX files | 5.526 ms | 439.275 ms | 489.953 ms |
| 200 CommonJS modules | 8.239 ms | 552.868 ms | 559.843 ms |

The 200-module CommonJS fixture remains about **67× faster than `tsc --noCheck`** on this machine. As before, this is a checkpoint comparison rather than an equivalent-work claim while semantic type checking and some compiler surface remain absent.


## v0.12.0 module-context checkpoint

The independent regression corpus now contains **431 cases**:

```text
404 pass
0 fail
27 semantic-checker-only skips
```

This checkpoint concentrated on CommonJS context correctness rather than adding another large subsystem. Fixes include `for`-loop bindings shadowing live imports, coordinated rewriting of multiple imported bindings inside one template literal, and stricter static import/export grammar validation. The CommonJS differential now additionally covers live aliases through re-export barrels, namespace re-exports, mutation after re-export, template-literal live reads and loop shadowing.

Current local benchmark medians:

| workload | tscc | tsc --noCheck | ratio |
|---|---:|---:|---:|
| 100 TS files | 4.703 ms | 455.430 ms | 96.8x |
| 100 TSX files | 5.651 ms | 433.109 ms | 76.6x |
| 200 CommonJS modules | 7.551 ms | 505.424 ms | 66.9x |

These remain performance checkpoints, not equivalent-work claims while semantic type checking and parts of the TypeScript compiler surface are absent.


## v0.13.0 scope/shadowing hardening checkpoint

The independent compiler regression corpus now contains **454 cases**:

```text
427 pass
0 fail
27 semantic-checker-only skips
```

This checkpoint expanded CommonJS import-shadowing and module-grammar coverage, generic object methods, nested runtime type erasure, dynamic import/import.meta preservation, and further TSX/generic-arrow cases. Confirmed bugs fixed include destructured `for...of` live-import shadowing and generic object methods whose type parameters/annotations could leak into emitted JavaScript. Malformed static import/export aliases are rejected only after `tsc --noCheck` confirms the syntax is invalid.

Current local benchmark medians:

| workload | tscc | tsc --noCheck | ratio |
|---|---:|---:|---:|
| 100 TS files | 5.152 ms | 415.022 ms | 80.55× |
| 100 TSX files | 6.265 ms | 448.592 ms | 71.60× |
| 200 CommonJS modules | 9.827 ms | 511.315 ms | 52.03× |

These remain transpilation-oriented checkpoint comparisons, not equivalent-work claims while tscc lacks TypeScript's semantic checker.


## v0.15.0 TSX/import-attributes checkpoint

The independent regression corpus now contains **511 cases** with **483 pass / 0 fail / 28 semantic-only skips**. This checkpoint expanded TSX generic-arrow contexts, enums/namespaces/parameter properties, `satisfies`/`as const`, mapped/conditional/template-literal types, import attributes, and malformed counterparts. It fixed TSX generic arrows with `extends` inside existing JSX expressions and prevented import-attribute objects such as `with {type: "json"}` from being misclassified as named import specifiers. Malformed `with` clauses are now rejected explicitly.

## v0.14.0 live-binding/module-grammar checkpoint

The regression corpus first grew to **481 cases** with **454 pass / 0 fail / 27 semantic-only skips**. New CommonJS cases attacked update/compound assignment, computed properties, destructuring defaults, default/rest/destructured parameter shadowing, destructured exports, namespace/default re-exports and malformed module clauses. Fixes included control-flow parentheses no longer being mistaken for parameter scopes, destructured parameter shadowing, label references after `break`/`continue`, and stricter namespace/default module grammar.
