# TSCC regression bug log

Status at the v0.5.0 language-hardening checkpoint.

The independent corpus now contains **216 cases**: 152 runtime/emit cases, 46 syntax-negative cases, and 18 deliberate semantic-checker-only cases. Current result: **198 pass / 0 fail / 18 skip**.

| ID | Status | Regression |
|---|---|---|
| TSCC-0002 | NOT-A-BUG (current scope) | semantic-checker-only restriction; retained for future type checker |
| TSCC-0003 | FIXED | enum prior-member and chained prior-member references |
| TSCC-0004 | FIXED | object-literal method parameter/return annotations leaking into JS |
| TSCC-0006 | NOT-A-BUG (current scope) | semantic-checker-only restriction; retained for future type checker |
| TSCC-0009 | FIXED | enum computed/shift initializer handling |
| TSCC-0010 | FIXED | malformed generic-arrow delimiter acceptance |
| TSCC-0011 | FIXED | empty type expressions after `:` / assertions |
| TSCC-0012 | FIXED | nameless class `?: Type` / `!: Type` members |
| TSCC-0013 | FIXED | angle-bracket assertions leaked into emitted JavaScript |
| TSCC-0014 | FIXED | repeated postfix non-null assertions such as `value!!` |
| TSCC-0015 | FIXED | function/method/constructor overload signatures emitted as invalid JS |
| TSCC-0016 | FIXED | `override`, ambient `declare`, and compile-time `this` parameters leaking into JS |
| TSCC-0017 | FIXED | parenthesized/fractional/exponent enum constants broke later auto-increment |
| TSCC-0018 | FIXED | namespace runtime transforms, including nested/exported members |
| TSCC-0019 | FIXED | generic close followed immediately by `=` tokenized as `>=` and swallowed initializers |
| TSCC-0020 | FIXED | optional class method declarations and richer class overload boundaries |
| TSCC-0021 | FIXED | malformed interface/type/assertion/enum forms silently accepted |

Reference classification remains strict: syntax-negative cases must be rejected by `tsc --noCheck`; restrictions accepted by `--noCheck` but rejected by ordinary `tsc` are semantic-only until `tscc` has a type checker.

## v0.11.0 CommonJS expansion

The independent corpus now contains **400 cases** with **377 pass / 0 fail / 23 semantic-only skips**. New fixed families include destructured CommonJS exports, namespace re-exports, named-default module specifiers, live imported references inside template literals, and transform ownership for reused imported names inside later import declarations. The last case was important because two individually valid transforms overlapped and produced corrupted JavaScript despite both being correct in isolation.

No confirmed `tsc` correctness bug was found in this checkpoint. Cases that TypeScript accepts are retained/reclassified rather than used as false positives.
