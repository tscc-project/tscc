#!/usr/bin/env bash
set -euo pipefail
ROOT=$(cd "$(dirname "$0")/.." && pwd)
TMP=$(mktemp -d "${TMPDIR:-/tmp}/tscc-checker.XXXXXX")
trap 'rm -rf "$TMP"' EXIT

cat >"$TMP/valid.ts" <<'TS'
const count: number = -1, label: string = "ready";
let enabled: boolean = true;
const commented /* binding */ : /* annotation */ number = /* initializer */ 2;
const object: {value: number} = {value: 1};
const complex: number | string = "outside the first checker slice";
const inferred = "not checked yet";
TS
"$ROOT/tscc" --pretty false --noEmit "$TMP/valid.ts" >/dev/null

for fixture in number string boolean; do
    case "$fixture" in
        number) source='const value: number = "wrong";' ; expected="Type 'string' is not assignable to type 'number'." ;;
        string) source='const value: string = 42;' ; expected="Type 'number' is not assignable to type 'string'." ;;
        boolean) source='const value: boolean = "wrong";' ; expected="Type 'string' is not assignable to type 'boolean'." ;;
    esac
    printf '%s\n' "$source" >"$TMP/$fixture.ts"
    if "$ROOT/tscc" --pretty false --noEmit "$TMP/$fixture.ts" >"$TMP/$fixture.out" 2>"$TMP/$fixture.err"; then
        echo "mismatched primitive initializer unexpectedly succeeded" >&2
        exit 1
    fi
    grep -Fq "$expected" "$TMP/$fixture.err"
done

printf '%s\n' 'const value: number = 1n;' >"$TMP/bigint.ts"
if "$ROOT/tscc" --pretty false --noEmit "$TMP/bigint.ts" >"$TMP/bigint.out" 2>"$TMP/bigint.err"; then
    echo "bigint-to-number mismatch unexpectedly succeeded" >&2
    exit 1
fi
grep -Fq "Type 'bigint' is not assignable to type 'number'." "$TMP/bigint.err"

cat >"$TMP/references-valid.ts" <<'TS'
const text: string = "ready";
const copy: string = text;
let count: number = 1;
const next: number = 2;
count = next;
{
    const text: number = 3;
    const local: number = text;
}
const outer: string = text;
TS
"$ROOT/tscc" --pretty false --noEmit "$TMP/references-valid.ts" >/dev/null

cat >"$TMP/reference-mismatch.ts" <<'TS'
const source: string = "wrong";
const value: number = source;
TS
if "$ROOT/tscc" --pretty false --noEmit "$TMP/reference-mismatch.ts" >"$TMP/ref.out" 2>"$TMP/ref.err"; then
    echo "bound identifier initializer mismatch unexpectedly succeeded" >&2
    exit 1
fi
grep -Fq "Type 'string' is not assignable to type 'number'." "$TMP/ref.err"

cat >"$TMP/assignment-mismatch.ts" <<'TS'
let value: number = 1;
const source: string = "wrong";
value = source;
TS
if "$ROOT/tscc" --pretty false --noEmit "$TMP/assignment-mismatch.ts" >"$TMP/assign.out" 2>"$TMP/assign.err"; then
    echo "bound direct assignment mismatch unexpectedly succeeded" >&2
    exit 1
fi
grep -Fq "Type 'string' is not assignable to type 'number'." "$TMP/assign.err"

cat >"$TMP/expressions-valid.ts" <<'TS'
const left: number = 2;
const right: number = 3;
const arithmetic: number = left + right * 4;
const grouped: number = (left + right) * 4;
const negative: number = -left;
const inverted: number = ~right;
const truthy: boolean = !left;
const kind: string = typeof left;
const big: bigint = 2n * 3n;
const text: string = "value=" + left;
TS
"$ROOT/tscc" --pretty false --noEmit "$TMP/expressions-valid.ts" >/dev/null

printf '%s\n' 'const value = "wrong" - 1;' >"$TMP/operator-mismatch.ts"
if "$ROOT/tscc" --pretty false --noEmit "$TMP/operator-mismatch.ts" >"$TMP/op.out" 2>"$TMP/op.err"; then
    echo "invalid primitive operator unexpectedly succeeded" >&2
    exit 1
fi
grep -Fq "Operator '-' cannot be applied to types 'string' and 'number'." "$TMP/op.err"

printf 'tscc primitive variable type-check test passed\n'
