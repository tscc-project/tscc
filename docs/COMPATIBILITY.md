# tscc compatibility matrix — CP72

This document is the public contract for the 2026-09-13 compiler-preview
snapshot. “Partial” means accepted behavior is useful and tested but must not be
read as complete TypeScript compatibility.

| Area | Supported | Partial / explicit limit |
|---|---|---|
| Target | ES2022 | No downlevel emit |
| Modules | preserve, esnext, CommonJS; relative and bounded Node resolution | package exports/conditions and project references excluded |
| Primitive checking | literals, unions, nullish values, assignment, calls | TypeScript's full relation/inference excluded |
| Functions | contextual callbacks, generics, bounded inference, overloads | recursive/high-complexity inference capped |
| Objects/collections | interfaces, aliases, classes, arrays/tuples, indexes, access | broad freshness and every class rule excluded |
| Advanced types | bounded mapped/conditional/template/intersection utilities | key remapping and recursive aliases excluded |
| Runtime TS | enum value/member typing; parameter-property class typing; enum/namespace/parameter-property lowering | namespace export typing and cross-module runtime-TS facts partial |
| JSX | TSX structure and preserve emit; same-unit `JSX.IntrinsicElements`/component props | spreads, `ElementClass`, cross-file augmentation and runtime transforms excluded |
| Output | JS/JSX, line source maps, bounded declarations/maps, transactional collision preflight | rewritten-token column maps, class/inferred declarations, stale manifest excluded |
| Incremental | build identity and unchanged-output retention | parse/check reuse, invalidation, cancellation and watch excluded |
| Configuration | JSONC `tsconfig` subset documented by the contract | new CP68–70 output switches are CLI-only in this snapshot |

The executable regression contract is 579 cases: **555 pass, 0 fail, 24
intentional semantic skips**, against Node 22.22.1 and TypeScript 7.0.2. Linux
and macOS run the full suite in CI; Windows runs the native compiler-core gate.
No browser, Node, TypeScript, or JS++ runtime is required by the compiler itself.

## Release decision

Keep the **Compiler Preview** designation. The production-transpiler and
production-checker milestones are not yet earned: the remaining semantic skips,
partial namespace/cross-module facts, non-column-exact replacement maps,
incomplete declaration surface, and lack of semantic incremental/watch service
are material. CP72 is therefore a decision/evidence checkpoint, not permission
to publish a release or claim drop-in `tsc` compatibility.
