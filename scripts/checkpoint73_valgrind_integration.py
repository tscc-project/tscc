#!/usr/bin/env python3
"""Bounded Valgrind integration workload for the real tscc binary.

The in-process lifetime checkpoint predates CP66-CP72 and does not exercise
the compiler-facing paths those checkpoints added: JSX.IntrinsicElements
semantic contracts, enums/namespaces/parameter properties, source maps,
declaration and declaration-map emit, incremental output handling, project
graph construction, and repeated compiler lifetimes.

This runner invokes the real `tscc` binary on temporary fixtures under the
same strict Valgrind options as the in-process checkpoint (plus
--track-fds=yes so file-descriptor leaks are visible). Every scenario
records its own Valgrind error summary and leak categories; the run fails
unless every invocation is clean and the compiler's expected behaviour is
reproduced.
"""
from __future__ import annotations

import argparse
import datetime as dt
import json
import os
import platform
import re
import shlex
import shutil
import subprocess
import sys
import tempfile
import time
from pathlib import Path

VALGRIND_OPTIONS = [
    "--leak-check=full",
    "--show-leak-kinds=all",
    "--errors-for-leak-kinds=definite,indirect,possible",
    "--track-origins=yes",
    "--error-exitcode=99",
    "--track-fds=yes",
]

FINDING_PATTERNS = {
    "valgrind_error": re.compile(r"ERROR SUMMARY:\s*[1-9][0-9]* errors", re.I),
    "valgrind_leak": re.compile(
        r"definitely lost:\s*(?!0 bytes)|indirectly lost:\s*(?!0 bytes)|possibly lost:\s*(?!0 bytes)",
        re.I,
    ),
    "invalid_read": re.compile(r"Invalid read of size", re.I),
    "invalid_write": re.compile(r"Invalid write of size", re.I),
    "uninitialised": re.compile(r"uninitialised value|Conditional jump or move depends on", re.I),
    "mismatched_free": re.compile(r"Mismatched free|different size block", re.I),
    "invalid_free": re.compile(r"Invalid free", re.I),
    "file_descriptor_leak": re.compile(r"FILE DESCRIPTORS:\s*\d+ open \(\d+ (?:inherited|std)\) at exit\s*\n\s*Open file descriptor", re.I),
}

FIXTURES = {
    "math.ts": (
        "export function sum(a: number, b: number): number { return a + b; }\n"
        "export const pi: number = 3.14159;\n"
    ),
    "greet.ts": (
        'import {pi} from "./math.js";\n'
        'export function greet(name: string, n: number): string { return name + ":" + n + ":" + pi; }\n'
    ),
    "main.ts": (
        'import {sum} from "./math.js";\n'
        'import {greet} from "./greet.js";\n'
        "const total: number = sum(2, 3);\n"
        'console.log(greet("tscc", total));\n'
    ),
    "app.tsx": (
        "namespace JSX { export interface IntrinsicElements {\n"
        "  panel: { title: string; count?: number; children?: unknown };\n"
        "} }\n"
        "interface CardProps { title: string; count?: number }\n"
        "declare function Card(props: CardProps): unknown;\n"
        "const n: number = 2;\n"
        'const good = <panel title="hello" count={n}><Card title="ok" /></panel>;\n'
        "console.log(good);\n"
    ),
    "features.ts": (
        'enum State { Ready = "ready", Done = "done", Count = 2 }\n'
        "const ready: State = State.Ready;\n"
        "const count: number = State.Count;\n"
        "class Ticket { constructor(public state: State, readonly id: number) {} }\n"
        "const ticket = new Ticket(State.Done, 3);\n"
        "const id: number = ticket.id;\n"
        "namespace Metrics { export const total: number = 3; }\n"
        "namespace Metrics { export function label(): string { return \"ok\"; } }\n"
        "console.log(ready, count, id, Metrics.total, Metrics.label());\n"
    ),
    "maps.ts": (
        "interface Hidden { value: number }\n"
        "enum Mode { A, B }\n"
        "const value: number = Mode.B;\n"
        "console.log(value);\n"
    ),
    "api.ts": (
        "export interface Point { x: number; y: number }\n"
        "export type Name = string;\n"
        "export enum Axis { X, Y }\n"
        "export const origin: Point = {x: 0, y: 0};\n"
        "export function distance(value: Point): number { return value.x + value.y; }\n"
    ),
    "a.ts": "export const answer: number = 42;\n",
    "b.ts": 'import {answer} from "./a.js"; console.log(answer);\n',
    "bad.ts": (
        'const broken: number = "not a number";\n'
        "interface Bad { x: number }\n"
        'const n: Bad = {x: "wrong"};\n'
    ),
    "cj-mod.ts": (
        "export const base: number = 21;\n"
        "export function scale(v: number): number { return v * base; }\n"
    ),
    "cj-main.ts": (
        'import {base, scale} from "./cj-mod.js";\n'
        "console.log(scale(base));\n"
    ),
}


