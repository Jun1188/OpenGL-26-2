"""공개 C++ 선언에 한글 기능 주석을 붙이고 API 목록을 만든다. 빌드/테스트 없음."""
from pathlib import Path
import importlib.util
import json
import os
import re
from tree_sitter import Language, Parser
import tree_sitter_cpp
ROOT=Path(__file__).resolve().parent.parent
spec=importlib.util.spec_from_file_location('guide',ROOT/'tools/Write-CodeGuide.py')
guide=importlib.util.module_from_spec(spec);spec.loader.exec_module(guide)
PARSER=Parser(Language(tree_sitter_cpp.language()))
DESCRIPTIONS={
'addPolygon':'점 목록으로 단일 다각형 면을 만들고 색·부모 관계와 함께 Scene에 등록한다. 오목 다각형의 자동 분할을 가정하지 않는다.',
'lineWorldMatrix':'선의 owner 엔티티를 따라 로컬 끝점을 월드로 옮길 행렬을 반환한다.',
'setInitHandler':'장면 초기화 콜백을 등록한다. run의 창·렌더러 초기화 후 한 번 호출하며 false면 실행을 종료한다.',
'setUpdateHandler':'매 프레임 dt(초)를 받는 갱신 콜백을 등록한다. 렌더 패스마다 중복 갱신하지 않는다.',
'setKeyHandler':'키와 action을 받는 콜백을 등록한다. 수정키 콜백이 있으면 그 콜백을 우선한다.',
'setModifiedKeyHandler':'키·action·mods를 받는 콜백을 등록하여 Shift 등 수정키를 처리한다.',
'setFocusHandler':'창 포커스 변경 콜백을 등록한다. 지속 입력 상태 해제 등에 사용한다.',
'setRenderHandler':'사용자 렌더 콜백을 등록한다. 등록하면 기본 renderScene 대신 호출한다.',
'setMouseHandler':'마우스 버튼과 창/프레임버퍼 좌표를 받는 콜백을 등록한다.',
'setCursorHandler':'창 좌표의 커서 이동 콜백을 등록한다. 드래그에는 이전 좌표와의 차이를 사용한다.',
'pickAtCursor':'현재 커서 위치를 읽어 기본 카메라의 장면 피킹 결과를 반환한다.',
'getAssets':'실행 중 사용하는 AssetLoader 참조를 반환한다. run 콜백 밖에서는 예외를 발생시킨다.',
'requestClose':'창 닫기 플래그를 설정하여 이벤트 루프의 정상 종료를 요청한다.',
'renderScene':'기본 카메라와 현재 프레임버퍼 크기로 Scene을 한 번 렌더링한다.',
'getRenderOptions':'애플리케이션의 렌더 옵션을 수정할 수 있는 참조를 반환한다.',
'getScene':'애플리케이션이 소유한 CPU Scene의 수정 가능한 참조를 반환한다.',
'getCamera':'기본 카메라의 수정 가능한 참조를 반환한다.',
'getWindow':'GLFW 창의 비소유 포인터를 반환한다. 창 수명 밖에서는 사용할 수 없다.',
'context':'진단에 첨부할 현재 파일·줄·호출 경로 문맥의 참조를 반환한다.',
'locate':'다음 진단에 사용할 소스 파일·줄 번호·호출 경로를 설정한다.',
'failAt':'주어진 소스 위치와 API 오류를 기록·출력하고 이전 진단 문맥을 복원한 뒤 false를 반환한다.',
'stageName':'파이프라인 단계 열거값에 대응하는 로그용 문자열을 반환한다.',
'configure':'오류 출력과 단계 추적 출력의 사용 여부를 설정한다.',
'lastError':'마지막으로 기록한 오류의 optional 참조를 반환한다.',
'trace':'추적 옵션이 켜져 있으면 해당 단계·API의 시작을 콘솔에 출력한다.',
'fail':'현재 진단 문맥에서 원인·조치·객체 ID를 저장하고 출력한 뒤 false를 반환한다.',
'addGroup':'메시 없는 변환 전용 엔티티를 생성하여 부모·관절 노드로 사용한다.',
'addSphere':'반지름과 분할 수로 구 모델을 생성하고 지정 위치에 등록한다.',
'addCone':'반지름·높이·분할 수로 원뿔을 생성하고 지정 위치에 등록한다.',
'setTint':'엔티티 전체에 곱할 색상 계수를 설정한다. 정점 자체의 색 배열과 구분한다.',
'push_entity':'주어진 다면체와 변환을 Scene에 등록하고 새 EntityId를 반환한다.',
'addCuboid':'중심 기준 직육면체를 변 길이와 위치로 생성하여 Scene에 등록한다.',
'addRightTetrahedron':'세 축의 모서리 길이로 직각 사면체를 생성하여 Scene에 등록한다.',
'addPyramid':'가로·높이·깊이에 맞는 피라미드를 생성하여 Scene에 등록한다.',
'setFaceVisible':'해당 엔티티의 한 면을 표시하거나 숨긴다. 컬링 조건과 별개의 가시성 상태다.',
'setFaceCullMode':'해당 면의 앞면/뒷면 제거 정책을 설정한다. 조명 법선과 삼각형 winding을 구분한다.',
'setFaceLocalMatrix':'해당 면에 적용할 추가 로컬 애니메이션 행렬을 설정한다.',
'setFaceVertexColors':'면의 정점별 색 배열을 교체한다. 정점 수와 색 배열 길이를 맞춰야 한다.',
'getMotions':'Scene이 소유한 MotionSystem의 참조를 반환한다. const 오버로드는 읽기 전용이다.',
'bindEntity':'엔티티를 기존 모션 레이어에 연결하여 월드 행렬 합성에 반영한다.',
'unbindEntity':'엔티티와 모션 레이어의 연결을 해제한다.',
'getEntityLayer':'엔티티에 연결된 레이어 ID를 반환한다. 연결이 없으면 빈 optional이다.',
'updateMotions':'Scene의 모션 시스템에 dt(초)를 전달한다. Application 루프에서는 자동 호출된다.',
'setFaceTransform':'면 변환과 독립 변환 여부를 함께 설정한다. 부모 모델 변환과의 합성 정책을 바꾼다.',
'addModel':'CPU 모델을 Scene에 등록한다. 피벗 모드와 사용자 기준점으로 localBasis를 결정한다.',
'setModel':'기존 엔티티의 모델을 교체한다. 형상 revision을 갱신한다.',
'setMesh':'기존 모델의 지정 메시를 교체한다. 정점·인덱스 등 메시 유효성을 검사한다.',
'getMesh':'지정 메시의 읽기 전용 비소유 포인터를 반환한다. 잘못된 ID/인덱스면 nullptr이다.',
'setMeshVertices':'메시의 정점과 삼각형 인덱스를 함께 교체한다.',
'setVertexPosition':'메시의 지정 정점 위치를 수정하고 형상 변경을 알린다.',
'setTriangleIndices':'메시의 삼각형 인덱스 배열을 교체한다. 범위와 세 개 단위 구성을 검사한다.',
'generateTexcoords':'평면·박스·원통·구 투영 옵션으로 메시 UV를 생성한다.',
'transformTexcoords':'메시 UV에 배율→회전→이동을 적용한다. 회전 입력 단위는 도다.',
'setTexcoords':'메시의 정점별 UV 배열을 교체한다.',
'setVertexNormals':'메시의 정점별 조명 법선 배열을 교체한다. 컬링 winding은 바뀌지 않는다.',
'setMaterial':'메시의 색상·텍스처 등 재질 데이터를 교체한다.',
'setOutline':'윤곽선 생성 방식과 선택적 명시 인덱스를 설정한다.',
'setVisible':'엔티티의 표시 여부를 바꾼다. 엔티티나 충돌 데이터를 삭제하지 않는다.',
'splitFace':'지정 면을 독립 변환 가능한 면으로 분리한다.',
'setFaceSRT':'면의 크기·회전·이동 변환을 설정한다.',
'addLine':'두 끝점의 선을 추가한다. owner가 있으면 해당 엔티티 변환을 따른다.',
'addVector':'시작점과 방향/길이 벡터를 선의 두 끝점으로 변환하여 추가한다.',
'addBasis':'기저 행렬의 열 벡터로 X/Y/Z 선 세 개를 추가하고 LineId 묶음을 반환한다.',
'setVector':'기존 선을 시작점과 방향/길이 벡터로 갱신한다.',
'getIds':'현재 Scene의 엔티티 ID 배열을 읽기 전용 참조로 반환한다.',
'getComponent':'엔티티 컴포넌트의 비소유 포인터를 반환한다. 유효하지 않은 ID면 nullptr이다.',
'getCubeFaceIds':'직육면체 면 ID 목록을 읽기 전용 참조로 반환한다.',
'getTetraFaceIds':'사면체 면 ID 목록을 읽기 전용 참조로 반환한다.',
'getPyramidFaceIds':'피라미드 면 ID 목록을 읽기 전용 참조로 반환한다.',
'getLines':'Scene의 선 데이터 배열을 읽기 전용 참조로 반환한다.',
'getGeometryRevision':'렌더러가 GPU 재업로드 필요성을 비교하는 형상 변경 번호를 반환한다.',
'makePolyhedron':'형상 종류와 변 길이로 CPU 다면체를 만든다. Scene 등록은 별도로 수행한다.',
'makeSphere':'반지름·분할 수로 CPU 구 메시 모델을 생성한다.',
'makeCone':'반지름·높이·분할 수로 CPU 원뿔 모델을 생성한다.',
'validPolyhedron':'다면체의 면·정점·인덱스 등 렌더 입력 조건을 검사한다.',
'validTransform':'변환의 유한성·회전 기저·배율 등 허용 조건을 검사한다.',
'modelMatrix':'T(basis)·R·S·T(-localBasis) 순서로 로컬 모델 행렬을 계산한다.',
'faceModelMatrix':'컴포넌트와 면의 변환·독립성 정책에 따라 면 모델 행렬을 계산한다.',
'validCamera':'시점·주목점·up·투영/클립 범위가 유효한 카메라인지 검사한다.',
'viewMatrix':'카메라 시점·주목점·up으로 월드→뷰 행렬을 계산한다.',
'projectionMatrix':'카메라의 원근/직각 모드와 주어진 aspect로 투영 행렬을 계산한다.',
'readTextFile':'지정 경로의 파일을 문자열로 읽는다. 읽기에 실패하면 예외를 전달한다.',
'render':'Scene을 카메라와 전체 화면 크기로 렌더링한다. 유효한 현재 GL 컨텍스트가 필요하다.',
'valid':'해당 데이터가 계산에 필요한 유효 범위와 유한성 조건을 만족하는지 반환한다.',
'fromVertices':'정점 목록에서 하나의 면 데이터를 만든다. 모델/Scene 등록은 수행하지 않는다.',
'meshes':'모델이 보관한 면/메시 배열의 참조를 반환한다. const 오버로드는 읽기 전용이다.',
'features':'로더의 텍스처 등 기능 사용 옵션을 읽기 전용 참조로 반환한다.',
'loadTexture':'프로젝트 상대경로의 이미지를 읽어 TextureResult로 반환한다. 실패 원인은 error에 담는다.',
'loadObj':'프로젝트 상대경로의 OBJ를 직접 파싱하여 모델·피벗·경고·오류 결과를 반환한다.',
'loadPrimitive':'기본 도형 OBJ를 읽는 호환 진입점이다. OBJ 로더로 처리를 위임한다.',
'loadModel':'모델 로딩 호환 진입점이다. 현재 구현은 loadObj로 위임하며 다른 형식 변환은 하지 않는다.',
'append':'모션 단계 배열의 끝에 유효한 단계를 추가한다. 자동 순차 배치는 then을 사용한다.',
'insert':'주어진 인덱스에 모션 단계를 삽입한다.',
'replace':'주어진 인덱스의 모션 단계를 새 단계로 교체한다.',
'erase':'주어진 인덱스의 모션 단계를 삭제한다.',
'getSteps':'등록된 모션 단계 배열을 읽기 전용 참조로 반환한다.',
'fixed':'고정 행렬을 반환하는 모션 단계를 만든다. 합성 공간을 함께 지정한다.',
'followPath':'경로·보간·재생 설정으로 시간에 따른 위치 이동 모션 단계를 만든다.',
'rootLayer':'삭제할 수 없는 루트 레이어의 ID 0을 반환한다.',
'createMotion':'모션을 시스템에 등록하고 새 MotionId를 반환한다.',
'setMotion':'기존 MotionId의 모션 데이터를 교체하고 현재 시각의 샘플을 갱신한다.',
'getMotion':'모션 상태의 읽기 전용 비소유 포인터를 반환한다. 없는 ID면 nullptr이다.',
'setMotionPlaying':'모션의 시간 진행을 정지하거나 재개한다. 모션 기여 자체의 사용 여부와 구분한다.',
'setMotionEnabled':'모션 샘플의 기여 여부를 설정한다. playing과 별도 상태다.',
'removeMotion':'모션을 제거하고 연결된 레이어의 해당 모션 연결을 해제한다.',
'setLayer':'기존 레이어의 고정 변환·합성 공간·모션 연결·활성 상태를 바꾼다.',
'getLayer':'레이어 상태의 읽기 전용 비소유 포인터를 반환한다. 없는 ID면 nullptr이다.',
'createSpin':'로컬 공간 회전 모션을 등록하여 자전에 사용한다.',
'createOrbit':'월드 공간 회전 모션을 등록하여 축 주위 공전에 사용한다.',
'createOrbitAndSpin':'월드 공전과 로컬 자전을 하나의 모션에 등록한다.',
'createPathAndSpin':'경로 이동과 로컬 자전을 하나의 모션에 등록한다.',
'contains':'점이 margin만큼 안쪽으로 줄인 Bounds3 안에 있는지 검사한다.',
'overlaps':'두 축 정렬 경계 상자가 겹치는지 검사한다.',
'setPoints':'연속 점들의 누적 거리 경로를 구성한다. closed면 마지막에서 처음으로 연결한다.',
'length':'경로 전체 누적 길이를 반환한다. 경로가 비어 있으면 0이다.',
'snakeCells':'셀 격자를 지그재그 순서로 방문하는 정수 인덱스 목록을 만든다.',
'aligned':'두 값의 차이가 허용 오차 이하인지 검사한다.',
'screenRay':'왼쪽 위 기준 프레임버퍼 픽셀을 카메라 ray로 역투영한다. 영역 밖/잘못된 투영이면 빈 결과다.',
'pick':'ray 또는 화면 좌표로 보이는 삼각형의 최근접 교차를 찾아 객체·면·위치·UV를 반환한다.',
'recalculateNormals':'메시의 삼각형 기하로 정점 법선을 다시 계산한다.',
'meshOutline':'메시의 윤곽 모드에 따라 선으로 그릴 정점 인덱스 쌍 목록을 계산한다.',
'validAffine':'행렬 원소가 유한하고 마지막 행이 아핀 변환 형식인지 검사한다.',
'translation':'이동 벡터로 아핀 이동 행렬을 만든다. 유효하지 않으면 빈 optional이다.',
'scaling':'중심점 주위의 축별 배율 행렬 T(C)·S·T(-C)를 만든다.',
'reflectPoint':'주어진 점을 중심으로 세 축을 반전하는 점 반사 행렬을 만든다.',
'reflectLine':'주어진 직선을 기준으로 선 반사 행렬을 만든다.',
'reflectPlane':'평면의 점과 법선으로 평면 반사 행렬을 만든다.',
'ellipse':'중심과 두 반축 벡터로 매개변수 타원 경로 함수를 만든다.',
'circle':'중심·반지름·평면 법선으로 원 경로 함수를 만든다.',
'polyline':'점 목록을 따라가는 꺾은선 경로 함수를 만든다. closed로 순환 여부를 정한다.',
'polygon':'마지막 점과 첫 점이 연결되는 닫힌 꺾은선 경로 함수를 만든다.',
'rectangle':'중심과 두 반변 벡터로 직사각형 테두리 경로 함수를 만든다.',
'linear':'입력 진행률을 선형 보간 진행률로 반환한다.',
'smoothStep':'진행률에 3t²-2t³ 완화 곡선을 적용한다.',
}
SPECIAL={
('GridLayout','valid'):'origin·extent·셀 개수와 frame의 유한성·양수 크기·가역성을 검사한다.',
('GridLayout','position'):'[0,1] 비율을 origin+extent*ratio로 바꾼 뒤 frame으로 월드 위치를 계산한다.',
('GridLayout','cellCenter'):'0부터 시작하는 셀 인덱스를 해당 셀 중심의 월드 위치로 변환한다.',
('GridLayout','relativeSize'):'양수 비율과 extent를 곱해 그리드 로컬 크기를 반환한다. frame 회전/배율은 적용하지 않는다.',
('GridLayout','positionInParent'):'비율 위치를 월드로 계산한 뒤 parentWorld의 역행렬로 부모 로컬 위치를 반환한다.',
('GridLayout','cellAt'):'월드 점을 frame 역변환으로 그리드 셀 인덱스에 대응시킨다. 그리드 밖이면 빈 결과다.',
('RoomBody','position'):'이탈 전에는 roomWorld로 로컬 중심을 월드로 변환하고 이탈 후에는 worldPosition을 반환한다.',
('TextureImage','valid'):'이미지 너비·높이와 RGBA 바이트 수가 맞는지 검사한다.',
('Diagnostics','clear'):'마지막 오류 기록을 지운다. 진단 설정과 현재 위치 문맥은 유지한다.',
('Scene','clear'):'Scene의 모델·선·연결·모션을 초기화한다. 엔티티 ID는 재사용되므로 저장한 ID를 갱신한다.',
('MotionSystem','clear'):'모션과 레이어를 비우고 루트 레이어를 복구한다. 다음 ID 카운터는 되돌리지 않는다.',
('Motion','sample'):'절대 시간(초)에서 활성 단계의 월드/로컬 행렬 기여를 합성하여 MotionSample을 반환한다.',
('DistancePath','sample'):'누적 거리와 Once/Loop/PingPong 정책에서 위치·접선·yaw를 샘플링한다.',
('Motion','rotation'):'축·초당 각속도·초기 각도·합성 공간으로 회전 모션 단계를 만든다.',
('TransformSystem','rotation'):'직선 위 점·축 방향과 도 단위 각도로 축 주위 회전 행렬을 만든다.',
('AnimationClock','reset'):'누적 시간을 0으로 되돌리고 재생 상태를 켠다.',
('Application','renderViews'):'현재 창의 한 컨텍스트에서 여러 카메라 패스를 렌더링한다. 시간은 진행시키지 않는다.',
('Renderer','initialize'):'현재 OpenGL 3.3 컨텍스트에서 셰이더·프로그램·버퍼를 생성한다. 빈 경로면 내장 GLSL을 사용한다.',
}
def save(p,text):
    p.parent.mkdir(parents=True,exist_ok=True)
    if p.exists() and p.read_text(encoding='utf-8-sig')==text:return
    temp=p.with_name(p.name+'.api.tmp');temp.write_text(text,encoding='utf-8-sig',newline='\n');os.replace(temp,p)
