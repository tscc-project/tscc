# CP67 runtime-bearing TypeScript semantics evidence

Enums now bind as value symbols and contribute two canonical facts: a runtime
object whose readonly properties are the members, and a named instance type
formed from direct string/numeric literals (computed numeric members widen to
`number`). Consequently `State.Ready` can initialize `State`, while an unrelated
literal is rejected. Constructor parameter properties continue through CP59's
class member/accessibility model.

`tests/runtime_types.sh` runs emitted enum, merged namespace and parameter-
property JavaScript, and independently checks an invalid enum assignment. The
whole suite guards the existing broader lowering differential.

This checkpoint does not claim typed namespace export tables or cross-module
enum/namespace type propagation. Those remain called out in the roadmap rather
than being inferred from successful runtime lowering.
