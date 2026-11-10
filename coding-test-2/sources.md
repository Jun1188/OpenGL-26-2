# 출처와 확인 범위

확인일 2026-11-10. 다음 정기 검토 기준일 2027-01-10(달력 두 달). 시험일은 2026-11-24이며 이후 자료는 기록으로 유지한다. 자동 예약이나 기말고사 작업을 만들지 않는다.

| ID | 직접 출처 | 난이도/목적 | 신뢰성·최신성·한계 |
|---|---|---|---|
| S001 | [GLFW 3.4 시작 가이드](https://www.glfw.org/docs/3.4/quick.html) | 입문, 창/컨텍스트/입력/루프 | 프로젝트 공식 버전 문서. 본문 확인. 게시일 미확인. 예제 로더는 glad이므로 이 자료의 GLEW 경로와 구분 |
| S002 | [Khronos glVertexAttribPointer 원문](https://github.com/KhronosGroup/OpenGL-Refpages/blob/main/gl4/glVertexAttribPointer.xml) | 기초, VAO 입력 계약 | 공식 API 원문 확인. gl4 문서의 관련 기본 계약을 사용하며 이 코드의 GL3.3 지원 범위를 넘는 기능을 가져오지 않음 |
| S003 | [Khronos glUniform 원문](https://github.com/KhronosGroup/OpenGL-Refpages/blob/main/gl4/glUniform.xml) | 기초, program/uniform 연결 | 공식 원문 확인. 현재 program, 타입과 location=-1 동작을 확인 |
| S004 | [GLM 1.0.3 태그 매뉴얼](https://github.com/g-truc/glm/blob/1.0.3/manual.md) | 기초~중급, 행렬 | 로컬 SDK 헤더는 1.0.3. 해당 태그 본문 제목은 1.0.0으로 남아 있음. translate/rotate/perspective 예를 확인 |
| S005 | [지정 ComputerGraphics 저장소](https://github.com/Jun1188/ComputerGraphics/tree/f42b28ab2cba2fb57f37bb67bb3738e27889765a) | 중급, 수업 코드와 비교 | 조명 vertex/fragment 및 CPU 소스를 읽음. FragPos 타입 불일치·법선 공간 처리를 그대로 답안으로 복제하지 않음. 실행 미확인 |
| S006 | lecture_pdf/homework/지엘4 조명 (실습 2627).pdf | 수업 요구 | 전체3쪽 글·그림·키 입력 확인. 시험 전체 범위를 단독으로 확정하는 문서는 아님 |
| S007 | lecture_pdf/homework/지엘4 조명 (실습 28).pdf | 수업 요구 | 전체2쪽 확인. 이동 대상과 광원/카메라 조합 |
| S008 | lecture_pdf/homework/코딩 시험 문제.pdf | 전이 사례 | 앞 대화에서 전체3쪽 확인. 특정 저울 정답을 새 시험 예상문제로 취급하지 않음 |
| S009 | native 및 Standard의 현재 소스 | 직접 작성/비교 | 소스는 작성·열람했지만 빌드·테스트·앱·HTML 브라우저 확인은 하지 않음 |

공식 문서와 프로젝트 저장소는 갱신 자료를 찾는 채널이며, 세부 주제의 원문을 읽는 근거 자료다. 뉴스·조회 수를 품질 기준으로 삼지 않는다. 온라인 링크는 선택 참고용이다. 학습 HTML 본문과 내장 코드 스냅샷은 인터넷 없이 사용할 수 있다.
