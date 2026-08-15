# TS -> JS compiler regression suite

Compiler-agnostic regression corpus used to compare `tscc` with TypeScript's reference compiler.

This checkpoint contains **265 cases**: **246 current parser/transpiler/runtime cases** and **19 deliberate semantic-checker-only cases**. Runtime and syntax expectations are validated with `tsc --noCheck`; semantic-only cases are additionally verified against normal `tsc` so missing type checking is not mislabeled as a parser defect.

Run against a compiler with:

```bash
python3 run.py --tscc /path/to/tscc
```
