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
120-file project rounds. Valgrind was not installed in this workspace, so no new
Valgrind result is claimed here (DeepSeek's preceding Valgrind evidence remains
the historical baseline). LSan is disabled only for the ptraced workspace where
it cannot read `/proc`; ASan and UBSan remain enabled.

These are regression budgets for the measured environment, not universal speed
claims. Third-party application corpora, Windows package installers, crash-safe
rename rollback and longer watch-service soak remain release-candidate work.
