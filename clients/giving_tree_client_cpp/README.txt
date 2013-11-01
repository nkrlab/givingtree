C/C++ 용 이클립스에 giving_tree_client_cpp 프로젝트를 생성하는 방법은 다음과 같습니다(
이 방법은 protobuf 컴파일러와 라이브러리가 이미 설치되어 있음을 전제로 합니다):


h2. 1. protobuf 소스 코드 생성

* Step 1: Open terminal.
* Step 2: Go to 'giving_tree_client_cpp' directory.
* Step 3: Execute 'funapi_client_protoc.sh' script.
* Step 4: Execute 'giving_tree_client_protoc.sh' script.
** Verify 'src/gen' directory and pb.h/cc files are generated.
* Step 5: Exit terminal.


h2. 2. 새 이클립스 프로젝트 생성

* Step 1: Select 'Workspace' as the parent directory of
          'giving_tree_client_cpp' directory while Eclipse launching.
** E.g.) Select 'clients' directory in case 'giving_tree_client_cpp' exists
         in 'clients/giving_tree_client_cpp'.
* Step 2: Select 'File -> New -> C++ Project'.
* Step 3: Enter "giving_tree_client_cpp" as 'Project Name'.
* Step 4: Click 'Next -> Next -> Finish'.


h2. 3. include 경로 등록

* Step 1: Select 'Project -> Properties'.
* Step 2: Select 'C/C++ General -> Paths and Symbols'.
* Step 3: Select '[All configurations]' as 'Configuration'.
* Step 4: Select 'Includes -> GNU C++'.
* Step 5: Add the path of project root to includes.
** Step 5-1: Click 'Add'.
** Step 5-2: Check 'Is a workspace path'.
** Step 5-3: Enter '/giving_tree_client_cpp/src' as 'Directory'.
** Step 5-4: Click 'OK'.
* Step 6: Add the path of auto-generated directory to includes.
** Step 6-1: Click 'Add'.
** Step 6-2: Check 'Is a workspace path'.
** Step 6-3: Enter '/giving_tree_client_cpp/src/gen' as 'Directory'.
** Step 6-4: Click 'OK'.


h2. 4. protobuf 라이브러리 등록

* Step 1: Select 'Project -> Properties'.
* Step 2: Select 'C/C++ General -> Paths and Symbols'.
* Step 3: Select '[All configurations]' as 'Configuration'.
* Step 4: Select 'Libraries'.
* Step 5: Add 'protobuf' library.
** Step 5-1: Click 'Add'.
** Step 5-2: Enter 'protobuf' as 'File' and click 'OK'.
** Step 5-3: Click 'OK'.

