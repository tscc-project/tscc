#!/usr/bin/env bash
set -euo pipefail
ROOT=$(cd "$(dirname "$0")/.." && pwd)
TMP=$(mktemp -d)
trap 'find "$TMP" -type f -delete; find "$TMP" -depth -type d -empty -delete' EXIT
cat >"$TMP/good.ts" <<'TS'
type Shape={kind:"circle";radius:number}|{kind:"square";side:number};
type Content={text:string}|{count:number};
class Left { left:number=1; } class Right { right:string="r"; }
function inspect(shape:Shape,content:Content,item:Left|Right,maybe:string|null):number {
  let assigned:string|number;
  assigned="ready";
  const assignmentNarrowed:string=assigned;
  if(shape.kind === "circle") { const radius:number=shape.radius; }
  else { const side:number=shape.side; }
  if("text" in content) { const text:string=content.text; }
  if(item instanceof Left) { const left:number=item.left; }
  if(maybe) { const value:string=maybe; }
  while(shape.kind === "circle") { const loopRadius:number=shape.radius; return loopRadius; }
  return 0;
}
TS
"$ROOT/tscc" --pretty false --noEmit "$TMP/good.ts"
cat >"$TMP/bad.ts" <<'TS'
type Shape={kind:"circle";radius:number}|{kind:"square";side:number};
function inspect(shape:Shape):void {
  if(shape.kind === "circle") { const wrong:number=shape.side; }
  else { const wrong:number=shape.radius; }
}
TS
if "$ROOT/tscc" --pretty false --noEmit "$TMP/bad.ts" >"$TMP/out" 2>"$TMP/err"; then echo "invalid discriminant access unexpectedly passed" >&2; exit 1; fi
grep -Fq "does not exist on type" "$TMP/err"
cat >"$TMP/unassigned.ts" <<'TS'
function read():number { let pending:number; return pending; }
TS
if "$ROOT/tscc" --pretty false --noEmit "$TMP/unassigned.ts" >"$TMP/out" 2>"$TMP/unassigned.err"; then echo "unassigned variable unexpectedly passed" >&2; exit 1; fi
grep -Fq "Variable 'pending' is used before being assigned." "$TMP/unassigned.err"
printf 'CFG-owned flow-sensitive narrowing passed\n'