def git_commit(cwd: Path) -> str:
    try:
        return subprocess.check_output(
            ["git", "rev-parse", "HEAD"], cwd=cwd, text=True,
            stderr=subprocess.DEVNULL,
        ).strip()
    except Exception:
        return "unknown"


def version_line(argv: list[str]) -> str | None:
    try:
        p = subprocess.run(argv, text=True, stdout=subprocess.PIPE,
                           stderr=subprocess.STDOUT, timeout=5)
        line = p.stdout.strip().splitlines()
        return line[0] if line else None
    except Exception:
        return None


def detect_findings(text: str) -> list[str]:
    return [name for name, pattern in FINDING_PATTERNS.items() if pattern.search(text)]


def parse_error_summary(text: str) -> int | None:
    m = re.search(r"ERROR SUMMARY:\s*(\d+) errors", text)
    return int(m.group(1)) if m else None


def parse_leaks(text: str) -> dict[str, int]:
    def lost(label: str) -> int:
        m = re.search(rf"^\s*{label}:\s*([0-9,]+) bytes", text, re.M)
        return int(m.group(1).replace(",", "")) if m else 0
    return {
        "definitely_lost_bytes": lost("definitely lost"),
        "indirectly_lost_bytes": lost("indirectly lost"),
        "possibly_lost_bytes": lost("possibly lost"),
        "still_reachable_bytes": lost("still reachable"),
    }


def parse_fds(text: str) -> dict[str, int] | None:
    m = re.search(r"FILE DESCRIPTORS:\s*(\d+) open \(\s*(\d+) (?:inherited|std)\) at exit", text)
    if not m:
        return None
    return {"open": int(m.group(1)), "inherited": int(m.group(2))}


