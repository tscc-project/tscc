#!/usr/bin/env bash
set -euo pipefail

root=$(cd "$(dirname "$0")/.." && pwd)
cd "$root"

test -f docs/handover/PRODUCT-BOUNDARY.md

if rg -n '#include[[:space:]]*[<"][^>"]*js\.h[>"]|libjs|(^|[[:space:]])-ljs([[:space:]]|$)|(^|/)js/src/' src Makefile; then
    echo "tscc product boundary violated: normal compiler sources/build depend on JS++" >&2
    exit 1
fi

if sed -n '/^test:/p;/^test-core:/p' Makefile | rg -q 'test-js-interop|JS_RUNTIME'; then
    echo "tscc product boundary violated: aggregate compiler gates require JS++" >&2
    exit 1
fi

echo "tscc/JS++ product boundary check passed"
