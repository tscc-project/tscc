#!/usr/bin/env bash
set -euo pipefail
ROOT=$(cd "$(dirname "$0")/.." && pwd)
TMP=$(mktemp -d "${TMPDIR:-/tmp}/tscc-incremental.XXXXXX")
trap 'rm -rf "$TMP"' EXIT
cat >"$TMP/a.ts" <<'TS'
export const answer: number = 42;
TS
cat >"$TMP/b.ts" <<'TS'
import {answer} from './a.js'; console.log(answer);
TS
"$ROOT/tscc" --pretty false --incremental --outDir "$TMP/inc" "$TMP/b.ts" >/dev/null
test -f "$TMP/inc/.tscc-buildinfo"
cp "$TMP/inc/a.js" "$TMP/a.first"
cp "$TMP/inc/b.js" "$TMP/b.first"
before=$(stat -c %Y "$TMP/inc/b.js")
sleep 1
"$ROOT/tscc" --pretty false --incremental --outDir "$TMP/inc" "$TMP/b.ts" >/dev/null
after=$(stat -c %Y "$TMP/inc/b.js")
test "$before" = "$after"
"$ROOT/tscc" --pretty false --outDir "$TMP/clean" "$TMP/b.ts" >/dev/null
cmp "$TMP/a.first" "$TMP/clean/a.js"
cmp "$TMP/b.first" "$TMP/clean/b.js"
echo "incremental output equivalence passed"
