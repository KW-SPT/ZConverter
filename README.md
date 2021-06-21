# ZConverter
2020년 광운대학교 산학연계 SW프로젝트 SPT팀 repository


### 시나리오1

**client**  
*    cli_option.txt* - 클라이언트 옵션 설정용 텍스트파일  
*    client.c* - 리눅스 클라이언트 C 파일  
*    client_demon.c* - 리눅스 클라이언트 (데몬버전)  
*    pyEXE_cli.tar.gz* - 파이썬 실행파일 압축  
*    hash.py* - 해시검사용 파이썬 파일  
*    http_cli.py* - http 프로토콜 클라이언트 파이썬 파일  
*    websocket_cli.py* - websocket 프로토콜 클라이언트  
*    ssh_info.txt* - ssh 프로토콜용 텍스트파일  
*    cli.cpp* - 윈도우 클라이언트 C++ 파일  


**server**  
*Linux* - 리눅스용 전송파일 리포지토리  
*Windows* - 윈도우용 전송파일 리포지토리    
*define.txt* - 메인 서버 옵션 설정용 텍스트파일    
*pyEXE_srv.tar.gz* - 서버에서 사용되는 파이썬용 실행파일 압축  
*server.c* - 메인 서버 C코드  
*server_daemon.c* - 메인서버 실행파일용 코드 (mainDaemon.c에 있는 코드)  
*mainDaemon.c* - 메인 서버 (데몬버전)   
*http_srv.py* - http 서버  
*httpDaemon.c* - http 서버용 데몬코드  
*websocket_srv.py* - websocket 서버  
*websocketDaemon.c* - websocket 서버용 데몬코드  
*ssh.py* - ssh 코드  
*hash_srv.py* - 해시 검사용 파이썬 파일  
*update_file.py* - update.txt에서 해당 클라이언트의 정보를 찾아서 최신 정보로 업데이트하는 파이썬 파일  
*new.txt* - 윈도우/리눅스의 가장 최신 업데이트 버전을 저장한 텍스트파일  
*update.txt* - 클라이언트별 최신버전 저장한 텍스트 파일  
*wh_path.txt* - websocketDaemon.c와 httpDaemon.c 경로를 저장한 텍스트 파일  
*mainDaemon.service* - 메인 서버 데몬용 서비스파일  
*httpDaemon.service* - http 서버 데몬용 서비스파일  
*websocketDaemon.service* - websocket 서버 데몬용 서비스파일  

---------------------------------------------------------------------------------------------------------------------------

### 시나리오2

**client**      
*cli_option.txt* - 시나리오2 버전 클라이언트 옵션 설정용 텍스트파일(시나리오 1에서와 동일)    
*client2-1.c* - 시나리오2 클라이언트(시나리오1 클라이언트와 동일하게 동작, 버전만 client2.c로부터 전달받음)  
*client2.c* - 시나리오2 클라이언트. 서버로부터 사용하고 있는 버전에 오류가 있다는 메시지를 받음  


**server**  
*log.py* - 오류가 있는 버전을 사용하고 있는 클라이언트 목록을 update.txt로부터 읽어 오는 파이썬 파일  
*server2.c* - 오류가 있는 버전의 사용자들에게 메시지를 보냄  
*update.txt* - 시나리오 1에서 생성한 클라이언트 목록 텍스트 파일  

---------------------------------------------------------------------------------------------------------------------------

