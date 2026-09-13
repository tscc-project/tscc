# CP68 source-map and diagnostics evidence

The CLI now accepts `--sourceMap`. Every emitted JavaScript/JSX file receives a
deterministic source-map-v3 sidecar and relative `sourceMappingURL`; sidecars use
the same staged output transaction as JavaScript. Mapping segments preserve the
source line for every generated line, which is exact for ordinary erasure and
single-line lowerings.

`tests/source_maps.sh` checks sidecar/trailer linkage, source identity and byte-
for-byte repeatability on erasure plus enum lowering. The complete diagnostic
and parser-recovery gates remain mandatory.

Column-exact mapping within replacement text and multi-source bundle maps are
not claimed. The current one-output-per-source architecture makes that boundary
explicit and leaves room for edit-owned segments in a later compatibility pass.
