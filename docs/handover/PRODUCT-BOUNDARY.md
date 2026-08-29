# tscc and JS++ product boundary

**Status:** settled at CP1 on 2026-08-30.

## Ownership

tscc owns TypeScript and supported JavaScript compiler input, compiler-quality
source locations and recovery, declarations, scopes, symbols, binding, types,
inference, narrowing, diagnostics, project graphs, module resolution,
configuration, incremental compilation, TypeScript lowering, JavaScript emission,
source maps, and declaration output.

JS++ owns executable ECMAScript parsing, bytecode, virtual-machine execution,
runtime values, objects, prototypes, lexical environments, closures, exceptions,
garbage collection, jobs, modules, built-ins, the `js` shell, and the public
`js.h`/`libjs` embedding surface.

## Dependency invariants

1. Ordinary tscc compilation does not execute compiler input.
2. Normal tscc build and test-core targets do not require JS++.
3. tscc source does not include JS++ private headers or compile JS++ private
   implementation files.
4. A future JS++ parser/runtime test adapter is an explicit test dependency.
5. A future optional production integration uses only a versioned public API and
   remains separable from ordinary compilation.
6. CommonJS lowering remains tscc-owned because it is a TypeScript compiler
   transform. ECMAScript module runtime semantics remain JS++-owned.
7. JavaScript-facing code does not migrate by name alone. Sharing requires a
   checkpoint proving compatible ownership, losslessness, recovery, diagnostic,
   and extension requirements.

## First integration

The first planned integration is dual-runtime evidence:

```text
TypeScript → tscc → JavaScript → Node reference
                             ↘ JS++ candidate
```

Until an intersection is explicitly promoted, a JS++ mismatch is an engine gap,
not a tscc regression.

## Enforcement

`make test-product-boundary` checks the normal compiler source/build graph for
accidental JS++ headers, private source paths, and `libjs` linkage. Amend that
check deliberately when an accepted integration checkpoint introduces an
explicit adapter; do not weaken it to make an accidental dependency pass.
