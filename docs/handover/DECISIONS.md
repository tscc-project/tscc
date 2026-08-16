# tscc decision ledger

## Independent product

**Status:** SETTLED

tscc is not Nift infrastructure and should not be forced into a shared runtime or
product suite.

## Experimental TypeScript-to-JavaScript positioning

**Status:** CURRENT

The project is not currently documented as a complete drop-in `tsc` replacement
and has only a first bounded primitive-initializer checker slice. Compatibility
promises must remain explicit.

## Feature support definition

**Status:** SETTLED ENGINEERING PRINCIPLE

Parser acceptance is not feature support. Required semantic stages and external
evidence determine support.

## Vertical semantic slices

**Status:** STRONG/SETTLED DIRECTION

Prefer complete end-to-end features over broad grammar accumulation.

## TypeScript reference

**Status:** CURRENT METHODOLOGY

Use `tsc --noCheck` for syntax/emit reference and full `tsc` for semantic
classification where compatibility is intended. Reference behavior is not
automatically the specification for deliberate divergences.

## Semantic type checker

**Status:** ABSENT / FUTURE PRODUCT DECISION

Do not fake semantic restrictions through parser heuristics. Decide checker scope
and architecture deliberately before implementation.

## TSX

**Status:** CURRENT

`--jsx preserve` strips supported TypeScript while retaining JSX for a later tool;
it is not a JSX runtime transform.

## CommonJS

**Status:** CURRENT, SUBSTANTIALLY HARDENED

The compiler performs runtime CommonJS transforms including live imported reads,
re-exports, aliases, and shadowing cases represented by current tests. Continue
to judge completeness by runtime/differential evidence.

## Module resolution scope

**Status:** CONSTRAINED CURRENT DIRECTION

Relative project discovery/resolution belongs; silently becoming a complete
package-manager/module-resolution clone does not without a deliberate decision.

## Diagnostics compatibility

**Status:** UNRESOLVED IN DETAIL

Precise attributed tscc diagnostics matter. Exact `tsc` wording/codes are not
currently established as a promise.

## Performance claims

**Status:** SETTLED QUALIFICATION

Keep `tscc`, `tsc --noCheck`, and full `tsc` separate and label results as local
checkpoint comparisons, not equivalent-work universal superiority while tscc's
scope is narrower.

## Unknown debug/benchmark artifacts

**Status:** VERIFY BEFORE CLEANING

Classify through history/scripts/references; do not delete by appearance.

## Public actions

**Status:** SETTLED WORKING RELATIONSHIP

Prepare locally; commits, pushes, tags, releases, deployments, and public
compatibility/version changes require explicit direction.
