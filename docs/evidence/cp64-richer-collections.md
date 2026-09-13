# CP64 richer object and collection types

CP64 extends the existing canonical `TypeStore`; it does not add a checker-local
collection representation.

- Tuple identity retains required, optional and rest element flags. Concrete
  tuple arguments spread through variadic generic substitution.
- Mutable and readonly arrays/tuples have distinct canonical identities.
  Mutable collections are assignable to readonly views; the reverse conversion
  and indexed writes through readonly views are rejected.
- Computed reads and writes share `indexed_access`, covering literal properties
  and string, number and symbol index domains.
- `keyof` produces canonical literal/key-domain unions, including numeric and
  symbol keys, and indexed-access annotations resolve through the same model.
- Numeric/string-literal member names are accepted by structural annotations.
- Ordinary object assignment retains the established structural, excess-key
  compatible preview policy. Fresh literals checked with `satisfies` reject
  unknown keys unless an applicable index signature admits them.

`tests/richer_collections.sh` supplies end-to-end positive and negative compiler
coverage. `tests/type_model_smoke.cpp` directly proves canonical identity,
readonly variance, tuple flags, symbol indexes, indexed access and `keyof`, so
unknown-expression fallback cannot manufacture a pass. The complete pinned
regression wall remains 555 pass, zero fail and 24 deliberate skips under Node
22.22.1 and TypeScript 7.0.2. The full optimized and ASan/UBSan gates pass.
