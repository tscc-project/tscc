# CP57 deterministic generic inference

Calls to retained generic signatures now collect inference candidates from
arguments, nested structural properties and contextual return positions.
Candidates are widened and canonicalized before substitution. A single stable
candidate is selected; incompatible candidates, unresolved parameters and
inferred constraint violations produce explicit diagnostics.

Explicit arguments continue to take precedence, inference precedes declared
defaults, and every chosen type is substituted through the complete callable
signature before ordinary argument and result checking. Collection and
substitution retain the CP56 recursion guard.

`tests/generic_inference.sh` covers direct, structural and contextual inference,
plus conflict, unresolved and inferred-constraint diagnostics.
