#!/usr/bin/env bash
set -euo pipefail
compiler=${TSCC:-./tscc}
tmp=$(mktemp -d)
trap 'rm -rf "$tmp"' EXIT
printf 'const value: number = "bad";\n' > "$tmp/z.ts"
printf 'const other: number = "also bad";\n' > "$tmp/a.ts"
if "$compiler" --pretty false --noEmit "$tmp/z.ts" "$tmp/a.ts" >"$tmp/out" 2>"$tmp/err"; then exit 1; fi
grep -q 'error TSCC1000:' "$tmp/err"
first=$(grep 'error TSCC1000:' "$tmp/err" | sed -n '1p')
case "$first" in *'/a.ts:1:'*) ;; *) echo "diagnostics are not deterministically ordered" >&2; exit 1;; esac
if "$compiler" --unknown >"$tmp/out" 2>"$tmp/cli"; then exit 1; fi
grep -q "error TSCC0001: unknown option '--unknown'" "$tmp/cli"
if "$compiler" --outDir >"$tmp/out" 2>"$tmp/missing"; then exit 1; fi
grep -q "error TSCC0002: option '--outDir' requires a value" "$tmp/missing"
echo 'tscc diagnostic contract passed'
