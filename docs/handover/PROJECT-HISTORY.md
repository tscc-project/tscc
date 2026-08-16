# Project history and institutional context

> This is a living historical companion to the repository's operational handover. The live repository remains authoritative. Maintain, correct, reorganize, or supersede this material as project evidence evolves while retaining durable rationale.

# tscc

## Comprehensive Compiler Project, Development, Testing and Production Roadmap Handover

# 1. Identity

tscc is the TypeScript/JavaScript compiler project developed alongside Nift and Minify++.

It is the most ambitious of the three from a language-semantics perspective.

It should be treated as an independent product.

---

# 2. Historical maturity assessment

Our broad historical assessment was:

```text
Nift:
    closest to production

Minify++:
    substantial but needs hardening

tscc:
    long-term compiler project
```

Codex has subsequently worked directly with tscc, so repository evidence may now justify a more advanced assessment.

Do not preserve the old maturity label if development has overtaken it.

---

# 3. First production question: what exactly is tscc promising?

Before defining production readiness, establish the intended compatibility contract.

Possibilities include:

```text
full TypeScript compiler replacement
large practical TypeScript subset
transpiler-focused TypeScript subset
JavaScript compiler plus selected TS syntax
deliberately different compiler
```

These imply radically different production roadmaps.

This should be one of Codex's first reconciliation tasks.

---

# 4. Compatibility target

If TypeScript compatibility is intended, document:

```text
TypeScript version/range
ECMAScript target(s)
module modes
type-checking scope
emit scope
module resolution scope
diagnostic compatibility expectations
```

Do not let "TypeScript support" remain an undefined phrase.

---

# 5. Support has multiple dimensions

For any language feature distinguish where necessary:

```text
parse
bind
analyze/type-check
transform
emit
runtime semantics
diagnostics
```

A parser accepting syntax is not equivalent to feature support.

This distinction is essential for both development and marketing.

---

# 6. Architecture

Codex should reconstruct exact architecture from current source.

A conceptual compiler pipeline may resemble:

```text
source
↓
lexer/scanner
↓
parser
↓
AST
↓
binding/scope
↓
semantic/type analysis
↓
lowering/transforms
↓
emission
↓
JavaScript
```

but do not force this abstraction onto source if tscc intentionally combines stages.

Once stable enough, tscc likely warrants a proper architecture document.

---

# 7. Development principle: vertical semantic slices

Prefer:

```text
feature
↓
parser
↓
scope/semantics
↓
lowering
↓
emission
↓
runtime test
```

over:

```text
implement syntax for twenty features
then eventually make them mean something
```

Production confidence comes from complete slices.

---

# 8. TypeScript as reference

Where compatibility is intended, TypeScript itself is an extremely useful oracle.

For each feature investigate:

```text
Does tsc accept this?
What diagnostics does it emit?
What JS does it emit?
What happens when that JS runs?
```

Then determine which dimensions tscc intends to match.

---

# 9. Reference behavior is not automatically specification

If `tsc` does something surprising:

```text
investigate
```

rather than blindly cloning it.

tscc may deliberately have a narrower/different contract.

Any deliberate divergence should be documented and tested.

---

# 10. Parser development

For each syntax feature test:

```text
minimal valid
nested
adjacent punctuation
comments
newline boundary
parenthesized
ambiguous prefix
malformed
unexpected EOF
```

where relevant.

Parser error recovery must not:

```text
hang
loop forever
crash
consume unrelated following declarations
```

---

# 11. Scope/binding

Compiler scope bugs are high leverage.

Test applicable contexts such as:

```text
global
function
nested function
block
loop
class
module
```

plus shadowing.

Avoid name-string shortcuts that ignore actual binding identity.

---

# 12. Lowering/transforms

The most important rule:

> Preserve evaluation semantics.

A transform must not accidentally:

```text
evaluate twice
evaluate too early
evaluate too late
skip side effects
change short-circuit behavior
change binding
```

---

# 13. Side-effect probes

Use expressions like:

```text
counter++
fn()
obj.getter
computedKey()
```

to expose lowering mistakes.

Testing only literals often hides real transform bugs.

---

# 14. Temporary variables

If lowering introduces compiler-generated temporaries, test:

```text
name collision
nested transforms
scope
capture
evaluation order
```

Compiler hygiene needs an explicit strategy.

---

# 15. Emission

Valid JavaScript syntax is not proof of correctness.

Use both:

```text
output inspection/snapshot
+
runtime execution
```

where appropriate.

Byte-identical `tsc` output is not necessary unless explicitly part of compatibility goals.

---

# 16. Diagnostics

Establish intended compatibility level.

Possibilities:

```text
success/failure only
stable tscc diagnostics
similar diagnostic categories
tsc-compatible wording
tsc-compatible codes
```

Do not accidentally promise exact TypeScript diagnostics unless there is a strong reason.

---

# 17. Modules

Module behavior can become an enormous project.

If supported, test actual intended scope:

```text
import
export
default
renaming
cycles
relative paths
extensions
resolution
```

Do not accidentally turn tscc into a package-manager compatibility project.

---

# 18. Runtime testing

A strong feature test is:

