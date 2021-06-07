//////////////////////////////////////////////////////////////////
//                        					//
//         	 server.c - main server                      	//
//                        					//
//////////////////////////////////////////////////////////////////


#define _CRT_SECURE_NO_WARNINGS
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <dirent.h>
#include <error.h>
#include <fcntl.h>
#include <sys/resource.h>
#include <syslog.h>
#include <signal.h>
#include <errno.h>

#define MAXLINE 1024

struct Info2
{
	char version[10];
	char filePath[50];
	char sendType[20];
	char os[20];
	int port;
	char hostname[256];
};

struct ssh_Info
{
	char username[MAXLINE];
	char password[MAXLINE];
	int ssh_port;
};

// init mutex
pthread_mutex_t mutex_lock;

int PORTNUM;
char IP[10];
char HTTP_URL[256];
char WEBSOCKET_URL[256];
char srv_localpath[512];

char *get_time(time_t t, struct tm tm)
{
	static char time_str[20];
	
	t = time(NULL);
	tm = *localtime(&t);
	sprintf(time_str, "%d-%d-%d.%d:%d:%d", tm.tm_year+1900, tm.tm_mon+1, tm.tm_mday, tm.tm_hour,  tm.tm_min, tm.tm_sec);

	return time_str;
}
////////////////https protocol file send Function////////////////
void send_https(int sockfd, char *cli_cmd, struct Info2 cli_info2, char *buf, char *fileName, char *log, char *log_info, char (*arr_log)[MAXLINE], int *logi)
{
	char url[MAXLINE]="";
	strcpy(url, HTTP_URL);

	if(!strcmp(cli_info2.os,"Windows"))
	{  
		strcpy(cli_cmd,"http_cli.exe ");
	}
	else
	{
		strcpy(cli_cmd,"./http_cli ");
	}
	//strcpy(cli_cmd, "python3.7 http_cli.py ");

	//make http url -> HTTP_URL + cli_info2.os + cli_info2.version + fileName
	strcat(url, srv_localpath);
	strcat(url, cli_info2.os);
	strcat(url, "/");
	strcat(url, cli_info2.version);
	strcat(url, "/");
	strcat(url, fileName);
	//----------------------------//
   
	strcat(cli_cmd, url);
	strcat(cli_cmd," ");
	strcat(cli_cmd, fileName);
	strcat(cli_cmd," ");
	strcat(cli_cmd,"\"");
	strcat(cli_cmd,cli_info2.filePath);
	strcat(cli_cmd,"\"");

	write(sockfd, cli_cmd, strlen(cli_cmd)); //send msg to client
	strcpy(log, log_info);
	strcat(log, " || [DEBUG] cli_cmd : ");
	strcat(log, cli_cmd);
	strcat(log, "\n");
	strcpy(arr_log[*logi], log);
	(*logi)++;

	memset(buf, 0x00, MAXLINE);
	read(sockfd,buf,MAXLINE); //C777

}
//////////////////////////////////////////////////////////////////////////////////////////////////


////////////////ssh protocol file send Function////////////////
void send_ssh(int sockfd, struct Info2 cli_info2, char *buf, struct ssh_Info ssh_info, char *fileName, char *cli_ip, char *srv_path, char *log, char *log_info, char (*arr_log)[MAXLINE], int *logi)
{

	char sshport[4];
	char ssh_cmd[MAXLINE];
	sprintf(sshport, "%d", ssh_info.ssh_port);

	//strcpy(ssh_cmd, "python3 ssh.py "); //@@@@@@@@@@@  need to delete   @@@@@@@@@@@
	strcpy(ssh_cmd, "./ssh ");
	strcat(ssh_cmd, cli_ip);
	strcat(ssh_cmd, " ");
	strcat(ssh_cmd, sshport);
	strcat(ssh_cmd, " ");
	strcat(ssh_cmd, ssh_info.username);
	strcat(ssh_cmd, " ");
	strcat(ssh_cmd, ssh_info.password);
	strcat(ssh_cmd, " ");
	strcat(ssh_cmd, srv_path);
	strcat(ssh_cmd, "/");
	strcat(ssh_cmd, fileName);
	strcat(ssh_cmd, " ");
	strcat(ssh_cmd,"\"");
	strcat(ssh_cmd, cli_info2.filePath);
	strcat(ssh_cmd, fileName);
	strcat(ssh_cmd,"\"");

	write(sockfd, "PS001", 5); //PS001 - code means "start file using ssh"
	system(ssh_cmd);

	strcpy(log, log_info);
	strcat(log, " || [DEBUG] ssh_cmd : ");
	strcat(log, ssh_cmd);
	strcat(log, "\n");
	strcpy(arr_log[*logi], log);
	(*logi)++;

	memset(buf, 0x00, MAXLINE);
	read(sockfd,buf,MAXLINE); //PS001

}
//////////////////////////////////////////////////////////////////////////////////////


