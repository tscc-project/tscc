#!/usr/bin/env bash
set -euo pipefail
ROOT=$(cd "$(dirname "$0")/.." && pwd)
TMP=$(mktemp -d)
trap 'find "$TMP" -type f -delete; find "$TMP" -depth -type d -empty -delete' EXIT
cat >"$TMP/good.ts" <<'TS'
type Box<T> = { value: T };
interface Pair<T> { left: T; right: T }
function identity<T extends string = string>(value: T): T { return value; }
const box: Box<number> = { value: 42 };
const pair: Pair<string> = { left: "a", right: "b" };
const answer: string = identity<string>("ok");
const fallback: string = identity("default");
TS
"$ROOT/tscc" --pretty false --noEmit "$TMP/good.ts"
cat >"$TMP/bad-shape.ts" <<'TS'
type Box<T> = { value: T };
const bad: Box<number> = { value: "wrong" };
TS
if "$ROOT/tscc" --pretty false --noEmit "$TMP/bad-shape.ts" >"$TMP/shape.out" 2>"$TMP/shape.err"; then
  echo "generic alias substitution was not enforced" >&2; exit 1
fi
grep -Fq "property 'value'" "$TMP/shape.err"
cat >"$TMP/bad-constraint.ts" <<'TS'
function identity<T extends string>(value: T): T { return value; }
identity<number>(42);
TS
if "$ROOT/tscc" --pretty false --noEmit "$TMP/bad-constraint.ts" >"$TMP/constraint.out" 2>"$TMP/constraint.err"; then
  echo "generic constraint was not enforced" >&2; exit 1
fi
grep -Fq "does not satisfy constraint 'string'" "$TMP/constraint.err"
cat >"$TMP/bad-count.ts" <<'TS'
function pair<T,U>(left:T,right:U):T { return left; }
pair<number,string,boolean>(1,"x");
TS
if "$ROOT/tscc" --pretty false --noEmit "$TMP/bad-count.ts" >"$TMP/count.out" 2>"$TMP/count.err"; then
  echo "excess explicit type arguments were accepted" >&2; exit 1
fi
grep -Fq "Expected 2 type arguments, but got 3" "$TMP/count.err"
printf 'tscc generic identity and explicit substitution passed\n'
