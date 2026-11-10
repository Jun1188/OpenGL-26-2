"""Generate an offline learning book from authored Markdown/JSON and real source. No builds/tests/apps."""
from pathlib import Path
import datetime as dt
import html
import json
ROOT=Path(__file__).resolve().parents[1]
BOOK=ROOT/'coding-test-2'
def read(p): return p.read_text(encoding='utf-8-sig')
def write(p,s):
    p.parent.mkdir(parents=True,exist_ok=True)
    p.write_text(s.rstrip()+'\n',encoding='utf-8-sig')
def load(p): return json.loads(read(p))
def problem_md(x):
    lines=[f'# {x["id"]} · {x["title"]}', '',f'경로: {x["track"]} · 권장 {x["minutes"]}분(실제 시험 시간 아님)', '', '## 문제', '',x['problem'], '', '## 상태·공간 설계', '',x['design'], '', '## 단계별 힌트', '']
    lines += [f'{i}. {h}' for i,h in enumerate(x['hints'],1)]
    lines += ['', '## 해설과 수정 코드', '',x['solution'], '', '## 사용자가 확인할 결과', '']
    lines += ['- [ ] '+o for o in x['observe']]
    lines += ['', '## 코드·학습 참조', '']+['- `'+r+'`' for r in x['refs']]
    return '\n'.join(lines)
def roadmap():
    work=[
      ('N01 + N07','정점 입력 직접 작성 / Standard 모델·재질 등록','한 장 사각형을 설명 없이 다시 만든다.'),
      ('N02 + N08','비균등 배율·피벗 / 위치만 상속','피벗의 최종 좌표를 행렬로 예측한다.'),
      ('N03 + N09','역전치 법선 / 부착 광원과 최종 프레임','법선 오류와 동기화 오류를 나눠 찾는다.'),
      ('N04 + N10','공전·반경 / held 입력과 복귀','정지·역방향·재개에서 위상을 보존한다.'),
      ('N05 + N11','카메라 입력 / 광원·재질 항 비교','원인 하나씩 바꾸고 화면 변화를 설명한다.'),
      ('N06 + N12','CPU uniform·GLSL / debugger 직접 편집','선언·전송·사용·파일 복사까지 연결한다.'),
      ('순정 빈 코드 재작성','native/main.cpp를 작업 사본에 보존한 뒤 골격부터 재작성','창→삼각형→MVP→조명→키 상태를 75분 안에 연결한다.'),
      ('Standard 새 컨트롤러','기본 파일을 읽고 새 상태 하나를 독립 구현','프레임워크 enum을 늘리지 않고 입력→상태→setLayer를 만든다.'),
      ('S01','월드↔부착 프레임 전환','전환 순간의 월드 위치 보존을 확인한다.'),
      ('S02','모서리 경첩·지연·부착 조명','네 판이 다른 시점에 움직여도 경첩은 고정된다.'),
      ('S03','경로 분기와 이산 상태 확정','선택 상태와 이미 확정한 경로를 분리한다.'),
      ('S04','이동 카메라·방향 제약','위치 상속과 자세 정책을 독립 구현한다.'),
      ('S05','반복 패턴과 법선','좌표 나열 없이 공식과 반복문으로 만든다.'),
      ('S06 + 제출 정리','55분 통합 연습 후 원인 3개만 복기','debugger·문서 없는 제출 사본과 필요한 SDK를 확인한다.')]
    lines=['# 11월 24일 화요일까지 14회 준비', '', '**시험: 2026-11-24(화). 학습: 11/10~11/23.** 실제 시험 문제와 시간은 미확인이다. 아래 시간은 연습을 위한 권장치다. 이미 지난 회차는 순서를 유지해 묶되 해설부터 읽고 완료 처리하지 않는다.', '',
      '매 회차는 요구를 읽고 상태·프레임을 적기 → 해설을 닫고 작성 → 사용자가 빌드·관찰 → 해설과 차이 기록 순서다. 대개 60~90분, N제 한 문항은 30~45분을 배정한다. 마지막 날은 새 범위를 늘리기보다 누락을 줄인다.', '',
      '| 날짜 | 과제 | 중점 | 종료 기준 |','|---|---|---|---|']
    for i,(task,topic,done) in enumerate(work):
        d=dt.date(2026,11,10)+dt.timedelta(days=i)
        lines.append(f'| {d.isoformat()} | {task} | {topic} | {done} |')
    lines += ['', '## 스스로 판정하기', '', '- 읽음: 코드를 보고 입력·상태·공간을 설명했다.', '- 작성: 해설을 닫고 요구 동작의 코드를 썼다.', '- 사용자 실행 확인: 직접 빌드하고 문항의 관찰 기준을 확인했다.', '',
      '타이머 종료 시 미완성이면 요구별로 완료/미완료를 적고, 좌표계·갱신 순서·uniform 중 첫 실패 지점을 고친다. 문항을 외워 맞히는 것보다 새 조건에서 같은 연산을 선택할 수 있는지 확인한다.', '',
      '11/24에는 새 설계를 추가하지 말고 프로젝트 열기, 경로, 키 입력 규칙, GLSL 복사 위치를 확인한다. 시험장의 도구·자료 허용 범위는 제공되지 않았으므로 별도 확인한다. 이 자료는 시험 중 사용 허가를 전제로 하지 않는다.']
    return '\n'.join(lines)
