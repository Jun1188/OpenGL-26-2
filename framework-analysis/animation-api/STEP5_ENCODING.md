# 5단계: sample0 진입점 BOM 반영

대상: openGL_sample0/main.cpp. 코드 문장은 바뀌지 않는다.
1단계에서 저장한 UTF-8 BOM을 최종 커밋에도 포함한다. 기존 원본은 BOM이 없었다.

## before

파일 시작 바이트: `23 69 66 6E 64 65 66` (`#ifndef`).

```cpp
#ifndef NOMINMAX
```

## after

파일 시작 바이트: `EF BB BF 23 69 66 6E 64 65 66` (UTF-8 BOM + `#ifndef`).

```cpp
#ifndef NOMINMAX
```

전체 주석 변경 전후는 STEP1_BEFORE_AFTER.md에 있다. graphify 결과는 이미 BOM이 있는 최종 작업 파일을 기준으로 추출했다. 빌드·테스트는 수행하지 않았다.
