# CP70 incremental compiler-service foundation

`--incremental` writes a deterministic `.tscc-buildinfo` graph identity and
performs content-aware commits. Unchanged JavaScript, declarations and maps are
not renamed, preserving their mtimes and preventing unnecessary downstream work.
The normal atomic staging path remains in force for changed outputs.

`tests/incremental.sh` builds a two-file graph, verifies build-info presence,
checks a no-change rebuild preserves the output mtime, and byte-compares clean
and incremental outputs.

This is the persistence/output boundary of an incremental service, not yet a
claim of reduced parse/check work. Reusable compilation units, reverse-dependency
invalidation, cancellation and a long-running watch driver remain future work.
