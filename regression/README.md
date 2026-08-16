# TS -> JS compiler regression suite

Compiler-agnostic regression corpus used to compare `tscc` with TypeScript's reference compiler.

This checkpoint contains **511 cases**: **483 current parser/transpiler/runtime cases** and **28 deliberate semantic-checker-only cases**. Runtime and syntax expectations are validated with `tsc --noCheck`; semantic-only cases are additionally verified against normal `tsc --module commonjs` so missing type checking is not mislabeled as a parser defect. The explicit module mode prevents TypeScript release changes to its default configuration from silently reclassifying the corpus.

Run against a compiler with:

```bash
python3 run.py --tscc /path/to/tscc
```
