# CP60 class relationships

The class model now resolves a deterministic base graph before downstream
annotations are finalized. Inherited instance and static shapes are canonical,
generic base applications and implemented interfaces are instantiated, and
cycles or missing bases become stable source-attributed semantic issues.

The checker enforces implemented-interface shape, public/protected/private
access, readonly writes, abstract-class construction, abstract-member
obligations, override existence, accessibility narrowing and override type
compatibility. `super` resolves through the same base record and constructor
calls use the retained public overload surface. Method and constructor overload
implementations remain private, with ordered applicable-signature selection.

The work also corrected two frontend edge cases discovered by the complete
corpus: `extends` inside a class generic constraint no longer marks the class as
derived, and contextual `get`/`set` words remain legal ordinary property names.
Variance markers on generic interfaces are retained without changing type
parameter identity in this bounded relationship model.

`tests/class_relationships.sh` covers valid inheritance plus negative interface,
override, abstract, access, readonly, `super`, overload, generic-class and cycle
cases. The complete aggregate remains 555 pass, zero fail and 24 deliberate
semantic skips against pinned Node 22.22.1 and TypeScript 7.0.2. ASan/UBSan
passes. CP61 remains responsible for replacing residual range-discovered
expressions with complete owned nodes.
