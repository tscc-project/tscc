#!/usr/bin/env bash
# Regression: JSX.IntrinsicElements property checking must not silently
# disappear or become incomplete for multi-member props objects. A member typed
# `unknown` (for example `children?: unknown`) previously made the whole
# contract parse as `unknown`, which caused the JSX checker to skip every
# attribute check. CP75 fixes the shared type-annotation parser so such object
# shapes parse correctly and property checking stays active (CP75).
set -euo pipefail
ROOT=$(cd "$(dirname "$0")/.." && pwd)
TMP=$(mktemp -d "${TMPDIR:-/tmp}/tscc-jsx-contract.XXXXXX")
trap 'rm -rf "$TMP"' EXIT

expect_diagnosed() {
  local name="$1" message="$2"; shift 2
  printf '%s\n' "$@" >"$TMP/$name.tsx"
  if "$ROOT/tscc" --pretty false --jsx preserve --noEmit "$TMP/$name.tsx" >"$TMP/$name.log" 2>&1; then
    echo "case '$name' unexpectedly compiled" >&2; exit 1
  fi
  if ! grep -Fq "$message" "$TMP/$name.log"; then
    echo "case '$name' did not report '$message'" >&2; cat "$TMP/$name.log" >&2; exit 1
  fi
}

expect_clean() {
  local name="$1"; shift
  printf '%s\n' "$@" >"$TMP/$name.tsx"
  if ! "$ROOT/tscc" --pretty false --jsx preserve --noEmit "$TMP/$name.tsx" >"$TMP/$name.log" 2>&1; then
    echo "case '$name' unexpectedly failed" >&2; cat "$TMP/$name.log" >&2; exit 1
  fi
}

# 1. One-property intrinsic props are checked.
expect_diagnosed "one-prop-wrong" "not assignable to JSX property 'title'" \
  'namespace JSX { export interface IntrinsicElements { panel: { title: string } } }' \
  'const bad = <panel title={3} />;'
expect_clean "one-prop-ok" \
  'namespace JSX { export interface IntrinsicElements { panel: { title: string } } }' \
  'const ok = <panel title="t" />;'

# 2. Several required properties are all enforced.
expect_diagnosed "several-required" "JSX property 'c' is required." \
  'namespace JSX { export interface IntrinsicElements { panel: { a: string; b: number; c: boolean } } }' \
  'const bad = <panel a="x" b={1} />;'

# 3. Required + optional properties.
expect_diagnosed "required-plus-optional-wrong" "not assignable to JSX property 'count'" \
  'namespace JSX { export interface IntrinsicElements { panel: { title: string; count?: number } } }' \
  'const bad = <panel title="t" count="x" />;'
expect_clean "required-plus-optional-omitted" \
  'namespace JSX { export interface IntrinsicElements { panel: { title: string; count?: number } } }' \
  'const ok = <panel title="t" />;'

# 4. `children?: unknown` in first, middle and last positions must not hide
#    other property checks.
expect_diagnosed "children-first" "not assignable to JSX property 'count'" \
  'namespace JSX { export interface IntrinsicElements { panel: { children?: unknown; title: string; count?: number } } }' \
  'const bad = <panel title="t" count="x" />;'
expect_diagnosed "children-middle" "not assignable to JSX property 'count'" \
  'namespace JSX { export interface IntrinsicElements { panel: { title: string; children?: unknown; count?: number } } }' \
  'const bad = <panel title="t" count="x" />;'
expect_diagnosed "children-last" "not assignable to JSX property 'count'" \
  'namespace JSX { export interface IntrinsicElements { panel: { title: string; count?: number; children?: unknown } } }' \
  'const bad = <panel title="t" count="x" />;'

# 5. At least four properties in one intrinsic contract are checked.
expect_diagnosed "four-props" "not assignable to JSX property 'a'" \
  'namespace JSX { export interface IntrinsicElements { panel: { a: string; b: number; c: boolean; d?: unknown; e?: string } } }' \
  'const bad = <panel a={3} b={1} c={true} />;'

# 6. Multiple intrinsic element declarations are each checked.
expect_diagnosed "multi-elements" "not assignable to JSX property 'width'" \
  'namespace JSX { export interface IntrinsicElements {' \
  '  panel: { title: string; children?: unknown };' \
  '  box: { width: number; height: number };' \
  '} }' \
  'const bad = <box width="x" height={1} />;'
expect_diagnosed "multi-elements-panel" "not assignable to JSX property 'title'" \
  'namespace JSX { export interface IntrinsicElements {' \
  '  panel: { title: string; children?: unknown };' \
  '  box: { width: number; height: number };' \
  '} }' \
  'const bad = <panel title={3} />;'

# 7. Wrong string literal supplied to a numeric property is diagnosed.
expect_diagnosed "literal-to-numeric" "not assignable to JSX property 'count'" \
  'namespace JSX { export interface IntrinsicElements { panel: { count: number } } }' \
  'const bad = <panel count="wrong" />;'

# 8. Wrong `{identifier}` expression type is diagnosed.
expect_diagnosed "expr-to-string" "not assignable to JSX property 'title'" \
  'namespace JSX { export interface IntrinsicElements { panel: { title: string } } }' \
  'const n: number = 1;' \
  'const bad = <panel title={n} />;'

# 9. Missing required property remains diagnosed.
expect_diagnosed "missing-required" "JSX property 'title' is required." \
  'namespace JSX { export interface IntrinsicElements { panel: { title: string; children?: unknown } } }' \
  'const bad = <panel />;'

# 10. Valid optional properties remain optional.
expect_clean "optional-valid" \
  'namespace JSX { export interface IntrinsicElements { panel: { title: string; count?: number; children?: unknown } } }' \
  'const ok = <panel title="t" />;' \
  'const ok2 = <panel title="t" count={5} />;'

# 11. Unknown JSX properties remain diagnosed.
expect_diagnosed "unknown-prop" "Property 'extra' does not exist" \
  'namespace JSX { export interface IntrinsicElements { panel: { title: string } } }' \
  'const bad = <panel title="t" extra="x" />;'

# 12. CP74: attribute name matching a local const is not an assignment target
#     and is not misreported, while its value is still type checked.
expect_clean "cp74-collision" \
  'namespace JSX { export interface IntrinsicElements { panel: { title: string; count?: number; children?: unknown } } }' \
  'const count: number = 2;' \
  'const ok = <panel title="t" count={count} />;'
expect_diagnosed "cp74-collision-value-checked" "not assignable to JSX property 'count'" \
  'namespace JSX { export interface IntrinsicElements { panel: { title: string; count?: number; children?: unknown } } }' \
  'const count: number = 2;' \
  'const bad = <panel title="t" count="x" />;'
if grep -Fq "Cannot assign to 'count'" "$TMP/cp74-collision-value-checked.log"; then
  echo "JSX attribute 'count' was misreported as an assignment" >&2; exit 1
fi

# 13. Genuine assignments inside JSX expression containers remain assignments.
expect_diagnosed "container-assignment" "Cannot assign to 'count' because it is a constant." \
  'namespace JSX { export interface IntrinsicElements { panel: { title?: string; children?: unknown } } }' \
  'const count: number = 2;' \
  'const bad = <panel>{count = 3}</panel>;'

# 14. Malformed JSX behavior remains unchanged.
expect_diagnosed "malformed-jsx" "JSX closing tag does not match" \
  'const x=<div><span></div>;'

echo "tscc JSX intrinsic contract regression passed"