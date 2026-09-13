# CP63 flow-sensitive narrowing and assignment

Control-flow regions now retain canonical predicate facts for truthiness,
equality, discriminants, `typeof`, `instanceof` and property-presence checks.
The checker consumes those CFG-owned regions for true/false branches and loops,
combines nested facts deterministically, and restores the declared type outside
the lexical region.

Straight-line assignments narrow subsequent reads within the owning scope.
Annotated `let` and `const` declarations without initializers receive a
conservative definite-assignment check; assignments in exited child scopes do
not leak. Loop predicates reuse the CP62 back-edge graph and do not manufacture
post-loop certainty.

Focused evidence is `tests/flow_narrowing.sh`. It covers positive and negative
discriminants, property presence, class-instance checks, truthiness, assignment
narrowing, loop facts, lexical restoration and use-before-assignment.
