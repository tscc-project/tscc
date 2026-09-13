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
cat >"$TMP/src/bad.ts" <<'EOF'
import {user} from "./api";
user(42);
EOF
if "$ROOT/tscc" --pretty false --noEmit --module commonjs "$TMP/src/bad.ts" \
    >"$TMP/bad-out" 2>"$TMP/bad-err"; then
    echo "expected imported declaration signature to reject wrong argument" >&2
    exit 1
fi
grep -F "not assignable to parameter of type 'string'" "$TMP/bad-err" >/dev/null
echo "TCP6A declaration graph and imported callable facts passed"
