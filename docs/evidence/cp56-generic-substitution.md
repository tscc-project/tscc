# CP56 generic identity and explicit substitution

TSCC now retains canonical type-parameter identities for generic call
signatures, aliases and interfaces. Parameters carry ordered constraints and
defaults. Explicit call-site type arguments and named generic type applications
substitute recursively through functions, objects, arrays, tuples and unions;
substitution has a fixed depth guard.

The production checker diagnoses excessive type arguments and violated
constraints. Defaulted parameters are instantiated deterministically. Class
generic syntax continues to retain parser identity, while constructor/static and
instance-side class typing deliberately remains CP59 rather than creating a
temporary parallel class model.

`tests/generic_substitution.sh` covers explicit function substitution, generic
aliases/interfaces, defaults, constraints and arity diagnostics.
