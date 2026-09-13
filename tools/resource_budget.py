#!/usr/bin/env python3
import argparse, resource, subprocess, time
p=argparse.ArgumentParser()
p.add_argument('--seconds',type=float,required=True);p.add_argument('--rss-kib',type=int,required=True)
p.add_argument('--record',required=True);p.add_argument('command',nargs=argparse.REMAINDER)
a=p.parse_args(); command=a.command[1:] if a.command and a.command[0]=='--' else a.command
started=time.monotonic();result=subprocess.run(command);elapsed=time.monotonic()-started
rss=resource.getrusage(resource.RUSAGE_CHILDREN).ru_maxrss
open(a.record,'w').write(f'{elapsed:.6f} {rss}\n')
if result.returncode: raise SystemExit(result.returncode)
if elapsed>=a.seconds: raise SystemExit(f'elapsed budget exceeded: {elapsed:.3f}s >= {a.seconds}s')
if rss>=a.rss_kib: raise SystemExit(f'RSS budget exceeded: {rss} KiB >= {a.rss_kib} KiB')
