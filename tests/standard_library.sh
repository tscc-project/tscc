#!/usr/bin/env bash
set -euo pipefail
ROOT=$(cd "$(dirname "$0")/.." && pwd)
TMP=$(mktemp -d)
trap 'find "$TMP" -type f -delete; find "$TMP" -depth -type d -empty -delete' EXIT
printf '%s\n' 'const bad:Error={message:42,name:"Error"};' >"$TMP/date.ts"
if "$ROOT/tscc" --pretty false --noEmit "$TMP/date.ts" >"$TMP/date.out" 2>"$TMP/date.err"; then
  echo "default es2022 library type was not enforced" >&2; exit 1
fi
grep -Fq "message" "$TMP/date.err"
"$ROOT/tscc" --pretty false --noLib --noEmit "$TMP/date.ts"

printf '%s\n' 'const bad:Buffer={length:"wrong",toString:()=>"ok"};' >"$TMP/node.ts"
if "$ROOT/tscc" --pretty false --noLib --types node --noEmit "$TMP/node.ts" >"$TMP/node.out" 2>"$TMP/node.err"; then
  echo "selected node type bundle was not enforced" >&2; exit 1
fi
grep -Fq "length" "$TMP/node.err"

cat >"$TMP/tsconfig.json" <<'JSON'
{"compilerOptions":{"noEmit":true,"noLib":true,"types":["node"]},"files":["node.ts"]}
JSON
if "$ROOT/tscc" --pretty false -p "$TMP/tsconfig.json" >"$TMP/config.out" 2>"$TMP/config.err"; then
  echo "configured type bundle was not enforced" >&2; exit 1
fi
grep -Fq "length" "$TMP/config.err"
if "$ROOT/tscc" --pretty false --lib host-machine --noEmit "$TMP/date.ts" >"$TMP/unknown.out" 2>"$TMP/unknown.err"; then
  echo "unknown library bundle was accepted" >&2; exit 1
fi
grep -Fq "unsupported library bundle 'host-machine'" "$TMP/unknown.err"
printf 'tscc hermetic standard library contract passed\n'
