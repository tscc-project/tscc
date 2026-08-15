#!/usr/bin/env python3
from pathlib import Path
import argparse, json, shutil, statistics, subprocess, time
ROOT=Path(__file__).resolve().parent.parent
P=ROOT/"benchmarks/fixtures/project-200"
def run(cmd):
    t=time.perf_counter_ns()
    r=subprocess.run(cmd,cwd=ROOT,stdout=subprocess.DEVNULL,stderr=subprocess.DEVNULL)
    return r.returncode,(time.perf_counter_ns()-t)/1e6
def sample(cmd,out,runs):
    vals=[]
    for _ in range(2):
        shutil.rmtree(out,ignore_errors=True); rc,_=run(cmd)
        if rc: raise SystemExit("warmup failed")
    for _ in range(runs):
        shutil.rmtree(out,ignore_errors=True); rc,ms=run(cmd)
        if rc: raise SystemExit("benchmark failed")
        vals.append(ms)
    return statistics.median(vals),vals
ap=argparse.ArgumentParser();ap.add_argument("--runs",type=int,default=5);ap.add_argument("--json",type=Path);a=ap.parse_args()
to=P/"dist"; tc=P/"dist-tsc"
m1,s1=sample([str(ROOT/"tscc"),"-p",str(P/"tsconfig.json"),"--outDir",str(to),"--pretty","false"],to,a.runs)
m2,s2=sample(["tsc","-p",str(P/"tsconfig.json"),"--outDir",str(tc),"--pretty","false","--noCheck"],tc,a.runs)
m3,s3=sample(["tsc","-p",str(P/"tsconfig.json"),"--outDir",str(tc),"--pretty","false"],tc,a.runs)
rows=[{"label":"tscc project-200","median_ms":m1,"samples_ms":s1},{"label":"tsc --noCheck project-200","median_ms":m2,"samples_ms":s2},{"label":"tsc full project-200","median_ms":m3,"samples_ms":s3}]
for r in rows:print(f"{r['label']:28} {r['median_ms']:9.3f} ms")
print(f"tsc --noCheck / tscc: {m2/m1:.2f}x")
print(f"tsc full / tscc: {m3/m1:.2f}x")
if a.json:a.json.write_text(json.dumps(rows,indent=2)+"\n")