```text
source.ts
↓
tscc
↓
output.js
↓
Node/runtime
↓
observable expected result
```

Where compatibility matters:

```text
source.ts
├→ tsc → JS → runtime
└→ tscc → JS → runtime

compare
```

This should become a core methodology.

---

# 19. Native safety

Assuming current native implementation remains C/C++:

```text
ASan
UBSan
warnings
malformed-input stress
```

should form part of production hardening.

Compiler input is arbitrary user text.

---

# 20. Performance

tscc may have strong performance ambitions, but optimize only after semantics are protected.

Potential dimensions:

```text
startup
lexing
parsing
analysis
transform
emit
whole compile
many files
large files
memory
```

Keep benchmark methodology stable enough for comparisons.

---

# 21. Existing benchmark/debug artifacts

Codex previously raised questions around things like:

```text
.shadow-debug
hidden probes
temporary benchmark outputs
historical benchmark JSON
```

Do not delete them blindly.

Classify through:

```text
Git history
scripts
test references
documentation
current code
```

as:

```text
fixture
debug facility
benchmark evidence
temporary residue
```

Then clean only with confidence.

---

# 22. Production-ready definition

This depends on the intended compatibility contract, but generically:

> tscc is production-ready when the subset/capabilities it publicly promises are implemented end-to-end with strong semantic evidence, failures are controlled, supported real-world projects compile reliably, diagnostics are usable, performance is stable, and the project has a repeatable validation/release process.

Production readiness does **not necessarily require implementing all of TypeScript**.

That would be an enormous and possibly unnecessary bar if tscc deliberately targets a useful subset.

The crucial requirement is:

```text
public promise
==
tested implementation
```

---

# 23. Current production roadmap

This is the most provisional of the three product roadmaps because tscc's exact compatibility target must first be confirmed.

### Phase 1 — define contract

Establish:

```text
language scope
TypeScript compatibility target
JavaScript target
module scope
type-analysis scope
diagnostic scope
unsupported behavior
```

### Phase 2 — architecture reconciliation

Document actual compiler stages and ownership.

Identify technical debt or cross-stage ambiguity.

### Phase 3 — feature inventory

Build an evidence-backed matrix:

```text
feature
parse
semantic support
emit
runtime
negative diagnostics
```

### Phase 4 — close incomplete semantic slices

Prioritize features that currently:

```text
parse but emit incorrectly
emit but fail runtime
work only in happy path
lack scope/evaluation-order correctness
```

over simply adding more syntax.

### Phase 5 — independent regression expansion

Every supported feature gets meaningful external behavior tests.

### Phase 6 — differential/reference validation

Use `tsc` where compatibility is intended.

### Phase 7 — real-world corpus/projects

Compile increasingly realistic TypeScript/JavaScript projects.

Reduce discovered failures into permanent regressions.

### Phase 8 — malformed/adversarial hardening

Attack parser and compiler assumptions.

### Phase 9 — native safety

Run sanitizers and appropriate fuzzing/stress testing.

### Phase 10 — performance

Establish reproducible benchmarks and address significant regressions/hotspots without compromising semantics.

### Phase 11 — documentation/site reconciliation

Make support claims match tested reality.

### Phase 12 — release candidate

Clean build + complete regression + runtime corpus + sanitizer + benchmark + docs/site validation.

Then decide whether the promised scope is production-ready.

---

# 24. Important production strategy

I would strongly resist defining tscc production readiness as:

```text
100% TypeScript implemented
```

unless full compatibility is explicitly the project's mission.

A more achievable and honest model could be:

```text
well-defined supported subset
+
strong correctness
+
clear unsupported behavior
+
real-world usefulness
```

and then expand post-production.

Codex should determine the intended product ambition with Nick/current docs before settling this.

---

# 25. Living roadmap

This is especially important for tscc.

Compiler development will almost certainly uncover interactions we have not anticipated.

Therefore:

> At every validated compiler checkpoint, reassess the production roadmap. A feature that exposes a scope-model deficiency may make architecture work more urgent than adding the next syntax feature. Real-world corpus results may reorder priorities. Compatibility findings may narrow or expand the intended contract. Performance results may reveal that an optimization can wait. The roadmap must follow evidence.

Do not treat roadmap completion percentage as the project.

---

# 26. Post-production roadmap

Once production status is achieved for a defined scope:

```text
expand supported language deliberately
track TypeScript evolution
expand project corpus
improve diagnostics
improve compatibility
optimize proven hotspots
add platforms/toolchain integration
keep regression suite cumulative
```

Production becomes a maintained quality state.

---

# 27. Checkpoint methodology

Every substantial feature should ideally leave:

```text
baseline
failing/new contract
implementation
focused tests
runtime evidence
neighboring semantic tests
full regression
sanitizer result where appropriate
benchmark result where relevant
docs/support matrix update
handover/roadmap review
```

---

# 28. Do not accidentally

```text
equate parsing with support
add syntax faster than semantics
duplicate side effects in transforms
ignore shadowing
copy tsc blindly
benchmark semantically broken output
delete investigative artifacts blindly
claim full TypeScript compatibility prematurely
freeze the production roadmap
```

---

---

