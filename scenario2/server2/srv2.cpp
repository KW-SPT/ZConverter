#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#define MAXLINE    1024

int cli_connect(char *ip,int PORT, char ver[]){
	struct sockaddr_in serveraddr;
	int server_sockfd;
	int client_len;
	char buf[MAXLINE];
	if ((server_sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		perror("error :");
		return 1;
	}
	/* 연결요청할 서버의 주소와 포트번호 프로토콜등을 지정한다. */
	server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(ip);
	serveraddr.sin_port = htons(PORT);
	client_len = sizeof(serveraddr);
	/* 서버에 연결을 시도한다. */
	if (connect(server_sockfd, (struct sockaddr *)&serveraddr, client_len)  == -1){
		perror("connect error :");
		return 1;
	}
	memset(buf, 0x00, MAXLINE);
	strcpy(buf,"There is a problem with the ");
	strcat(buf,ver);
	strcat(buf," version you are using.\nWe recommend updating new version!\n");
	if (write(server_sockfd, buf, MAXLINE) <= 0) /* 입력 받은 데이터를 서버로 전송한다. */{
		perror("write error : ");
		return 1;
	}
	close(server_sockfd);
	return 0;
}
int main() {
	char ver[10];
	printf("오류 있는 버전 입력하세요 : ");
	scanf("%s",ver);
	char cmd[256];
	strcpy(cmd, "./log ");
	strcat(cmd,ver);
	system(cmd); 
	FILE* fp;
	fp = fopen("scenario2.txt", "r");
	if (fp != NULL) {
		char buffer[256];
		char *ip;
		char *port;
		int i=0;
		while (!feof(fp)) {
			fgets(buffer, sizeof(buffer), fp);
			buffer[strlen(buffer) - 1] = '\0'; //개행문자("\n") 제거
			if(strtok(buffer, " ")==NULL) break;
			ip=strtok(NULL, " ");
			port=strtok(NULL," ");
			printf("ip :%s\nport : %s\n",ip,port);
			cli_connect(ip,atoi(port),ver);
			memset(buffer,0,sizeof(buffer));
		}
		printf("오류 있는 버전을 사용하는 클라이언트들에게 메세지 전송 완료!\n");
		system("rm scenario2.txt");
	}
	else{
		printf("오류 있는 버전을 사용하는 클라이언트가 없습니다!\n");
	}
	return 0;
}
