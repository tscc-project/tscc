#!/usr/bin/env bash
set -euo pipefail
ROOT=$(cd "$(dirname "$0")/.." && pwd)
TMP=$(mktemp -d "${TMPDIR:-/tmp}/tscc-cjs.XXXXXX")
trap 'rm -rf "$TMP"' EXIT
mkdir -p "$TMP/src"

cat >"$TMP/src/math.ts" <<'TS'
export const a: number = 2;
export let b: number = 3;
export function add(x: number, y: number): number { return x + y; }
export class Box { constructor(public value: number) {} }
const secret: number = 9;
export { secret as hidden };
export default function twice(x: number): number { return x * 2; }
TS

cat >"$TMP/src/re.ts" <<'TS'
export { a as alpha, add } from "./math.js";
export * from "./more.js";
TS

cat >"$TMP/src/more.ts" <<'TS'
export const extra: number = 7;
export default 99;
TS

cat >"$TMP/src/side.ts" <<'TS'
(globalThis as any).__side = 5;
TS

cat >"$TMP/src/main.ts" <<'TS'
import "./side.js";
import twice, { a, b as bee, add, Box, hidden } from "./math.js";
import * as re from "./re.js";
console.log(twice(4), a, bee, add(1,2), new Box(6).value, hidden,
            re.alpha, re.add(2,3), re.extra, (globalThis as any).__side);
TS

"$ROOT/tscc" --pretty false --module commonjs --rootDir "$TMP/src" --outDir "$TMP/out" "$TMP/src/main.ts" >/dev/null
test "$(FORCE_COLOR=0 node "$TMP/out/main.js")" = "8 2 3 3 6 9 2 5 7 5"

tsc --noCheck --pretty false --target es2022 --module commonjs \
  --rootDir "$TMP/src" --outDir "$TMP/tsc" "$TMP/src/main.ts"
test "$(FORCE_COLOR=0 node "$TMP/tsc/main.js")" = "8 2 3 3 6 9 2 5 7 5"

# Default class, anonymous default class/function and export assignment.
cat >"$TMP/src/defaults.ts" <<'TS'
export default class Named { value = 10; }
TS
cat >"$TMP/src/anon.ts" <<'TS'
export default function (x: number) { return x + 1; }
TS
cat >"$TMP/src/use-defaults.ts" <<'TS'
import Named from "./defaults.js";
import inc from "./anon.js";
console.log(new Named().value, inc(10));
TS
"$ROOT/tscc" --pretty false --module commonjs --rootDir "$TMP/src" --outDir "$TMP/out2" "$TMP/src/use-defaults.ts" >/dev/null
test "$(FORCE_COLOR=0 node "$TMP/out2/use-defaults.js")" = "10 11"

cat >"$TMP/src/assign.ts" <<'TS'
const x = 12;
export = x;
TS
"$ROOT/tscc" --pretty false --module commonjs --outDir "$TMP/out3" "$TMP/src/assign.ts" >/dev/null
test "$(FORCE_COLOR=0 node -e "console.log(require('$TMP/out3/assign.js'))")" = "12"

# Cycle must compile once per module and execute.
cat >"$TMP/src/cycle-a.ts" <<'TS'
import { b } from "./cycle-b.js";
export const a: number = 1;
export function sum(): number { return a + b; }
TS
cat >"$TMP/src/cycle-b.ts" <<'TS'
import { a } from "./cycle-a.js";
export const b: number = 2;
export function readA(): number { return a; }
TS
cat >"$TMP/src/cycle-main.ts" <<'TS'
import {sum} from "./cycle-a.js";
console.log(sum());
TS
"$ROOT/tscc" --pretty false --module commonjs --rootDir "$TMP/src" --outDir "$TMP/cycle-out" "$TMP/src/cycle-main.ts" >/dev/null
test "$(FORCE_COLOR=0 node "$TMP/cycle-out/cycle-main.js")" = "3"


# Live imported bindings must observe later exporter mutation.
cat >"$TMP/src/live.ts" <<'TS'
export let value: number = 1;
export function bump(): void { value += 1; }
export default function read(): number { return value; }
TS
cat >"$TMP/src/live-main.ts" <<'TS'
import read, {value, bump} from "./live.js";
console.log(value, read());
bump();
console.log(value, read());
function shadow(value: number) { return value + 10; }
console.log(shadow(5));
const obj = {value};
console.log(obj.value);
TS
"$ROOT/tscc" --pretty false --module commonjs --rootDir "$TMP/src" --outDir "$TMP/live-out" "$TMP/src/live-main.ts" >/dev/null
"$ROOT/tscc" --pretty false --module commonjs --rootDir "$TMP/src" --outDir "$TMP/live-out" "$TMP/src/live.ts" >/dev/null
FORCE_COLOR=0 node "$TMP/live-out/live-main.js" >"$TMP/live-ours.txt"

tsc --noCheck --pretty false --target es2022 --module commonjs \
  --rootDir "$TMP/src" --outDir "$TMP/live-tsc" "$TMP/src/live-main.ts" "$TMP/src/live.ts"
FORCE_COLOR=0 node "$TMP/live-tsc/live-main.js" >"$TMP/live-tsc.txt"
cmp "$TMP/live-ours.txt" "$TMP/live-tsc.txt"

