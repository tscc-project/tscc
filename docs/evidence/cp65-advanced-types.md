# CP65 bounded advanced type operators

CP65 adds advanced type operations to the canonical type arena and generic
substitution path. It does not introduce a second annotation evaluator in the
checker.

Implemented behavior:

- normalized intersections, including structural object merging and reduction
  of disjoint primitive intersections to `never`;
- deferred `keyof` and indexed-access identities that reduce after generic
  substitution;
- homomorphic and finite literal-key mapped types with optional/readonly
  modifier preservation, addition and removal;
- conditional types with naked-parameter distribution and bounded `infer` for
  structural patterns such as array element extraction;
- finite template-literal expansion over literal unions;
- `Partial`, `Required`, `Readonly`, `Pick`, `Omit`, `Record`, `Exclude`,
  `Extract`, `NonNullable` and `ReturnType` on the same primitives.

Safety boundaries are executable: recursive substitution stops after 64 levels,
and combinatorial operator surfaces admit at most 256 members. Template
interpolation that cannot be finitely enumerated widens to `string`; other
unsupported reductions return the conservative unknown type.

`tests/advanced_types.sh` covers positive and negative compiler behavior for all
four operator families and representative utilities. `tests/type_model_smoke.cpp`
directly proves intersection identity/reduction, conditional selection and
template expansion. The complete pinned regression result remains 555 pass,
zero fail and 24 deliberate skips under Node 22.22.1 and TypeScript 7.0.2. The
optimized and ASan/UBSan gates pass.
