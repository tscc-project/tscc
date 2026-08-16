# Regression notes

The first semantic-checker checkpoint retains simple variable declaration spans through parsing and checks explicit `number`, `string`, and `boolean` annotations against direct literal initializers. `semantic-type-mismatch` is consequently the first semantic-only case promoted into the tscc contract. Current result: **484 pass / 0 fail / 27 semantic skips** under TypeScript 7.0.2.

The corpus grew from 216 to **265 cases** in the v0.6.0 checkpoint. The added families cover mapped/conditional/template-literal/indexed/variadic types, const type parameters, destructured parameters, class static/accessor syntax, type-only module forms, ambient enums/classes/functions, namespace merging and dotted namespaces, instantiation expressions, generic expression references, and additional malformed advanced-type grammar.

Current result: **246 pass / 0 fail / 19 semantic-only skips**.

# Regression hardening notes

The corpus grew from 78 to **216 cases** during the v0.5.0 checkpoint. Two adversarial expansion batches attacked richer type grammar, generics, assertions, overloads, classes, ambient declarations, namespaces, enums and malformed recovery.

The expansion repeatedly caught tempting but incorrect test assumptions. In particular, empty generic parameter/argument lists, rest-parameter placement, empty `implements` lists and some parameter-property forms are accepted by `tsc --noCheck` and rejected later by normal `tsc`; they therefore remain semantic-checker cases rather than parser bugs. String-named enum members without explicit initializers are valid TypeScript and were converted into a runtime regression instead of a negative case.

The reference harness was rewritten to batch `tsc --noCheck` and ordinary `tsc` validation. This reduced the 216-case reference pass from hundreds of TypeScript process startups to a handful of invocations while preserving per-file classification.

Current result: **198 pass / 0 fail / 18 deliberate semantic-only skips**. No confirmed TypeScript compiler correctness bug was found in this checkpoint.