///////////////websocket protocol file send Function////////////////
void send_websocket(int sockfd, char *cli_cmd, struct Info2 cli_info2, char *buf, char *fileName, char *log, char *log_info, char (*arr_log)[MAXLINE], int *logi)
{

	if(!strcmp(cli_info2.os,"Windows"))
	{   
		strcpy(cli_cmd,"websocket_cli.exe ");
	}
	else
	{
		strcpy(cli_cmd,"./websocket_cli ");
	}

	//strcpy(cli_cmd, "python3 websocket_cli.py "); //@@@@@@@@@@@  need to delete   @@@@@@@@@@@
	strcat(cli_cmd, srv_localpath);
	strcat(cli_cmd, cli_info2.os);
	strcat(cli_cmd, "/");
	strcat(cli_cmd, cli_info2.version);
	strcat(cli_cmd," ");
	strcat(cli_cmd, fileName);
	strcat(cli_cmd, " ");
	strcat(cli_cmd,"\"");
	strcat(cli_cmd,cli_info2.filePath);
	strcat(cli_cmd,"\"");
	strcat(cli_cmd," ");
	strcat(cli_cmd, WEBSOCKET_URL);

	write(sockfd, cli_cmd, strlen(cli_cmd)); //send msg to client

	strcpy(log, log_info);
	strcat(log, " || [DEBUG] cli_cmd : ");
	strcat(log, cli_cmd);
	strcat(log, "\n");
	strcpy(arr_log[*logi], log);
	(*logi)++;	
	
	memset(buf, 0x00, MAXLINE);
	read(sockfd,buf,MAXLINE); //C777
}

void logging_err(char *log, char *errmsg)
{
	// logfile.txt - [time] [ip] [hostname] [log level + log content]
	FILE *fp;
	char text[MAXLINE];

	strcpy(text,log);
	strcat(text, " || ");
	strcat(text, errmsg);

	// critical section
	pthread_mutex_lock(&mutex_lock);
	fp = fopen("logfile.txt", "a+");
	if(fp == NULL) //file open error
	{
		//printf("file open error!\n");
		return;
	}
	fputs(text, fp); //save log at logfile.txt
	fclose(fp);
	pthread_mutex_unlock(&mutex_lock);
	//end
	return;
}
void logging_suc(char arr_log[][MAXLINE], int logi)
{
	FILE *fp;
	int i;

	// critical section
	pthread_mutex_lock(&mutex_lock);
	fp = fopen("logfile.txt", "a+");
	if(fp == NULL) //file open error
	{
		//printf("file open error!\n");
		return;
	}
	for(i=0;i<logi;i++)
	{
		fputs(arr_log[i], fp); //save log at logfile.txt
	}
	fclose(fp);
	pthread_mutex_unlock(&mutex_lock);
	//end
	return;
}

