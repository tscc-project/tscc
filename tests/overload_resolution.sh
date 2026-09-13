#!/usr/bin/env bash
set -euo pipefail
ROOT=$(cd "$(dirname "$0")/.." && pwd)
TMP=$(mktemp -d)
trap 'find "$TMP" -type f -delete; find "$TMP" -depth -type d -empty -delete' EXIT

cat >"$TMP/good.ts" <<'TS'
function convert(value:string):string;
function convert(value:number):number;
function convert(value:string|number):string|number { return value; }
const text:string=convert("ok");
const count:number=convert(42);

function pick<T>(value:T[]):T;
function pick(value:string):string;
function pick(value:unknown):unknown { return value; }
const inferred:number=pick([1,2]);
const selected:string=pick("yes");

function ordered(value:number|string):number;
function ordered(value:number|boolean):string;
function ordered(value:number|string|boolean):number|string { return 1; }
const firstApplicable:number=ordered(1);
TS
"$ROOT/tscc" --pretty false --noEmit "$TMP/good.ts"

cat >"$TMP/no-match.ts" <<'TS'
function convert(value:string):string;
function convert(value:number):number;
function convert(value:string|number|boolean):string|number|boolean { return value; }
convert(true);
TS
if "$ROOT/tscc" --pretty false --noEmit "$TMP/no-match.ts" >"$TMP/no-match.out" 2>"$TMP/no-match.err"; then
  echo "implementation signature leaked into overload surface" >&2; exit 1
fi
grep -Fq "No overload matches this call." "$TMP/no-match.err"

cat >"$TMP/arity.ts" <<'TS'
function join(left:string,right:string):string;
function join(value:number):number;
function join(left:string|number,right?:string):string|number { return left; }
join();
TS
if "$ROOT/tscc" --pretty false --noEmit "$TMP/arity.ts" >"$TMP/arity.out" 2>"$TMP/arity.err"; then
  echo "invalid overload arity was accepted" >&2; exit 1
fi
grep -Fq "No overload matches this call." "$TMP/arity.err"

printf 'tscc ordered overload resolution passed\n'