def declarations(path):
    b=path.read_bytes();out=[]
    def text(n):return b[n.start_byte:n.end_byte].decode('utf-8') if n else ''
    def find(n):
        if n.type in ('function_declarator','operator_cast'):return n
        for c in n.named_children:
            if c.type not in ('parameter_list','compound_statement','template_argument_list'):
                d=find(c)
                if d:return d
    def walk(n,scope='',public=True):
        if n.type=='namespace_definition' and text(n.child_by_field_name('name'))=='detail':return
        if n.type in ('class_specifier','struct_specifier'):
            name=text(n.child_by_field_name('name'));body=n.child_by_field_name('body');vis=n.type=='struct_specifier'
            if body:
                for c in body.named_children:
                    if c.type=='access_specifier':vis=text(c)=='public'
                    else:walk(c,name,vis)
            return
        if n.type in ('declaration','field_declaration','function_definition'):
            d=find(n)
            if d and public:
                name='operator bool' if d.type=='operator_cast' else text(d.child_by_field_name('declarator'))
                owner=scope
                if '::' in name:owner,name=name.rsplit('::',1)
                body=n.child_by_field_name('body');end=body.start_byte if body else n.end_byte
                sig=b[n.start_byte:end].decode('utf-8').strip().rstrip(';').strip()
                out.append({'owner':owner,'name':name,'signature':re.sub(r'\s+',' ',sig),'file':path.relative_to(ROOT).as_posix(),'line':n.start_point.row+1})
            return
        for c in n.named_children:walk(c,scope,public)
    walk(PARSER.parse(b).root_node);return out
