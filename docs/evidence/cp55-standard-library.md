# CP55 standard-library contract evidence

TSCC now has a hermetic, versioned library contract. `es2022` is the default
library bundle; `node` is an explicitly selected environment/type bundle.
`--lib`, `--types`, `--noLib` and their JSONC configuration counterparts select
only these compiler-owned definitions. Unknown bundle names are diagnostics;
the compiler never searches ambient host state.

The first bounded bundles establish canonical structural identities for Date,
RegExp, Error, Buffer and ProcessEnv. Broader APIs and generic library identities
remain evidence-driven work after CP56 introduces generic substitution.

`tests/standard_library.sh` proves default-library checking, explicit no-library
compilation, CLI/config type selection and rejection of host-dependent names.
