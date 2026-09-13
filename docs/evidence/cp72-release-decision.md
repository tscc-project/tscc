# CP72 compatibility and release decision

Decision: retain **tscc Compiler Preview** and continue development. Do not
publish or describe this snapshot as a production compiler or drop-in `tsc`.

Positive evidence includes 555/579 oracle cases with no failures, all 24 gaps
explicitly skipped; deterministic runtime/project/output gates; ASan/UBSan and
project lifetime passes; deterministic fuzz; a bounded generated-project budget;
and reproducible packaging. CP66–70 add useful JSX contracts, local enum typing,
line maps, bounded declarations and incremental output retention.

The blockers are substantive: skipped semantic families still exist; namespace
and runtime-bearing facts are not complete across modules; replacement mappings
are not column exact; declaration emit omits supported class/inferred surfaces;
and incremental mode does not reuse semantic graphs or provide watch/cancellation.

`docs/COMPATIBILITY.md` is the canonical matrix. A future release reassessment
must reduce or explicitly product-scope those gaps, rerun checked `tsc` and
`tsc --noCheck` comparisons separately, and use a separately authorized release
process.
