#!/usr/bin/env bash
set -euo pipefail
ROOT=$(cd "$(dirname "$0")/.." && pwd)
TMP=$(mktemp -d)
trap 'find "$TMP" -type f -delete; find "$TMP" -depth -type d -empty -delete' EXIT

cat >"$TMP/good.ts" <<'TS'
class Box {
  static kind:string="box";
  label:string;
  constructor(public readonly value:number,label:string) { this.label=label; }
  getValue():number { return this.value; }
}
const box:Box=new Box(42,"answer");
const value:number=box.value;
const label:string=box.label;
const viaMethod:number=box.getValue();
const kind:string=Box.kind;
TS
"$ROOT/tscc" --pretty false --noEmit "$TMP/good.ts"

cat >"$TMP/constructor.ts" <<'TS'
class Box { constructor(public value:number) {} }
const bad:Box=new Box("wrong");
TS
if "$ROOT/tscc" --pretty false --noEmit "$TMP/constructor.ts" >"$TMP/constructor.out" 2>"$TMP/constructor.err"; then
  echo "invalid constructor argument was accepted" >&2; exit 1
fi
grep -Fq "not assignable to parameter of type 'number'" "$TMP/constructor.err"

cat >"$TMP/sides.ts" <<'TS'
class Box { static kind:string="box"; value:number=1; }
const box:Box=new Box();
const bad:number=Box.value;
const alsoBad:string=box.kind;
TS
if "$ROOT/tscc" --pretty false --noEmit "$TMP/sides.ts" >"$TMP/sides.out" 2>"$TMP/sides.err"; then
  echo "class static and instance sides were conflated" >&2; exit 1
fi
grep -Fq "Property 'value' does not exist" "$TMP/sides.err"
grep -Fq "Property 'kind' does not exist" "$TMP/sides.err"

printf 'tscc canonical two-sided class types passed\n'
