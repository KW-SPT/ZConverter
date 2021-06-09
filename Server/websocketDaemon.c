#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/resource.h>
#include <syslog.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <dirent.h>
#include <error.h>
#define MAXLINE 1024

char websocket_path[512];

void logging_err(char *log, char *errmsg)
{
	// logfile.txt - [time] [ip] [hostname] [log level + log content]
	FILE *fp;
	char text[MAXLINE];

	strcpy(text,log);
	strcat(text, " || ");
	strcat(text, errmsg);
	
	fp = fopen("websocket_logfile.txt", "a+");
	if(fp == NULL) //file open error
	{
		//printf("file open error!\n");
		return;
	}
	fputs(text, fp); //save log at logfile.txt
	fclose(fp);
	//end
	return;
}

int main(int argc, char* argv[])
{
   	int status;
      	switch (fork())
      	{
      		case -1: exit(0);
      		case 0:
      		{
         	//sprintf(sMessage, "child pid parent : %d", getpid());
         		break;
      		}
	      	//default: _exit(EXIT_SUCCESS);
      		default: 
         	//sprintf(sMessage, "parent pid parent : %d", getpid());
         		exit(0);
      	}

      	umask(0);
      	if (setsid() == -1) exit(0);
      	signal(SIGHUP, SIG_IGN);
      	signal(SIGCHLD, SIG_IGN);
      	close(0);
      	close(1);
      	close(2);

	openlog("Websocket server start", LOG_CONS, LOG_DAEMON);

	while(1)
	{
		sleep(20);

		char def_line[255];
		char *def_ptr;
		char def_arr[256];

		char time_str[20];
		time_t t = time(NULL);
		struct tm tm = *localtime(&t);

	        sprintf(time_str, "%d-%d-%d.%d:%d:%d", tm.tm_year+1900, tm.tm_mon+1, tm.tm_mday, tm.tm_hour,  tm.tm_min, tm.tm_sec);
		
		FILE* path_file = fopen("wh_path.txt", "r");
		if (path_file == NULL)
		{
			logging_err(time_str, "[FATAL] : Fail to open wh_path.txt!\n");
			return 1;
		}

		if(fgets(def_line, sizeof(def_line), path_file) == NULL) //fail to open file
		{
			logging_err(time_str, "[FATAL] : Fail to read wh_path.txt!\n");
			return 1;
		}
		def_ptr = strtok(def_line, ";");
		strcpy(def_arr, def_ptr);
	
		fclose(path_file);
		
		strcpy(websocket_path, def_arr);
		strcat(websocket_path, "websocket_srv");

		if(fork() == 0)
			execl(websocket_path, "./websocket_srv", "9998", NULL);
	}
	closelog();

   return 0;
}