////////////////check hashsum Function////////////////
int hash_srv(int sockfd, char *fileName, char *srv_path, char *buf, char *log_info, char (*arr_log)[MAXLINE], char* log, int *logi)
{
   
	char cli_sha256[1024]="";
	char srv_sha256[1024]="";
	char hash_pwd[1024];
	char line[MAXLINE];
	char *ptr;

	if(write(sockfd, fileName, MAXLINE)==-1) //send file name to client
	{
		//printf("write error! : %s\n", fileName);
		//logging_err(log_info, "[ERROR] - Fail to send file name to Client!\n");
		sprintf(log, "%s || [ERROR] - Fail to send file name to Client!\n", log_info);
		strcpy(arr_log[*logi], log);
		(*logi)++;
	}	

	memset(buf, 0x00, MAXLINE);
	read(sockfd,buf,MAXLINE); //Receive hashsum from client (sha256)
	strcpy(cli_sha256, buf);

	if(!strncmp(cli_sha256, "C404", 4)) //no exist
	{
		//printf("no file at client!\n");
		return -2;
	}	
	else if(!strncmp(cli_sha256, "C500", 4)) //no file name
	{
		//printf("not sent file name to client!\n");
		return -3;
	}

	strcpy(hash_pwd, srv_path);
	strcat(hash_pwd, "allHashsum.txt");

	FILE* hfile = fopen(hash_pwd, "r");
	if (hfile == NULL) 
	{
		//logging_err(log_info, "[ERROR] - Fail to send open file \'allHashsum.txt\'!\n");
		sprintf(log, "%s || [ERROR] - Fail to send open file \'allHashsum.txt\'!\n", log_info);
		strcpy(arr_log[*logi], log);
		(*logi)++;
		strcpy(arr_log[*logi], "================================================================================================================================\n");
		(*logi)++;
		logging_suc(arr_log, *logi);
		return 1;
	}
	while (fgets(line, sizeof(line), hfile) != NULL) 
	{
		ptr=strtok(line,";");
		if(!strncmp(ptr, fileName, strlen(fileName)))
		{
			ptr=strtok(NULL,"\n");
			strcpy(srv_sha256, ptr);
			break;
		}
	}
	fclose(hfile);

	if(strncmp(cli_sha256,srv_sha256,strlen(srv_sha256))) //different hash
	{
		//printf("send error! - different hashsum!!!\n");
		return -1;
	}
	return 0; //same hash

}

