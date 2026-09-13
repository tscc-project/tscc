# CP58 overload sets and resolution

Function declarations with the same name and lexical scope now form one ordered
public overload set. The type model retains each declared signature, while an
implementation signature is excluded whenever bodyless overload declarations
exist. A function without overload declarations continues to expose its single
implementation signature.

Call checking instantiates generic candidates independently, rejects candidates
whose arity, explicit type arguments, inferred constraints or argument types do
not apply, and ranks applicable signatures by specificity with declaration order
as the stable tie-break. The selected signature supplies contextual argument
types and the call result. Calls for which no candidate applies produce the
single deterministic diagnostic `No overload matches this call.`; single
callable signatures retain their more specific argument, arity and inference
diagnostics.

`tests/overload_resolution.sh` covers ordered primitive selection, generic
overload inference, arity failure and implementation-signature privacy. The
checkpoint also closes the discovered tuple-to-generic-array inference gap by
using a tuple's numeric index type as its array-element candidate.

The complete aggregate remains 555 pass, zero fail and 24 deliberate semantic
skips against pinned Node 22.22.1 and TypeScript 7.0.2. CP58 does not include
method or constructor overloads; those depend on the canonical class model in
CP59-CP60.
