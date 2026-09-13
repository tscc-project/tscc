#!/usr/bin/env bash
set -euo pipefail
ROOT=$(cd "$(dirname "$0")/.." && pwd)
TMP=$(mktemp -d "${TMPDIR:-/tmp}/tscc-hardening.XXXXXX")
trap 'rm -rf "$TMP"' EXIT
mkdir -p "$TMP/src"
for i in $(seq 1 250); do
  printf 'export interface Item%s { value: number }\nexport const item%s: Item%s = {value: %s};\n' "$i" "$i" "$i" "$i" >"$TMP/src/file$i.ts"
done
python3 "$ROOT/tools/resource_budget.py" --seconds 15 --rss-kib 262144 --record "$TMP/resources" -- "$ROOT/tscc" --pretty false --noResolve --incremental --sourceMap --declaration --outDir "$TMP/out" "$TMP"/src/*.ts >/dev/null
test "$(find "$TMP/out" -name '*.js' | wc -l)" -eq 250
test "$(find "$TMP/out" -name '*.d.ts' | wc -l)" -eq 250
read -r elapsed rss <"$TMP/resources"
python3 - "$elapsed" "$rss" <<'PY'
import sys
elapsed=float(sys.argv[1]); rss=int(sys.argv[2])
if elapsed >= 15: raise SystemExit(f"large graph exceeded 15s: {elapsed}")
if rss >= 262144: raise SystemExit(f"large graph exceeded 256 MiB RSS: {rss} KiB")
PY
python3 "$ROOT/tools/preview_fuzz.py" "$ROOT/tscc"
echo "production hardening budget passed: ${elapsed}s, ${rss} KiB"
