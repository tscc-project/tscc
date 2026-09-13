#!/usr/bin/env bash
set -euo pipefail
ROOT=$(cd "$(dirname "$0")/.." && pwd)
TMP=$(mktemp -d "${TMPDIR:-/tmp}/tscc-declarations.XXXXXX")
trap 'rm -rf "$TMP"' EXIT
cat >"$TMP/api.ts" <<'TS'
export interface Point { x: number; y: number }
export type Name = string;
export enum Axis { X, Y }
export const origin: Point = {x: 0, y: 0};
export function distance(value: Point): number { return value.x + value.y; }
TS
"$ROOT/tscc" --pretty false --declarationMap --outDir "$TMP/out" "$TMP/api.ts" >/dev/null
test -f "$TMP/out/api.js"
test -f "$TMP/out/api.d.ts"
test -f "$TMP/out/api.d.ts.map"
grep -Fq 'interface Point' "$TMP/out/api.d.ts"
grep -Fq 'declare const origin: Point;' "$TMP/out/api.d.ts"
grep -Fq 'declare function distance(value: Point): number;' "$TMP/out/api.d.ts"
grep -Fq 'sourceMappingURL=api.d.ts.map' "$TMP/out/api.d.ts"
! grep -Fq 'return value' "$TMP/out/api.d.ts"
echo "bounded declaration emit passed"
