#!/usr/bin/env python3
from pathlib import Path
import argparse, json, os, shutil, statistics, subprocess, time

ROOT=Path(__file__).resolve().parent.parent
BENCH=ROOT/"benchmarks"
TSCC=ROOT/"tscc"

def run(cmd,cwd=None):
    t=time.perf_counter_ns()
    p=subprocess.run(cmd,cwd=cwd,stdout=subprocess.DEVNULL,stderr=subprocess.DEVNULL)
    return p.returncode,(time.perf_counter_ns()-t)/1e6

def sample(label,cmd,cwd,out_dir,runs,warmups=2):
    values=[]
    for _ in range(warmups):
        shutil.rmtree(out_dir,ignore_errors=True)
        out_dir.mkdir(parents=True,exist_ok=True)
        rc,_=run(cmd,cwd)
        if rc: raise RuntimeError(f"{label} warmup failed")
    for _ in range(runs):
        shutil.rmtree(out_dir,ignore_errors=True)
        out_dir.mkdir(parents=True,exist_ok=True)
        rc,ms=run(cmd,cwd)
        if rc: raise RuntimeError(f"{label} failed")
        values.append(ms)
    return {
      "label":label,"runs":runs,
      "median_ms":statistics.median(values),
      "mean_ms":statistics.mean(values),
      "min_ms":min(values),"max_ms":max(values),
      "samples_ms":values,
    }

def main():
    ap=argparse.ArgumentParser()
    ap.add_argument("--runs",type=int,default=10)
    ap.add_argument("--json",type=Path)
    ap.add_argument("--case", action="append", choices=["small","many-100","many-500","features-100","advanced-100"],
                    help="run only selected case(s); may be repeated")
    a=ap.parse_args()
    small=BENCH/"fixtures/small/app.ts"
    many=sorted((BENCH/"fixtures/many").glob("*.ts"))
    many500=sorted((BENCH/"fixtures/many-500").glob("*.ts"))
    features100=sorted((BENCH/"fixtures/features-100").glob("*.ts"))
    advanced100=sorted((BENCH/"fixtures/advanced-100").glob("*.ts"))
    tmp=BENCH/".tmp"
    results=[]
    cases=[
      ("small", [str(small)]),
      ("many-100", [str(x) for x in many]),
      ("many-500", [str(x) for x in many500]),
      ("features-100", [str(x) for x in features100]),
      ("advanced-100", [str(x) for x in advanced100]),
    ]
    if a.case:
      selected=set(a.case)
      cases=[case for case in cases if case[0] in selected]
    for cname,files in cases:
      o=tmp/(cname+"-tscc")
      results.append(sample(f"tscc {cname}",
        [str(TSCC),"--pretty","false","--outDir",str(o),*files],ROOT,o,a.runs))
      o=tmp/(cname+"-tsc-nocheck")
      results.append(sample(f"tsc --noCheck {cname}",
        ["tsc","--noCheck","--pretty","false","--target","es2022","--module","commonjs","--outDir",str(o),*files],
        ROOT,o,a.runs))
      o=tmp/(cname+"-tsc")
      results.append(sample(f"tsc full {cname}",
        ["tsc","--pretty","false","--target","es2022","--module","commonjs","--skipLibCheck","--outDir",str(o),*files],
        ROOT,o,a.runs))
    shutil.rmtree(tmp,ignore_errors=True)
    print(f"{'benchmark':28} {'median ms':>10} {'mean ms':>10} {'min ms':>10}")
    for r in results:
      print(f"{r['label']:28} {r['median_ms']:10.3f} {r['mean_ms']:10.3f} {r['min_ms']:10.3f}")
    # Speedups against analogous tsc rows.
    by={r["label"]:r for r in results}
    for cname,_ in cases:
      base=by[f"tscc {cname}"]["median_ms"]
      for other in (f"tsc --noCheck {cname}",f"tsc full {cname}"):
        print(f"{other} / tscc: {by[other]['median_ms']/base:.2f}x")
    if a.json:
      a.json.parent.mkdir(parents=True,exist_ok=True)
      a.json.write_text(json.dumps(results,indent=2)+"\n")
if __name__=="__main__": main()
