# 실습23: 로봇

lecture_pdf/homework의 실습23 PDF 기준. GridLayout → addWalkingRig → CharacterBody/stepCharacter → poseWalkingRig로 연결한다. 다른 색 팔·다리, 머리·코·몸통, 랜덤 배치 장애물3개, 가장자리/장애물 방향반전, 속도별 보행진폭, 점프·윗면착지를 제공한다. I 리셋은 같은 난수 seed로 초기 배치를 복원한다.

W/A/S/D는 +Z/-X/-Z/+X로 진행 방향을 설정하고 계속 걷는다. +/- 속도(0~3), J 점프, Space 보행 정지/재개. 정지 후 J는 제자리 점프, 보행 중 J는 수평 이동을 유지해 장애물에 올라간다. O 문 열기/닫기, I 리셋, x/X·z/Z 카메라 이동, y/Y 카메라 target 기준 Y공전, Q/Esc 종료. 문은 본문 슬라이딩/키설명 스케일의 불일치 중 키설명의 상단 기준 축소를 사용한다.

stage의 각 면에 다른색/Front컬링을 적용한다. 장애물·로봇은 일반 물체다. 충돌은 축 정렬 대리 상자이며 팔다리 개별 충돌과 정밀 강체가 아니다. .sln에서 Debug/Release x64를 사용한다. 빌드·실행·테스트는 하지 않았고 사용자에게 맡긴다.
