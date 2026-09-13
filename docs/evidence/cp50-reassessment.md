# CP50 reassessment

Date: 13 September 2026

## Delivered since TCP7

- Every checker-owned expression has a structural supported/unknown result;
  unknown descendants carry a durable reason and cannot be hidden by a supported
  root.
- Relative `.d.ts` inputs have canonical graph identity and never emit JavaScript.
- Ambient function declarations bind parameters and callable signatures.
- Export facts are graph-owned and identify their originating unit and symbol.
- Default and namespace imports clone callable/object facts into the consumer.
- Named/star re-exports and aliases converge through cyclic graphs by a bounded,
  deterministic fixed point.
- TCP6A is 3 pass, 0 unsupported, 0 fail. The independent wall remains
  555 pass, 0 fail and 24 explicit semantic skips across 579 cases.

## Next decision

CP51 practical relative resolution remains next. The current resolver already
has deterministic `.ts`, `.tsx`, `.d.ts`, and directory-index order, but does not
yet cover the full supported-extension set, package entry metadata, or ambiguity
diagnostics. CP52 must follow only after CP51 fixes the resolution result model;
otherwise Node-style lookup would duplicate an underspecified boolean resolver.

CP53-CP72 remain queued in dependency order. They are not completed by this
reassessment and must not be represented as such: generic inference, overloads,
class semantics, control-flow, JSX typing, declaration emit, incremental service,
hardening and release qualification are substantial compiler campaigns.
