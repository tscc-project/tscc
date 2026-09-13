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
if "$ROOT/tscc" --pretty false --rootDir "$TMP/src" --outDir "$TMP/dist" \
    --module commonjs "$TMP/src/main.ts" >"$TMP/out" 2>"$TMP/err"; then
    echo "expected declaration-only relative module to remain a classified blocker" >&2
    exit 1
fi
grep -F "cannot resolve module './api'" "$TMP/err" >/dev/null
echo "TCP6A declaration-input blocker remains independently reproduced"
