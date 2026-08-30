#!/usr/bin/env bash
set -euo pipefail
root=$(cd "$(dirname "$0")/.." && pwd)
"$root/tscc" --pretty false --noEmit -p "$root/examples/compiler-preview/valid/tsconfig.json"
if "$root/tscc" --pretty false --noEmit -p "$root/examples/compiler-preview/invalid/tsconfig.json" >/dev/null 2>&1; then exit 1; fi
"$root/tscc" --pretty false --noEmit -p "$root/tests/project_fixture/tsconfig.json"
python3 "$root/tools/preview_fuzz.py" "$root/tscc"
echo "tscc compiler preview representative projects passed"
