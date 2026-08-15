#!/usr/bin/env bash
set -euo pipefail
ROOT=$(cd "$(dirname "$0")/.." && pwd)
TMP=$(mktemp -d "${TMPDIR:-/tmp}/tscc-runtime.XXXXXX")
trap 'rm -rf "$TMP"' EXIT

cat >"$TMP/features.ts" <<'TS'
function id<T>(value: T): T { return value; }
const arrow = <T>(x: T): T => x;

class Pair<T> {
  constructor(public left: T, readonly right: T) {}
  map<U extends Array<T>>(x: U): U { return x; }
}
class Child extends Pair<number> {
  constructor(public readonly name: string, left: number, right: number) {
    super(left, right);
  }
}

enum Direction { Up, Down = 3, Left, Label = "label" }
enum Numbers { Neg = -2, Next, Hex = 0x10, HexNext, Bin = 0b10, BinNext }

class Fields {
  definite!: number;
  optional?: string;
  constructor(public readonly id: number, private label?: string) {
    this.definite = id;
  }
  getLabel(): string | undefined { return this.label; }
}

const nested: Promise<Array<string>> | null = null;
const value = arrow<Array<string>>(["a","b"]);
const p = new Pair<number>(1, 2);
const c = new Child("kid", 4, 5);
const f = new Fields(7, "x");
console.log(id<string>("x"), value[1], p.left, p.right, c.name,
            Direction.Up, Direction[3], Direction.Left, Direction.Label,
            Numbers.Neg, Numbers.Next, Numbers.HexNext, Numbers.BinNext,
            f.id, f.getLabel(), f.definite, nested);
TS

"$ROOT/tscc" --pretty false --outDir "$TMP/ours" "$TMP/features.ts" >/dev/null
node --check "$TMP/ours/features.js"
FORCE_COLOR=0 node "$TMP/ours/features.js" >"$TMP/ours.out"

tsc --pretty false --target es2022 --module commonjs --outDir "$TMP/tsc" "$TMP/features.ts"
FORCE_COLOR=0 node "$TMP/tsc/features.js" >"$TMP/tsc.out"
cmp "$TMP/ours.out" "$TMP/tsc.out"

# Parameter properties must be emitted once per parameter and after super in derived classes.
test "$(grep -o 'this.id = id;' "$TMP/ours/features.js" | wc -l)" -eq 1
python3 - "$TMP/ours/features.js" <<'PY'
import sys
s=open(sys.argv[1]).read()
child=s[s.index("class Child"):s.index("var Direction")]
assert child.index("super(left, right);") < child.index("this.name = name;")
PY

echo "tscc runtime/generics differential test passed"