///////////////execute at thread Function////////////////
void *thread_function(void *data)
{
	//struct Info1 cli_info1;
	struct Info2 cli_info2;
	struct ssh_Info ssh_info; //ssh struct
	struct sockaddr_in client_addr;

        int sockfd = *((int *) data);
        int readn;
	int receivedBytes;

        socklen_t addrlen;

        char buf[MAXLINE];
	char cli_cmd[MAXLINE]; //client exe command
	char time_str[20]; //client connect time
	char cli_ip[20]; //client ip
	char fileName[20]; //file name which send to client

	char update_log[MAXLINE];
	char update_cmd[MAXLINE];
	char log_info[128]; //time + cli_ip + cli_hostname
	char log[MAXLINE]; //using for success log
	char arr_log[256][MAXLINE]; //save success log
	int logi = 0; //index of arr_log

	FILE *fp;
	FILE *up_fp;
	   
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
        
	memset(buf, 0x00, MAXLINE);
        addrlen = sizeof(client_addr);
        getpeername(sockfd, (struct sockaddr *) &client_addr, &addrlen);
   
	memset(&cli_info2, 0, sizeof(struct Info2)); //initilaize struct
	memset(&ssh_info, 0, sizeof(struct ssh_Info));  

	sprintf(time_str, "%d-%d-%d.%d:%d:%d", tm.tm_year+1900, tm.tm_mon+1, tm.tm_mday, tm.tm_hour,  tm.tm_min, tm.tm_sec);
	//printf("client connect now : %s\n", time_str);
	//printf("Client IP : [%s]\n", inet_ntoa(client_addr.sin_addr));      //save at log.txt(client ip)
	strcpy(cli_ip, inet_ntoa(client_addr.sin_addr));    //save client ip
	
	strcpy(log_info, time_str);
	strcat(log_info, " ");
	strcat(log_info, cli_ip);

	strcpy(arr_log[logi], "\n================================================================================================================================\n");
	logi++;

	strcpy(log, log_info);
	strcat(log, " || ");
	strcat(log, "[INFO] : Client connect now!\n");
	strcpy(arr_log[logi], log);
	logi++;
	
	// receive client info struct
	receivedBytes = recv(sockfd, (struct Info2*)&cli_info2, sizeof(cli_info2), 0);
	if(receivedBytes <= 0)
	{
		write(sockfd, "E202", 4); //Structure of client information Transfer Error
		
		//strcpy(log_info, get_time(t, tm));
		//logging_err(log_info, "[ERROR] : E202 - Fail to transfer Structure of Client information!\n");
		//logging_err(log_info, "[ERROR] : Client thread end\n");
		strcpy(log_info, get_time(t, tm));
		sprintf(log, "%s || [ERROR] : E202 - Fail to transfer Structure of Client information!\n", log_info);
		strcpy(arr_log[logi], log);
		logi++;
		sprintf(log, "%s || [ERROR] : Client thread end\n", log_info);
		strcpy(arr_log[logi], log);
		logi++;
		//printf("Client thread end\n");
		close(sockfd);
		strcpy(arr_log[logi], "================================================================================================================================\n");
		logi++;
		logging_suc(arr_log, logi); //logging success log to logfile.txt
        	return 0;
	}
	strcat(log_info, " ");
	strcat(log_info, cli_info2.hostname);

	// logging the information of client -> INFO
	sprintf(log, "%s || [INFO] : Client Information\n\n-------------------- client information ------------------------\nfile path : %s\nprotocol : %s\nos : %s\nip : %s\nport : %d\nhostname : %s\nversion : %s\n----------------------------------------------------------------\n\n",log_info, cli_info2.filePath, cli_info2.sendType, cli_info2.os, cli_ip, cli_info2.port, cli_info2.hostname, cli_info2.version);
	
	strcpy(arr_log[logi], log);
	logi++;


	///////////////// receive add information(struct) for send file /////////////////////
	if(!strncmp(cli_info2.sendType,"ssh",4)) //ssh
	{
		//receive ssh struct
		receivedBytes = recv(sockfd, (struct ssh_Info*)&ssh_info, sizeof(ssh_info), 0);
		if(receivedBytes <= 0) //receive error
		{
			write(sockfd, "ES202", 5); ///////write error log & close socket!!!!!!!!!!!!!
			strcpy(log_info, get_time(t, tm));
			strcat(log_info, " ");
			strcat(log_info, cli_info2.hostname);
			sprintf(log, "%s || [ERROR] : E202 - Fail to transfer Structure of Client's SSH information!\n", log_info);
			strcpy(arr_log[logi], log);
			logi++;
			
			sprintf(log, "%s || [ERROR] : Client Thread END\n", log_info);
			strcpy(arr_log[logi], log);
			logi++;
        		//printf("Client thread end\n");
			close(sockfd);
			strcpy(arr_log[logi], "================================================================================================================================\n");
			logi++;
			logging_suc(arr_log, logi); //logging success log to logfile.txt
        		return 0;
		}
  
	}
	/////////////////////////////////////////////////////////////////////////////////////

	//////// if client wants new version -> USER input version "new" ///////////
	if(!strncmp(cli_info2.version,"new",3)) //window, linux
	{
		char new_ver[3][10]={0,};
		char line[255];
		int index = 0;
	
		FILE* file = fopen("new.txt", "r"); // "new.txt" : lastest version list file
		if (file == NULL) 
		{
			strcpy(log_info, get_time(t, tm));
			strcat(log_info, " ");
			strcat(log_info, cli_ip);
			strcat(log_info, " ");
			strcat(log_info, cli_info2.hostname);
			sprintf(log, "%s || [FATAL] : E201 - Fail to open file \'new.txt'\\n", log_info);
			strcpy(arr_log[logi], log);
			logi++;
			sprintf(log, "%s || [ERROR] : Client Thread END\n", log_info);
			strcpy(arr_log[logi], log);
			logi++;
			//printf("Client thread end\n");
			close(sockfd);
			strcpy(arr_log[logi], "================================================================================================================================\n");
			logi++;
			logging_suc(arr_log, logi); //logging success log to logfile.txt
			return 0;
		}
      
		while (fgets(line, sizeof(line), file) != NULL) 
		{
			line[strlen(line)-1]='\0';
			strcpy(new_ver[index],line);
			index++;       
		}
		fclose(file);

		if(!strncmp(cli_info2.os,"Windows",7))
		{
			strcpy(cli_info2.version, new_ver[0]);
		}
		else if(!strncmp(cli_info2.os,"Linux",5))
		{
			strcpy(cli_info2.version, new_ver[1]);
		}
		//printf("change cli_info2.version(new) : %s\n",cli_info2.version);
		strcpy(log_info, get_time(t, tm));
		strcat(log_info, " ");
		strcat(log_info, cli_ip);
		strcat(log_info, " ");
		strcat(log_info, cli_info2.hostname);
		sprintf(log, "%s || [DEBUG] : change cli_info2.version(new) to %s\n",log_info, cli_info2.version);
		strcpy(arr_log[logi], log);
		logi++;

		write(sockfd, cli_info2.version, strlen(cli_info2.version));
		memset(buf, 0x00, MAXLINE);
		read(sockfd,buf,MAXLINE); // "VN777" message from client
	}
	////////////////////////////////////////////////////////////////////////////

	strcat(cli_info2.filePath, cli_info2.version);
	strcat(cli_info2.filePath, "/");

	///////// is there version file which client wants? //////// + log for error code(no_version)
	char srv_path[MAXLINE]; //file place
	strcpy(srv_path, srv_localpath);
	strcat(srv_path, cli_info2.os); // Linux or Windows
	strcat(srv_path, "/");
	strcat(srv_path, cli_info2.version);
	strcat(srv_path, "/");

	strcpy(log_info, get_time(t, tm));
	strcat(log_info, " ");
	strcat(log_info, cli_ip);
	strcat(log_info, " ");
	strcat(log_info, cli_info2.hostname);
	sprintf(log, "%s || [DEBUG] : srv_path = %s\n", log_info, srv_path);
	strcpy(arr_log[logi], log);
	logi++;


	if(access(srv_path,F_OK) < 0) //if no version? thread exit
	{
		write(sockfd, "V404", 4); //error - no version at server repository
		strcpy(log_info, get_time(t, tm));
		strcat(log_info, " ");
		strcat(log_info, cli_ip);
		strcat(log_info, " ");
		strcat(log_info, cli_info2.hostname);
		sprintf(log, "%s || [ERROR] : V404 - No version at Server repository\n", log_info);
		strcpy(arr_log[logi], log);
		logi++;
		sprintf(log, "%s || [ERROR] : Client Thread END\n", log_info);
		strcpy(arr_log[logi], log);
		logi++;
		close(sockfd);
		//printf("Client thread end\n");
		strcpy(arr_log[logi], "================================================================================================================================\n");
		logi++;
		logging_suc(arr_log, logi); //logging success log to logfile.txt
		return 0;
	}
	///////////////////////////////////////////////////////////////



	////////////// Get file list of directory to send ////////////////////
	DIR *dir;
	struct dirent *ent;
	char file_list[MAXLINE][MAXLINE]; //file list of to sending
	int list_index = 0;

	dir = opendir(srv_path);
	if (dir != NULL) 
	{
		strcpy(log_info, get_time(t, tm));
		strcat(log_info, " ");
		strcat(log_info, cli_ip);
		strcat(log_info, " ");
		strcat(log_info, cli_info2.hostname);
		strcpy(log, log_info);
		strcat(log, " || [DEBUG] : File list to transfer\n");
		/* print all the files and directories within directory */
		while ((ent = readdir (dir)) != NULL) 
		{
			if(!strcmp(ent->d_name,".") || !strcmp(ent->d_name, "..") || !strncmp(ent->d_name, "allHashsum.txt", 14))
				continue;
			strcpy(file_list[list_index], ent->d_name);
			strcat(log, file_list[list_index]);
			strcat(log, "\n");
			list_index++;
		}
		closedir(dir);
		strcpy(arr_log[logi], log);
		logi++;
	} 
	else // could not open directory
	{
		strcpy(log_info, get_time(t, tm));
		strcat(log_info, " ");
		strcat(log_info, cli_ip);
		strcat(log_info, " ");
		strcat(log_info, cli_info2.hostname);
		sprintf(log, "%s || [ERROR] : Fail to get file list from Server repository!\n", log_info);
		strcpy(arr_log[logi], log);
		logi++;
		sprintf(log, "%s || [ERROR] : Client Thread END\n", log_info);
		strcpy(arr_log[logi], log);
		logi++;
		close(sockfd);
		//printf("Client thread end\n");
		strcpy(arr_log[logi], "================================================================================================================================\n");
		logi++;
		logging_suc(arr_log, logi); //logging success log to logfile.txt
		close(sockfd);
		return -1;
	}
	//////////////////////////////////////////////////////////////////////


	/////////////////// check allHashsum.txt ///////////////////
	char checkHash[MAXLINE];
	strcpy(checkHash, srv_path);
	strcat(checkHash, "allHashsum.txt");
	
	//critical section
	pthread_mutex_lock(&mutex_lock);
	if(access(checkHash,F_OK) < 0) //if no allHashsum.txt? execute hash_srv.py
	{
		int hh;
		char hash_cmd[MAXLINE];
		for(hh=0;hh<list_index;hh++)
		{
			strcpy(hash_cmd, "./hash_srv ");
			strcat(hash_cmd, srv_path);
			strcat(hash_cmd, " ");
			strcat(hash_cmd, file_list[hh]);
			system(hash_cmd);
		}
	}
	pthread_mutex_unlock(&mutex_lock);
	//end
	/////////////////////////////////////////////////////////////


	/////////////////// start to send files(command) by protocol /////////////////////////////
	write(sockfd, "V777", 4); //exist version at server repository
	memset(buf,0x00,MAXLINE);
	read(sockfd, buf, MAXLINE);
	// set the file name to send & send file by protocol
	int f;
	for(f=0; f<list_index; f++)
	{
		strcpy(fileName, file_list[f]); // directory name which send to client(list_index)
		if(!strncmp(fileName, "allHashsum.txt", 14))
			continue;
		strcpy(log_info, get_time(t, tm));
		strcat(log_info, " ");
		strcat(log_info, cli_ip);
		strcat(log_info, " ");
		strcat(log_info, cli_info2.hostname);
		int hash_res = hash_srv(sockfd, fileName, srv_path, buf, log_info, arr_log, log, &logi);
		if(hash_res==0) //write, read
		{
			strcpy(log_info, get_time(t, tm));
			strcat(log_info, " ");
			strcat(log_info, cli_ip);
			strcat(log_info, " ");
			strcat(log_info, cli_info2.hostname);
			sprintf(log, "%s || [DEBUG] : Right hashsum with client - %s\n", log_info, fileName);
			strcpy(arr_log[logi], log);
			logi++;
			write(sockfd,"HS777", 5); // right hash from client
			memset(buf, 0x00, MAXLINE);
			read(sockfd,buf,MAXLINE); // "HS777" from client
			continue;
		}
		else if(hash_res == -3)
		{
			f--;
			continue;
		}
		else
		{
			if(hash_res == -1)
			{
				strcpy(log_info, get_time(t, tm));
				strcat(log_info, " ");
				strcat(log_info, cli_ip);
				strcat(log_info, " ");
				strcat(log_info, cli_info2.hostname);
				sprintf(log, "%s || [WARNING] : Wrong hashsum with client - %s\n", log_info, fileName);
				strcpy(arr_log[logi], log);
				logi++;
				write(sockfd,"HS100", 5); //wrong hash from client
				memset(buf, 0x00, MAXLINE);
				read(sockfd,buf,MAXLINE); // "HS100" from client
			}
			else if(hash_res == -2) //no file at client
			{
				strcpy(log_info, get_time(t, tm));
				strcat(log_info, " ");
				strcat(log_info, cli_ip);
				strcat(log_info, " ");
				strcat(log_info, cli_info2.hostname);
				sprintf(log, "%s || [WARNING] : No file at client - %s\n", log_info, fileName);
				strcpy(arr_log[logi], log);
				logi++;
				write(sockfd,"HS404", 5); //wrong hash from client
				memset(buf, 0x00, MAXLINE);
				read(sockfd,buf,MAXLINE); // "HS404" from client
			}
		}	
		strcpy(log_info, get_time(t, tm));
		strcat(log_info, " ");
		strcat(log_info, cli_ip);
		strcat(log_info, " ");
		strcat(log_info, cli_info2.hostname);
		if(!strncmp(cli_info2.sendType,"https",5))
		{
			send_https(sockfd, cli_cmd, cli_info2, buf, fileName, log, log_info, arr_log, &logi);
		}

		else if(!strncmp(cli_info2.sendType,"ssh",3))
		{
			send_ssh(sockfd, cli_info2, buf, ssh_info, fileName, cli_ip, srv_path, log, log_info, arr_log, &logi);
		}
		else if(!strncmp(cli_info2.sendType,"websocket",9))
		{
			send_websocket(sockfd, cli_cmd, cli_info2, buf, fileName, log, log_info, arr_log, &logi);
		}
		
		hash_res = hash_srv(sockfd, fileName, srv_path, buf, log_info, arr_log, log, &logi);
		if(hash_res == -1) //-1 -> different hash, 0 -> same hash
		{
			f--;
			sprintf(log, "%s || [WARNING] : Wrong hashsum with client - %s\n", log_info, fileName);
			strcpy(arr_log[logi], log);
			logi++;
			write(sockfd,"HS100", 5); // right hash from client
			memset(buf, 0x00, MAXLINE);
			read(sockfd,buf,MAXLINE); // "HS777" from client
		}
		else if(hash_res == 0)
		{
			sprintf(log, "%s || [DEBUG] : Right hashsum with client - %s\n", log_info, fileName);
			strcpy(arr_log[logi], log);
			logi++;
			write(sockfd,"HS777", 5); // right hash from client
			memset(buf, 0x00, MAXLINE);
			read(sockfd,buf,MAXLINE); // "HS777" from client
		}
		else //probably protocol error -> Transmission impossible
		{
			strcpy(log_info, get_time(t, tm));
			strcat(log_info, " ");
			strcat(log_info, cli_ip);
			strcat(log_info, " ");
			strcat(log_info, cli_info2.hostname);
			sprintf(log, "%s || [WARNING] : Wrong hashsum with client - %s\n", log_info, fileName);
			strcpy(arr_log[logi], log);
			logi++;

			close(sockfd); //close socket
			strcpy(log_info, get_time(t, tm));
			strcat(log_info, " ");
			strcat(log_info, cli_ip);
			strcat(log_info, " ");
			strcat(log_info, cli_info2.hostname);
			sprintf(log, "%s || [ERROR] : Client Thread END\n", log_info);
			strcpy(arr_log[logi], log);
			logi++;

			strcpy(arr_log[logi], "================================================================================================================================\n");
			logi++;

			logging_suc(arr_log, logi); //logging success log to logfile.txt
        		return -1;
		}
	}
	// end of file to send
	write(sockfd, "FE100", 5); //End of file list to transfer
	strcpy(log_info, get_time(t, tm));
	strcat(log_info, " ");
	strcat(log_info, cli_ip);
	strcat(log_info, " ");
	strcat(log_info, cli_info2.hostname);
	sprintf(log, "%s || [INFO] : S777 - Success to transfer all files to Client\n", log_info);
	strcpy(arr_log[logi], log);
	logi++;

	///////////////////////////////////////////////////////////////////////////////////

	
	//update.txt - [hostname] [ip] [client port] [os] [protocol] [version] (python)
	sprintf(update_log, "%s %s %d %s %s %s\n", cli_info2.hostname, cli_ip, cli_info2.port, cli_info2.os, cli_info2.sendType, cli_info2.version); // save lastest log

	if(access("./update.txt",F_OK) < 0)
	{
		int fd;
		if ( 0 < (fd = creat( "./update.txt", 0644)))
		{
			close(fd);
			strcpy(log_info, get_time(t, tm));
			strcat(log_info, " ");
			strcat(log_info, cli_ip);
			strcat(log_info, " ");
			strcat(log_info, cli_info2.hostname);
			sprintf(log, "%s || [INFO] : Success to create file \'update.txt\'\n", log_info);
			strcpy(arr_log[logi], log);
			logi++;
		}
		else //fail to create file
		{
			strcpy(log_info, get_time(t, tm));
			strcat(log_info, " ");
			strcat(log_info, cli_ip);
			strcat(log_info, " ");
			strcat(log_info, cli_info2.hostname);
			sprintf(log, "%s || [ERROR] : Fail to create file \'update.txt\'\n", log_info);
			strcpy(arr_log[logi], log);
			logi++;
		}
	}

	// critical section
        pthread_mutex_lock(&mutex_lock);
	strcpy(update_cmd, "./update_file ");
	strcat(update_cmd, update_log);
	system(update_cmd); //execute update_file.py
        pthread_mutex_unlock(&mutex_lock);
        // end
	strcpy(log_info, get_time(t, tm));
	strcat(log_info, " ");
	strcat(log_info, cli_ip);
	strcat(log_info, " ");
	strcat(log_info, cli_info2.hostname);
	sprintf(log, "%s || [INFO] : Client Thread END\n", log_info);
	strcpy(arr_log[logi], log);
	logi++;
        close(sockfd);
        //printf("Client thread end\n");

	strcpy(arr_log[logi], "================================================================================================================================\n");
	logi++;

	logging_suc(arr_log, logi); //logging success log to logfile.txt
        return 0;
}

