#!/usr/bin/env python3
from __future__ import annotations
import argparse,json,os,pathlib,platform,re,subprocess,tempfile,time
ap=argparse.ArgumentParser()
ap.add_argument("--tscc",required=True)
ap.add_argument("--rounds",type=int,default=20)
ap.add_argument("--files",type=int,default=120)
ap.add_argument("--output",required=True)
a=ap.parse_args(); tscc=str(pathlib.Path(a.tscc).resolve()); repo=pathlib.Path(__file__).resolve().parents[1]
def git_commit():
    try:return subprocess.check_output(["git","rev-parse","HEAD"],cwd=repo,text=True,stderr=subprocess.DEVNULL).strip()
    except Exception:return "unknown"
def rss(pid):
    try:
        for line in pathlib.Path(f"/proc/{pid}/status").read_text().splitlines():
            if line.startswith("VmRSS:"):return int(line.split()[1])
    except FileNotFoundError:pass
    return None
runs=[]; started=time.monotonic()
with tempfile.TemporaryDirectory(prefix="tscc-cp5-project-") as td:
    root=pathlib.Path(td); src=root/"src"; out=root/"out"; src.mkdir()
    # A broad acyclic graph with a cycle near the root.
    for i in range(a.files):
        dep=f'import {{v{i-1}}} from "./m{i-1}.js"; ' if i else ""
        (src/f"m{i}.ts").write_text(dep+f"export const v{i}:number={(f'v{i-1}+1' if i else '1')};\\n")
    (src/"cycle-a.ts").write_text('import {b} from "./cycle-b.js"; export const a:number=b+1;\\n')
    (src/"cycle-b.ts").write_text('import {a} from "./cycle-a.js"; export const b:number=1;\\n')
    (src/"main.ts").write_text(f'import {{v{a.files-1}}} from "./m{a.files-1}.js"; import {{a}} from "./cycle-a.js"; console.log(v{a.files-1},a);\\n')
    bad=src/"bad.ts"; bad.write_text('import {x} from "./missing.js"; console.log(x);\\n')
    for i in range(a.rounds):
        t=time.monotonic()
        p=subprocess.run([tscc,"--pretty","false","--rootDir",str(src),"--outDir",str(out),str(src/"main.ts")],
                         text=True,stdout=subprocess.PIPE,stderr=subprocess.PIPE)
        runs.append({"round":i,"kind":"graph","exit":p.returncode,"elapsed_seconds":round(time.monotonic()-t,6)})
        if p.returncode: raise RuntimeError(p.stderr)
        # Alternate controlled failure after a successful graph compile.
        q=subprocess.run([tscc,"--pretty","false","--outDir",str(out),str(bad)],
                         text=True,stdout=subprocess.PIPE,stderr=subprocess.PIPE)
        runs.append({"round":i,"kind":"missing-module","exit":q.returncode})
        if q.returncode==0: raise RuntimeError("missing module unexpectedly succeeded")
        # Repair and compile a tiny source immediately after failure.
        tiny=src/"tiny.ts"; tiny.write_text(f"const n:number={i}; console.log(n);\\n")
        q=subprocess.run([tscc,"--pretty","false","--outDir",str(out),str(tiny)],
                         text=True,stdout=subprocess.PIPE,stderr=subprocess.PIPE)
        if q.returncode: raise RuntimeError(q.stderr)
outp=pathlib.Path(a.output); outp.parent.mkdir(parents=True,exist_ok=True)
data={"schema_version":1,"checkpoint":"5-project","commit":git_commit(),"platform":platform.platform(),
      "rounds":a.rounds,"files":a.files,"elapsed_seconds":round(time.monotonic()-started,3),
      "phases":len(runs),"pass":True,"runs":runs}
outp.write_text(json.dumps(data,indent=2)+"\\n")
print(f"tscc checkpoint 5 project lifecycle: PASS ({a.rounds} rounds, {a.files}-file graph)")
print("evidence="+str(outp))
