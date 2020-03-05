#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <stdbool.h>
#include <time.h>

#define role "client"
#define LENGTH 2048

// Global variables
volatile sig_atomic_t flag = 0;
int sockfd = 0;
char name[32];
char query[1024]="INSERT INTO users values (NULL, \"";
time_t start;
time_t stop;
int diff;

void purify(char *s){
	for(int i=0;i<=255;i++){
		if(s[i]=='\n'){
			s[i]='\0';
			
		}
	}
	
}

char *registration(){
	#define role "client"
	
	char  name[255], username[255], upassword[255];
	bzero(name, 255);bzero(username, 255);bzero(upassword, 255);
	printf("WELCOME TO REGISTRATION\n");
	
	printf("PLEASE ENTER YOUR NAME\n" );
	fgets(name,255, stdin);
	purify(name);

	printf("PLEASE ENTER YOUR USERNAME\n" );
	fgets(username,255, stdin);
	purify(username);
	
	printf("PLEASE ENTER YOUR PASSWORD\n" );
	fgets(upassword,255, stdin);
	purify(upassword);
	
	// 
	// char query[1024]="INSERT INTO users values (NULL, \"";
	
	char querymid[]="\",\"";
	char queryend[]="\")";
	
	strcat(query, name);
	strcat(query, querymid);
	strcat(query, username);
	strcat(query, querymid);
	strcat(query, upassword);
	strcat(query, querymid);
	strcat(query, role);
	strcat(query, queryend);
	

	printf("%s",query);
	return query;
}


void str_overwrite_stdout() {
  printf("%s", "> ");
  fflush(stdout);
}

void str_trim_lf (char* arr, int length) {
  int i;
  for (i = 0; i < length; i++) { // trim \n
    if (arr[i] == '\n') {
      arr[i] = '\0';
      break;
    }
  }
}

void catch_ctrl_c_and_exit(int sig) {
    flag = 1;
}

// void send_msg_handler() {
//   char message[LENGTH] = {};
// 	char buffer[LENGTH + 32] = {};

//   while(1) {
//   	str_overwrite_stdout();
//     fgets(message, LENGTH, stdin);
//     str_trim_lf(message, LENGTH);

//     if (strcmp(message, "exit") == 0) {
// 			break;
//     } else {
//       sprintf(buffer, "%s: %s\n", name, message);
//       send(sockfd, buffer, strlen(buffer), 0);
//     }

// 		bzero(message, LENGTH);
//     bzero(buffer, LENGTH + 32);
//   }
//   catch_ctrl_c_and_exit(2);
// }

// void recv_msg_handler() {
// 	char message[LENGTH] = {};
//   while (1) {
// 		int receive = recv(sockfd, message, LENGTH, 0);
//     if (receive > 0) {
//       printf("%s", message);
//       str_overwrite_stdout();
//     } else if (receive == 0) {
// 			break;
//     } else {
// 			// -1
// 		}
// 		memset(message, 0, sizeof(message));
//   }
// }

int registrationFull(){
	 char *quearyToDb=registration();
	 char *ip = "127.0.0.1";
	int port = atoi("9898");

	signal(SIGINT, catch_ctrl_c_and_exit);

	
	struct sockaddr_in server_addr;


	/* Socket settings */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ip);
    server_addr.sin_port = htons(port);

    int err = connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
  
    if (err == -1) {
		printf("ERROR: connect\n");
		return EXIT_FAILURE;
	}

	// Send query
	send(sockfd, query, sizeof(query), 0);

	close(sockfd);
	return EXIT_SUCCESS;
}
bool auth(){
	char  username[255], upassword[255],auth[255];
	bzero(name, 255);bzero(username, 255);bzero(upassword, 255);
	printf("WELCOME TO AUTHENTICATION\n");
	
	

	printf("PLEASE ENTER YOUR USERNAME\n" );
	fgets(username,255, stdin);
	purify(username);
	
	printf("PLEASE ENTER YOUR PASSWORD\n" );
	fgets(upassword,255, stdin);
	purify(upassword);
	
	

	char *ip = "127.0.0.1";
	int port = atoi("9899");

	signal(SIGINT, catch_ctrl_c_and_exit);

	
	struct sockaddr_in server_addr;


	/* Socket settings */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ip);
    server_addr.sin_port = htons(port);

    int err = connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
  
    if (err == -1) {
		printf("ERROR: connect\n");
		return EXIT_FAILURE;
	}

	char salt[]=" ";

	strcat(username, salt);
	strcat(username,upassword);

	// Send query
	send(sockfd, username, sizeof(username), 0);
	int receive = recv(sockfd, auth, 255, 0);

	if (receive > 0){
			if(strlen(auth) > 0){
				if(strcmp(auth,"A")==0){
					printf("%s\n", "AUTHENTICATION PASSED");
				}else{
					printf("%s\n", "AUTHENTICATION FAILED, PLEASE TRY AGAIN");
				}
			}
		} else {
			printf("ERROR: -1\n");
		
		}

    close(sockfd);
	return EXIT_SUCCESS;


	return true;
}

