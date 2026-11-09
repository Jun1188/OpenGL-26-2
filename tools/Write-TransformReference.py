"""Generate operation-based templates and references, independent of practice assignments."""
from pathlib import Path
import html
import json
import re

ROOT=Path(__file__).resolve().parents[1]
STANDARD=ROOT/'standard'
TEMPLATES=[
 ('01-local-order','지역 변환의 순서','이동한 축·회전한 축에서 다음 연산을 적용할 때',[
  ('기본 물체 생성','add box item at 0 0 0'),('회전 레이어 생성','layer add turn'),('지역 Y축으로 30도 회전','layer r y 30'),
  ('앞 레이어의 회전된 축을 상속하는 이동 레이어 생성','layer add offset'),('회전된 지역 X축으로 2 이동','layer t 2 0 0'),
  ('비균등 형상 레이어 생성','layer add shape'),('지역 세 축 배율을 따로 적용','layer s 2 1 0.5')]),
 ('02-pivot','피벗을 고정한 회전','중심·끝점·관절·임의 기준점 주위로 돌 때',[
  ('길이 4의 막대 생성','add box rod 0 0 0 4 0.2 0.2'),('막대 왼쪽 끝을 피벗으로 지정하되 배치는 유지','pivot rod -2 0 0 keep'),
  ('지정한 피벗 주위로 지역 Z축 자전','insert spin z 45')]),
 ('03-inherit-world','월드 좌표로 부착하고 상속','서로 다른 물체가 공통 이동을 따라갈 때',[
  ('부모 프레임을 월드 X=10에 생성','add joint base at 10 0 0'),('자식을 월드 X=12에 생성','add box item at 12 0 0'),
  ('현재 월드 배치를 유지해 부모 연결','parent base'),('자식 내부 회전 추가','insert spin y 45'),
  ('부모 선택','select base'),('부모를 월드 X=15로 이동. 자식 기준 배치는 X=17','t 15 0 0')]),
 ('04-orbit-spin','공전과 자전의 독립 조합','위치 변화와 자체 회전을 각각 조절할 때',[
  ('중심에서 떨어진 물체 생성','add box item at 3 0 0'),('월드 원점을 지역 중심으로 변환해 공전 레이어 생성','insert orbit y 30 center 0 0 0'),
  ('공전 레이어를 상속하는 자체 회전 레이어 생성','insert spin x 90')]),
 ('05-fixed-end-scale','끝점을 고정한 비균등 배율','한쪽 끝은 그대로 두고 길이·폭만 바꿀 때',[
  ('길이 4 막대 생성','add box rod 0 0 0 4 0.2 0.2'),('왼쪽 끝을 피벗으로 지정','pivot rod -2 0 0 keep'),
  ('월드 X축 길이만 두 배. 왼쪽 X=-2는 유지','s 2 1 1')]),
 ('06-periodic','시간 함수로 반복 회전','단순 주기 운동을 물리 적분 없이 작성할 때',[
  ('기본 물체 생성','add box item at 0 0 0'),('Z축 진폭25도·주기3초의 사인 회전','insert pendulum z 25 3')]),
 ('07-input-rate','입력량과 시간당 변화량','한 번 누름과 길게 누름을 구분할 때',[
  ('제어할 물체 생성','add box item at 0 0 0'),('누른 동안 초당 X+1 이동','bind RIGHT hold "move item 1 0 0"'),
  ('한 번 누를 때 Y축5도 회전','bind Q press "rotate item y 5"')]),
 ('08-light-camera','같은 변환을 조명·카메라에 사용','시각 객체와 관찰·광원 배치를 함께 조립할 때',[
  ('관찰할 물체 생성','add box item at 0 0 0'),('점광원 생성','light point lamp at 3 4 2'),
  ('광원 강도 설정','light intensity 2'),('카메라 생성','camera add observer at 6 4 8'),
  ('월드 원점을 바라보게 설정','camera look 0 0 0'),('선택 카메라로 전환','camera use')]),
 ('09-arbitrary-axis','임의 축 회전','좌표축이 아닌 방향 벡터를 기준으로 돌 때',[
  ('기본 물체 생성','add box item at 2 0 0'),('지역 (1,1,0) 방향 축의 공전','insert orbit-axis 1 1 0 30 center 0 0 0')]),
]

def write(path,text):
    path=ROOT/path;path.parent.mkdir(parents=True,exist_ok=True)
    text=text.rstrip()+'\n'
    if not path.exists() or path.read_text(encoding='utf-8-sig')!=text:path.write_text(text,encoding='utf-8-sig')

