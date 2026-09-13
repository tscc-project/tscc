#!/usr/bin/env python3
import random, subprocess, sys, tempfile, time
from pathlib import Path

tscc=sys.argv[1];random.seed(0x54534343)
seeds=["const x:number=42;","function f(x:number):number{return x+1;}f(2);","interface X{value:number}const x:X={value:1};","const xs:number[]=[1,2];xs[0];","enum E{A,B=3}const e:E=E.A;","type X<T>=T extends string?T:never;","class C{constructor(public x:number){}}"]
alphabet="{}[]();,+-*/=!<>?:.'\"abcdefghijklmnopqrstuvwxyz0123456789 "
with tempfile.TemporaryDirectory(prefix="tscc-preview-fuzz-") as directory:
    source=Path(directory)/"case.ts"
    for index in range(400):
        text=list(random.choice(seeds))
        for _ in range(1+random.randrange(5)):
            action=random.randrange(3);position=random.randrange(len(text)+1)
            if action==0:text.insert(position,random.choice(alphabet))
            elif action==1 and text:text.pop(min(position,len(text)-1))
            elif text:text[min(position,len(text)-1)]=random.choice(alphabet)
        source.write_text("".join(text))
        try:result=subprocess.run([tscc,"--pretty","false","--noEmit","--noResolve",str(source)],stdout=subprocess.DEVNULL,stderr=subprocess.DEVNULL,timeout=1)
        except subprocess.TimeoutExpired:raise SystemExit(f"mutation {index} timed out")
        if result.returncode not in (0,1,2):raise SystemExit(f"mutation {index} exited {result.returncode}: {''.join(text)!r}")
    start=time.monotonic()
    for _ in range(100):
        result=subprocess.run([tscc,"--pretty","false","--noEmit","--noResolve",str(source)],stdout=subprocess.DEVNULL,stderr=subprocess.DEVNULL)
        if result.returncode not in (0,1,2):raise SystemExit("startup guard candidate failed")
    elapsed=time.monotonic()-start
    if elapsed>=10:raise SystemExit(f"startup guard exceeded 10s: {elapsed:.3f}s")
print(f"tscc deterministic fuzz passed: 400 mutations; startup guard: {elapsed:.3f}s/100 runs")
