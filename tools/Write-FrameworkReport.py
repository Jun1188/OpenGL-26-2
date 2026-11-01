"""Write complete source before/after pairs from the actual working-tree snapshot."""
from pathlib import Path
import argparse
import collections
import hashlib
import json
import os
import re
import subprocess
import zipfile

ROOT=Path(__file__).resolve().parent.parent
OUT=ROOT/'docs/practice-20-23'
ROOTS=['framework','standard','tools','framework-analysis','graphify-out','docs/practice-20-23'] + [f'openGL_3dAssign_{n}' for n in range(15,24)]
EXCLUDED={'.vs','x64','Debug','Release','cache','__pycache__'}
SUFFIXES={'.h','.hpp','.inl','.cpp','.glsl','.props','.vcxproj','.filters','.sln','.slnx','.py','.ps1','.md','.html','.json'}
OUTPUTS={'BEFORE_AFTER.md','CHANGE_MANIFEST.json','COMMITS.md','PACKAGES.json'}

def write(path,value):
    path.parent.mkdir(parents=True,exist_ok=True)
    temporary=path.with_name(path.name+'.report.tmp')
    temporary.write_text(value,encoding='utf-8-sig',newline='\n');os.replace(temporary,path)

def digest(value):
    return hashlib.sha256(value).hexdigest() if value is not None else None

def source_text(data):
    if data is None:return None
    try:return data.decode('utf-8-sig').replace('\r\n','\n')
    except UnicodeDecodeError:return None

def generated(name):
    return (name.startswith('graphify-out/') or name.startswith('framework-analysis/graphify-out/')) and not name.endswith(('.py','FRAMEWORK_FLOW.md','TOP_DOWN.md'))

def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--baseline',type=Path,default=ROOT/'.obj-work/framework-20261101-before.zip')
    args=parser.parse_args()
    with zipfile.ZipFile(args.baseline) as baseline:
        before={name:baseline.read(name) for name in baseline.namelist()}
    current={}
    for directory in ROOTS:
        for p in (ROOT/directory).rglob('*'):
            if not p.is_file() or any(part in EXCLUDED for part in p.relative_to(ROOT).parts):continue
            if p.suffix.lower() not in SUFFIXES or p.name in OUTPUTS:continue
            if 'dependencies' in p.parts:continue
            current[p.relative_to(ROOT).as_posix()]=p.read_bytes()
    groups=collections.defaultdict(list); contents={}; records=[]
    for name in sorted((set(before)|set(current))):
        if Path(name).name in OUTPUTS:continue
        old=before.get(name);new=current.get(name)
        if old==new:continue
        old_text,new_text=source_text(old),source_text(new)
        pair=None
        if not generated(name) and (old_text is not None or new_text is not None):
            key=(old_text,new_text)
            pair='pair-'+hashlib.sha256((str(digest(old))+'|'+str(digest(new))).encode()).hexdigest()[:14]
            groups[key].append(name);contents[key]=pair
        records.append({'path':name,'change':'added' if old is None else 'deleted' if new is None else 'modified',
                        'before_sha256':digest(old),'after_sha256':digest(new),
                        'before_bytes':None if old is None else len(old),'after_bytes':None if new is None else len(new),
                        'full_source_pair':pair,'generated_graph_artifact':generated(name)})
    parts=['# 전체 변경 전후\n\n',
           'before는 코드 작성 직전 실제 작업 폴더의 스냅샷이다. 사용자가 먼저 삭제한 실습20 폴더는 before 없음이다. Git HEAD의 실습19 탱크 코드를 before로 대신하지 않았다. after는 최종 저장 소스다. 같은 코드 쌍의 배포 사본은 적용 경로를 묶었다. 전체 파일을 표시하므로 모든 변경 블록과 그 문맥을 포함한다.\n\n',
           '빌드·실행·테스트·검증기는 사용자 담당이다. SDK 원본 사본과 바이너리·자동 생성 그래프 데이터는 새로 작성한 C++ 코드와 구분하며 manifest/패키지 목록을 따른다. 기존 미추적 문서와 이론 자료 삭제 상태는 이번 커밋에 섞지 않았다.\n\n']
    for key,names in sorted(groups.items(),key=lambda item:item[1][0]):
        old,new=key;pair=contents[key]
        reason='기존 공개 반환 계약을 유지하고 단계/API 원인을 출력한다.' if any('Diagnostics' in n for n in names) else (
            '공통 계산·변환·렌더링을 API로 제공하고 각 독립 프로젝트에 같은 구현을 배포한다.' if any('/framework/' in n or n.startswith('framework/') for n in names) else (
            '실습 PDF의 입력·상태·출력을 공통 프레임워크 호출로 연결한다.' if any('openGL_3dAssign_' in n for n in names) else
            '샘플·배포 설정·문서·분석을 현재 소스와 일치시킨다.'))
        parts.extend([f'<a id="{pair}"></a>\n\n## {names[0]}\n\n',reason+'\n\n',
                      '적용 경로:\n\n'+'\n'.join('- `'+name+'`' for name in names)+'\n\n'])
        for label,value in [('before',old),('after',new)]:
            parts.append('### '+label+'\n\n')
            if value is None:parts.append('없음\n\n');continue
            ticks=max([len(x) for x in re.findall(r'`+',value)]+[2])+1
            fence='`'*max(3,ticks)
            parts.append(fence+'text\n'+value+('' if value.endswith('\n') else '\n')+fence+'\n\n')
    write(OUT/'BEFORE_AFTER.md',''.join(parts))
    write(OUT/'CHANGE_MANIFEST.json',json.dumps({'baseline_worktree_snapshot':'.obj-work/framework-20261101-before.zip',
        'baseline_commit':'e5f0620619072456415d531a89aa1a9891f1a2c9','source_pairs':len(groups),'files':records},ensure_ascii=False,indent=2)+'\n')
    log=subprocess.check_output(['git','log','--reverse','--format=%h %s','e5f0620619072456415d531a89aa1a9891f1a2c9..HEAD'],cwd=ROOT,text=True,encoding='utf-8')
    write(OUT/'COMMITS.md','# 단계별 커밋\n\n'+'\n'.join('- `'+line+'`' for line in log.splitlines())+'\n\n이 문서와 구조 그래프·배포 결과를 담는 마지막 커밋의 해시는 최종 응답 및 git log에서 확인한다. 원격 push는 이번 작업에 포함하지 않았다.\n')
    print(f'Wrote {len(groups)} complete source pairs for {len(records)} changed files.')

if __name__=='__main__':main()
