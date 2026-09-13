#!/usr/bin/env bash
set -euo pipefail
ROOT=$(cd "$(dirname "$0")/.." && pwd)
TMP=$(mktemp -d)
trap 'find "$TMP" -type f -delete; find "$TMP" -depth -type d -empty -delete' EXIT
cat >"$TMP/api.d.ts" <<'TS'
export interface Box { readonly value: number }
export type Name = string;
export declare function parse(value: Name): Box;
export declare const version: Name;
export declare class Reader { read(value: Name): Box; }
TS
cat >"$TMP/good.ts" <<'TS'
import type { Box, Name } from "./api";
import { parse, version, Reader } from "./api";
const name: Name = version;
const box: Box = parse(name);
TS
"$ROOT/tscc" --pretty false --noEmit "$TMP/good.ts"
cat >"$TMP/bad.ts" <<'TS'
import type { Box, Name } from "./api";
import { parse } from "./api";
const wrongName: Name = 42;
const wrongBox: Box = { value: "wrong" };
parse(42);
TS
if "$ROOT/tscc" --pretty false --noEmit "$TMP/bad.ts" >"$TMP/bad.out" 2>"$TMP/bad.err"; then
  echo "declaration types were not enforced" >&2; exit 1
fi
grep -Fq "not assignable to type 'string'" "$TMP/bad.err"
grep -Fq "property 'value'" "$TMP/bad.err"
mkdir "$TMP/out"
"$ROOT/tscc" --pretty false --outDir "$TMP/out" "$TMP/good.ts" >/dev/null
test -f "$TMP/out/good.js"
test ! -e "$TMP/out/api.js"
printf 'tscc declaration frontend test passed\n'
