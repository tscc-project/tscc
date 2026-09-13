# CP62 control-flow graph foundation

Each ordinary or arrow function now owns a deterministic `FunctionControlFlow`
with stable block IDs, entry/exit blocks, typed edges, predecessor sets and a
reachability fixed point. The graph represents branches, loops, short-circuit
operators, returns, throws, and try/catch/finally transfer. Loop back edges and
multi-predecessor joins are explicit and graph construction is idempotent.

This checkpoint intentionally does not add flow-sensitive diagnostics. CP63 can
attach narrowing and assignment facts to these canonical blocks and perform
conservative fixed-point joins without rebuilding control flow inside the
checker. Focused evidence is `tests/control_flow.cpp`.

The CP62 evidence reassessment leaves CP63 first: the TCP6A trial shapes remain
green, while the existing checker still has a bounded token-range narrowing
bridge. Moving those facts onto the CFG has greater architectural leverage than
starting richer object operators before flow identity is consumed.
