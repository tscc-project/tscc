# CP75 multi-member JSX intrinsic contracts evidence

CP75 fixes a pre-existing JSX semantic bug where a member typed `unknown` in a
`JSX.IntrinsicElements` props object silently disabled property checking for the
whole intrinsic tag, regardless of member ordering or count.

## Root cause

`JSX.cpp` derives the props contract with the shared type-annotation parser
(`parse_type_annotation` in `TypeParser.cpp`). That parser maps the unknown
TypeId (`TypeStore::unknown()`, value 0) both to the top type and to its own
parse-failure sentinel, and `AnnotationParser::object()` aborted the whole object
parse whenever a property type resolved to that value. Because the type keyword
`unknown` (and `any`) was not recognized by `atom()`, a member such as
`children?: unknown` made the entire props object parse as `unknown`;
`check_jsx_semantics` then skipped checking every attribute on that intrinsic
tag. The bug was independent of member count, ordering, or `children` naming -
any member typed `unknown`/`any` triggered it.

## Fix

In `TypeParser.cpp` (the shared parser boundary, not a JSX workaround):

- `atom()` now recognizes the `unknown` and `any` keywords, returning the
  unknown TypeId and setting a new `recognized_unknown_` flag.
- `AnnotationParser::object()` resets that flag before each property type and
  no longer aborts the whole object when the property type is the unknown
  TypeId provided the annotation was an explicit `unknown`/`any` keyword.

Genuinely unsupported or malformed type syntax still yields the conservative
unknown result, so the parser's existing failure behavior is unchanged.

## Regression

- `tests/type_parser_smoke.cpp` (new, `test-type-parser`): calls
  `parse_type_annotation` directly on
  `{required:string;maybe?:unknown;children?:unknown;count:number}`, asserts the
  object shape with correct types/optionality, and asserts an unsupported member
  type still conservatively yields unknown. Fails before the fix.
- `tests/type_model_smoke.cpp` (`test-types`): asserts a `type Mixed=...` alias
  with `unknown`-typed members parses to the expected object shape.
- `tests/jsx_intrinsic_contracts.sh` (new, `test-jsx-intrinsic-contracts`,
  part of `make test`): covers one-property contracts, several required
  properties, required+optional, `children?: unknown` in first/middle/last
  positions, a four-property contract, multiple intrinsic element declarations,
  wrong literal and `{identifier}` expression types, missing required props,
  valid optional props, unknown JSX props, the CP74 attribute/local-const
  collision, genuine assignments inside JSX expression containers, and
  malformed JSX. Fails before the fix.
- The CP74 regression (`tests/jsx_attribute_assignment.sh`) continues to pass.