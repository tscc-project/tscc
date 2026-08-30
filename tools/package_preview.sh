#!/usr/bin/env bash
set -euo pipefail
root=$(cd "$(dirname "$0")/.." && pwd)
mkdir -p "$root/.build/package"
build_one() {
    local stage=$1 output=$2
    make -C "$root" install DESTDIR="$stage" PREFIX=/usr
    tar --sort=name --mtime='UTC 2026-08-30' --owner=0 --group=0 --numeric-owner -C "$stage" -czf "$output" usr
}
first=$(mktemp -d);second=$(mktemp -d)
trap 'rm -rf "$first" "$second"' EXIT
build_one "$first" "$root/.build/package/tscc-compiler-preview.tar.gz"
build_one "$second" "$root/.build/package/reproducibility-check.tar.gz"
cmp "$root/.build/package/tscc-compiler-preview.tar.gz" "$root/.build/package/reproducibility-check.tar.gz"
echo "tscc reproducible compiler preview package built"