def description(r):
    owner,name=r['owner'],r['name']
    if '= delete' in r['signature']:return '이 자원 소유 타입의 복사 생성/대입을 금지한다. 창·GPU 핸들을 중복 소유하지 않게 한다.'
    if name==owner:return {'Application':'초기 창 포인터와 콜백·CPU 장면 상태를 준비한다. 실제 창은 run에서 생성한다.','Scene':'빈 CPU 장면과 ID·형상 변경 상태를 초기화한다.','Renderer':'GPU 핸들을 0과 미업로드 상태로 준비한다. 실제 GL 생성은 initialize에서 수행한다.','AssetLoader':'프로젝트 루트와 기능 옵션을 보관하고 상대경로 자산 로딩을 준비한다.'}[owner]
    if name.startswith('~'):return '실행 자원을 정리한다. GPU 해제는 창의 컨텍스트가 유효한 동안 수행한다.'
    if name=='operator bool':return '모델 또는 이미지 결과가 있는지 성공 여부로 변환한다. 실패 시 error 문자열을 확인한다.'
    if (owner,name) in SPECIAL:return SPECIAL[owner,name]
    if name in DESCRIPTIONS:return DESCRIPTIONS[name]
    if name in guide.PURPOSE:return guide.PURPOSE[name]
    raise ValueError('한글 기능 설명 누락: '+owner+'::'+name)
