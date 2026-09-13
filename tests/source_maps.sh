#!/usr/bin/env bash
set -euo pipefail
ROOT=$(cd "$(dirname "$0")/.." && pwd)
TMP=$(mktemp -d "${TMPDIR:-/tmp}/tscc-maps.XXXXXX")
trap 'rm -rf "$TMP"' EXIT
cat >"$TMP/input.ts" <<'TS'
interface Hidden { value: number }
enum Mode { A, B }
const value: number = Mode.B;
console.log(value);
TS
"$ROOT/tscc" --pretty false --sourceMap --outDir "$TMP/out" "$TMP/input.ts" >/dev/null
test -f "$TMP/out/input.js.map"
grep -Fq 'sourceMappingURL=input.js.map' "$TMP/out/input.js"
grep -Fq '"version":3' "$TMP/out/input.js.map"
grep -Fq '"sources":["input.ts"]' "$TMP/out/input.js.map"
cp "$TMP/out/input.js.map" "$TMP/first.map"
"$ROOT/tscc" --pretty false --sourceMap --outDir "$TMP/out" "$TMP/input.ts" >/dev/null
cmp "$TMP/first.map" "$TMP/out/input.js.map"
echo "deterministic source maps passed"