int main(int argc, char **argv)
{
        int listen_fd, client_fd;
        socklen_t addrlen;
        int readn;
        char buf[MAXLINE];
        pthread_t thread_id;

	char def_line[255];
	char *def_ptr;
	char def_arr[5][256];
	int ii;
	char time_str[20];

	time_t t = time(NULL);
	struct tm tm = *localtime(&t);

        sprintf(time_str, "%d-%d-%d.%d:%d:%d", tm.tm_year+1900, tm.tm_mon+1, tm.tm_mday, tm.tm_hour,  tm.tm_min, tm.tm_sec);

	// read define file and setting information for server //
	FILE* def_file = fopen("define.txt", "r"); //fail to open file
	if (def_file == NULL) //fail to open file define.txt
	{
		//printf("fail to open file \'define.txt\'\n");
		logging_err(time_str, "[FATAL] : Fail to start open define.txt!");
		return 1;
	}

	for(ii=0;ii<5;ii++) 
	{
		if(fgets(def_line, sizeof(def_line), def_file) == NULL) //fail to open file
		{
			//printf("read file \'define.txt\' error!\n");
			logging_err(time_str, "[FATAL] : Fail to start Server!");
			return 1;
		}
		def_ptr = strtok(def_line, ";");
		strcpy(def_arr[ii],def_ptr);
	}

	fclose(def_file);

	PORTNUM = atoi(def_arr[0]);
	strcpy(IP, def_arr[1]);
	strcpy(HTTP_URL, def_arr[2]);
	strcpy(WEBSOCKET_URL, def_arr[3]);
	strcpy(srv_localpath, def_arr[4]);

	if(srv_localpath[strlen(srv_localpath)-1] != '/')
	{	
		strcat(srv_localpath, "/"); 
	}
	//////////////////////////////////////////////////////////


        struct sockaddr_in server_addr, client_addr;

        if((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		logging_err(time_str, "[ERROR] : Socket error!\n");
                return 1;
        }

        memset((void *) &server_addr, 0x00, sizeof(server_addr));
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = inet_addr(IP); //IP revise
        server_addr.sin_port = htons(PORTNUM); //Port number revise

        if(bind(listen_fd, (struct sockaddr *) &server_addr,sizeof(server_addr)) == -1)
	{
                //perror("bind error");
		logging_err(time_str, "[ERROR] : Bind error!\n");
                return 1;
        }

	logging_err(time_str, "Main Server Start...\n");

        if(listen(listen_fd, 5) == -1) //listen error
	{
		logging_err(time_str, "[ERROR] : Listen error!\n");
                return 1;
        }

	while(1)
	{
                addrlen = sizeof(client_addr);
                client_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &addrlen);
                if(client_fd == -1) //accept error
		{
			logging_err(time_str, "[ERROR] : Accept error!\n");
		}
                else
            	{
                        pthread_create(&thread_id, NULL, thread_function, (void *) &client_fd);
                        // Main server will continue even after thread isn't finished
                        pthread_detach(thread_id);
                }
        }
        return 0;
}
