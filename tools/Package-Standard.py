"""Create a self-contained Standard source ZIP. No build/tests/validators/apps."""
from pathlib import Path
import argparse
import datetime
import hashlib
import json
import subprocess
import zipfile
from standard_package_policy import LIMIT, inventory
ROOT=Path(__file__).resolve().parent.parent
SOURCE=ROOT/'standard'

def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--output',default='dist/standard-coding-test-2-20261110.zip')
    parser.add_argument('--include-learning',action='store_true',help='Development copy including debugger and learning docs; default is submission')
    args=parser.parse_args();output=(ROOT/args.output).resolve();output.relative_to(ROOT)
    if output.is_relative_to(SOURCE.resolve()): raise SystemExit('Output must be outside Standard.')
    if output.exists(): raise SystemExit('Output already exists. Choose another output; prior artifacts are preserved.')
    included,excluded=inventory(SOURCE,args.include_learning)
    for entry in included: entry['sha256']=hashlib.sha256((SOURCE/entry['path']).read_bytes()).hexdigest()
    raw=sum(p['bytes'] for p in included)
    if raw>LIMIT: raise SystemExit(f'Source exceeds 30,000,000 bytes: {raw}. No files deleted.')
    output.parent.mkdir(parents=True,exist_ok=True)
    with zipfile.ZipFile(output,'x',compression=zipfile.ZIP_DEFLATED,compresslevel=9) as archive:
        for e in included: archive.write(SOURCE/e['path'],'standard/'+e['path'])
    size=output.stat().st_size
    commit=subprocess.check_output(['git','rev-parse','HEAD'],cwd=ROOT,text=True).strip()
    result={'date':datetime.datetime.now(datetime.timezone(datetime.timedelta(hours=9))).isoformat(),
      'source_commit':commit,'source':str(SOURCE),'archive':str(output),'profile':'development' if args.include_learning else 'submission',
      'raw_bytes':raw,'zip_bytes':size,'limit_bytes':LIMIT,'included_files':len(included),
      'archive_sha256':hashlib.sha256(output.read_bytes()).hexdigest(),'included':included,'excluded':excluded,
      'notes':['Input is the saved working tree at package time; source_commit identifies HEAD, not an assertion of a clean tree.',
        'Submission excludes debugger, project x64/.vs, docs/tutorials/HTML/examples.',
        'Bundled SDK x64 libraries, headers, runtime DLLs and licenses remain.',
        'OS, Windows SDK, MSVC tools and compatible graphics drivers are environment prerequisites.',
        'No files were deleted. No build, tests, app run, archive testzip or validators were run.']}
    output.with_suffix('.manifest.json').write_text(json.dumps(result,ensure_ascii=False,indent=2)+'\n',encoding='utf-8-sig')
    report=['# Standard 제출용 소스 패키지','',f'프로필: {result["profile"]} · 패키징 시 HEAD: `{commit}`. 실제 입력은 저장된 작업 트리다.','',
      f'- 포함 파일: {len(included)}',f'- 압축 전: **{raw:,}바이트** ({raw/1e6:.2f} MB)',f'- ZIP: **{size:,}바이트** ({size/1e6:.2f} MB)',
      f'- 기준: {LIMIT:,}바이트 이하',f'- 제외: {sum(e["bytes"] for e in excluded):,}바이트 / {len(excluded)}개',
      '- 실제 삭제: 0바이트','', '제출 프로필은 debugger·프로젝트 x64/.vs·문서·튜토리얼·HTML·예제를 제외한다. SDK의 x64 입력과 라이선스는 남긴다. 옆 manifest에 모든 포함/제외 경로와 크기·포함 파일 해시가 있다. 기존 .exe는 제출하지 않는 C++ 소스 프로젝트다.','',
      '빌드·테스트·검증기·앱 실행·압축 해제 후 단독 동작 확인은 사용자가 수행한다. 용량 충족은 컴파일/실행 성공을 뜻하지 않는다.']
    output.with_suffix('.SIZE.md').write_text('\n'.join(report)+'\n',encoding='utf-8-sig')
    book=ROOT/'coding-test-2/reports';book.mkdir(parents=True,exist_ok=True)
    if not args.include_learning:
        (book/'SUBMISSION_SIZE.md').write_text('\n'.join(report)+'\n\n[제출 ZIP](../../'+output.relative_to(ROOT).as_posix()+')\n',encoding='utf-8-sig')
    print(json.dumps({k:result[k] for k in ('archive','profile','included_files','raw_bytes','zip_bytes','limit_bytes')},ensure_ascii=False))
    if size>LIMIT: raise SystemExit('ZIP exceeds limit; artifact retained for review.')
if __name__=='__main__': main()