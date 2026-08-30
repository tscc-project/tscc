#!/usr/bin/env bash
set -euo pipefail
compiler=${TSCC:-./tscc}
compiler=$(realpath "$compiler")
tmp=$(mktemp -d)
trap 'rm -rf "$tmp"' EXIT
cp -R examples/compiler-preview/valid "$tmp/valid"
(cd "$tmp/valid" && "$compiler" --pretty false -p tsconfig.json >/dev/null)
test "$(node "$tmp/valid/out/main.js")" = 42
(cd "$tmp/valid/out" && sha256sum main.js model.js | sort) > "$tmp/first"
rm -rf "$tmp/valid/out"
(cd "$tmp/valid" && "$compiler" --pretty false -p tsconfig.json >/dev/null)
(cd "$tmp/valid/out" && sha256sum main.js model.js | sort) > "$tmp/second"
cmp "$tmp/first" "$tmp/second"

mkdir -p "$tmp/mixed"
printf 'const good: number = 42; console.log(good);\n' > "$tmp/mixed/good.ts"
printf 'const bad: number = "wrong";\n' > "$tmp/mixed/bad.ts"
if "$compiler" --pretty false --outDir "$tmp/default-out" "$tmp/mixed/good.ts" "$tmp/mixed/bad.ts" >/dev/null 2>&1; then exit 1; fi
test -f "$tmp/default-out/good.js"
if "$compiler" --pretty false --noEmitOnError --outDir "$tmp/all-out" "$tmp/mixed/good.ts" "$tmp/mixed/bad.ts" >/dev/null 2>&1; then exit 1; fi
test ! -e "$tmp/all-out/good.js"
if "$compiler" --pretty false --target es5 "$tmp/mixed/good.ts" >"$tmp/out" 2>"$tmp/target"; then exit 1; fi
grep -q "error TSCC3001: unsupported target 'es5'" "$tmp/target"
printf '{"compilerOptions":{"unknownPreviewOption":true},"files":["good.ts"]}\n' > "$tmp/mixed/tsconfig.json"
if (cd "$tmp/mixed" && "$compiler" --pretty false -p tsconfig.json >"$tmp/out" 2>"$tmp/config"); then exit 1; fi
grep -q "error TSCC3001: unsupported compiler option 'unknownPreviewOption'" "$tmp/config"
echo 'tscc project and output contract passed'
