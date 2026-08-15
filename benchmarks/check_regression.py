#!/usr/bin/env python3
from pathlib import Path
import argparse, json, sys

ap=argparse.ArgumentParser()
ap.add_argument("baseline",type=Path)
ap.add_argument("candidate",type=Path)
ap.add_argument("--max-slowdown",type=float,default=0.25,
                help="allowed median slowdown as a fraction (default 0.25)")
a=ap.parse_args()

def load(p):
    rows=json.loads(p.read_text())
    return {r["label"]:r for r in rows if r["label"].startswith("tscc ")}

base=load(a.baseline); cand=load(a.candidate)
failed=False
for label,b in base.items():
    if label not in cand:
        print(f"missing candidate benchmark: {label}")
        failed=True; continue
    old=b["median_ms"]; new=cand[label]["median_ms"]
    ratio=(new/old)-1 if old else 0
    state="PASS" if ratio <= a.max_slowdown else "FAIL"
    print(f"{state:4} {label:24} {old:8.3f} -> {new:8.3f} ms ({ratio*100:+.1f}%)")
    failed |= state=="FAIL"
sys.exit(1 if failed else 0)
