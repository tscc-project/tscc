#!/usr/bin/env python3
from pathlib import Path
import argparse, json, shutil, statistics, subprocess, time
ROOT=Path(__file__).resolve().parent.parent
P=ROOT/"benchmarks/fixtures/commonjs-200"
ENTRY=P/"src/main.ts"
TMP=ROOT/"benchmarks/.cjs-tmp"

def timed(cmd):
    start=time.perf_counter_ns()
    r=subprocess.run(cmd,cwd=ROOT,stdout=subprocess.DEVNULL,stderr=subprocess.DEVNULL)
    return r.returncode,(time.perf_counter_ns()-start)/1e6

def sample(label,cmd,out,runs):
    vals=[]
    for _ in range(2):
        shutil.rmtree(out,ignore_errors=True)
        rc,_=timed(cmd)
        if rc: raise RuntimeError(label+" warmup failed")
    for _ in range(runs):
        shutil.rmtree(out,ignore_errors=True)
        rc,ms=timed(cmd)
        if rc: raise RuntimeError(label+" failed")
        vals.append(ms)
    return {"label":label,"median_ms":statistics.median(vals),"mean_ms":statistics.mean(vals),"samples_ms":vals}

ap=argparse.ArgumentParser();ap.add_argument("--runs",type=int,default=5);ap.add_argument("--json",type=Path);a=ap.parse_args()
ours=TMP/"ours";nochk=TMP/"noCheck";full=TMP/"full"
rows=[
 sample("tscc commonjs-200",[str(ROOT/"tscc"),"--pretty","false","--module","commonjs","--rootDir",str(P/"src"),"--outDir",str(ours),str(ENTRY)],ours,a.runs),
 sample("tsc --noCheck commonjs-200",["tsc","--noCheck","--pretty","false","--target","es2022","--module","commonjs","--rootDir",str(P/"src"),"--outDir",str(nochk),str(ENTRY)],nochk,a.runs),
 sample("tsc full commonjs-200",["tsc","--pretty","false","--target","es2022","--module","commonjs","--skipLibCheck","--rootDir",str(P/"src"),"--outDir",str(full),str(ENTRY)],full,a.runs),
]
for r in rows: print(f"{r['label']:31} {r['median_ms']:9.3f} ms")
print(f"tsc --noCheck / tscc: {rows[1]['median_ms']/rows[0]['median_ms']:.2f}x")
if a.json:a.json.write_text(json.dumps(rows,indent=2)+"\n")
shutil.rmtree(TMP,ignore_errors=True)
