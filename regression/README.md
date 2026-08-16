# TS -> JS compiler regression suite

Compiler-agnostic regression corpus used to compare `tscc` with TypeScript's reference compiler.

This checkpoint contains **511 cases**: **483 parser/transpiler/runtime cases** and **28 semantic cases**. One semantic case is now an implemented tscc checker contract, producing **484 pass / 0 fail / 27 deliberate semantic skips**; the remaining semantic cases describe future checker work. Runtime and syntax expectations are validated with `tsc --noCheck`; semantic cases are additionally verified against normal `tsc --module commonjs`. The explicit module mode prevents TypeScript release changes to its default configuration from silently reclassifying the corpus.

Run against a compiler with:

```bash
python3 run.py --tscc /path/to/tscc
```
