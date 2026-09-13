# CP69 declaration emit and output-integrity evidence

`--declaration` emits `.d.ts` files for the checked declaration subset:
interfaces, type aliases, enums, annotated variables, and function signatures.
Function bodies and variable initializers cannot leak into declarations.
`--declarationMap` implies declaration emit and adds a deterministic v3 sidecar.

All JavaScript, declaration and map outputs are staged before any rename. A
normalized target set rejects project collisions before staging, and a failure
removes staged temporaries. `tests/declaration_emit.sh` checks the complete
artifact set and absence of implementation text.

Class declarations, inferred public variables, re-export synthesis, stale-output
manifests and rollback after an operating-system rename failure remain explicit
limits; declaration emit is useful only for the documented checked subset.
