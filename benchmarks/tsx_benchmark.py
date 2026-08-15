#!/usr/bin/env python3
from pathlib import Path
import argparse, json, shutil, statistics, subprocess, time
ROOT=Path(__file__).resolve().parent.parent
FILES=sorted((ROOT/"benchmarks/fixtures/tsx-100").glob("*.tsx"))
TMP=ROOT/"benchmarks/.tsx-tmp"
def run(cmd):
    t=time.perf_counter_ns()
    p=subprocess.run(cmd,cwd=ROOT,stdout=subprocess.DEVNULL,stderr=subprocess.DEVNULL)
    return p.returncode,(time.perf_counter_ns()-t)/1e6
def sample(label,cmd,out,runs):
    vals=[]
    for _ in range(2):
        shutil.rmtree(out,ignore_errors=True);out.mkdir(parents=True,exist_ok=True)
        rc,_=run(cmd)
        if rc: raise RuntimeError(label+" warmup failed")
    for _ in range(runs):
        shutil.rmtree(out,ignore_errors=True);out.mkdir(parents=True,exist_ok=True)
        rc,ms=run(cmd)
        if rc: raise RuntimeError(label+" failed")
        vals.append(ms)
    return {"label":label,"median_ms":statistics.median(vals),"mean_ms":statistics.mean(vals),"samples_ms":vals}
ap=argparse.ArgumentParser();ap.add_argument("--runs",type=int,default=5);ap.add_argument("--json",type=Path);a=ap.parse_args()
fs=[str(x) for x in FILES]
ours=TMP/"ours";ref=TMP/"tsc";full=TMP/"full"
rows=[
 sample("tscc tsx-100",[str(ROOT/"tscc"),"--pretty","false","--jsx","preserve","--outDir",str(ours),*fs],ours,a.runs),
 sample("tsc --noCheck tsx-100",["tsc","--noCheck","--pretty","false","--target","es2022","--jsx","preserve","--outDir",str(ref),*fs],ref,a.runs),
 sample("tsc full tsx-100",["tsc","--pretty","false","--target","es2022","--jsx","preserve","--skipLibCheck","--outDir",str(full),*fs],full,a.runs),
]
for r in rows: print(f"{r['label']:26} {r['median_ms']:9.3f} ms")
print(f"tsc --noCheck / tscc: {rows[1]['median_ms']/rows[0]['median_ms']:.2f}x")
if a.json:a.json.write_text(json.dumps(rows,indent=2)+"\n")
shutil.rmtree(TMP,ignore_errors=True)
