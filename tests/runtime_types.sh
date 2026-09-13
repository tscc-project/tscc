#!/usr/bin/env bash
set -euo pipefail
ROOT=$(cd "$(dirname "$0")/.." && pwd)
TMP=$(mktemp -d "${TMPDIR:-/tmp}/tscc-runtime-types.XXXXXX")
trap 'rm -rf "$TMP"' EXIT
cat >"$TMP/good.ts" <<'TS'
enum State { Ready = "ready", Done = "done", Count = 2 }
const ready: State = State.Ready;
const count: number = State.Count;
class Ticket { constructor(public state: State, readonly id: number) {} }
const ticket = new Ticket(State.Done, 3);
const id: number = ticket.id;
namespace Metrics { export const total: number = 3; }
namespace Metrics { export function label(): string { return "ok"; } }
console.log(ready, count, id, Metrics.total, Metrics.label());
TS
"$ROOT/tscc" --pretty false --outDir "$TMP/out" "$TMP/good.ts" >/dev/null
node "$TMP/out/good.js" >"$TMP/result"
grep -Fq 'ready 2 3 3 ok' "$TMP/result"
cat >"$TMP/bad.ts" <<'TS'
enum State { Ready = "ready", Done = "done" }
const impossible: State = "other";
TS
if "$ROOT/tscc" --pretty false --noEmit "$TMP/bad.ts" >"$TMP/bad.log" 2>&1; then
  echo "invalid enum assignment unexpectedly checked" >&2; exit 1
fi
grep -Fq "not assignable to type" "$TMP/bad.log"
echo "runtime-bearing TypeScript type semantics passed"
