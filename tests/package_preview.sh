#!/usr/bin/env bash
set -euo pipefail
root=$(cd "$(dirname "$0")/.." && pwd)
stage=$(mktemp -d)
trap 'rm -rf "$stage"' EXIT
tar -xzf "$root/.build/package/tscc-compiler-preview.tar.gz" -C "$stage"
test "$($stage/usr/bin/tscc --version | head -n 1)" = "tscc 0.15.0"
(cd "$stage/usr/share/tscc/examples/compiler-preview/valid" && "$stage/usr/bin/tscc" --pretty false -p tsconfig.json >/dev/null)
test "$(node "$stage/usr/share/tscc/examples/compiler-preview/valid/out/main.js")" = 42
echo "tscc installed compiler preview package passed"
