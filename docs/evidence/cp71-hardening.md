# CP71 production-hardening campaign

The repeatable hardening target combines two bounded workloads:

- 400 deterministic syntax mutations over primitive, object, collection, enum,
  conditional-type and class seeds, each with a one-second liveness ceiling;
- a generated 250-file project using incremental state, source maps and
  declaration output, capped at 15 seconds and 256 MiB peak RSS.

Unix CI now runs this target after the normal suite and sanitizer build. The
local checkpoint wall additionally runs the existing ASan/UBSan compilation-unit
and 120-file project lifetime workloads, RSS soak, preview package test, and
Valgrind when installed.

The checkpoint run completed the generated graph in 0.087 seconds at 9,472 KiB
peak RSS; ASan/UBSan passed 80 in-process lifetime rounds and eight 120-file
project rounds; the non-sanitized soak passed 40 lifetime runs and twenty
120-file project rounds. LSan is disabled only for the ptraced workspace where
it cannot read `/proc`; ASan and UBSan remain enabled.

## CP73 Valgrind validation

Valgrind 3.26.0 was available for this run, so the compiler-facing paths added
in CP66-CP72 are now validated under Memcheck in addition to the in-process
lifetime corpus.

Environment

- OS / architecture: Ubuntu 26.04 LTS, Linux 7.0.0-29-generic, x86_64
  (12th Gen Intel Core i7-12700H)
- Compiler: g++ (Ubuntu 15.2.0-16ubuntu1) 15.2.0
- Valgrind: valgrind-3.26.0
- Python: 3.14.4
- Worktree baseline: `ce8992a CP72 freeze compatibility and release decision`

Strict options shared by every Valgrind invocation:

```text
--leak-check=full --show-leak-kinds=all
--errors-for-leak-kinds=definite,indirect,possible
--track-origins=yes --error-exitcode=99 --track-fds=yes
```

`--track-fds=yes` is added so file-descriptor leaks (listed in the acceptance
criteria) are visible; it is a strict superset of the pre-existing runner's
options and does not weaken detection.

Targets and workloads

1. `make valgrind-memory-safety-checkpoint-5` - the in-process lifetime corpus
   (`.build/tscc-memory-lifetime 40`), 73,841 allocations, 40 iterations.
2. `make checkpoint73-valgrind-integration` - the new bounded workload that
   invokes the real `tscc` binary on temporary fixtures: a multi-file project,
   a TSX file with an explicit `JSX.IntrinsicElements` contract, local enum
   typing with namespaces and parameter properties, `--sourceMap`,
   `--declaration --declarationMap`, two consecutive `--incremental` builds, a
   deliberately invalid input (diagnostic cleanup), `--noEmitOnError`, CommonJS
   output, a tsconfig project graph, and three repeated-lifetime rounds (CLI and
   tsconfig) to expose lifetime accumulation. 17 clean Valgrind invocations.
3. `make valgrind-memory-safety-checkpoint-73` - the aggregate of (1) and (2).

Final summaries

- `ERROR SUMMARY: 0 errors from 0 contexts` on every invocation.
- definitely lost: 0 bytes.
- indirectly lost: 0 bytes.
- possibly lost: 0 bytes.
- still reachable: 0 bytes ("All heap blocks were freed -- no leaks are
  possible").
- No invalid reads or writes, no use of uninitialised values, no mismatched
  allocation/deallocation, no double frees.
- File descriptors: 3 open (3 inherited, standard) at exit in every run;
  `--track-fds=yes` records no descriptor opened by TSCC still open at exit in
  the measured workloads. Temporary-output staging (`.tscc-tmp-*` rename-then-
  commit) is exercised by these workloads, but absence of leftover staging
  files is not separately asserted here.

Defects found and fixed: none. No Valgrind finding required a source change, so
no suppressions were added.

Remaining "still reachable" allocations: none retained at exit. Standard-library
process-lifetime state holds nothing, so there is nothing to investigate beyond
the clean heap summary.

Ignored generated evidence (not committed; `.build/` is ignored):

```text
.build/memory-safety/checkpoint-5-valgrind.json
.build/memory-safety/checkpoint-5-lifetime.json
.build/memory-safety/checkpoint-5-rss.json
.build/memory-safety/checkpoint-5-project.json
.build/memory-safety/checkpoint-5-project-sanitizer.json
.build/memory-safety/checkpoint-73-valgrind-integration.json
```

The integration workload is reproducible via
`scripts/checkpoint73_valgrind_integration.py` (dependency-free) and its Make
target, so the run above can be regenerated exactly.

Known limitation discovered while building the workload (not a memory-safety
finding and not triggered under Valgrind): the general checker's assignment scan
treats a JSX attribute of the form `name={value}` as an assignment when the
attribute name matches a local `const`, emitting "Cannot assign to ... because
it is a constant." The workload fixture therefore avoids a JSX attribute name
that shadows a local const; the underlying diagnostic correctness issue remains
future work.

These are regression budgets for the measured environment, not universal speed
claims. Third-party application corpora, Windows package installers, crash-safe
rename rollback and longer watch-service soak remain release-candidate work.
