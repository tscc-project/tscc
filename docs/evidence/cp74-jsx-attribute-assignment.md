# CP74 JSX attribute assignment classification evidence

CP74 fixes a correctness false positive discovered while building the CP73
Valgrind workload: a JSX attribute whose name matches a local `const` was
misinterpreted by the general assignment scan as an assignment to that binding.

## Root cause

`Checker.cpp` scans the flat token stream for assignment operators and treats
every `identifier = ...` shape as an assignment. JSX attributes
(`<panel title="t" count={count} />`) expose the same shape: `name = value`
inside an opening element tag. When the attribute name resolved to an in-scope
`const`, the scan reported "Cannot assign to 'count' because it is a constant."
on valid TSX.

## Fix

A conservative collector (`collect_jsx_attribute_equals`) mirrors the JSX
element detection used by the semantic checker, records the `=` token positions
that are JSX attribute separators, and descends into `{...}` attribute-value
containers so nested JSX elements are classified too. The general assignment
scan, the assignment-facts narrowing scan, and the definite-assignment scans
all ignore those positions. It is gated to `.tsx` sources, is name-agnostic, and
does not touch the JSX semantic checker or const-assignment rejection.

## Regression

`tests/jsx_attribute_assignment.sh` (Make target `test-jsx-attribute-assignment`,
part of `make test`) fails before the fix and covers: attribute name identical to
a local `const`; several colliding attributes; `{value}` expression references
still type checked; a colliding attribute's literal value still type checked;
genuine const reassignment still rejected; a real assignment inside a JSX
expression container still rejected; intrinsic property type checking
(including missing required props) unchanged; malformed JSX diagnostics
unchanged; and nested JSX inside an attribute-value container classified
correctly.

## Known limitation discovered (pre-existing, out of scope)

JSX intrinsic property type checking in `JSX.cpp` only reliably applies to the
members of a multi-member `IntrinsicElements` interface shape in some cases; the
CP74 workload and regression use interfaces for which the parsed props contract
behaves deterministically. A deeper audit of the JSX type-annotation parser is
explicitly future work.