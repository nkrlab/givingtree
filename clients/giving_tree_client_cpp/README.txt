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


h2. 5. GivingTreeClientProtoc 빌더 생성

만약 *.proto 파일을 수정했을 경우,
protoc 를 실행해서 *.pb.h 와 *.pb.cc 파일을 다시 생성해야 합니다.
(즉, 터미널에서 'giving_tree_client_protoc.sh' 스크립트를 다시 실행해야 합니다.)
그런데 만약 이 수동 작업이 번거롭다면,
새로 빌더를 생성해서 빌드 과정에 자동으로 스크립트가 실행되게 할 수 있습니다.

이클립스 CDT 에 GivingTrreeClientProtoc 빌더를 생성하는 방법은 다음과 같습니다:

* Step 1: Select 'Project -> Properties'.
* Step 2: Select 'Builders'.
* Step 3: Click 'New'.
* Step 4: Select 'Program' and Click 'OK'.
* Step 5: Edit launch configuration properties.
** Step 5-1: Enter 'GivingTreeClientProtoc' as 'Name'.
** Step 5-2: Enter
'${workspace_loc:/giving_tree_client_cpp/giving_tree_client_protoc.sh}'
as 'Location'.
** Step 5-3: Enter '${workspace_loc:/giving_tree_client_cpp}'
as 'Working Directory'.
** Step 5-4: Enter '${workspace_loc:/giving_tree_client_cpp}' as 'Arguments'.
** Step 5-5: Click 'OK'.
* Step 6: Select 'GivingTreeClientProtoc' and Click 'Up' Repeatedly
up to the top of the list.
* Step 7: Click 'OK'.

