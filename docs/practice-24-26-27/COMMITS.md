# 이번 작업의 커밋

문서 보고서 생성 시점까지의 단계 및 교정 커밋이다. 최종 문서 커밋 해시는 최종 응답에서 제공한다.

```text
436cde7 Practice step 1: add composable Phong materials and attached cameras
8502c91 Practice step 2: implement parcel loading and attached views for lab 24
85f8671 Practice step 3: implement Phong light orbit and radius controls for lab 26
7d1f932 Practice step 4: implement three lit spheres and light colors for lab 27
14b3fd4 Practice step 5: compose materials, attachments and custom mechanism references
b7fdbcb Practice correction: make labs 24 26 27 self-contained and record folder isolation
78669b8 Practice 24 correction: place truck outside L belt and map five parcel cameras
60d9d93 Practice step 6: publish requirements, novel-mechanism exercises and final source references
```

공유 스킬/사건 로그 기준 저장소: 5597a28, origin/main 일반 push 완료. 과제 저장소는 별도 push 요청 없이 로컬 커밋으로 남긴다.

## 2026-11-09 후속 수정

```text
219615a 이전 작업의 최종 소스·참조 문서 커밋
6cefc23 1단계: 상자 카메라를 뒤로 이동
d277470 2단계: 트럭을 세로 컨베이어 바깥 옆에 배치
```

3단계 문서 커밋은 최종 응답에서 제공한다. [변경 블록 전체](CAMERA24_ADJUSTMENT.md). 빌드·테스트·앱 실행은 사용자 확인 전이다.

## 적재 선형보간

- `7a9f8ed`: 1단계, 1초 위치 선형보간·중복 입력 방지·도착 시 부착·리셋·이동 중 카메라 갱신.
- 2단계 문서 커밋은 최종 응답에 기록한다. [전체 변경 전후와 사용자 확인](LOADING24.md).
