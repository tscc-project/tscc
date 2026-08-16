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

printf 'tscc primitive variable type-check test passed\n'
