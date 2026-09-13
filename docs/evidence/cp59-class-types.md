# CP59 class symbols and two-sided types

Each bound class now owns one canonical semantic record. It retains the source
range, generic identity, constructor signature and overload declarations,
parameter properties, fields, methods, modifiers, and separate instance and
static object types. The class value symbol receives the static side while the
class name in an annotation resolves to the instance side.

The bounded expression model now recognizes construction and `this` where CP59
needs them. Constructor arguments and explicit class type arguments are checked,
constructed values carry the instance type, and property access distinguishes
static from instance members. `this` resolves from the containing class range
and therefore uses the same canonical instance shape rather than a synthetic
checker-only object.

`tests/class_types.sh` covers fields, methods, static members, constructor
arguments, parameter properties, `this`, construction and the two-sided type
boundary. The complete aggregate remains 555 pass, zero fail and 24 deliberate
semantic skips against pinned Node 22.22.1 and TypeScript 7.0.2, and ASan/UBSan
passes. Relationships and access enforcement remain CP60.
