# TS -> JS compiler regression suite

Compiler-agnostic regression corpus used to compare `tscc` with TypeScript's reference compiler.

This checkpoint contains **519 cases**: **485 parser/transpiler/runtime cases** and **34 semantic cases**. Seven semantic cases are now implemented tscc checker contracts, producing **492 pass / 0 fail / 27 deliberate semantic skips**; the remaining semantic cases describe future checker work. Runtime and syntax expectations are validated with `tsc --noCheck`; semantic cases are additionally verified against normal `tsc --module commonjs`. The explicit module mode prevents TypeScript release changes to its default configuration from silently reclassifying the corpus.

Run against a compiler with:

```bash
python3 run.py --tscc /path/to/tscc
```