def walkthrough():
    path=ROOT/'standard/SceneSetup.cpp'; lines=read(path).splitlines()
    def entry(command,needle,after):
        n=next(i for i,s in enumerate(lines,1) if needle in s)
        return f'### `{command}`\n\n소스 스냅샷: `standard/SceneSetup.cpp:{n}`\n\n```cpp\n// before\n{lines[n-1]}\n// after\n{after}\n```\n'
    out=['# debugger 시작부터 직접 편집까지', '', '아래 행은 문서 생성 시 저장된 실제 소스에서 계산했다. debugger를 실행해 얻은 출력 기록은 아니다. 파일을 편집한 뒤에는 debugger의 현재 행 번호를 우선한다. debugger는 저장된 파일을 읽으며 IDE 미저장 버퍼를 읽지 않는다.', '',
      '## 1. 실행', '', '사용자가 standard/debugger/debugger.sln에서 x64로 빌드한 뒤 자기 폴더 경로로 실행한다.', '',
      '```powershell\n$studyStandard = "C:/Work/OpenGL-26-2/standard"\n& "$studyStandard/debugger/x64/Debug/debugger.exe" "$studyStandard"\n```', '',
      '## 2. 물체 추가', '', '`add object examPlate box`를 입력한다. debugger가 현재 삽입 위치와 **전체 ObjectSetup 선언·등록 after**를 제안한다. 그 코드를 IDE에 복사해 저장한다. 저장 전에는 show object examPlate가 새 구획을 찾을 수 없다.', '',
      f'현재 삽입 표식은 `standard/SceneSetup.cpp:{next(i for i,s in enumerate(lines,1) if "// @debug insert" in s)}`다. 추가한 물체의 size/position/scale을 그 구획에서 직접 정한다. 기본 단위 박스의 왼쪽 피벗은 x=-.5다.']
    out += ['', '## 3. 현재 panel로 정확한 before/after 연습', '']
    out.append(entry('object panel scale 2 1 0.5','cfg_panel.scale =','    cfg_panel.scale = {2.f, 1.f, 0.5f};'))
    out.append(entry('layer hinge pivot -1 0 0','cfg_hinge.pivot =','    cfg_hinge.pivot = {-1.f, 0.f, 0.f};'))
    out.append(entry('layer hinge rotation 0 45 0','cfg_hinge.rotationDegrees =','    cfg_hinge.rotationDegrees = {0.f, 45.f, 0.f};'))
    out.append(entry('light key position 3 4 1','cfg_key.position =','    cfg_key.position = {3.f, 4.f, 1.f};'))
    out += ['', '각 명령을 입력하고 절대 경로·실제 행·before를 IDE와 대조한다. after를 직접 저장한다. 이어 where input/update/uniform/shader로 상태 변화와 GLSL 위치를 찾는다. 재빌드·실행하여 비균등 배율, 피벗 고정, m 조명 토글, y/Y 광원 공전, z/Z 반경, c 색, F2 항별 출력을 확인한다. 런타임 상태는 ExamController가 소유하므로 새 입력은 그 컨트롤러를 수정한다.', '',
      '## 4. 행 번호가 바뀐 뒤', '', '구획 앞에 빈 줄을 넣어 저장한 뒤 같은 명령을 다시 입력한다. before와 행 번호가 현재 디스크를 따라야 한다. 안내만 읽었다고 코드가 저장되거나 실행 중 장면이 변경되는 것은 아니다. 임의 코드 수정은 code/replace/insert로 제안받되 의미 검토와 저장은 직접 한다.']
    return '\n'.join(out)