def main():
    md='''# 변환 조합 템플릿

기존 실습이 시험에 재등장한다는 가정은 사용하지 않는다. **요구한 움직임을 변환·좌표계·계층·시간 함수·힘으로 나누고 필요한 블록을 조합한다.** 실습 소스는 그 연산을 구현한 코드 위치를 찾을 때만 참고한다.

아래는 완성된 문제 풀이가 아닌 최소 동작 블록이다. 각 파일은 clear로 시작하므로 별도 장면에서 읽는다. 합칠 때는 clear·카메라 초기화를 제외하고 객체 이름을 구분한다. 입력은 실제 구현된 Standard 명령이며 실행 확인은 사용자가 수행한다.

| 필요한 연산 | 템플릿 | 바꿀 인수 |
|---|---|---|
'''
    body='<p><a href="../index.html">전체 문서</a> · <a href="../document.html?doc=docs/TRANSFORM_REFERENCE.md">변환 조합 레퍼런스</a> · <a href="../document.html?doc=docs/QUICK_COMMANDS.md">명령어</a></p>'
    body+='<p>실습 재출제를 가정하지 않는다. 요구한 움직임을 연산·좌표계·상속·시간·힘으로 분해한다. 파일마다 장면 초기화가 있으므로 조합할 때 clear와 카메라 초기화를 제외한다.</p><nav>'
    for slug,title,condition,rows in TEMPLATES:body+='<a href="#'+slug+'">'+html.escape(title)+'</a> · '
    body+='</nav>'
    for slug,title,condition,rows in TEMPLATES:
        recipe='# @title: '+title+'\n# @goal: '+condition+'\n#: 독립 템플릿의 시작 장면\nclear\n#: 관찰 카메라\ncamera pose 8 6 10 0 0 0 0 1 0\n'
        for note,command in rows:recipe+='\n#: '+note+'\n'+command+'\n'
        relative='examples/assembly/templates/'+slug+'.assembly';write('standard/'+relative,recipe)
        md+='| '+condition+' | ['+title+'](../'+relative+') | '+('축·중심·각속도' if any('spin' in c or 'orbit' in c for _,c in rows) else '좌표·배율·기간·대상')+' |\n'
        body+='<section id="'+slug+'"><h2>'+html.escape(title)+'</h2><p>'+html.escape(condition)+'</p><p><code>load '+relative+'</code></p><table><tr><th>명령어 [결과]</th><th>인수·관찰점</th></tr>'
        for note,command in rows:body+='<tr><td><code>'+html.escape(command)+'</code></td><td>'+html.escape(note)+'</td></tr>'
        body+='</table><details><summary>복사할 원문</summary><pre>'+html.escape(recipe)+'</pre></details></section>'
    md+='\n[HTML 템플릿 목차](TRANSFORM_TEMPLATES.html) · [연산 선택과 C++ 레퍼런스](TRANSFORM_REFERENCE.md) · [전체 명령](QUICK_COMMANDS.md)\n\n저울은 중력·기준 프레임·회전·힘 콜백을 조합하는 [응용 예제](APPLICATION_RECIPES.md) 하나다. 템플릿을 저울 형상에 종속시키지 않는다.\n'
    write('standard/docs/TRANSFORM_TEMPLATES.md',md)
    body+='<h2>힘을 포함하는 응용 한 가지: 회전 스프링 저울</h2><p><code>load examples/assembly/scenarios/s13-moving-scale.assembly</code></p><p>앞의 변환 블록에 중력·이동/회전 프레임·스프링 콜백을 추가한 예다. 영자연길이 선형 스프링 모델이며 시험 문제의 형상·제약 조건은 별도로 작성한다.</p>'
    body+='<p><a href="../document.html?doc=docs/APPLICATION_RECIPES.md">힘의 단위·가정·저울 예제 설명</a></p>'
    html_page='<!doctype html><html lang="ko"><head><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1"><title>변환 조합 템플릿</title><style>body{font:16px/1.8 "Malgun Gothic",sans-serif;max-width:1100px;margin:32px auto;padding:0 24px}pre{background:#142d42;color:#fff;padding:18px;overflow:auto}td,th{border:1px solid #ccd6de;padding:10px}table{border-collapse:collapse;width:100%}section{margin:40px 0}a{color:#15649c}</style></head><body><h1>변환 조합 템플릿</h1>'+body+'</body></html>'
    write('standard/docs/TRANSFORM_TEMPLATES.html',html_page)
    write('standard/docs/EXAM_SCENARIOS.html','<!doctype html><html lang="ko"><meta charset="utf-8"><title>변환 템플릿으로 이동</title><h1>문서 기준 변경</h1><p>실습별 시나리오 안내를 변환 연산별 템플릿으로 바꿨습니다.</p><a href="TRANSFORM_TEMPLATES.html">변환 조합 템플릿 열기</a></html>')
    reference='''# 변환 조합 레퍼런스

SRT와 중심 회전의 뜻을 알고 있다는 전제로, **요구 문장 → 저장할 값 → 적용할 공간·순서 → 실제 API**를 연결한다. 특정 실습·형상을 정답으로 고정하지 않는다.

| 요구 문장 | 저장할 값 | 연산·공간 | Standard / C++ |
|---|---|---|---|
| 이 점에 물체를 둔다 | 월드 기준 배치 D | inverse(부모W)×D를 부착에 저장 | t / Scene::setWorldPlacement |
| 부모 이동을 함께 따른다 | 부모 ID와 부착 행렬 | 부모 최종W×자식 부착 | parent / setParentPreserveWorld |
| 물체 내부 동작만 바꾼다 | 그 객체의 지역 레이어 목록 | 앞 레이어 결과를 뒤에서 상속 | layer / setLocalLayers |
| 회전한 축으로 이동한다 | 각도, 지역 이동량 | R×T | 회전 레이어 뒤 이동 레이어 |
| 이동 후 월드 축으로 돌린다 | 월드 피벗 C와 축 | T(C)×R×T(-C)를 왼쪽에 곱함 | r / TransformSystem::rotation |
| 한쪽 끝을 유지하며 늘린다 | 모델의 고정점, 축별 배율 | 피벗 유지 보정 후 비균등 S | pivot keep + s |
| 점 주위로 공전한다 | 축 직선, 시간별 각도 | T(C)×R(θ(t))×T(-C) | insert orbit / RotationMotion |
| 임의 축으로 회전한다 | 0 아닌 방향 벡터, 축 위 점 | 축 정규화 후 중심 회전 | insert spin-axis/orbit-axis / AxisLine |
| 자전과 공전을 따로 제어한다 | 서로 다른 레이어·각속도 | 공전 프레임 아래 자전 | insert orbit → insert spin |
| 주기적으로 왕복한다 | 진폭, 주기, 위상 | θ(t)=offset+A sin(2πt/P+phase) | insert pendulum / PendulumMotion |
| 시간에 따라 위치를 바꾼다 | from/to, 기간, 보간 함수 | p(u), u(t), 재생 방식 | insert linear / PathMotion |
| 일정 시간 뒤 동작을 추가한다 | 시작 시각, 지속 시간 | 활성 구간에서 행렬 평가 | MotionStep::startSeconds/durationSeconds |
| 누른 시간만큼 바꾼다 | 속도와 키 누름 상태 | 변화량=속도×dt | bind hold / update 콜백 |
| 조명·시점을 이동시킨다 | 광원 위치·카메라 시선 | 같은 객체 상속 후 렌더 파라미터로 전달 | light/camera / PointLight·Camera |
| 중력·원심력 등이 작용한다 | 질량, 기준 프레임 운동, 힘 | 가속도 계산 → 적분 → 위치 변환 | force / DynamicsSystem |

## 행렬을 직접 끼우는 공통 API

다음 조각은 이미 생성한 `scene`과 `id`를 사용한다. 행렬 값은 실제 문제에서 계산해 넣는다. 클래스·문서·명령 파서를 복사할 필요가 없다.

```cpp
auto T = graphics::TransformSystem::translation({2,0,0});
auto R = graphics::TransformSystem::rotation({{0,0,0},{0,1,0}}, 30);
auto S = graphics::TransformSystem::scaling({2,1,0.5f});
if (T && R && S) {
    // Each matrix is a dependent local layer. Geometry receives S, R, T in that order.
    bool ok = scene.setLocalLayers(id, {*T, *R, *S});
}
```

부모와의 관계는 객체의 부착 행렬, 내부 조합은 자기 레이어에 저장한다. 월드 기준 배치를 설정할 때 자기 레이어를 다시 초기화하지 않는다. 비균등 배율과 회전의 합성으로 생긴 전단은 행렬 상태로 유지한다.

## 시간·조건을 문제에 맞게 작성

```cpp
graphics::Motion motion;
graphics::LinearMotion move;
move.to = {3,0,0}; move.durationSeconds = 2;
move.space = graphics::TransformSpace::Local;
bool moving = motion.append(graphics::Motion::linear(move));
graphics::RotationMotion turn;
turn.degreesPerSecond = 45;
turn.space = graphics::TransformSpace::Local;
auto rotate = graphics::Motion::rotation(turn);
rotate.startSeconds = 2;
bool turning = motion.append(rotate);
auto sample = moving && turning ? motion.sample(time) : std::nullopt;
if (sample) {
    bool ok = scene.setLocalLayers(id, {sample->parent, sample->local});
}
```

위 이동은 Once 재생이므로 끝 위치를 유지하고, 회전은 2초부터 시작한다. `durationSeconds`로 단계 자체를 끝내면 그 단계의 기여도 사라진다. `then`은 앞 단계들의 종료 시각이 모두 유한할 때만 사용할 수 있다. 조건에 따라 기준점·각속도·레이어를 바꾸려면 업데이트 콜백에서 조건을 판단하고 해당 값만 갱신한다. sample은 행렬 평가이고 의미 action 실행은 별도 계약이다.

## 여러 물체의 기준점·질량·힘

형상 크기와 부모 배치만으로 물리 질량·관성·제약이 자동 결정되지는 않는다. 기준 프레임 운동, 힘 법칙, 적분 상태, 시각적 형상을 별도 값으로 둔다. [사용자 힘 연결](APPLICATION_RECIPES.md)에서 콜백을 바꾸면 스프링·외력·감쇠 등을 조합할 수 있다. 저울·회전 구조물·매달린 추 등 문제 형상은 이 값들을 사용하는 응용 코드다.

## 빠른 소스 위치

- `modes/AssemblyWorkspace.cpp`: 선택 대상 해석, 피벗, 축별 배율, 객체 내부 레이어, 순서 변경, 템플릿 삽입.
- `framework/include/Transform.h`, `detail/Transform.cpp`: 이동·배율·임의 축 회전·반사 행렬.
- `framework/include/Motion.h`, `detail/Motion.cpp`: 시간 함수와 단계 삽입·활성 구간.
- `framework/detail/Scene.cpp`: 부착·상속·레이어·렌더/피킹의 공통 월드 합성.
- `framework/include/Dynamics.h`, `detail/Dynamics.cpp`: 기준 프레임 가속도·힘·적분.
- `modes/AssemblySceneTools.cpp`: 같은 변환을 광원·카메라에 연결.

[연산별 실행 템플릿](TRANSFORM_TEMPLATES.md) · [전체 명령과 인수](QUICK_COMMANDS.md) · [실습 소스 참조](REFERENCE_PRACTICES.md)
'''
    write('standard/docs/TRANSFORM_REFERENCE.md',reference)
    readme=(STANDARD/'README.md').read_text(encoding='utf-8-sig')
    note='[변환 조합 템플릿](docs/TRANSFORM_TEMPLATES.html) · [변환 조합 레퍼런스](document.html?doc=docs/TRANSFORM_REFERENCE.md)\n\n실습 재출제를 가정하지 않는다. 요구된 변환과 공간·순서·시간·힘을 조합하는 작업공간이다.\n\n'
    write('standard/README.md',readme.replace('# Standard: 0번 장면 작업공간\n\n','# Standard: 0번 장면 작업공간\n\n'+note))
    catalog=json.loads((STANDARD/'docs/DOCUMENT_CATALOG.json').read_text(encoding='utf-8-sig'))
    primary={'docs/TRANSFORM_TEMPLATES.html','docs/TRANSFORM_TEMPLATES.md','docs/TRANSFORM_REFERENCE.md'}
    for category in catalog['categories']:
        category['documents']=[d for d in category['documents'] if d['path'] not in primary]
        if category['id']=='start':
            for d in category['documents']:
                if d['path']=='docs/APPLICATION_RECIPES.md':d.update(title='광원·카메라·힘 API 연결',featured=False)
            category['documents'][1:1]=[{'path':'docs/TRANSFORM_TEMPLATES.html','title':'연산별 변환 템플릿','featured':True},
                {'path':'docs/TRANSFORM_REFERENCE.md','title':'요구 문장 → 변환·공간·API','featured':True},
                {'path':'docs/TRANSFORM_TEMPLATES.md','title':'템플릿 파일 목록'}]
        if category['id']=='exam':
            category['title']='변환과 응용';category['description']='재출제 시나리오 대신 연산·좌표계·시간·힘을 조합한다.'
            for d in category['documents']:
                if d['path']=='docs/EXAM_SCENARIOS.html':d.update(title='이전 시나리오 문서의 이동 안내',featured=False,summary='변환 조합 템플릿으로 통합')
        if category['id']=='cookbook':
            category.update(title='과거 실습 코드 참조',description='변환 구현을 찾는 참고 코드. 시험 유형이나 재출제 예상 목록이 아니다.')
    for category in catalog['categories']:
        if category['id']=='practice-reference':
            category['documents']=[{'path':'docs/REFERENCE_PRACTICES.md'}]+[{'path':p.relative_to(STANDARD).as_posix()} for p in sorted((STANDARD/'docs/reference/modes').glob('*.md'))]
    write('standard/docs/DOCUMENT_CATALOG.json',json.dumps(catalog,ensure_ascii=False,indent=2))

if __name__=='__main__':main()
