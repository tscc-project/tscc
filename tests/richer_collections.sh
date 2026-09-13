#!/usr/bin/env bash
set -euo pipefail
ROOT=$(cd "$(dirname "$0")/.." && pwd)
TMP=$(mktemp -d)
trap 'find "$TMP" -type f -delete; find "$TMP" -depth -type d -empty -delete' EXIT

check_bad() {
  local name=$1 needle=$2
  if "$ROOT/tscc" --pretty false --noEmit "$TMP/$name.ts" >"$TMP/$name.out" 2>"$TMP/$name.err"; then
    echo "$name unexpectedly passed" >&2; exit 1
  fi
  grep -Fq "$needle" "$TMP/$name.err"
}

cat >"$TMP/good.ts" <<'TS'
type Pair=readonly [string, number?, ...boolean[]];
type Mutable=[string, number?];
type Keys=keyof {name:string;7:boolean;[key:symbol]:number};
type Name={name:string}["name"];
const pair:Pair=["ok", 1, true];
const short:Mutable=["ok"];
const name:Name="ok";
const stringKey:Keys="name";
const numericKey:Keys=7;
const symbolKey:Keys=Symbol.iterator;
let values:Array<number>=[1,2];
values[0]=3;
let bag:{[key:string]:number}={count:1};
bag["count"]=2;
TS
"$ROOT/tscc" --pretty false --noEmit "$TMP/good.ts"

cat >"$TMP/readonly.ts" <<'TS'
let values:readonly number[]=[1,2];
values[0]=3;
TS
check_bad readonly "read-only"

cat >"$TMP/index-write.ts" <<'TS'
let values:number[]=[1,2];
values[0]="bad";
TS
check_bad index-write "not assignable"

cat >"$TMP/excess.ts" <<'TS'
const point={x:1,y:2} satisfies {x:number};
TS
check_bad excess "Object literal may only specify known properties"

printf 'richer object and collection types passed\n'