void project_creation(){
	char  clientName[255], workerName[255], projectName[255];char price[255];
	char status[]="not_accepted";char workedHours[]="0";
	bzero(clientName, 255);bzero(workerName, 255);bzero(projectName, 255);
	char pQuery[1024]="INSERT INTO project values (NULL, \"";

	printf("WELCOME TO PROJECT CREATION\n");
	
	printf("PLEASE ENTER NAME OF PROJECT\n" );
	fgets(projectName,255, stdin);
	purify(projectName);

	printf("PLEASE ENTER YOUR USERNAME\n" );
	fgets(clientName,255, stdin);
	purify(clientName);
	
	printf("PLEASE ENTER WORKERS USERNAME\n" );
	fgets(workerName,255, stdin);
	purify(workerName);

	printf("PLEASE ENTER PRICE PER HOUR\n" );
	fgets(price,255, stdin);
	purify(price);
	
	// 
	
	char querymid[]="\",\"";
	char specialLeftQueryMid[]="\",";
	char specialRightQueryMid[]=",\"";
	char specialFullQueryMid[]=",";
	char specialFullQueryEnd[]=")";
	char queryend[]="\")";

	strcat(pQuery, projectName);
	strcat(pQuery, querymid);
	strcat(pQuery, clientName);
	strcat(pQuery, querymid);
	strcat(pQuery, workerName);
	strcat(pQuery, querymid);
	strcat(pQuery, status);
	strcat(pQuery, specialLeftQueryMid);
	strcat(pQuery, price);
	strcat(pQuery, specialFullQueryMid);
	strcat(pQuery, workedHours);
	strcat(pQuery, specialFullQueryEnd);
	

	printf("%s",pQuery);

	char *ip = "127.0.0.1";
	int port = atoi("9898");

	signal(SIGINT, catch_ctrl_c_and_exit);

	
	struct sockaddr_in server_addr;


	/* Socket settings */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ip);
    server_addr.sin_port = htons(port);

    int err = connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
  
    if (err == -1) {
		printf("ERROR: connect\n");
		
	}

	// Send query
	send(sockfd, pQuery, sizeof(pQuery), 0);

	close(sockfd);
	// return query;
}

void listProjects(){
	char  clientName[255], workerName[255], projectName[255];char price[255];
	char status[]="not_accepted";char workedHours[]="0";char buff_in[1024];
	bzero(clientName, 255);bzero(workerName, 255);bzero(projectName, 255);bzero(buff_in, 1024);
	char pQuery[1024]="SELECT * FROM project where client_name=\"";

	
	printf("LISTING PROJECTS\n");
	

	printf("PLEASE ENTER YOUR USERNAME\n" );
	fgets(clientName,255, stdin);
	purify(clientName);
	
	
	// 
	
	char querymid[]="\",\"";
	char specialLeftQueryMid[]="\",";
	char specialRightQueryMid[]=",\"";
	char specialFullQueryMid[]=",";
	char specialFullQueryEnd[]=")";
	char queryend[]="\";";

	
	strcat(pQuery, clientName);
	strcat(pQuery, queryend);
	

	printf("%s\n",pQuery);

	char *ip = "127.0.0.1";
	int port = atoi("9999");

	signal(SIGINT, catch_ctrl_c_and_exit);

	
	struct sockaddr_in server_addr;


	/* Socket settings */
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ip);
    server_addr.sin_port = htons(port);

    int err = connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
  
    if (err == -1) {
		printf("ERROR: connect\n");
		
	}

	// Send query
	send(sockfd, pQuery, sizeof(pQuery), 0);
	int i=0;
	while(1){
		int receive = recv(sockfd, buff_in, 1024, 0);
		if (receive > 0){
			printf("%s\n", buff_in);
			if(strcmp(buff_in,"ItIsEnd")==0){
				break;
			}
			
		} else {
			printf("ERROR: -1\n");
		
		}
		
		i++;
		bzero(buff_in,sizeof(buff_in));
	}
    close(sockfd);



	// close(sockfd);
	// return query;
}
void startTime(){
	start=time(0);
}

void stopTime(){
	stop=time(0);

}
void calculate(){
	diff=stop-start;
}
void working(){
	char ans[255];char mk[]="mkdir PROJECTS1/";
	time_t base=time(0);
	time_t now=base;


  	startTime();
  	
  	printf("WHat project do you want ot work on?\n" );
	fgets(ans,255, stdin);
	purify(ans);
	strcat(mk,ans);
  	system(mk);

	while(1){
		if(now-base>3){
			system("scrot -b -d 5 '%Y:%m:%d:%H:%M:%S.png' -e 'mv $f ~/DWM/CLIENT/PROJECTS/'");
			base=now;
		}else{
			now=time(0);
		}
    }

    stopTime();
    
    calculate();

    printf("%d\n",diff );
}
int main(){

	
	// registrationFull();
	
	 // auth();
	
	// project_creation();

	listProjects();
	
	// working();

	return 0;
	
	
}