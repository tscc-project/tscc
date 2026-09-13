# CP66 JSX/TSX semantics evidence

CP66 adds an opt-in semantic layer over the existing structural TSX validator.
The opt-in is the standard-shaped `JSX.IntrinsicElements` declaration in the
same compilation unit. Intrinsic names resolve through that object contract;
attributes are checked for existence, required attributes are enforced, and
primitive string, numeric, boolean and simple identifier values are checked.
Upper-case tags resolve callable declarations and use their first parameter as
the props contract.

The existing no-contract preservation behavior remains compatible, and emit is
still exactly `--jsx preserve`. `tests/tsx_preserve.sh` covers valid intrinsic
and component calls plus wrong, unknown, missing and undeclared-tag cases.

This is deliberately not React's complete JSX type system: cross-file namespace
augmentation, spreads, contextual expression typing, `ElementClass`, children
tuple/cardinality rules and runtime JSX modes remain explicit future work.
