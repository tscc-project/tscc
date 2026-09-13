#!/usr/bin/env bash
# Regression: the general assignment scan in the checker must not treat a JSX
# attribute of the form `name={value}` or `name="value"` as an assignment to a
# binding whose name matches the attribute name (CP74). Const-assignment
# checking, JSX property type checking and malformed-JSX diagnostics must be
# unaffected.
set -euo pipefail
ROOT=$(cd "$(dirname "$0")/.." && pwd)
TMP=$(mktemp -d "${TMPDIR:-/tmp}/tscc-jsx-attr.XXXXXX")
trap 'rm -rf "$TMP"' EXIT

# 1. Attribute name identical to a local const is not an assignment target.
cat >"$TMP/const-collision.tsx" <<'TSX'
namespace JSX { export interface IntrinsicElements {
  panel: { title: string; count?: number; children?: unknown };
} }
const count: number = 2;
const good = <panel title="hello" count={count} />;
console.log(good);
TSX
"$ROOT/tscc" --pretty false --jsx preserve --outDir "$TMP/out1" "$TMP/const-collision.tsx" >/dev/null
test -f "$TMP/out1/const-collision.jsx"
grep -Fq '<panel title="hello" count={count} />' "$TMP/out1/const-collision.jsx"

# 2. Several attributes with local-name collisions compile cleanly.
cat >"$TMP/multi-collision.tsx" <<'TSX'
namespace JSX { export interface IntrinsicElements {
  box: { width: number; height: number; label?: string; children?: unknown };
} }
const width: number = 10;
const height: number = 20;
const label: string = "ok";
const box = <box width={width} height={height} label={label} />;
console.log(box);
TSX
"$ROOT/tscc" --pretty false --jsx preserve --outDir "$TMP/out2" "$TMP/multi-collision.tsx" >/dev/null
test -f "$TMP/out2/multi-collision.jsx"

# 3. `{value}` expression references are still type checked normally.
cat >"$TMP/value-typecheck.tsx" <<'TSX'
namespace JSX { export interface IntrinsicElements {
  panel: { title: string };
} }
const count: number = 3;
const bad = <panel title={count} />;
TSX
if "$ROOT/tscc" --pretty false --jsx preserve --noEmit "$TMP/value-typecheck.tsx" >"$TMP/vt.log" 2>&1; then
  echo "JSX expression-reference type error unexpectedly accepted" >&2; exit 1
fi
grep -Fq "not assignable to JSX property 'title'" "$TMP/vt.log"

# 3b. An attribute name colliding with a local const still has its value type
#     checked, and is not misreported as an assignment to the const.
cat >"$TMP/value-typecheck-collision.tsx" <<'TSX'
namespace JSX { export interface IntrinsicElements {
  panel: { title: string; count?: number };
} }
const count: number = 3;
const bad = <panel title="t" count="wrong" />;
TSX
if "$ROOT/tscc" --pretty false --jsx preserve --noEmit "$TMP/value-typecheck-collision.tsx" >"$TMP/vtc.log" 2>&1; then
  echo "JSX attribute value type error unexpectedly accepted" >&2; exit 1
fi
grep -Fq "not assignable to JSX property 'count'" "$TMP/vtc.log"
! grep -Fq "Cannot assign to 'count'" "$TMP/vtc.log"

# 4. Genuine const reassignment is still rejected.
cat >"$TMP/const-reassign.tsx" <<'TSX'
const count: number = 2;
count = 3;
TSX
if "$ROOT/tscc" --pretty false --noEmit "$TMP/const-reassign.tsx" >"$TMP/cr.log" 2>&1; then
  echo "const reassignment unexpectedly accepted" >&2; exit 1
fi
grep -Fq "Cannot assign to 'count' because it is a constant" "$TMP/cr.log"

# 5. JSX intrinsic property type checking still works (missing required prop).
cat >"$TMP/missing-prop.tsx" <<'TSX'
namespace JSX { export interface IntrinsicElements { panel: { title: string } } }
const missing = <panel />;
TSX
if "$ROOT/tscc" --pretty false --jsx preserve --noEmit "$TMP/missing-prop.tsx" >"$TMP/mp.log" 2>&1; then
  echo "missing required JSX prop unexpectedly accepted" >&2; exit 1
fi
grep -Fq "JSX property 'title' is required" "$TMP/mp.log"

# 6. Malformed JSX behavior is unchanged.
printf 'const x=<div><span></div>;\n' >"$TMP/bad.tsx"
if "$ROOT/tscc" --pretty false --jsx preserve "$TMP/bad.tsx" >"$TMP/bad.log" 2>&1; then
  echo "malformed TSX unexpectedly compiled" >&2; exit 1
fi
grep -Fq 'JSX closing tag does not match' "$TMP/bad.log"

# 7. A real assignment inside a JSX expression container is still rejected.
cat >"$TMP/container-assign.tsx" <<'TSX'
namespace JSX { export interface IntrinsicElements { panel: { title?: string; children?: unknown } } }
const count: number = 2;
const bad = <panel>{count = 3}</panel>;
TSX
if "$ROOT/tscc" --pretty false --jsx preserve --noEmit "$TMP/container-assign.tsx" >"$TMP/ca.log" 2>&1; then
  echo "const assignment inside JSX container unexpectedly accepted" >&2; exit 1
fi
grep -Fq "Cannot assign to 'count' because it is a constant" "$TMP/ca.log"

# 8. Nested JSX inside an attribute-value container is classified correctly.
cat >"$TMP/nested.tsx" <<'TSX'
namespace JSX { export interface IntrinsicElements {
  panel: { title: string; render?: unknown; children?: unknown };
  inner: { x: number; children?: unknown };
} }
const x: number = 1;
const good = <panel title="t" render={<inner x={x} />} />;
console.log(good);
TSX
"$ROOT/tscc" --pretty false --jsx preserve --outDir "$TMP/out8" "$TMP/nested.tsx" >/dev/null
test -f "$TMP/out8/nested.jsx"

echo "tscc JSX attribute assignment classification passed"