def main():
    exercises=load(BOOK/'exercises/problems.json'); scenarios=load(BOOK/'scenarios/scenarios.json')
    for folder,data in [('exercises',exercises),('scenarios',scenarios)]:
        lines=[f'# {"N제 12문항" if folder=="exercises" else "통합 실전 6시나리오"}', '', '문제·설계·힌트·해설·코드·관찰 기준을 제공한다. 각 문항은 지정한 원본의 작업 복사본에서 독립적으로 적용한다. 관찰 기준은 사용자 실행으로 확인한다.', '']
        for x in data:
            write(BOOK/folder/(x['id']+'.md'),problem_md(x))
            lines.append(f'- [{x["id"]} {x["title"]}]({x["id"]}.md) · {x["track"]} · 권장 {x["minutes"]}분')
        lines += ['', '원문은 같은 폴더의 JSON이다. 문항 ID는 유지한다. 생성기는 원문을 덮어쓰지 않는다. 실제 시험 출제 예측이나 시간 보장은 아니다.']
        write(BOOK/folder/'README.md','\n'.join(lines))
    write(BOOK/'ROADMAP.md',roadmap()); write(BOOK/'implementation/DEBUGGER_WALKTHROUGH.md',walkthrough())
    docs=[]
    paths=['README.md','ROADMAP.md','implementation/WORKFLOW.md','implementation/DEBUGGER_WALKTHROUGH.md']
    paths += [p.relative_to(BOOK).as_posix() for p in sorted((BOOK/'learning').glob('*.md'))]
    paths += ['requirements/COVERAGE.md','learning-graph.md','sources.md','USER_CHECKS.md']
    for p in paths:
        content=read(BOOK/p); title=content.splitlines()[0].removeprefix('# ')
        docs.append(dict(id=p,title=title,markdown=content))
    sources=[]
    source_paths=['coding-test-2/native/main.cpp','coding-test-2/native/shaders/VertexShader.glsl','coding-test-2/native/shaders/FragmentShader.glsl',
      'standard/main.cpp','standard/Console.h','standard/SceneSetup.h','standard/SceneSetup.cpp','standard/ExamController.h','standard/ExamController.cpp','standard/Console.cpp',
      'standard/framework/include/SceneAuthoring.h','standard/framework/detail/SceneAuthoring.cpp',
      'standard/framework/shaders/VertexShader.glsl','standard/framework/shaders/FragmentShader.glsl','standard/debugger/Commands.cpp']
    summary=['# 실제 소스 참조', '', '현재 저장된 소스의 전체 내용을 HTML 소스 탭에 포함했다. 번호는 생성 시점의 1-based 행이며 저장 후 바뀔 수 있다. 원문 편집은 해당 실제 파일에서 한다.', '', '| 파일 | 행 수 | 위치 표식 |', '|---|---:|---|']
    for p in source_paths:
        text=read(ROOT/p); rows=text.splitlines(); hooks=[f'{i}: {s.strip()}' for i,s in enumerate(rows,1) if '@debug hook' in s]
        sources.append(dict(path=p,text=text,hooks=hooks))
        summary.append(f'| [{p}](../{p}) | {len(rows)} | '+html.escape('; '.join(hooks))+' |')
    write(BOOK/'SOURCE_GUIDE.md','\n'.join(summary))
    docs.append(dict(id='SOURCE_GUIDE.md',title='실제 소스 파일·행',markdown=read(BOOK/'SOURCE_GUIDE.md')))
    payload=json.dumps(dict(exam='2026-11-24',docs=docs,exercises=exercises,scenarios=scenarios,sources=sources),ensure_ascii=False).replace('<','\u003c')
    page='''<!doctype html><html lang="ko"><head><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1"><title>코딩테스트 2 · 11월 24일</title><link rel="stylesheet" href="assets/study.css"></head><body>
<header><a href="#README.md">CODE / TRANSFORM / LIGHT</a><strong>11월 24일 화요일 · 코딩테스트 2</strong><span id="deadline"></span></header>
<div class="layout"><aside><label for="search">자료·문항 찾기</label><input id="search" type="search" placeholder="피벗, 조명, N02…"><nav id="nav" aria-label="학습 목차"></nav><p class="muted">읽음 / 작성 / 사용자 실행 확인은 별도 기록입니다.</p></aside>
<main id="main" tabindex="-1"><noscript>JavaScript를 켜거나 같은 폴더의 Markdown을 읽으세요.</noscript></main></div>
<footer>오프라인 학습 자료 · 빌드·테스트·화면 확인은 사용자 수행 · 순정 OpenGL과 Standard 두 경로</footer>
<script id="book-data" type="application/json">'''+payload+'''</script><script src="assets/marked.umd.js"></script><script src="assets/study.js"></script></body></html>'''
    write(BOOK/'index.html',page)
    print(f'Generated offline book: {len(docs)} chapters, {len(exercises)} exercises, {len(scenarios)} scenarios, {len(sources)} full-source snapshots. No browser/build/tests run.')
if __name__=='__main__': main()