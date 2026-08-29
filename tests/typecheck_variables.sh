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

cat >"$TMP/compound-valid.ts" <<'TS'
let count: number = 1;
count += 2;
count *= 3;
let text: string = "count=";
text += count;
let big: bigint = 2n;
big **= 3n;
TS
"$ROOT/tscc" --pretty false --noEmit "$TMP/compound-valid.ts" >/dev/null

printf '%s\n' 'const fixed:number=1;fixed=2;' >"$TMP/const-assignment.ts"
if "$ROOT/tscc" --pretty false --noEmit "$TMP/const-assignment.ts" >"$TMP/const.out" 2>"$TMP/const.err"; then
    echo "const reassignment unexpectedly succeeded" >&2
    exit 1
fi
grep -Fq "Cannot assign to 'fixed' because it is a constant." "$TMP/const.err"

printf 'tscc primitive variable type-check test passed\n'

cat >"$TMP/functions-valid.ts" <<'TS'
function add(left: number, right: number): number { return left + right; }
const answer: number = add(20, 22);
TS
"$ROOT/tscc" --pretty false --noEmit "$TMP/functions-valid.ts" >/dev/null

printf '%s\n' 'function add(left:number,right:number):number{return left+right;}const value=add(1);' >"$TMP/function-arity.ts"
if "$ROOT/tscc" --pretty false --noEmit "$TMP/function-arity.ts" >"$TMP/arity.out" 2>"$TMP/arity.err"; then
    echo "invalid function arity unexpectedly succeeded" >&2
    exit 1
fi
grep -Fq "Expected 2 arguments, but got 1." "$TMP/arity.err"

printf '%s\n' 'function add(left:number,right:number):number{return left+right;}const value=add("wrong",2);' >"$TMP/function-argument.ts"
if "$ROOT/tscc" --pretty false --noEmit "$TMP/function-argument.ts" >"$TMP/argument.out" 2>"$TMP/argument.err"; then
    echo "invalid function argument unexpectedly succeeded" >&2
    exit 1
fi
grep -Fq "Argument of type 'string' is not assignable to parameter of type 'number'." "$TMP/argument.err"

printf '%s\n' 'function wrong():number{return "wrong";}' >"$TMP/function-return.ts"
if "$ROOT/tscc" --pretty false --noEmit "$TMP/function-return.ts" >"$TMP/return.out" 2>"$TMP/return.err"; then
    echo "invalid function return unexpectedly succeeded" >&2
    exit 1
fi
grep -Fq "Type 'string' is not assignable to type 'number'." "$TMP/return.err"

printf 'tscc bounded function signature test passed\n'

cat >"$TMP/type-algebra-valid.ts" <<'TS'
const mode: "on" | "off" = "on";
const answer: 42 | string = 42;
const maybe: number | null | undefined = null;
TS
"$ROOT/tscc" --pretty false --noEmit "$TMP/type-algebra-valid.ts" >/dev/null

printf '%s\n' 'const mode:"on"|"off"="other";' >"$TMP/literal-union.ts"
if "$ROOT/tscc" --pretty false --noEmit "$TMP/literal-union.ts" >"$TMP/literal.out" 2>"$TMP/literal.err"; then
    echo "invalid literal union unexpectedly succeeded" >&2
    exit 1
fi
grep -Fq "is not assignable to type" "$TMP/literal.err"

printf 'tscc literal/union/nullish type algebra test passed\n'

cat >"$TMP/narrowing-valid.ts" <<'TS'
const value: string | number = "ready";
if (typeof value === "string") { const text: string = value; }
const maybe: number | null = null;
if (maybe === null) { const absent: null = maybe; }
const mode: "on" | "off" = "on";
if (mode === "on") { const exact: "on" = mode; }
TS
"$ROOT/tscc" --pretty false --noEmit "$TMP/narrowing-valid.ts" >/dev/null

printf '%s\n' 'const value:string|number="x";if(typeof value==="string"){const bad:number=value;}' >"$TMP/narrowing-invalid.ts"
if "$ROOT/tscc" --pretty false --noEmit "$TMP/narrowing-invalid.ts" >"$TMP/narrow.out" 2>"$TMP/narrow.err"; then
    echo "invalid narrowed assignment unexpectedly succeeded" >&2
    exit 1
fi
grep -Fq "Type 'string' is not assignable to type 'number'." "$TMP/narrow.err"

printf 'tscc bounded branch narrowing test passed\n'

cat > "$TMP/named-object.ts" <<'EOF'
interface Address { city: string }
interface User { readonly id: number }
interface User { address?: Address }
type Envelope = { user: User };
const valid: Envelope = {user: {id: 1, address: {city: "Rome"}}};
const invalid: Envelope = {user: {id: 1, address: {city: 42}}};
EOF
if "$ROOT/tscc" --pretty false --noEmit "$TMP/named-object.ts" 2>"$TMP/named-object.err"; then
    echo "named object mismatch accepted" >&2; exit 1
fi
grep -Fq "property 'user.address.city'" "$TMP/named-object.err"
printf 'tscc named object declaration test passed\n'
