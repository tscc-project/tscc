#!/usr/bin/env bash
set -euo pipefail
ROOT=$(cd "$(dirname "$0")/.." && pwd)
TMP=$(mktemp -d "${TMPDIR:-/tmp}/tscc-project.XXXXXX")
trap 'rm -rf "$TMP"' EXIT
mkdir -p "$TMP/src/lib"
cat >"$TMP/package.json" <<'JSON'
{"type":"module"}
JSON
cat >"$TMP/tsconfig.json" <<'JSON'
{
  // JSONC + trailing comma are intentional.
  "compilerOptions": {
    "rootDir": "src",
    "outDir": "dist",
    "target": "es2022",
    "module": "esnext",
    "removeComments": true,
  },
  "files": ["src/main.ts"],
}
JSON
cat >"$TMP/src/types.ts" <<'TS'
export interface User { name: string }
export type ID = string | number;
TS
cat >"$TMP/src/lib/math.ts" <<'TS'
export function double<T extends number>(x: T): number { return x * 2; }
export enum Mode { A, B = 3 }
TS
cat >"$TMP/src/main.ts" <<'TS'
import type { User } from "./types.js";
import { double, Mode } from "./lib/math.js";
const user: User = {name: "Ada"};
console.log(user.name, double<number>(4), Mode.B);
TS

"$ROOT/tscc" -p "$TMP/tsconfig.json" --pretty false >/dev/null
test -f "$TMP/dist/main.js"
test -f "$TMP/dist/lib/math.js"
test -f "$TMP/dist/types.js"
! grep -Fq 'import type' "$TMP/dist/main.js"
! grep -Fq 'interface User' "$TMP/dist/types.js"
test "$(FORCE_COLOR=0 node "$TMP/dist/main.js")" = "Ada 8 3"

# Relative cycles are deduplicated rather than recursively compiling forever.
mkdir -p "$TMP/cycle"
cat >"$TMP/cycle/a.ts" <<'TS'
import {b} from "./b.js"; export const a: number = b + 1;
TS
cat >"$TMP/cycle/b.ts" <<'TS'
import {a} from "./a.js"; export const b: number = 1;
TS
"$ROOT/tscc" --pretty false --rootDir "$TMP/cycle" --outDir "$TMP/cycle-out" "$TMP/cycle/a.ts" >/dev/null
test -f "$TMP/cycle-out/a.js"
test -f "$TMP/cycle-out/b.js"

# A missing relative module is an attributed compiler error.
cat >"$TMP/missing.ts" <<'TS'
import {x} from "./does-not-exist.js";
console.log(x);
TS
if "$ROOT/tscc" --pretty false "$TMP/missing.ts" >"$TMP/missing.log" 2>&1; then
  echo "missing relative module unexpectedly succeeded" >&2; exit 1
fi
grep -Fq "cannot resolve module './does-not-exist.js'" "$TMP/missing.log"

# --noResolve compiles only explicitly supplied files and leaves imports alone.
"$ROOT/tscc" --pretty false --noResolve --outDir "$TMP/noresolve" "$TMP/missing.ts" >/dev/null
test -f "$TMP/noresolve/missing.js"


# TSX project mode: tsconfig jsx=preserve, .jsx specifier resolves to .tsx,
# and output keeps the runtime-correct .jsx import path.
mkdir -p "$TMP/tsx-project/src/ui"
cat >"$TMP/tsx-project/package.json" <<'JSON'
{"type":"module"}
JSON
cat >"$TMP/tsx-project/tsconfig.json" <<'JSON'
{
  "compilerOptions": {
    "rootDir": "src",
    "outDir": "dist",
    "target": "es2022",
    "module": "esnext",
    "jsx": "preserve"
  },
  "files": ["src/main.ts"]
}
JSON
cat >"$TMP/tsx-project/src/ui/View.tsx" <<'TSX'
export const View = (n: number) => <div>{n}</div>;
TSX
cat >"$TMP/tsx-project/src/main.ts" <<'TS'
import {View} from "./ui/View.jsx";
console.log(typeof View);
TS
"$ROOT/tscc" -p "$TMP/tsx-project/tsconfig.json" --pretty false >/dev/null
test -f "$TMP/tsx-project/dist/main.js"
test -f "$TMP/tsx-project/dist/ui/View.jsx"
grep -Fq 'from "./ui/View.jsx"' "$TMP/tsx-project/dist/main.js"
grep -Fq '<div>{n}</div>' "$TMP/tsx-project/dist/ui/View.jsx"

echo "tscc project/module graph test passed"

# Mixed type-only named specifiers preserve runtime imports while erasing types.
mkdir -p "$TMP/mixed/src"
cat >"$TMP/mixed/package.json" <<'JSON'
{"type":"module"}
JSON
cat >"$TMP/mixed/src/lib.ts" <<'TS'
export interface Shape {x:number}
export const value:number=9;
export type Name=string;
TS
cat >"$TMP/mixed/src/main.ts" <<'TS'
import {type Shape, value} from "./lib.js";
type Local=Shape;
export {type Name, value} from "./lib.js";
const x:Local={x:value}; console.log(x.x);
TS
"$ROOT/tscc" --pretty false --rootDir "$TMP/mixed/src" --outDir "$TMP/mixed/dist" "$TMP/mixed/src/main.ts" >/dev/null
! grep -Fq 'type Shape' "$TMP/mixed/dist/main.js"
! grep -Fq 'type Name' "$TMP/mixed/dist/main.js"
test "$(FORCE_COLOR=0 node "$TMP/mixed/dist/main.js")" = "9"

# include/exclude project discovery.
mkdir -p "$TMP/glob/src/skip" "$TMP/glob/tests"
cat >"$TMP/glob/tsconfig.json" <<'JSON'
{"compilerOptions":{"rootDir":"src","outDir":"dist","module":"esnext"},"include":["src/**/*.ts"],"exclude":["src/skip/**"]}
JSON
printf 'console.log(1);\n' >"$TMP/glob/src/a.ts"
printf 'console.log(2);\n' >"$TMP/glob/src/skip/b.ts"
printf 'console.log(3);\n' >"$TMP/glob/tests/c.ts"
"$ROOT/tscc" -p "$TMP/glob/tsconfig.json" --pretty false >/dev/null
test -f "$TMP/glob/dist/a.js"
test ! -e "$TMP/glob/dist/skip/b.js"
test ! -e "$TMP/glob/dist/tests/c.js"
