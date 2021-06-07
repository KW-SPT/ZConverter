//////////////////////////////////////////////////////////////////////////////////////////////////////////
// 
    /*-----------------execution format => ./client IP version protocol save_path port--------------------*/

//////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <sys/socket.h>  
#include <arpa/inet.h>   
#include <sys/stat.h>
#include <stdio.h>     
#include <string.h>     
#include <unistd.h>     
#include <stdio.h>
#include <stdlib.h>
#include <pwd.h>
#include <sys/types.h>
#include <dirent.h>
#include <time.h>

#define MAXLINE    1024
//#define PORT 3600


///////////// client information struct ///////////////

struct Info2{
   char version[10];
   char filePath[50];
   char sendType[20];
   char os[20];
   int port;
   char hostname[256];
};

///////////////////////////////////////////////////////


//////// ssh additional information struct ///////////
struct ssh_Info{

   char username[MAXLINE];
   char password[MAXLINE];
   int ssh_port;
};
///////////////////////////////////////////////////////

char *get_time(time_t t, struct tm tm)
{
	static char time_str[20];
	
	t = time(NULL);
	tm = *localtime(&t);
	sprintf(time_str, "%d-%d-%d.%d:%d:%d", tm.tm_year+1900, tm.tm_mon+1, tm.tm_mday, tm.tm_hour,  tm.tm_min, tm.tm_sec);

	return time_str;
}
//////////////////////////////////////////////////////////
//    		 hashsum check function       		//
//////////////////////////////////////////////////////////
int hash_cli(int server_sockfd, struct Info2 info2, char *log, FILE *fp, time_t t, struct tm tm)
{
	char fileName[20]="";
	char filepwd[100]="";
	char hash_cmd[1024];
	char cli_sha256[1024]="";
	int res=0;
	if(read(server_sockfd, fileName, MAXLINE)==-1) //Fail to receive file name from server
	{
		strcpy(log, get_time(t, tm));
		strcat(log, " || ");
		strcat(log, "[ERROR] : Read error!\n");
		fputs(log, fp);
	}

	if(!strncmp(fileName,"FE100", 5)) //return -1 (end of file from server)
		return -1;
	strcpy(log, get_time(t, tm));
	strcat(log, " || ");
	strcat(log, "[DEBUG] : File name to check hashsum = ");
	strcat(log, fileName);
	strcat(log, "\n");
	fputs(log, fp);
  
	strcpy(filepwd, info2.filePath);
	strcat(filepwd, "/");
	strcat(filepwd, fileName); 
	
	strcpy(log, get_time(t, tm));
	strcat(log, " || ");
	strcat(log, "[DEBUG] : File path to check hashsum = ");
	strcat(log, filepwd);
	strcat(log, "\n");
	fputs(log, fp);
	
	if(access(filepwd,0)==-1) //No file at client repository
	{
		strcpy(cli_sha256, "C404");
		res=-2;
	}
	else if(strlen(fileName)==0) //Not send the file name from server
	{
		strcpy(cli_sha256, "C500");
		res=-3;
	}
	else //return 0
	{
		strcpy(hash_cmd, "sudo chmod 755 ");
		strcat(hash_cmd, filepwd);
		system(hash_cmd); //user must enter linux's password!

		strcpy(hash_cmd,"./hash ");
		strcat(hash_cmd, filepwd);
		system(hash_cmd); //execute python hash program - result at hash.txt

		//hash.txt open and store value of hashsum
		FILE* hfile = fopen("hash.txt", "r");
		if (hfile == NULL) //open file error
		{
			//printf("fail to open file hash.txt!\n");
			strcpy(log, get_time(t, tm));
			strcat(log, " || ");
			strcat(log, "[ERROR] : Fail to open file \'hash.txt\'\n");
			fputs(log, fp);
			return 1;
		}

		fgets(cli_sha256, sizeof(cli_sha256), hfile);
		fclose(hfile);

		//remove hash.txt
		int nResult = remove("hash.txt");

		if( nResult == 0 )
		{
			//printf( "success to remove hash.txt\n" );
			strcpy(log, get_time(t, tm));
			strcat(log, " || ");
			strcat(log, "[INFO] : Success to remove file \'hash.txt\'!\n");
			fputs(log, fp);
			
		}
		else if( nResult == -1 )
		{
			//perror( "fail to remove hash.txt\n" );
			strcpy(log, get_time(t, tm));
			strcat(log, " || ");
			strcat(log, "[WARNING] : Fail to remove file \'hash.txt\'!\n");
			fputs(log, fp);
		}
	}

	write(server_sockfd,cli_sha256,MAXLINE); //Send hashsum to server (sha256) 
	return res;

}
/////////////////////////////////////////////////////////////////////////

