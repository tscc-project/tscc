#!/usr/bin/env bash
set -euo pipefail
ROOT=$(cd "$(dirname "$0")/.." && pwd)
TMP=$(mktemp -d)
trap 'find "$TMP" -type f -delete; find "$TMP" -depth -type d -empty -delete' EXIT
cat >"$TMP/good.ts" <<'TS'
function identity<T>(value:T):T { return value; }
function read<T>(box:{value:T}):T { return box.value; }
function create<T>():T { throw new Error(); }
const numberValue:number=identity(42);
const stringValue:string=identity("ok");
const nested:number=read({value:42});
const contextual:string=create();
TS
"$ROOT/tscc" --pretty false --noEmit "$TMP/good.ts"
cat >"$TMP/conflict.ts" <<'TS'
function same<T>(left:T,right:T):T { return left; }
same(1,"wrong");
TS
if "$ROOT/tscc" --pretty false --noEmit "$TMP/conflict.ts" >"$TMP/conflict.out" 2>"$TMP/conflict.err"; then
  echo "conflicting generic inference was accepted" >&2; exit 1
fi
grep -Fq "Conflicting inferences for type parameter 'T'" "$TMP/conflict.err"
cat >"$TMP/constraint.ts" <<'TS'
function text<T extends string>(value:T):T { return value; }
text(42);
TS
if "$ROOT/tscc" --pretty false --noEmit "$TMP/constraint.ts" >"$TMP/constraint.out" 2>"$TMP/constraint.err"; then
  echo "inferred constraint violation was accepted" >&2; exit 1
fi
grep -Fq "does not satisfy constraint 'string'" "$TMP/constraint.err"
cat >"$TMP/unresolved.ts" <<'TS'
function create<T>():T { throw new Error(); }
create();
TS
if "$ROOT/tscc" --pretty false --noEmit "$TMP/unresolved.ts" >"$TMP/unresolved.out" 2>"$TMP/unresolved.err"; then
  echo "unresolved generic inference was accepted" >&2; exit 1
fi
grep -Fq "Could not infer type parameter 'T'" "$TMP/unresolved.err"
printf 'tscc deterministic generic inference passed\n'
