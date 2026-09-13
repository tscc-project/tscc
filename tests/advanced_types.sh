#!/usr/bin/env bash
set -euo pipefail
ROOT=$(cd "$(dirname "$0")/.." && pwd)
TMP=$(mktemp -d)
trap 'find "$TMP" -type f -delete; find "$TMP" -depth -type d -empty -delete' EXIT

cat >"$TMP/good.ts" <<'TS'
type Both={answer:number}&{label:string};
type Flags<T>={[K in keyof T]?:boolean};
type Concrete={[K in "left"|"right"]:number};
type MutableRequired<T>={-readonly[K in keyof T]-?:T[K]};
type Element<T>=T extends Array<infer U>?U:never;
type Choice<T>=T extends string?"text":"other";
type OnlyStrings<T>=T extends string?T:never;
type Event="save"|"load";
type Handler=`on${Event}`;
const both:Both={answer:42,label:"ok"};
const flags:Flags<{answer:number;label:string}>={answer:true};
const concrete:Concrete={left:1,right:2};
let mutable:MutableRequired<{readonly value?:number}>={value:1};
mutable.value=2;
const element:Element<number[]>=7;
const choice:Choice<number>="other";
const distributed:OnlyStrings<string|number>="ok";
const handler:Handler="onsave";
const partial:Partial<{a:number;b:string}>={a:1};
const required:Required<{a?:number}>={a:1};
const picked:Pick<{a:number;b:string},"b">={b:"ok"};
const record:Record<"a"|"b",number>={a:1,b:2};
const excluded:Exclude<"a"|"b","a">="b";
const extracted:Extract<string|number,string>="ok";
TS
"$ROOT/tscc" --pretty false --noEmit "$TMP/good.ts"

check_bad() {
  local name=$1 needle=$2
  if "$ROOT/tscc" --pretty false --noEmit "$TMP/$name.ts" >"$TMP/$name.out" 2>"$TMP/$name.err"; then
    echo "$name unexpectedly passed" >&2; exit 1
  fi
  grep -Fq "$needle" "$TMP/$name.err"
}

printf '%s\n' 'type X={a:number}&{b:string};const x:X={a:1};' >"$TMP/intersection.ts"
check_bad intersection "Property 'b' is missing"
printf '%s\n' 'type F<T>={[K in keyof T]:boolean};const x:F<{a:number}>={a:1};' >"$TMP/mapped.ts"
check_bad mapped "not assignable to type 'boolean'"
printf '%s\n' 'type E<T>=T extends Array<infer U>?U:never;const x:E<number[]>="bad";' >"$TMP/conditional.ts"
check_bad conditional "not assignable to type 'number'"
printf '%s\n' 'type E="save"|"load";type H=`on${E}`;const x:H="other";' >"$TMP/template.ts"
check_bad template "not assignable"
printf '%s\n' 'const x:Required<{a?:number}>={};' >"$TMP/required.ts"
check_bad required "Property 'a' is missing"

printf 'bounded advanced type operators passed\n'
