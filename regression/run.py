#!/usr/bin/env python3
import os
from pathlib import Path
import argparse, json, subprocess, tempfile, concurrent.futures, re, sys

ap=argparse.ArgumentParser()
ap.add_argument('--tscc',required=True)
ap.add_argument('--workers',type=int,default=min(24,os.cpu_count() or 8))
a=ap.parse_args()
root=Path(__file__).resolve().parent
cases=json.loads((root/'cases.json').read_text())

def diag_files(text, file_map):
    found=set()
    # tsc commonly prints absolute inputs without the leading slash. The
    # generated basenames are unique, so use them as stable diagnostic keys.
    basename_map={Path(path).name:name for path,name in file_map.items()}
    for line in text.splitlines():
        head=line.split('(',1)[0].split(':',1)[0]
        base=Path(head).name
        if base in basename_map:
            found.add(basename_map[base])
    return found


with tempfile.TemporaryDirectory(prefix='tscc-reg-ref-') as td:
    td=Path(td)
    refs=td/'ref'; refs.mkdir()
    file_map={}
    by_kind={'runtime':[],'emit':[],'syntax-negative':[],'semantic-only':[]}
    for idx,c in enumerate(cases):
        p=refs/f'{idx:04d}-{c["name"]}{c.get("ext",".ts")}'
        p.write_text(c['src'])
        file_map[str(p)]=c['name']
        by_kind[c['kind']].append((c,p))

    reference_status={}
    # Runtime and syntax-negative cases are parser/emit-reference questions.
    ref_cases=by_kind['runtime']+by_kind['emit']+by_kind['syntax-negative']+by_kind['semantic-only']
    ref_files=[str(p) for _,p in ref_cases]
    ref=subprocess.run(['tsc','--noCheck','--pretty','false','--target','es2022','--jsx','preserve','--moduleDetection','force','--noEmit',*ref_files],capture_output=True,text=True)
    no_check_diags=diag_files(ref.stdout+ref.stderr,file_map)
    for c,_ in by_kind['runtime']:
        reference_status[c['name']]=('bad-runtime-ref' if c['name'] in no_check_diags else 'ok')
    for c,_ in by_kind['emit']:
        reference_status[c['name']]=('bad-runtime-ref' if c['name'] in no_check_diags else 'ok')
    for c,_ in by_kind['syntax-negative']:
        reference_status[c['name']]=('ok' if c['name'] in no_check_diags else 'bad-negative-ref')
    for c,_ in by_kind['semantic-only']:
        reference_status[c['name']]=('bad-semantic-nocheck' if c['name'] in no_check_diags else 'pending-semantic')

    # One normal tsc invocation determines which noCheck-accepted cases are
    # genuinely semantic-checker restrictions. moduleDetection keeps cases
    # isolated from one another's globals without changing their syntax.
    sem_files=[str(p) for _,p in by_kind['semantic-only']]
    if sem_files:
        full=subprocess.run(['tsc','--pretty','false','--target','es2022','--moduleDetection','force','--skipLibCheck','--noEmit',*sem_files],capture_output=True,text=True)
        full_diags=diag_files(full.stdout+full.stderr,file_map)
        for c,_ in by_kind['semantic-only']:
            if reference_status[c['name']]=='pending-semantic':
                reference_status[c['name']]=('ok-skip' if c['name'] in full_diags else 'bad-semantic-full')

    def one(c):
        rs=reference_status[c['name']]
        if c['kind']=='semantic-only':
            if rs=='ok-skip': return ('skip',c['name'],'')
            return ('fail',c['name'],f'bad semantic-only classification ({rs})')
        if c['kind'] in ('runtime','emit') and rs!='ok':
            return ('fail',c['name'],'reference tsc --noCheck rejected valid case')
        if c['kind']=='syntax-negative' and rs!='ok':
            return ('fail',c['name'],'reference tsc --noCheck accepted syntax-negative case')

        with tempfile.TemporaryDirectory(prefix='tscc-reg-case-') as cd:
            cd=Path(cd); src=cd/('case'+c.get('ext','.ts')); src.write_text(c['src']); out=cd/'out'; out.mkdir()
            got=subprocess.run([a.tscc,'--pretty','false','--noResolve',*c.get('args',[]),'--outDir',str(out),str(src)],capture_output=True,text=True)
            if c['kind']=='syntax-negative':
                if got.returncode!=0:return ('pass',c['name'],'')
                return ('fail',c['name'],'tscc accepted syntax rejected by tsc\n'+got.stdout+got.stderr)
            if got.returncode!=0:return ('fail',c['name'],'tscc compile failed\n'+got.stdout+got.stderr)
            emitted=out/('case.jsx' if c.get('ext')=='.tsx' else 'case.js')
            if not emitted.exists():return ('fail',c['name'],'no output')
            if c['kind']=='emit':
                text=emitted.read_text()
                required=c.get('contains',[])
                absent=c.get('not_contains',[])
                if all(x in text for x in required) and all(x not in text for x in absent):
                    return ('pass',c['name'],'')
                return ('fail',c['name'],f'emit mismatch\nOUTPUT:\n{text}')
            env=dict(os.environ); env['NO_COLOR']='1'; env['FORCE_COLOR']='0'
            run=subprocess.run(['node',str(emitted)],capture_output=True,text=True,env=env)
            actual=run.stdout.strip()
            if run.returncode==0 and actual==c['expect']:return ('pass',c['name'],'')
            return ('fail',c['name'],f'expect={c["expect"]!r} actual={actual!r}\n{run.stderr}\nJS:\n{emitted.read_text()}')

    with concurrent.futures.ThreadPoolExecutor(max_workers=a.workers) as ex:
        res=list(ex.map(one,cases))
    counts={k:sum(1 for x in res if x[0]==k) for k in ['pass','fail','skip']}
    print(json.dumps(counts))
    for status,name,detail in res:
        if status=='fail': print(f'FAIL {name}:\n{detail[:1800]}')
    sys.exit(1 if counts['fail'] else 0)
