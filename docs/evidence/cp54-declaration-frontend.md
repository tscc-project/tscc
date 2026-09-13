# CP54 declaration-file frontend evidence

CP54 promotes declaration files from resolution-only graph nodes to semantic
inputs. Relative and package-selected `.d.ts` files contribute exported ambient
values and structural type aliases/interfaces. Named `import type` bindings are
copied into the consumer's canonical type store before its graph-level check.

The bounded frontend accepts exported values, functions and class declarations
already represented by the binder, plus exported interfaces and aliases. It
keeps declaration files non-emitting. Ambient module wrappers are parsed, while
package-selected declaration files remain the supported production lookup route.

Generic declaration identity remains CP56; overloads and full class semantics
remain CP58-CP60.
