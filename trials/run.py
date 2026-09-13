#!/usr/bin/env python3
"""Run pinned TCP6A projects without disguising unsupported behavior as passes."""
import argparse
import json
import pathlib
import shutil
import subprocess
import sys
import time

PHASES = {"acquisition", "configuration", "parser", "binder", "checker",
          "resolution", "lowering", "emission", "runtime", "diagnostic",
          "crash", "timeout"}


def classify(returncode, stderr):
    text = stderr.lower()
    if returncode < 0:
        return "crash"
    if "cannot resolve" in text or "module" in text and "not found" in text:
        return "resolution"
    if "tsconfig" in text or "compiler option" in text:
        return "configuration"
    if "type" in text and ("assignable" in text or "call" in text or "property" in text):
        return "checker"
    if "expected" in text or "unexpected" in text or "unterminated" in text:
        return "parser"
    return "diagnostic"


def run(command, cwd, timeout):
    started = time.monotonic()
    try:
        proc = subprocess.run(command, cwd=cwd, text=True, capture_output=True,
                              timeout=timeout)
        return proc.returncode, proc.stdout, proc.stderr, (time.monotonic() - started) * 1000
    except subprocess.TimeoutExpired as exc:
        return 124, exc.stdout or "", exc.stderr or "", (time.monotonic() - started) * 1000


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--tscc", required=True)
    parser.add_argument("--manifest", required=True)
    parser.add_argument("--json")
    args = parser.parse_args()
    manifest_path = pathlib.Path(args.manifest).resolve()
    root = manifest_path.parent
    manifest = json.loads(manifest_path.read_text())
    if manifest.get("schema") != 1:
        raise SystemExit("unsupported trial manifest schema")
    results = []
    fatal = False
    for project in manifest["projects"]:
        project_root = root / project["path"]
        timeout = project.get("timeout_seconds", manifest["defaults"]["timeout_seconds"])
        dist = project_root / "dist"
        shutil.rmtree(dist, ignore_errors=True)
        missing = [name for name in project["entry"] if not (project_root / name).is_file()]
        if missing:
            result = {"id": project["id"], "status": "fail", "phase": "acquisition",
                      "detail": "missing: " + ", ".join(missing)}
        else:
            command = [str(pathlib.Path(args.tscc).resolve()), "--pretty", "false",
                       *project["compiler_options"],
                       *project["entry"]]
            code, out, err, compile_ms = run(command, project_root, timeout)
            if code == 124:
                result = {"id": project["id"], "status": "fail", "phase": "timeout"}
            elif code != 0:
                result = {"id": project["id"], "status": "unsupported",
                          "phase": classify(code, err), "exit_code": code,
                          "stderr": err.strip(), "compile_ms": round(compile_ms, 3)}
            else:
                oracle = project["oracle"]
                code, out, err, runtime_ms = run(oracle["command"], project_root, timeout)
                if code == 124:
                    result = {"id": project["id"], "status": "fail", "phase": "timeout"}
                elif code != 0 or out != oracle["stdout"]:
                    result = {"id": project["id"], "status": "fail", "phase": "runtime",
                              "exit_code": code, "stdout": out, "stderr": err.strip()}
                else:
                    result = {"id": project["id"], "status": "pass",
                              "compile_ms": round(compile_ms, 3),
                              "runtime_ms": round(runtime_ms, 3)}
        expected = project["expected"]
        result["expected"] = expected
        result["matches_expectation"] = result["status"] == expected
        if expected == "unsupported" and result["status"] == "unsupported":
            result["matches_expectation"] &= result["phase"] == project["expected_phase"]
        fatal |= not result["matches_expectation"]
        results.append(result)
        print(f"{project['id']}: {result['status']} ({result.get('phase', 'oracle')})")
    report = {"schema": 1, "compiler": str(pathlib.Path(args.tscc).resolve()),
              "manifest": str(manifest_path), "results": results,
              "summary": {name: sum(r["status"] == name for r in results)
                          for name in ("pass", "unsupported", "fail")}}
    if args.json:
        output = pathlib.Path(args.json)
        output.parent.mkdir(parents=True, exist_ok=True)
        output.write_text(json.dumps(report, indent=2) + "\n")
    return 1 if fatal else 0


if __name__ == "__main__":
    sys.exit(main())
