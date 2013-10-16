C/C++ 용 이클립스에 GivingTreeClient 프로젝트를 생성하는 방법은 다음과 같습니다(
이 방법은 protobuf 컴파일러와 라이브러리가 이미 설치되어 있음을 전제로 합니다):


h2. 1. protobuf 소스 코드 생성

* Step 1: Open terminal.
* Step 2: Go to 'cpp' directory.
* Step 3: Execute 'funapi_client_protoc.sh' script.
* Step 4: Execute 'giving_tree_client_protoc.sh' script.
* Step 5: Verify 'src/gen' directory and pb.h/cc files are generated.


h2. 2. 새 이클립스 프로젝트 생성

* Step 1: Select 'File -> New -> C++ Project'.
* Step 2: Enter "GivingTreeClient" as 'Project Name'.
* Step 3: Click 'Next -> Next -> Finish'.


h2. 3. 소스 코드 임포트

* Step 1: Select 'File -> Import'.
* Step 2: Select 'General -> File System' and click 'Next'.
* Step 3: Import 'src' directory.
** Step 3-1: Enter the full path of 'src' directory as 'From directory'.
** Step 3-2: Click 'Select All'.
** Step 3-3: Enter 'GivingTreeClient' as 'Into folder'.
** Step 3-4: Click 'Finish'.


h2. 4. include 경로 등록

* Step 1: Select 'Project -> Properties'.
* Step 2: Select 'C/C++ General -> Paths and Symbols'.
* Step 3: Select '[All configurations]' as 'Configuration'.
* Step 4: Select 'Includes -> GNU C++'.
* Step 5: Add the path of project root to includes.
** Step 5-1: Click 'Add'.
** Step 5-2: Check 'Is a workspace path'.
** Step 5-3: Enter '/GivingTreeClient' as 'Directory'.
** Step 5-4: Click 'OK'.
* Step 6: Add the path of auto-generated directory to includes.
** Step 6-1: Click 'Add'.
** Step 6-2: Check 'Is a workspace path'.
** Step 6-3: Enter '/GivingTreeClient/gen' as 'Directory'.
** Step 6-4: Click 'OK'.


h2. 5. protobuf 라이브러리 등록

* Step 1: Select 'Project -> Properties'.
* Step 2: Select 'C/C++ General -> Paths and Symbols'.
* Step 3: Select '[All configurations]' as 'Configuration'.
* Step 4: Select 'Libraries'.
* Step 5: Add 'protobuf' libraray.
** Step 5-1: Click 'Add'.
** Step 5-2: Enter 'protobuf' as 'File' and click 'OK'.
** Step 5-3: Click 'OK'.