# Block/catch/arrow shadows must not be rewritten to imported properties.
cat >"$TMP/src/shadow-main.ts" <<'TS'
import {value} from "./live.js";
{
  const value = 20;
  console.log(value);
}
try { throw 3; } catch (value) { console.log(value); }
const f = (value: number) => value + 1;
console.log(f(4), value);
TS
"$ROOT/tscc" --pretty false --module commonjs --rootDir "$TMP/src" --outDir "$TMP/shadow-out" "$TMP/src/shadow-main.ts" >/dev/null
test "$(FORCE_COLOR=0 node "$TMP/shadow-out/shadow-main.js")" = $'20\n3\n5 1'


# Destructured exports, named-default specifiers and namespace re-exports.
cat >"$TMP/src/modern-base.ts" <<'TS'
export let value: number = 30;
export function bump(): void { value++; }
const internal = 4;
export { internal as default };
TS
cat >"$TMP/src/modern-re.ts" <<'TS'
export { default as four, value, bump } from "./modern-base.js";
export * as ns from "./modern-base.js";
TS
cat >"$TMP/src/destructure.ts" <<'TS'
const src = {a: 2, b: 3};
export const {a, b} = src;
export const [c, d] = [4, 5];
TS
cat >"$TMP/src/modern-main.ts" <<'TS'
import { default as four } from "./modern-base.js";
import {four as again, value, bump, ns} from "./modern-re.js";
import {a,b,c,d} from "./destructure.js";
console.log(four, again, value, ns.value, a+b+c+d);
bump();
console.log(value, ns.value);
TS
"$ROOT/tscc" --pretty false --module commonjs --rootDir "$TMP/src" --outDir "$TMP/modern-out" "$TMP/src/modern-main.ts" >/dev/null
FORCE_COLOR=0 node "$TMP/modern-out/modern-main.js" >"$TMP/modern-ours.txt"
tsc --noCheck --pretty false --target es2022 --module commonjs --rootDir "$TMP/src" --outDir "$TMP/modern-tsc" "$TMP/src/modern-main.ts"
FORCE_COLOR=0 node "$TMP/modern-tsc/modern-main.js" >"$TMP/modern-tsc.txt"
cmp "$TMP/modern-ours.txt" "$TMP/modern-tsc.txt"


# Live re-export aliases and namespace re-exports.
cat >"$TMP/src/state2.ts" <<'TS'
export let value: number = 1;
export function bump(): void { value++; }
export default function read(): number { return value; }
TS
cat >"$TMP/src/barrel2.ts" <<'TS'
export {value as current, bump, default as read} from "./state2.js";
export * as state from "./state2.js";
TS
cat >"$TMP/src/reexport-main.ts" <<'TS'
import {current, bump, read, state} from "./barrel2.js";
console.log(current, read(), state.value);
bump();
console.log(current, read(), state.value);
TS
"$ROOT/tscc" --pretty false --module commonjs --rootDir "$TMP/src" --outDir "$TMP/reexport-out" "$TMP/src/reexport-main.ts" >/dev/null
FORCE_COLOR=0 node "$TMP/reexport-out/reexport-main.js" >"$TMP/reexport-ours.txt"
tsc --noCheck --pretty false --target es2022 --module commonjs --rootDir "$TMP/src" --outDir "$TMP/reexport-tsc" "$TMP/src/reexport-main.ts"
FORCE_COLOR=0 node "$TMP/reexport-tsc/reexport-main.js" >"$TMP/reexport-tsc.txt"
cmp "$TMP/reexport-ours.txt" "$TMP/reexport-tsc.txt"

# Live import references inside templates and for-loop shadowing.
cat >"$TMP/src/context-main.ts" <<'TS'
import {value, bump} from "./state2.js";
console.log(`${value}:${value}`);
for (let value = 0; value < 1; value++) console.log(value);
bump();
console.log(`${value}:${value}`);
TS
"$ROOT/tscc" --pretty false --module commonjs --rootDir "$TMP/src" --outDir "$TMP/context-out" "$TMP/src/context-main.ts" >/dev/null
FORCE_COLOR=0 node "$TMP/context-out/context-main.js" >"$TMP/context-ours.txt"
tsc --noCheck --pretty false --target es2022 --module commonjs --rootDir "$TMP/src" --outDir "$TMP/context-tsc" "$TMP/src/context-main.ts"
FORCE_COLOR=0 node "$TMP/context-tsc/context-main.js" >"$TMP/context-tsc.txt"
cmp "$TMP/context-ours.txt" "$TMP/context-tsc.txt"

# A semicolonless import ends at its module clause, not at the next statement.
cat >"$TMP/src/semicolonless-main.ts" <<'TS'
import {value} from "./state2.js"
console.log(value)
TS
"$ROOT/tscc" --pretty false --module commonjs --rootDir "$TMP/src" \
  --outDir "$TMP/semicolonless-out" "$TMP/src/semicolonless-main.ts" >/dev/null
test "$(FORCE_COLOR=0 node "$TMP/semicolonless-out/semicolonless-main.js")" = "1"

echo "tscc CommonJS module transform test passed"
