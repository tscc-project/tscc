#!/usr/bin/env bash
set -euo pipefail
ROOT=$(cd "$(dirname "$0")/.." && pwd)
TMP=$(mktemp -d); trap 'rm -rf "$TMP"' EXIT
cat > "$TMP/basic.ts" <<'TS'
interface User { name: string; age?: number }
type ID = string | number;
const greet = (user: User, suffix: string = "!"): string => {
  const name: string = user.name as string;
  return name + suffix;
};
class Box implements User {
  public readonly name: string = "Ada";
  private value: number = 3;
}
console.log(greet({name:"Ada"} as User));
TS
"$ROOT/tscc" --outDir "$TMP/out" "$TMP/basic.ts" >/dev/null
node --check "$TMP/out/basic.js"
node "$TMP/out/basic.js" | grep -Fx 'Ada!'
"$ROOT/tscc" --noEmit "$TMP/basic.ts" >/dev/null
"$ROOT/tscc" --version | grep -Fq '0.15.0'
"$ROOT/tscc" --help | grep -Fq -- '--outDir'
printf 'const x: string = "ok";\n' > "$TMP/compare.ts"
"$ROOT/tscc" --outDir "$TMP/ours" "$TMP/compare.ts" >/dev/null
tsc --target es2022 --module none --outDir "$TMP/tsc" "$TMP/compare.ts"
node --check "$TMP/ours/compare.js"
printf 'tscc smoke test passed\n'

cat >"$TMP/parser-cases.ts" <<'TS'
interface User<T> { name: string; value: T }
type Maybe<T> = T | null;
function greet(user: User<number>, suffix: string = "!"): string {
  const obj = {name: "Ada", score: 3};
  const typed: number = obj.score;
  return user.name + suffix + typed;
}
class Box implements User<number> {
  public readonly name: string = "Ada";
  private value: number = 3;
  method(x: number, y: string): string { const z: number = x + 1; return y + z; }
}
const literal = {name: "Ada", nested: {x: 1}};
const typedObject: {name: string; x: number} = {name: "Grace", x: 2};
console.log(greet({name:"A",value:1},"?"), literal.name, typedObject.x, new Box().method(2,"x"));
TS
"$ROOT/tscc" --pretty false --outDir "$TMP/out-parser" "$TMP/parser-cases.ts" >/dev/null
node --check "$TMP/out-parser/parser-cases.js"
test "$(FORCE_COLOR=0 node "$TMP/out-parser/parser-cases.js")" = "A?3 Ada 2 x3"

TMP2=$(mktemp -d); trap 'rm -rf "$TMP" "$TMP2"' EXIT
printf 'const x: string = "unterminated\n' > "$TMP2/bad.ts"
if "$ROOT/tscc" --pretty false "$TMP2/bad.ts" >"$TMP2/out" 2>"$TMP2/err"; then
  echo "malformed TypeScript unexpectedly succeeded" >&2; exit 1
fi
grep -Fq 'bad.ts:1:' "$TMP2/err"
grep -Fq 'unterminated string/template literal' "$TMP2/err"
! grep -Fq $'\033[' "$TMP2/err"
