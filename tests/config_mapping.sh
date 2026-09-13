#!/usr/bin/env bash
set -euo pipefail
root=$(cd "$(dirname "$0")/.." && pwd)
tmp=$(mktemp -d)
trap 'rm -rf "$tmp"' EXIT
mkdir -p "$tmp/src/lib" "$tmp/generated"
cat >"$tmp/base.json" <<'JSON'
{"compilerOptions":{"moduleResolution":"relative","noEmit":true},"include":["src/**/*.ts"]}
JSON
cat >"$tmp/tsconfig.json" <<'JSON'
{
  // JSONC and inheritance are intentional.
  "extends": "./base",
  "compilerOptions": {
    "baseUrl": ".",
    "paths": { "@lib/*": ["src/lib/*"] },
    "rootDirs": ["src", "generated"],
  },
}
JSON
cat >"$tmp/src/lib/value.ts" <<'TS'
export function value(x: number): number { return x; }
TS
cat >"$tmp/src/main.ts" <<'TS'
import { value } from "@lib/value";
value("wrong");
TS
if "$root/tscc" -p "$tmp/tsconfig.json" --pretty false >"$tmp/out" 2>&1; then
  echo "path-mapped type error was not detected" >&2; exit 1
fi
grep -F "Argument of type 'string'" "$tmp/out" >/dev/null
cat >"$tmp/a.json" <<'JSON'
{"extends":"./b","files":["src/main.ts"]}
JSON
cat >"$tmp/b.json" <<'JSON'
{"extends":"./a"}
JSON
if "$root/tscc" -p "$tmp/a.json" --pretty false >"$tmp/cycle" 2>&1; then
  echo "configuration cycle was accepted" >&2; exit 1
fi
grep -F "configuration inheritance cycle" "$tmp/cycle" >/dev/null
echo "tscc configuration mapping test passed"
