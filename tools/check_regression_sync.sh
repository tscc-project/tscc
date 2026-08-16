#!/usr/bin/env bash
set -euo pipefail

ROOT=$(cd "$(dirname "$0")/.." && pwd)
SUITE=${1:-"$ROOT/../tscc-regression-suite"}

if [[ ! -d "$SUITE" ]]; then
    echo "canonical regression suite not found: $SUITE" >&2
    echo "pass its path as the first argument when it is not a sibling of tscc" >&2
    exit 2
fi

for file in cases.json run.py; do
    if ! cmp -s "$SUITE/$file" "$ROOT/regression/$file"; then
        echo "regression mirror drift: $file" >&2
        diff -u "$SUITE/$file" "$ROOT/regression/$file" || true
        exit 1
    fi
done

echo "tscc regression mirror matches canonical executable contract"
