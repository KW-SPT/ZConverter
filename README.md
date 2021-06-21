# ZConverter
2020년 광운대학교 산학연계 SW프로젝트 SPT팀 repository


### 시나리오1

**client**  
*cli_option.txt* - 클라이언트 옵션 설정용 텍스트파일  
*client.c* - 리눅스 클라이언트 C 파일  
*client_demon.c* - 리눅스 클라이언트 (데몬버전)  
*pyEXE_cli.tar.gz* - 파이썬 실행파일 압축  
*hash.py* - 해시검사용 파이썬 파일  
*http_cli.py* - http 프로토콜 클라이언트 파이썬 파일  
*websocket_cli.py* - websocket 프로토콜 클라이언트  
*ssh_info.txt* - ssh 프로토콜용 텍스트파일  
*cli.cpp* - 윈도우 클라이언트 C++ 파일  


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