def result_contract(r):
    s=r['signature']
    if '= delete' in s:return '호출 불가(복사 금지 선언).'
    if 'std::optional<' in s.split('(')[0]:return '값이 있으면 사용한다. 빈 optional은 실패 또는 선택/교차 결과 없음이다. 정상적인 빈 결과도 있으므로 호출부 조건을 확인한다.'
    if re.search(r'\bbool\b',s.split('(')[0]):return 'true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.'
    if '*' in s.split('(')[0]:return '비소유 포인터다. nullptr과 원본 객체 수명·컨테이너 변경을 확인한다.'
    if '&' in s.split('(')[0]:return '비소유 참조다. 반환한 Scene/배열/상태의 수명 동안만 사용한다.'
    return '반환형과 위 기능 설명을 따른다. 이 호출만으로 Scene 표시나 시간 진행이 자동 발생한다고 가정하지 않는다.'
def main():
    paths=sorted((ROOT/'framework/include').glob('*.h'))
    # 기존 생성 주석을 제거하고 AST 위치를 다시 구해 반복 실행 시 중복을 막는다.
    for p in paths:
        s=re.sub(r'^[ \t]*/// \[API\].*\n','',p.read_text(encoding='utf-8-sig'),flags=re.M)
        save(p,s)
        records=declarations(p);lines=s.splitlines()
        for r in sorted(records,key=lambda r:r['line'],reverse=True):
            indent=re.match(r'\s*',lines[r['line']-1])[0]
            lines.insert(r['line']-1,indent+'/// [API] '+description(r))
        save(p,'\n'.join(lines)+'\n')
    records=[]
    for p in paths:
        for r in declarations(p):r.update(description=description(r),result=result_contract(r));records.append(r)
    save(ROOT/'framework/docs/API_KO.json',json.dumps(records,ensure_ascii=False,indent=2)+'\n')
    md='# 프레임워크 공개 API 한글 목록\n\n대상: `framework/include/*.h`의 공개 함수·메서드·생성/소멸·변환 연산자. 비공개 멤버, detail 구현, SDK 함수는 제외한다. 오버로드와 헤더 내 정의는 각각 기록한다. 각 함수의 한글 주석은 선언 바로 위에 있다.\n\n'
    md+='이 문서는 API의 전체 탐색 목록이다. 시험장에서 사용할 호출 순서는 [공통 행동 그래프](../../docs/code-guide/exam.html)와 [요구사항별 API 호출](../../docs/code-guide/api.html)을 먼저 읽는다.\n\n'
    md+='## 공통 계약\n\n- 시간은 초, 각도는 별도 표기가 없으면 도다. `glm::radians`에 넣은 뒤 GLM 회전 행렬을 만든다.\n- 부모가 있는 엔티티의 위치는 부모 로컬이다. GridLayout.position은 월드 위치, relativeSize는 그리드 로컬 크기다.\n- bool/optional/포인터를 검사한 뒤 결과를 사용한다. 빈 피킹 결과·공중 jump의 false는 정상적인 판정일 수 있다.\n- Application은 모션을 프레임당 한 번 갱신한다. 여러 RenderPass에서도 물리와 시간을 중복 갱신하지 않는다.\n- ID는 소유 포인터가 아니다. Scene.clear 후 엔티티 ID를 다시 받는다. GPU 자원은 현재 컨텍스트에서 생성·해제한다.\n- Front cull은 winding에 따른 면 제거다. 법선 기반 명암과 다르다. 현재 셰이더에는 투영 그림자가 없다.\n\n## 파일별 목록\n\n'
    for p in paths:
        rows=[r for r in records if r['file']==p.relative_to(ROOT).as_posix()]
        md+='- ['+p.name+'](#'+p.stem.lower()+'): '+str(len(rows))+'개 선언\n'
    for p in paths:
        rows=[r for r in records if r['file']==p.relative_to(ROOT).as_posix()]
        md+='\n## '+p.stem+'\n\n'
        if not rows:md+='공개 함수 없음. 데이터/내부 지원 헤더다.\n'
        for r in rows:
            full=(r['owner']+'::' if r['owner'] else '')+r['name']
            md+='### '+full+'\n\n```cpp\n'+r['signature']+';\n```\n\n'+r['description']+'\n\n입력: 위 시그니처의 타입·기본값과 기능 설명을 함께 읽는다.\n\n반환·실패: '+r['result']+'\n\n소스: [`'+r['file']+':'+str(r['line'])+'`](../include/'+p.name+')\n\n'
    md+='\n## 확인 범위\n\n소스 선언을 AST로 읽어 만든 목록이며 빌드·테스트·실행을 수행하지 않았다. 사용자에게 API 사용 코드와 화면 확인을 맡긴다.\n'
    save(ROOT/'framework/docs/API_KO.md',md)
    print('공개 API 한글 주석 및 목록 생성: '+str(len(records))+'개 선언')
if __name__=='__main__':main()