class Workload:
    def __init__(self, tscc: str, root: Path, rounds: int):
        self.tscc = str(Path(tscc).resolve())
        self.root = root
        self.rounds = rounds
        self.src = root / "src"
        self.runs: list[dict] = []
        self.failures: list[str] = []

    def write_fixtures(self) -> None:
        self.src.mkdir(parents=True, exist_ok=True)
        for name, text in FIXTURES.items():
            (self.src / name).write_text(text, encoding="utf-8")
        tsconfig = self.root / "tsconfig.json"
        tsconfig.write_text(json.dumps({
            "compilerOptions": {
                "moduleResolution": "relative",
                "outDir": "./outts",
                "rootDir": "./src",
            },
            "files": ["src/main.ts", "src/features.ts"],
        }) + "\n", encoding="utf-8")

    def run(self, name: str, args: list[str], expected_exit: int = 0,
            check_outputs: list[str] | None = None,
            check_no_outputs: list[str] | None = None) -> dict:
        valgrind = shutil.which("valgrind")
        if not valgrind:
            self.failures.append(f"{name}: valgrind not found")
            return {"name": name, "error": "valgrind not found"}
        command = [valgrind, *VALGRIND_OPTIONS, self.tscc, *args]
        started = time.monotonic()
        p = subprocess.run(command, cwd=self.root, text=True,
                           stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        elapsed = time.monotonic() - started
        combined = p.stdout + "\n" + p.stderr
        leaks = parse_leaks(combined)
        record = {
            "name": name,
            "command": command,
            "expected_exit": expected_exit,
            "exit_status": p.returncode,
            "elapsed_seconds": round(elapsed, 6),
            "error_summary": parse_error_summary(combined),
            "leaks": leaks,
            "file_descriptors": parse_fds(combined),
            "findings": detect_findings(combined),
            "stdout_tail": "\n".join(p.stdout.splitlines()[-20:]),
            "stderr_tail": "\n".join(p.stderr.splitlines()[-40:]),
        }
        problems = []
        if p.returncode != expected_exit:
            problems.append(
                f"exit {p.returncode} != expected {expected_exit}")
        if record["error_summary"]:
            problems.append(
                f"ERROR SUMMARY: {record['error_summary']} errors")
        for key in ("definitely_lost_bytes", "indirectly_lost_bytes",
                    "possibly_lost_bytes"):
            if leaks[key]:
                problems.append(f"{key}={leaks[key]}")
        if record["findings"]:
            problems.append(f"findings={record['findings']}")
        fds = record["file_descriptors"]
        if fds and fds["open"] > fds["inherited"]:
            problems.append(f"file descriptors open={fds['open']} inherited={fds['inherited']}")
        for path in check_outputs or []:
            if not (self.root / path).exists():
                problems.append(f"missing expected output {path}")
        for path in check_no_outputs or []:
            if (self.root / path).exists():
                problems.append(f"unexpected output present {path}")
        if problems:
            record["problems"] = problems
            self.failures.append(f"{name}: " + "; ".join(problems))
        self.runs.append(record)
        return record

    def execute(self) -> dict:
        self.write_fixtures()

        self.run(
            "multi-file-project",
            ["--pretty", "false", "--rootDir", "src", "--outDir", "out1", "src/main.ts"],
            check_outputs=["out1/main.js", "out1/math.js", "out1/greet.js"],
        )
        self.run(
            "tsx-jsx-intrinsics-contract",
            ["--pretty", "false", "--jsx", "preserve", "--rootDir", "src",
             "--outDir", "out2", "src/app.tsx"],
            check_outputs=["out2/app.jsx"],
        )
        self.run(
            "enums-namespaces-parameter-properties",
            ["--pretty", "false", "--rootDir", "src", "--outDir", "out3",
             "src/features.ts"],
            check_outputs=["out3/features.js"],
        )
        self.run(
            "source-map",
            ["--pretty", "false", "--sourceMap", "--rootDir", "src",
             "--outDir", "out4", "src/maps.ts"],
            check_outputs=["out4/maps.js", "out4/maps.js.map"],
        )
        self.run(
            "declaration-declaration-map",
            ["--pretty", "false", "--declaration", "--declarationMap",
             "--rootDir", "src", "--outDir", "out5", "src/api.ts"],
            check_outputs=["out5/api.js", "out5/api.d.ts", "out5/api.d.ts.map"],
        )
        self.run(
            "incremental-build-1",
            ["--pretty", "false", "--incremental", "--rootDir", "src",
             "--outDir", "inc", "src/b.ts"],
            check_outputs=["inc/.tscc-buildinfo", "inc/a.js", "inc/b.js"],
        )
        before_a = (self.root / "inc" / "a.js").read_bytes()
        before_b = (self.root / "inc" / "b.js").read_bytes()
        self.run(
            "incremental-build-2",
            ["--pretty", "false", "--incremental", "--rootDir", "src",
             "--outDir", "inc", "src/b.ts"],
            check_outputs=["inc/.tscc-buildinfo", "inc/a.js", "inc/b.js"],
        )
        after_a = (self.root / "inc" / "a.js").read_bytes()
        after_b = (self.root / "inc" / "b.js").read_bytes()
        if before_a != after_a or before_b != after_b:
            self.failures.append("incremental-build: unchanged outputs were rewritten")

        self.run(
            "invalid-input-diagnostics",
            ["--pretty", "false", "src/bad.ts"],
            expected_exit=2,
        )
        self.run(
            "no-emit-on-error",
            ["--pretty", "false", "--noEmitOnError", "--rootDir", "src",
             "--outDir", "outerr", "src/bad.ts"],
            expected_exit=2,
            check_no_outputs=["outerr/bad.js"],
        )
        self.run(
            "commonjs-output",
            ["--pretty", "false", "--module", "commonjs", "--rootDir", "src",
             "--outDir", "out9", "src/cj-main.ts"],
            check_outputs=["out9/cj-main.js"],
        )
        self.run(
            "tsconfig-project-graph",
            ["--pretty", "false", "-p", "tsconfig.json"],
            check_outputs=["outts/main.js", "outts/features.js"],
        )

        for round_no in range(1, self.rounds + 1):
            self.run(
                f"repeated-lifetime-round-{round_no}-cli",
                ["--pretty", "false", "--rootDir", "src", "--outDir",
                 f"outr{round_no}", "src/main.ts"],
                check_outputs=[f"outr{round_no}/main.js"],
            )
            self.run(
                f"repeated-lifetime-round-{round_no}-tsconfig",
                ["--pretty", "false", "-p", "tsconfig.json"],
                check_outputs=["outts/main.js"],
            )

        summary = {
            "completed_runs": len(self.runs),
            "pass": not self.failures,
            "failures": self.failures,
            "valgrind_error_total": sum(
                r.get("error_summary") or 0 for r in self.runs),
            "definitely_lost_bytes": sum(
                r["leaks"]["definitely_lost_bytes"] for r in self.runs),
            "indirectly_lost_bytes": sum(
                r["leaks"]["indirectly_lost_bytes"] for r in self.runs),
            "possibly_lost_bytes": sum(
                r["leaks"]["possibly_lost_bytes"] for r in self.runs),
            "still_reachable_bytes": sum(
                r["leaks"]["still_reachable_bytes"] for r in self.runs),
            "file_descriptor_leaks": sum(
                1 for r in self.runs
                if r.get("file_descriptors")
                and r["file_descriptors"]["open"] > r["file_descriptors"]["inherited"]),
        }
        return summary


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("--tscc", required=True, help="path to the tscc binary")
    ap.add_argument("--rounds", type=int, default=3,
                    help="repeated-lifetime rounds (each round is two clean invocations)")
    ap.add_argument("--output", required=True)
    ap.add_argument("--cwd", default=".")
    args = ap.parse_args()

    if not shutil.which("valgrind"):
        print("error: valgrind not found", file=sys.stderr)
        return 2

    cwd = Path(args.cwd).resolve()
    metadata = {
        "schema_version": 1,
        "checkpoint": "73-valgrind-integration",
        "commit": git_commit(cwd),
        "timestamp_utc": dt.datetime.now(dt.timezone.utc).isoformat(),
        "platform": {
            "system": platform.system(),
            "release": platform.release(),
            "machine": platform.machine(),
            "python": platform.python_version(),
        },
        "toolchain": {
            "compiler": version_line([os.environ.get("CXX", "c++"), "--version"]),
            "valgrind": version_line(["valgrind", "--version"]),
        },
        "valgrind_options": VALGRIND_OPTIONS,
        "rounds": args.rounds,
    }

    with tempfile.TemporaryDirectory(prefix="tscc-cp73-valgrind-") as td:
        root = Path(td)
        workload = Workload(args.tscc, root, args.rounds)
        metadata["summary"] = workload.execute()
        metadata["scenarios"] = workload.runs

    out = Path(args.output)
    if not out.is_absolute():
        out = cwd / out
    out.parent.mkdir(parents=True, exist_ok=True)
    out.write_text(json.dumps(metadata, indent=2) + "\n", encoding="utf-8")

    s = metadata["summary"]
    print(f"tscc CP73 valgrind integration: {'PASS' if s['pass'] else 'FAIL'}")
    print(f"runs={s['completed_runs']} valgrind_errors={s['valgrind_error_total']} "
          f"definitely_lost={s['definitely_lost_bytes']}B "
          f"indirectly_lost={s['indirectly_lost_bytes']}B "
          f"possibly_lost={s['possibly_lost_bytes']}B "
          f"still_reachable={s['still_reachable_bytes']}B "
          f"fd_leaks={s['file_descriptor_leaks']}")
    print(f"evidence={out}")
    if s["failures"]:
        for failure in s["failures"]:
            print("failure: " + failure, file=sys.stderr)
    return 0 if s["pass"] else 1


if __name__ == "__main__":
    raise SystemExit(main())