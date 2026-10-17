# Write full before/after source pairs; no tests or builds.
from pathlib import Path
import collections
import hashlib
import io
import json
import re
import subprocess
import zipfile

ROOT=Path(__file__).resolve().parent.parent
BASE='6ec317bc71beab82835bdbde6e227d2ceed1cd87'
OUT=ROOT/'docs/practice-14-19'

def git(*args):
    return subprocess.check_output(['git','-c','core.safecrlf=false',*args],cwd=ROOT,stderr=subprocess.DEVNULL)

def write(path,s):
    path.parent.mkdir(parents=True,exist_ok=True)
    path.write_text(s,encoding='utf-8-sig',newline='\n')

def decode(data):
    if data is None or b'\0' in data:return None
    try:return data.decode('utf-8-sig').replace('\r\n','\n')
    except UnicodeDecodeError:
        try:return data.decode('cp949').replace('\r\n','\n')
        except UnicodeDecodeError:return None

def digest(data):return hashlib.sha256(data).hexdigest() if data is not None else None

def main():
    OUT.mkdir(parents=True,exist_ok=True)
    baseline=zipfile.ZipFile(io.BytesIO(git('archive','--format=zip',BASE)))
    old_names=set(baseline.namelist())
    tracked=git('diff','--name-only','-z',BASE).decode('utf-8').split('\0')
    new=git('ls-files','--others','--exclude-standard','-z').decode('utf-8').split('\0')
    names=sorted(set(tracked+new)-{''})
    groups=collections.defaultdict(list);contents={};manifest=[]
    suffixes={'.h','.hpp','.inl','.cpp','.glsl','.py','.ps1','.props','.example','.vcxproj','.filters','.slnx','.config','.md','.patch'}
    for name in names:
        if Path(name).name in {'BEFORE_AFTER.md','CHANGE_MANIFEST.json','COMMITS.md'} and name.startswith('docs/practice-14-19/'):continue
        old=baseline.read(name) if name in old_names else None
        current=ROOT/name;after=current.read_bytes() if current.is_file() else None
        if old==after:continue
        before_text,after_text=decode(old),decode(after)
        is_source=Path(name).suffix.lower() in suffixes or Path(name).name in {'.gitignore','.gitattributes'}
        historical='/archive/' in name
        generated=(name.startswith('graphify-out/') or name.startswith('framework-analysis/graphify-out/')) and Path(name).suffix!='.py'
        pair=None
        if is_source and not historical and not generated and (before_text is not None or after_text is not None):
            key=(digest(before_text.encode()) if before_text is not None else None,digest(after_text.encode()) if after_text is not None else None)
            groups[key].append(name);contents[key]=(before_text,after_text)
            pair='pair-'+hashlib.sha256(str(key).encode()).hexdigest()[:12]
        manifest.append({'path':name,'change':'added' if old is None else 'deleted' if after is None else 'modified',
            'before_sha256':digest(old),'after_sha256':digest(after),
            'before_bytes':len(old) if old is not None else None,'after_bytes':len(after) if after is not None else None,
            'full_source_pair':pair,'historical_archive':historical,'generated_graph_artifact':generated})
    intro=f"""# 전체 코드 변경 전후

기준 커밋: `{BASE}`. before는 작업 시작 상태, after는 최종 저장 상태다. 같은 before/after를 가진 배포 파일은 적용 경로를 한 코드 쌍에 연결했다. 코드 본문은 생략하지 않는다. 새 코드는 before 없음, 삭제 코드는 after 없음이다.

변경 파일 {len(manifest)}개, 서로 다른 전체 소스 쌍 {len(groups)}개다. 정확한 바이트 해시는 CHANGE_MANIFEST.json에 있다. 단순 보관 이동 문서·생성 그래프·바이너리는 원본과 manifest로 연결하며 작성 코드와 구분한다. 변경 이력은 COMMITS.md, 목적·동작 차이는 README.md를 따른다.

SDK 배포 변경도 포함하므로 파일이 크다. 일반 소스와 실습 쌍을 먼저 보고 SDK는 필요할 때 검색한다.

"""
    parts=[intro]
    ordered=sorted(groups,key=lambda k:(all('/dependencies/' in p or p.startswith('dependencies/') for p in groups[k]),groups[k][0]))
    for key in ordered:
        pair='pair-'+hashlib.sha256(str(key).encode()).hexdigest()[:12]
        paths=groups[key];before,after=contents[key]
        parts.append(f'<a id="{pair}"></a>\n\n## {paths[0]}\n\n')
        if any('/dependencies/' in p or p.startswith('dependencies/') for p in paths):reason='SDK 위치·중복·배포를 프레임워크 내부로 통합한다. 유지된 원본 내용과 라이선스를 함께 배포한다.'
        elif all(p.endswith('/resource.h') for p in paths):reason='주석과 정의는 보존하고 CP949를 UTF-8 BOM으로 변환한다. 바이트 차이는 manifest에 기록한다.'
        elif any('/Console.' in p or '/main.cpp' in p for p in paths):reason='실습 입력·상태·변환·콜백을 승인된 동작으로 연결한다. 구체적인 동작 차이는 README.md의 해당 실습 항목을 따른다.'
        elif any('/detail/' in p or '/include/' in p for p in paths):reason='회전 표현·계층·공통 도형·색상·경로 처리를 통일하며 호출부도 함께 이식한다.'
        else:reason='설정·문서·배포·분석을 현재 구현과 일치시키고 이전 잔재를 제거한다.'
        parts.append(reason+'\n\n적용 경로:\n\n'+'\n'.join('- `'+p+'`' for p in paths)+'\n\n')
        for label,text in [('before',before),('after',after)]:
            parts.append('### '+label+'\n\n')
            if text is None:parts.append('없음\n\n')
            else:
                longest=max((len(x) for x in re.findall(r'`+',text)),default=0);fence='`'*max(3,longest+1)
                parts.append(fence+'text\n'+text+('' if text.endswith('\n') else '\n')+fence+'\n\n')
    write(OUT/'BEFORE_AFTER.md',''.join(parts))
    write(OUT/'CHANGE_MANIFEST.json',json.dumps({'baseline':BASE,'files':manifest},ensure_ascii=False,indent=2)+'\n')
    log=git('log','--reverse','--format=%h %s',BASE+'..HEAD').decode('utf-8').splitlines()
    write(OUT/'COMMITS.md','# 단계별 커밋\n\n'+'\n'.join('- `'+line+'`' for line in log)+'\n\n8단계는 이 문서와 그래프·최종 소스 검토를 포함한 마지막 커밋이다. 자기 해시를 문서에 기록하면 해시가 다시 바뀌므로 최종 응답과 git log -8 --oneline에서 확인한다.\n')
    print(f'Wrote {len(manifest)} file records and {len(groups)} full source pairs; report bytes={(OUT/"BEFORE_AFTER.md").stat().st_size}.')

if __name__=='__main__':main()
