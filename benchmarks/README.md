# tscc benchmarks

The benchmark corpus grows with compiler coverage. It currently measures a small single-file program and 100 independent TypeScript files.

Three rows are kept separate:
- `tscc`: current tscc transpile+emit path (no semantic type checking yet).
- `tsc --noCheck`: the fairest current TypeScript emit-only comparison.
- `tsc full`: normal TypeScript checking+emit with `--skipLibCheck`.

Run from the repository root:

```bash
python3 benchmarks/benchmark.py --runs 20 --json benchmarks/results/latest.json
```

Treat ratios as local-machine measurements, not universal claims. New language features should add representative fixtures before optimisation work is accepted.
