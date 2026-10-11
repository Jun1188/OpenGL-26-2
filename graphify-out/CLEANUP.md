# 단계 10: 그래프 폴더와 push 추적 범위 정리

## before / after

| before | after | 처리 |
| --- | --- | --- |
| graphify-out | graphify-out | 통합 그래프의 대표 위치 유지 |
| graphify-out/graphify-out/cache | graphify-out/cache | 캐시 이동 후 빈 중첩 폴더 제거 |
| openGL_sample0/graphify-out | graphify-out/archive/openGL_sample0-2026-09-08 | 이전 결과를 보관본으로 이동 |
| framework-analysis/graphify-out | 같은 위치 | framework 전용 결과 유지 |
| framework-analysis/archive | 같은 위치 | 이전 framework 결과 보관 |

노드·관계 데이터는 이번에 재추출·병합하지 않았다. 보관본의 원래 내용과 SHA-256은 CLEANUP_MANIFEST.json에 기록했다. README에 대표 경로와 보관 범위를 추가했다.

## Git 추적

루트와 각 분석 폴더의 .gitignore는 캐시·컴퓨터별 실행 경로·중간 추출 파일만 제외한다. 공개 graph.json/graph.html/보고서/라벨/manifest는 포함한다. 이전의 넓은 .* 및 중첩 graphify-out/ 제외 규칙은 제거했다.

MSVC 빌드 결과, 개인 .vcxproj.user, NuGet packages 캐시는 Git 추적에서 제외한다. 패키지 복원 정보인 packages.config, 실제 소스·프로젝트 파일, 번들 dependencies의 필요한 DLL/LIB와 라이선스는 유지한다. 이미 추적된 산출물은 git rm --cached로 인덱스에서만 제거하며 로컬 파일을 삭제하지 않는다.

미커밋 Console.cpp 수정은 보존하며 이 정리 커밋에 추가하지 않는다. 사용자 요청에 따라 현재 main의 기존 미push 커밋과 이번 정리 커밋을 origin/main에 push한다. force push와 이력 재작성은 사용하지 않는다.

소스·빌드·동작 테스트는 수행하지 않았다. 확인 범위는 이동 후 내용 대조, ignore 규칙, Git 추적과 원격 커밋 상태다.
