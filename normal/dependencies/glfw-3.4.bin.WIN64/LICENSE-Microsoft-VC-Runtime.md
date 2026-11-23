# Microsoft VC runtime 동봉 정보

Copyright Microsoft Corporation. All rights reserved.

- 파일: `lib-vc2022/vcruntime140.dll`, x64, FileVersion 14.51.36247.0.
- 출처: 설치된 정식 Visual Studio Community의 `VC/Redist/MSVC/14.51.36231/x64/Microsoft.VC145.CRT/`. Debug/비재배포 디렉터리의 파일이 아니다. DLL은 수정하지 않았다.
- 목적: 동봉 GLFW 3.4 DLL의 import table에 있는 VCRUNTIME140.dll 의존성을 해소한다. GLFW 옆 디렉터리에 두어 LoadLibraryEx의 DLL_LOAD_DIR 검색으로 로드한다.
- 원본 라이선스: 같은 디렉터리의 `LICENSE-Microsoft-VC-Runtime.docx`. Microsoft가 배포한 원본을 변경 없이 보관한다.
- 라이선스 원본 URL: https://visualstudio.microsoft.com/wp-content/uploads/2025/10/Visual-C-V14-License-Redistributable_and_Runtime_ENU.docx
- 재배포 목록: https://learn.microsoft.com/en-us/visualstudio/releases/2026/redistribution
- 배포 설명: https://learn.microsoft.com/en-us/cpp/windows/redistributing-visual-cpp-files

Microsoft의 사용 조건에 따른 재배포 파일이다. 프레임워크 자체 라이선스나 GLFW 라이선스로 재허가하지 않는다. Windows 10/11 x64의 UCRT/API-set, 시스템 DLL, GPU 드라이버는 운영체제에서 제공한다. 앱 로컬 런타임의 향후 업데이트는 배포자가 관리한다. 이 작업에서는 설치 프로그램이나 DLL을 실행하지 않았다.
