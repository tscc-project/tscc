#!/usr/bin/env bash
set -euo pipefail
ROOT=$(cd "$(dirname "$0")/.." && pwd)
TMP=$(mktemp -d)
trap 'rm -rf "$TMP"' EXIT
mkdir -p "$TMP/src"
cat >"$TMP/src/api.d.ts" <<'EOF'
export interface User { name: string; }
export declare function user(name: string): User;
EOF
cat >"$TMP/src/main.ts" <<'EOF'
import {user} from "./api";
console.log(user("Ada").name);
EOF
"$ROOT/tscc" --pretty false --rootDir "$TMP/src" --outDir "$TMP/dist" \
    --module commonjs "$TMP/src/main.ts" >"$TMP/out" 2>"$TMP/err"
test -f "$TMP/dist/main.js"
test ! -e "$TMP/dist/api.js"
grep -F 'require("./api")' "$TMP/dist/main.js" >/dev/null
echo "TCP6A declaration input joins graph without JavaScript emission"