### 폴더 및 파일 소개
**Client/**
: 클라이언트 관련 파일들이 있는 폴더  
    **python code/**  
    : 클라이언트의 파일 다운로드 관련 파일이 있는 폴더  
        *hash.py*  
        : SHA-256 검사  
        *http_cli.py*  
        : http 프로토콜 클라이언트  
        *websocket_cli.py*  
        : websocket 클라이언트  
    *cli.cpp*  
    : Windows 사용자의 메인 클라이언트  
    *cli_option.txt*  
    : 클라이언트 옵션(인자) 설정용 텍스트 파일. 줄 단위로 정보 입력  
        - 첫줄 : 접속할 서버의  ip  
        - 두번째 : 접속할 서버의 port  
        - 세번째 : 새로운 버전을 다운받고 싶다면 new를 특정 버전을 원한다면 특정 버전을 입력  
        - 네번째 : 다운받을 프로토콜 방식(https, websocket, ssh)  
        - 다섯번째 : 클라이언트가 파일을 다운받을 경로  
        - 여섯번째 : 시나리오2에서 사용하는 클라이언트의 port 번호  
    *client.c*  
    : Linux 사용자의 메인 클라이언트  
    *client_daemon.c*  
    : 데몬 사용을 위해 만들어둔 Linux 메인 클라이언트(데몬 코드 X, 터미널 입력 X)  
    *pyEXE_cli.tar.gz*  
    : python 실행파일(hash.exe, http_cli.exe, websocket_cli.exe)들의 압축파일  
    *ssh_info.txt*  
    : ssh 계정 정보(사용자 이름, 패스워드, 포트)를 기록한 txt파일  


**Server/**  
: 서버 관련 파일들이 있는 폴더  
	**Linux/**  
	: Linux 사용자가 다운받을 버전 파일들. 버전명을 폴더 이름으로 두고 그 안에 버전별 다운받을 파일을 넣어놓으면 됨  
	**버전명/**  
         		다운받을 파일  
   	**Windows/**  
   	: Windows 사용자가 다운받을 버전 파일들. 버전명을 폴더 이름으로 두고 그 안에 버전별 다운받을 파일을 넣어놓으면 됨  
      		**버전명/**  
         		다운받을 파일  
   	**python code/**  
   	: 프로토콜 관련 파일들이 있는 폴더  
      		*hash_srv.py*  
      		: SHA-256 검사  
      		: 실행방법: ./hash_srv 경로(해시값 구할 파일이 있는 경로)  
      		: 해쉬 값 계산후 파일에 저장 (allHashsum.txt - 해당 파일에는 현재 레포지토리 폴더에 있는 모든 파일들의 해시값을 저장한다.)  
      		*http_srv.py*  
      		: http 프로토콜 서버  
      		*ssh.py*  
      		: ssh 프로토콜   
      		*update_file.py*  
      		*websocket_srv.py*  
      		: websocket 프로토콜 서버  
   	*define.txt*  
   	: 메인 서버 옵션  
       		- (순서대로 한줄 씩 입력) 서버 포트, 서버 ip, http 서버 url, websocket 서버 url, 메인서버 경로  
       		- ‘;’ 이전까지 입력  
   	*httpDaemon.c*  
   	: http 서버 데몬코드, http_srv.py 호출하여 실행  
   	*httpDaemon.service*  
   	: http 서버 데몬파일을 서비스로 실행  
       		- ExecStart=실행할 파일 절대 경로 ex)경로/실행파일 => /home/kwu/Desktop/server/httpDaemon  
       		- WorkingDirectory=실행할 파일이 위치한 폴더 절대 경로 (실행파일명 포함 x)  
   	*mainDaemon.c*  
   	: 메인 서버 코드 + 데몬 코드 (httpDaemon처럼 호출 x)  
   	: 서버 코드 수정 시 여기서  
   	*mainDaemon.service*  
   	: 메인 서버 데몬파일을 서비스로 실행  
       		- ExecStart=실행할 파일 절대 경로 ex)경로/실행파일 => /home/kwu/Desktop/server/mainDaemon  
       		- WorkingDirectory=실행할 파일이 위치한 폴더 절대 경로 (실행파일명 포함 x)  
   	*new.txt*  
   	: 윈도우/리눅스의 가장 최신 업데이트 버전을 저장한 텍스트파일  
       		- 한줄 씩 버전 입력  
     	*pyEXE_srv.tar.gz*  
     	: python 실행파일(hash_srv.exe, http_srv.exe, ssh.exe, update_file.exe, websocket_srv.exe)들의 압축파일  
     	*server.c*  
     	: 메인 서버 C코드   
     	*server_daemon.c*  
     	: 메인서버 실행파일용 C코드 (mainDaemon.c에 있는 코드)  
     	*update.txt*  
     	: 클라이언트들이 가장 최신으로 다운받은 버전을 저장한 텍스트 파일  
     	*websocketDaemon.c*  
     	: websocket 서버 데몬코드, websocket_srv.py 호출하여 실행  
     	*websocketDaemon.service*  
   	: websocket 서버 데몬파일을 서비스로 실행  
       		- ExecStart=실행할 파일 절대 경로 ex)경로/실행파일 => /home/kwu/Desktop/server/websocketDaemon  
       		- WorkingDirectory=실행할 파일이 위치한 폴더 절대 경로 (실행파일명 포함 x)  
     	*wh_path.txt*  
     	: http 서버(실행파일), websockek 서버(실행파일) 경로   
 	  

**install_script/**  
   	*Client.tar.gz*  
   	: client_demon.c(Linux 클라이언트 C코드), pyEXE_cli.tar.gz(파이썬 실행파일-http_cli.exe, websocket_cli.exe, hash.exe) 를 압축한 파일  
   	*Server.tar.gz*  
   	: server.c(서버 c코드), pyEXE_srv.tar.gz(파이썬 실행파일 - websocket_srv, update_file, ssh, http_srv, hash_srv)을 압축한 파일  
   	*client_linux.sh*  
   	: 사용자의 입력을 받아 cli_option.txt, ssh_info.txt를 생성하고 이들과 클라이언트용 파이썬 실행파일의 압축파일을 Client라는 폴더를 생성하여 그 안으로 이동 및 압축을 해제하여 클라이언트 실행환경을 구성하는 설치 스크립트  
   	*server_exe.sh*  
   	: 사용자의 입력을 받아 define.txt와 new.txt를 생성하고, 이들과 파이썬 실행파일의 압축파일을 srv_localpath에 이동 및 압축 해제하는 설치 스크립트  
	  

**scenario2/**  
: 시나리오2  
   	**Client2/**  
   	: 시나리오2의 클라이언트 폴더  
      		*cli_option.txt*  
      		: 시나리오2 클라이언트 옵션(인자) 설정용 텍스트 파일. 순서대로 한줄씩 입력(시나리오1의 cli_option.txt와 동일한 파일  
         		- 접속할 서버 ip  
         		- 접속할 서버 포트  
            		- 버전  
         		- 다운받을 프로토콜 방식(https, websocket, ssh)  
         		- 클라이언트가 파일을 다운받을 경로  
         		- 시나리오2에서 사용하는 클라이언트의 port 번호  
      		*client2-1.c*  
      		: 시나리오1의 메인 클라이언트와 동작은 똑같으나, 업데이트 버전(오류가 있는 버전)을 client2.c로부터 전달 받는다.  
      		*client2.c*  
      		: 서버로부터 사용하고 있는 버전에 오류가 있다는 메시지를 받음  
   	**server2/**  
   	: 시나리오2의 서버 폴더  
      		*log.py*  
      		: 오류가 있는 버전을 사용하고 있는 클라이언트 목록을 update.txt로부터 읽어 오는 파이썬 파일  
      		*server2.c*  
      		: 오류가 있는 버전의 사용자들에게 메시지를 보냄  
       		*update.txt*  
       		: 시나리오 1에서 생성한 클라이언트 목록 텍스트 파일  
		
		
**errorcode.txt**  
: 에러코드 설명  


