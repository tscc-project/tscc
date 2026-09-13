#!/usr/bin/env bash
set -euo pipefail
ROOT=$(cd "$(dirname "$0")/.." && pwd)
TMP=$(mktemp -d "${TMPDIR:-/tmp}/tscc-tsx.XXXXXX")
trap 'rm -rf "$TMP"' EXIT

cat >"$TMP/app.tsx" <<'TSX'
interface Props { count: number }
const count: number = 3;
const View = (p: Props): unknown => (
  <main className="app">
    <h1>{p.count as number}</h1>
    {p.count > 1 ? <span>many</span> : <b>one</b>}
    <Comp<number> value={p.count} />
  </main>
);
TSX

"$ROOT/tscc" --pretty false --jsx preserve --outDir "$TMP/out" "$TMP/app.tsx" >/dev/null
test -f "$TMP/out/app.jsx"
grep -Fq '<main className="app">' "$TMP/out/app.jsx"
grep -Fq '<Comp<number> value={p.count} />' "$TMP/out/app.jsx"
! grep -Fq 'interface Props' "$TMP/out/app.jsx"
! grep -Fq ' as number' "$TMP/out/app.jsx"

tsc --noCheck --pretty false --target es2022 --jsx preserve --outDir "$TMP/tsc" "$TMP/app.tsx"
test -f "$TMP/tsc/app.jsx"

# Malformed nesting must be diagnosed rather than emitted.
printf 'const x=<div><span></div>;\n' >"$TMP/bad.tsx"
if "$ROOT/tscc" --pretty false --jsx preserve "$TMP/bad.tsx" >"$TMP/bad.log" 2>&1; then
  echo "malformed TSX unexpectedly compiled" >&2; exit 1
fi
grep -Fq 'JSX closing tag does not match' "$TMP/bad.log"

# TSX without the mode is rejected explicitly.
if "$ROOT/tscc" --pretty false "$TMP/app.tsx" >"$TMP/mode.log" 2>&1; then
  echo "TSX without --jsx preserve unexpectedly compiled" >&2; exit 1
fi
grep -Fq 'TSX input requires --jsx preserve' "$TMP/mode.log"

echo "tscc TSX preserve test passed"

# An explicit JSX namespace opts into the bounded semantic contract.
cat >"$TMP/semantic.tsx" <<'TSX'
namespace JSX { export interface IntrinsicElements {
  panel: { title: string; count?: number; children?: unknown };
} }
interface CardProps { title: string; count?: number }
declare function Card(props: CardProps): unknown;
const good = <panel title="hello" count={2}><Card title="ok" /></panel>;
TSX
"$ROOT/tscc" --pretty false --jsx preserve --outDir "$TMP/semantic-out" "$TMP/semantic.tsx" >/dev/null
grep -Fq '<panel title="hello" count={2}>' "$TMP/semantic-out/semantic.jsx"

cat >"$TMP/semantic-bad.tsx" <<'TSX'
namespace JSX { export interface IntrinsicElements { panel: { title: string } } }
const wrongType = <panel title={3} />;
const unknownProp = <panel title="ok" extra="no" />;
const missingProp = <panel />;
const unknownTag = <missing title="x" />;
TSX
if "$ROOT/tscc" --pretty false --jsx preserve --noEmit "$TMP/semantic-bad.tsx" >"$TMP/semantic-bad.log" 2>&1; then
  echo "invalid JSX props unexpectedly checked" >&2; exit 1
fi
grep -Fq "not assignable to JSX property 'title'" "$TMP/semantic-bad.log"
grep -Fq "Property 'extra' does not exist" "$TMP/semantic-bad.log"
grep -Fq "JSX property 'title' is required" "$TMP/semantic-bad.log"
grep -Fq "intrinsic element 'missing' is not declared" "$TMP/semantic-bad.log"
