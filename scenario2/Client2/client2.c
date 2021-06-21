#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<sys/sem.h> 
#include <sys/stat.h>  
#include <stdlib.h>
#include <pwd.h>

#define MAXLINE 1024
#define PORTNUM 3000

void *function(void *data){
        int sockfd = *((int *) data);
        int readn;
        socklen_t addrlen;
        char buf[MAXLINE];
	char ver_cmd[MAXLINE];
        struct sockaddr_in client_addr;
        memset(buf, 0x00, MAXLINE);
        addrlen = sizeof(client_addr);
        getpeername(sockfd, (struct sockaddr *) &client_addr, &addrlen);
	printf("Connect : %s (%d)\n",inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));
	
	if(read(sockfd, buf, MAXLINE) <= 0){
		perror("read error : ");
		return 0;
	}
	printf("%s",buf);
	memset(buf,0x00,MAXLINE);

	printf("enter your answer(yes/no)! : ");
	scanf("%s", buf);

	write(sockfd, buf, MAXLINE); //send the answer to server2

	memset(buf,0x00,MAXLINE);
	read(sockfd, buf, MAXLINE);

	if(!strncmp(buf, "Please", 6))
	{
		printf("%s\n", buf);
	}
	close(sockfd);

	printf("Server connection end\n");
	if(!strncmp(buf,"yes",3)){
		write(1,"update execute\n",sizeof("update execute\n"));
		strcpy(ver_cmd, "./client2-1 ");
		strcat(ver_cmd, buf);
		system(ver_cmd);
	}
	else{
		write(1,"no update\n",sizeof("no update\n"));
	}
        return 0;
}

int main(){
        int listen_fd, client_fd;
        socklen_t addrlen;
        int readn;
        char buf[MAXLINE];
        pthread_t thread_id;
        struct sockaddr_in server_addr, client_addr;
        if( (listen_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
                return 1;
        }
        memset((void *) &server_addr, 0x00, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
        server_addr.sin_port = htons(PORTNUM);

        if(bind(listen_fd, (struct sockaddr *) &server_addr,sizeof(server_addr)) == -1){
                perror("bind error");
                return 1;
        }

        printf("Client's Server Start...\n");

        if(listen(listen_fd, 5) == -1){
                perror("listen error");
                return 1;
        }
        addrlen = sizeof(client_addr);
	while(1){
		client_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &addrlen);
		if(client_fd == -1){
			printf("accept error\n");
		}
		else{
			function((void *) &client_fd);
	        }
	}
        return 0;
}

