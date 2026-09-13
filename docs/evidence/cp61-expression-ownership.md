# CP61 complete expression ownership

CP61 replaces the remaining identifier/call discovery scan with durable semantic
statement, condition and expression-root identities. Initializers, returns,
standalone and nested expression statements, control headers and throw operands
now retain canonical `ExpressionId` children before checking.

The selected expression grammar now owns logical, nullish, comparison,
conditional, prefix/postfix update, `new`, optional property/call, non-null,
`as` assertion and `satisfies` nodes. All nodes use the existing
`NodeExpressionTyper`; unsupported syntax remains explicitly classified rather
than acquiring a second typing path.

Focused evidence is `tests/expression_ownership.cpp`. The complete pinned test,
regression, runtime/project/TSX/CommonJS and sanitizer gates remain required for
the checkpoint commit.