/////////////////////// main //////////////////////////
int main(int argc, char **argv)
{
	struct Info2 info2;
	char IP[20]="";
	int PORT=3600;
	int nPath;
	char host_name[256];

	char log[MAXLINE]; //using for success log
	time_t t;
	struct tm tm;

	struct sockaddr_in serveraddr;
	int server_sockfd;
	int client_len;
	char buf[MAXLINE];
	char fileName[20]="";
	char filepwd[100]="";

	char opt_line[255];
	char *opt_ptr;
	char opt_arr[10][256];
	int ii;

	FILE *fp;

	fp = fopen("logfile.txt", "a+");
	if(fp == NULL) //file open error
	{
		return -1;
	}

	fputs("\n============================================================================================================\n", fp);
	// Input the information of Client
	if(gethostname(host_name, sizeof(host_name)) == 1) //get hostname error
	{
		strcpy(log, get_time(t, tm));
		strcat(log, " || ");
		strcat(log, "[ERROR] : gethostname error\n");
		fputs(log, fp);
		fputs("============================================================================================================\n", fp);
		fclose(fp);
		return -1;
	}

	strcpy(info2.hostname, host_name); //save hostname at Info2
	strcpy(info2.os,"Linux"); //Window or Linux

	// read define file and setting information for server //
	FILE* opt_file = fopen("cli_option.txt", "r"); //fail to open file
	if (opt_file == NULL) //fail to open file define.txt
	{
		//printf("fail to open file \'cli_option.txt\'\n");
		strcpy(log, get_time(t, tm));
		strcat(log, " || ");
		strcat(log, "[ERROR] : fail to open file \'cli_option.txt\\n");
		fputs(log, fp);
		fputs("============================================================================================================\n", fp);
		fclose(fp);
		return 1;
	}

	for(ii=0;ii<6;ii++) 
	{
		if(fgets(opt_line, sizeof(opt_line), opt_file) == NULL) //fail to open file
		{
			//printf("read file \'define.txt\' error!\n");
			strcpy(log, get_time(t, tm));
			strcat(log, " || ");
			strcat(log, "[FATAL] : Fail to start Server!");
			fputs(log,fp);
			fputs("============================================================================================================\n", fp);
			fclose(fp);
			return 1;
		}
		opt_ptr = strtok(opt_line, "\n");
		strcpy(opt_arr[ii],opt_ptr);
	}

	fclose(opt_file);

	strcpy(IP,opt_arr[0]);
	PORT = atoi(opt_arr[1]);
	strcpy(info2.version,opt_arr[2]);
	strcpy(info2.sendType,opt_arr[3]);
	strcpy(info2.filePath,opt_arr[4]);
	info2.port=atoi(opt_arr[5]);

	//////////////////////////////////////////////////////////
	
	if(info2.filePath[strlen(info2.filePath)-1] != '/')
	{	
		strcat(info2.filePath, "/"); 
	}

	//npath -> Determine the existence of the client's path
	nPath=access(info2.filePath,0); // 0 = exist, -1 = not exist
	if(nPath==-1) //path error
	{
		strcpy(log, get_time(t, tm));
		strcat(log, " || ");
		strcat(log, "[ERROR] : The path entered doesn't exist!\n");
		fputs(log, fp);
		fputs("============================================================================================================\n", fp);
		fclose(fp);
		return -1;
	}
	if ((server_sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) //socket error
	{
		strcpy(log, get_time(t, tm));
		strcat(log, " || ");
		strcat(log, "[ERROR] : Socket error\n");
		fputs(log, fp);
		fputs("============================================================================================================\n", fp);
		fclose(fp);
		return 1;
	}

       /* Specify the address and port number protocol of the server to request a connection */
       server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
       serveraddr.sin_family = AF_INET;
       serveraddr.sin_addr.s_addr = inet_addr(IP);
       serveraddr.sin_port = htons(PORT);

       client_len = sizeof(serveraddr);

       /* Try to connect to the server */
       if (connect(server_sockfd, (struct sockaddr *)&serveraddr, client_len)  == -1) //connect error
       {
		strcpy(log, get_time(t, tm));
		strcat(log, " || ");
		strcat(log, "[ERROR] : Connect error!\n");
		fputs(log, fp);
		fputs("============================================================================================================\n", fp);
		fclose(fp);
		return 1;
       }

	//printf("\nConnected to Server %s!\n", IP);
	strcpy(log, get_time(t, tm));
	strcat(log, " || ");
	strcat(log, "[INFO] : Connect to Server ");
	strcat(log, IP);
	strcat(log, "\n");
	fputs(log, fp);

	///////////////////////////////////////////////////////////////////////   
	//             Send basic information of client                      //
	///////////////////////////////////////////////////////////////////////   
          
	if (send(server_sockfd, (struct Info2*)&info2, sizeof(info2), 0) == -1) //client information struct
	{
		strcpy(log, get_time(t, tm));
		strcat(log, " || ");
		strcat(log, "[ERROR] : Send Client Information Struct error\n");
		fputs(log, fp);
		fputs("============================================================================================================\n", fp);
		fclose(fp);
		return 1;
	}

	//////////////////////////// Send additional information by protocol //////////////////////////////
	if(!strncmp(info2.sendType, "ssh", 3)) //ssh
	{
		struct ssh_Info ssh_info;
		memset(&ssh_info, 0, sizeof(struct ssh_Info)); //Initialize struct of ssh

		char ssh_line[255];
		char *ssh_ptr;
		char ssh_arr[3][128];
		int sii;
	
		FILE* ssh_file = fopen("ssh_info.txt", "r");
		if (ssh_file == NULL) //fail to open file
		{
			strcpy(log, get_time(t, tm));
			strcat(log, " || ");
			strcat(log, "[ERROR] : Fail to open file \'ssh_info.txt\'\n");
			fputs(log, fp);
			fputs("============================================================================================================\n", fp);
			fclose(fp);
			return 1;
		}
	
		for(sii=0;sii<3;sii++) 
		{
			if(fgets(ssh_line, sizeof(ssh_line), ssh_file) == NULL) //read file 'ssh_info.txt' error
			{
				strcpy(log, get_time(t, tm));
				strcat(log, " || ");
				strcat(log, "[ERROR] : Fail to read file \'ssh_info.txt\'\n");
				fputs(log, fp);
				fputs("============================================================================================================\n", fp);
				fclose(fp);
				return -1;
			}
			ssh_ptr = strtok(ssh_line, ";");
			strcpy(ssh_arr[sii],ssh_ptr);
		}

		fclose(ssh_file);

		strcpy(ssh_info.username, ssh_arr[0]);
		strcpy(ssh_info.password, ssh_arr[1]);
		ssh_info.ssh_port = atoi(ssh_arr[2]);

		if(send(server_sockfd, (struct ssh_Info*)&ssh_info, sizeof(ssh_info), 0) == -1) //ssh struct send error
		{
			strcpy(log, get_time(t, tm));
			strcat(log, " || ");
			strcat(log, "[ERROR] : Send SSH Information Struct error\n");
			fputs(log, fp);
			fputs("============================================================================================================\n", fp);
			fclose(fp);
			return 1;
		}   
	}
	
	//////////////////////////////////////////////////////////////////////////////////////////////////////////


	// new version change to number version //
	if(!strncmp(info2.version, "new", 3))
	{
		memset(buf, 0x00, MAXLINE);
		read(server_sockfd, buf, MAXLINE);
		strcpy(info2.version,buf);
		write(server_sockfd, "VN777", 5); //success receive new version from server
		strcpy(log, get_time(t, tm));
		strcat(log, " || ");
		strcat(log, "[INFO] : Success receive new version = ");
		strcat(log, info2.version);
		strcat(log, "\n");
		fputs(log, fp);
	}
	//////////////////////////////////////////

	// make directoy -> filePath = filePath + version //
	strcat(info2.filePath, info2.version);
	if(mkdir(info2.filePath, 0777)==-1) //mkdir error
	{	
		strcpy(log, get_time(t, tm));
		strcat(log, " || ");
		strcat(log, "[WARNING] : Fail to make directory at store path!\n");
		fputs(log, fp);
		
	}
	else //success mkdir
	{
		strcpy(log, get_time(t, tm));
		strcat(log, " || ");
		strcat(log, "[INFO] : Make directory at store path -> dir_name = ");
		strcat(log, info2.version);
		strcat(log, "\n");
		fputs(log, fp);
	}
	char mm[1024]="";
	strcpy(mm, "chmod 777 ");
	strcat(mm, info2.filePath);
	system(mm);
	////////////////////////////////////////////////////


	////////////////////////// start to receive file ////////////////////////
	memset(buf, 0x00, MAXLINE);
	read(server_sockfd,buf,MAXLINE); //receive msg from server - already use / no version / start sending
	write(server_sockfd,"V423",4);

	if(!strncmp(buf, "V404", 4)) //no version at Server
	{
		strcpy(log, get_time(t, tm));
		strcat(log, " || ");
		strcat(log, "[ERROR] : No version at Server repository!\n");
		fputs(log, fp);
		close(server_sockfd);
		strcpy(log, get_time(t, tm));
		strcat(log, " || ");
		strcat(log, "[ERROR] : Close Socket\n");
		fputs(log, fp);
		fputs("============================================================================================================\n", fp);
		fclose(fp);
		return 1;
	}
	else if(!strncmp(buf, "V777", 4)) 
	{
						
		while(1)
		{
			int hash_res = hash_cli(server_sockfd, info2, log, fp, t, tm);
			if(hash_res==-1) //End of file
				break;
			else if(hash_res==-3) //Not receive file name from server
				continue;
			memset(buf, 0x00, MAXLINE);
			read(server_sockfd, buf, MAXLINE); // right or wrong

			if(!strncmp(buf,"HS777",5)) //right hash
			{
				write(server_sockfd,"HS777",5);
				strcpy(log, get_time(t, tm));
				strcat(log, " || ");
				strcat(log, "[DEBUG] : Right hashsum\n");
				fputs(log, fp);
				continue;
			}
			else if(!strncmp(buf,"HS100",5)) //wrong hash
			{
				write(server_sockfd,"HS100",strlen("HS100"));
				strcpy(log, get_time(t, tm));
				strcat(log, " || ");
				strcat(log, "[DEBUG] : Wrong hashsum\n");
				fputs(log, fp);
			}
			else if(!strncmp(buf,"HS404",5)) //no file at client
			{
				write(server_sockfd,"HS404",strlen("HS404"));
				strcpy(log, get_time(t, tm));
				strcat(log, " || ");
				strcat(log, "[DEBUG] : No file at client\n");
				fputs(log, fp);
			}
			memset(buf, 0x00, MAXLINE);
			read(server_sockfd, buf, MAXLINE); 
			if(!strncmp(buf, "PS001", 5)) //ssh protocol
			{
				write(server_sockfd, "PS001", 5); //success code using ssh
				hash_cli(server_sockfd, info2, log, fp, t, tm); //call hash function	
			}
			else // http or websocket protocol
			{
				system(buf); //python execute file running
				write(server_sockfd, "C777", 4); //success code using https, websocket	
				hash_cli(server_sockfd, info2, log, fp, t, tm ); //call hash function
			}
			memset(buf, 0x00, MAXLINE);
			read(server_sockfd, buf, MAXLINE); // right or wrong

			if(!strncmp(buf,"HS777",5)) //right hash
			{
				write(server_sockfd,"HS777",5);
				strcpy(log, get_time(t, tm));
				strcat(log, " || ");
				strcat(log, "[DEBUG] : Right hashsum\n");
				fputs(log, fp);
				continue;
			}
			else if(!strncmp(buf,"HS100",5)) //wrong hash
			{
				write(server_sockfd,"HS100",strlen("HS100"));
				strcpy(log, get_time(t, tm));
				strcat(log, " || ");
				strcat(log, "[DEBUG] : Wrong hashsum\n");
				fputs(log, fp);
			}
		}

		close(server_sockfd);
		strcpy(log, get_time(t, tm));
		strcat(log, " || ");
		strcat(log, "[INFO] :Close Socket\n");
		fputs(log, fp);
		fputs("============================================================================================================\n", fp);
		fclose(fp);
		return 0;
	}
	else //E202, ES202 -> send error
	{
		strcpy(log, get_time(t, tm));
		strcat(log, " || ");
		strcat(log, "[ERROR] : Send error!\n");
		fputs(log, fp);
		close(server_sockfd);
		strcpy(log, get_time(t, tm));
		strcat(log, " || ");
		strcat(log, "[INFO] :Close Socket\n");
		fputs(log, fp);
		fputs("============================================================================================================\n", fp);
		fclose(fp);
		return 0;
	}
}   
