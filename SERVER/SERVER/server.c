#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <mysql/mysql.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <sys/types.h>
#include <signal.h>


#define MAX_CLIENTS 100
#define BUFFER_SZ 512

static _Atomic unsigned int cli_count = 0;
static int uid = 10;
MYSQL *conn;
MYSQL_RES *res;
MYSQL_ROW row;
char *server="localhost";
char *user="root";
char *password="";
char *database="dwm_db";



/* Client structure */
typedef struct{
	struct sockaddr_in address;
	int sockfd;
	int uid;
} client_t;

client_t *clients[MAX_CLIENTS];

pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

void str_overwrite_stdout() {
    printf("\r%s", "> ");
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


void writeToDb(char *query){
	int myflag=0;
	 conn = mysql_init(NULL);
	 if (!mysql_real_connect(conn, server, user, password, database, 0, NULL, 0)) {
	 	fprintf(stderr, "%s\n", mysql_error(conn));
	 	exit(1);
	 }	
	
	 if (mysql_query(conn, query)) {
		fprintf(stderr, "%s\n", mysql_error(conn));
	 	myflag=1;
	 }


	 if(myflag==0){
	res = mysql_use_result(conn);
	

	// printf("MySQL Tables in mysql database:\n");
	// // while ((row = mysql_fetch_row(res)) != NULL){
	// // 	printf("%s \n", row[0]);
	// // }
	
	mysql_free_result(res);
	mysql_close(conn);
	}
}
/* Add clients to queue */
void queue_add(client_t *cl){
	pthread_mutex_lock(&clients_mutex);

	for(int i=0; i < MAX_CLIENTS; ++i){
		if(!clients[i]){
			clients[i] = cl;
			break;
		}
	}

	pthread_mutex_unlock(&clients_mutex);
}

/* Remove clients to queue */
void queue_remove(int uid){
	pthread_mutex_lock(&clients_mutex);

	for(int i=0; i < MAX_CLIENTS; ++i){
		if(clients[i]){
			if(clients[i]->uid == uid){
				clients[i] = NULL;
				break;
			}
		}
	}

	pthread_mutex_unlock(&clients_mutex);
}

/* Send message to all clients except sender */
void send_message(char *s, int uid){
	pthread_mutex_lock(&clients_mutex);

	for(int i=0; i<MAX_CLIENTS; ++i){
		if(clients[i]){
			if(clients[i]->uid != uid){
				if(write(clients[i]->sockfd, s, strlen(s)) < 0){
					perror("ERROR: write to descriptor failed");
					break;
				}
			}
		}
	}

	pthread_mutex_unlock(&clients_mutex);
}

/* Handle all communication with the client */
void *handle_client(void *arg){

	char buff_out[512];
	int leave_flag = 0;

	cli_count++;
	client_t *cli = (client_t *)arg;

	// Name
	
	bzero(buff_out, 512);

	while(1){
		if (leave_flag) {
			break;
		}

		int receive = recv(cli->sockfd, buff_out, 512, 0);
		if (receive > 0){
			if(strlen(buff_out) > 0){
				// send_message(buff_out, cli->uid);

				str_trim_lf(buff_out, strlen(buff_out));
				printf("%s\n", buff_out);
				writeToDb(buff_out);
			}
		} else if (receive == 0 || strcmp(buff_out, "exit") == 0){
			printf("%s", buff_out);
			// send_message(buff_out, cli->uid);
			leave_flag = 1;
		} else {
			printf("ERROR: -1\n");
			leave_flag = 1;
		}

		bzero(buff_out, BUFFER_SZ);
	}

  /* Delete client from queue and yield thread */
	close(cli->sockfd);
  queue_remove(cli->uid);
  free(cli);
  cli_count--;
  pthread_detach(pthread_self());

	return NULL;
}

bool check_db(char *username, char *apassword){
	 bool answer=false; 
	 conn = mysql_init(NULL);
	 if (!mysql_real_connect(conn, server, user, password, database, 0, NULL, 0)) {
	 	fprintf(stderr, "%s\n", mysql_error(conn));
	 	exit(1);
	 }	
	 char query[]="Select password from users where username=\"";
	 char endquery[]="\"";
	
	 strcat(query,username);
	 strcat(query,endquery);
	 printf("%s\n",query );
	 if (mysql_query(conn, query)) {
		fprintf(stderr, "%s\n", mysql_error(conn));
	 	exit(1);
	 }


	res = mysql_use_result(conn);
	
	char name[255]="abdumalik";
	char surname[110]="abdumalik";
	
	row = mysql_fetch_row(res);
	
	if(row!=NULL){
		int isSame=strcmp(apassword,row[0]);
		printf("%d\n",isSame );
		if(isSame==0){
			answer=true;
		}else{
			answer=false;
		}	
	}else{
		answer=false;
	}
	
	mysql_free_result(res);
	mysql_close(conn);

	return answer;

}



void *auth_client(void *arg){

	char username[512];
	char apassword[512];

	char *authAns;

	char buff_out[BUFFER_SZ];
	int leave_flag = 0;

	cli_count++;
	client_t *cli = (client_t *)arg;

	// Name
	
	bzero(buff_out, BUFFER_SZ);

	while(1){
		if (leave_flag) {
			break;
		}

		// clearing here

		bzero(username, sizeof(username));
		bzero(apassword, sizeof(apassword));



		int receive = recv(cli->sockfd, buff_out, BUFFER_SZ, 0);
		if (receive > 0){
			if(strlen(buff_out) > 0){
				// send_message(buff_out, cli->uid);

				str_trim_lf(buff_out, strlen(buff_out));
				// printf("%s\n", buff_out);
				int spflag=0;
				int i=0;
				int j=0;
				for(i=0;i<sizeof(buff_out);i++){

					if(buff_out[i]==' '){
						spflag=1;
						j=i;
						continue;
					}
					if(spflag==0){
						username[i]=buff_out[i];	
					}else if(spflag==1){
						apassword[i-j-1]=buff_out[i];

					}

				}
				
				printf("%s\n", apassword);
				printf("%s\n", username);

				// ==============check database==================
				bool authenticated=check_db(username, apassword);

				if(authenticated){
					printf("%s\n","Happy authentication" );
					authAns="A";
				}else{
					printf("%s\n", "Authentication not passed");
					authAns="N";
				}
				send(cli->sockfd, authAns, sizeof(authAns), 0);
				// ==============check database end==============
			}
		} else if (receive == 0 || strcmp(buff_out, "exit") == 0){
			printf("%s", buff_out);
			// send_message(buff_out, cli->uid);
			leave_flag = 1;
		} else {
			printf("ERROR: -1\n");
			leave_flag = 1;
		}

		bzero(buff_out, BUFFER_SZ);
	}

  /* Delete client from queue and yield thread */
	close(cli->sockfd);
  queue_remove(cli->uid);
  free(cli);
  cli_count--;
  pthread_detach(pthread_self());

	return NULL;
}


void *list_it(void *arg){
	int myflag=0;
	char username[512];
	

	

	char buff_out[BUFFER_SZ];
	int leave_flag = 0;

	cli_count++;
	client_t *cli = (client_t *)arg;

	// Name
	
	bzero(buff_out, BUFFER_SZ);

	while(1){
		if (leave_flag) {
			break;
		}

		// clearing here

		



		int receive = recv(cli->sockfd, buff_out, BUFFER_SZ, 0);
		if (receive > 0){
			if(strlen(buff_out) > 0){
				
				printf("%s\n",buff_out );

				 conn = mysql_init(NULL);
				 if (!mysql_real_connect(conn, server, user, password, database, 0, NULL, 0)) {
				 	fprintf(stderr, "%s\n", mysql_error(conn));
				 	exit(1);
				 }	
				
				 if (mysql_query(conn, buff_out)) {
					fprintf(stderr, "%s\n", mysql_error(conn));
				 	myflag=1;
				 }

				 if(myflag==0){
				res = mysql_use_result(conn);
				
				int num_fields = mysql_num_fields(res);

				printf("MySQL Tables in mysql database:\n");
				while ((row = mysql_fetch_row(res)) != NULL){
					 for(int i = 0; i < num_fields; i++) 
				    { 
				        printf("%s\n", row[i]); 
				        send(cli->sockfd, row[i], 1024, 0);
				    } 

				}
				send(cli->sockfd, "ItIsEnd", 1024, 0);
				mysql_free_result(res);
				mysql_close(conn);	
				// send(cli->sockfd, authAns, sizeof(authAns), 0);
				}
			}
		} else if (receive == 0 || strcmp(buff_out, "exit") == 0){
			printf("%s", buff_out);
			// send_message(buff_out, cli->uid);
			leave_flag = 1;
		} else {
			printf("ERROR: -1\n");
			leave_flag = 1;
		}

		bzero(buff_out, BUFFER_SZ);
	}

  /* Delete client from queue and yield thread */
	close(cli->sockfd);
  queue_remove(cli->uid);
  free(cli);
  cli_count--;
  pthread_detach(pthread_self());

	return NULL;
}




void *registration(){
	pthread_t tid;
	//sockets

	char *ip = "192.168.43.28";
	int port = atoi("9898");
	int option = 1;
	int listenfd = 0, connfd = 0;
  	struct sockaddr_in serv_addr;
  	struct sockaddr_in cli_addr;


  	/* Socket settings */
  	listenfd = socket(AF_INET, SOCK_STREAM, 0);
  	serv_addr.sin_family = AF_INET;
  	serv_addr.sin_addr.s_addr = inet_addr(ip);
  	serv_addr.sin_port = htons(port);


  	signal(SIGPIPE, SIG_IGN);

	if(setsockopt(listenfd, SOL_SOCKET,(SO_REUSEPORT | SO_REUSEADDR),(char*)&option,sizeof(option)) < 0){
		perror("ERROR: setsockopt failed");
   
	}

	/* Bind */
    if(bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
      perror("ERROR: Socket binding failed");
    
    }

   /* Listen */
    if (listen(listenfd, 10) < 0) {
      perror("ERROR: Socket listening failed");
      
	}

	printf("=== WELCOME TO THE DWM(DISTANT WORK MANAGEMENT) REGISTRATION ===\n");
	printf("=== WELCOME TO THE DWM(DISTANT WORK MANAGEMENT) PROJECT CREATION ===\n");

	while(1){
		socklen_t clilen = sizeof(cli_addr);
		connfd = accept(listenfd, (struct sockaddr*)&cli_addr, &clilen);

		/* Check if max clients is reached */
		if((cli_count + 1) == MAX_CLIENTS){
			printf("Max clients reached. Rejected: ");
			
			printf(":%d\n", cli_addr.sin_port);
			close(connfd);
			continue;
		}

		/* Client settings */
		client_t *cli = (client_t *)malloc(sizeof(client_t));
		cli->address = cli_addr;
		cli->sockfd = connfd;
		cli->uid = uid++;

		/* Add client to the queue and fork thread */
		queue_add(cli);
		pthread_create(&tid, NULL, &handle_client, (void*)cli);

		/* Reduce CPU usage */
		sleep(1);
	}

	

}

void *auth(){
	pthread_t tid;
	//sockets

	char *ip = "192.168.43.28";
	int port = atoi("9899");
	int option = 1;
	int listenfd = 0, connfd = 0;
  	struct sockaddr_in serv_addr;
  	struct sockaddr_in cli_addr;


  	/* Socket settings */
  	listenfd = socket(AF_INET, SOCK_STREAM, 0);
  	serv_addr.sin_family = AF_INET;
  	serv_addr.sin_addr.s_addr = inet_addr(ip);
  	serv_addr.sin_port = htons(port);


  	signal(SIGPIPE, SIG_IGN);

	if(setsockopt(listenfd, SOL_SOCKET,(SO_REUSEPORT | SO_REUSEADDR),(char*)&option,sizeof(option)) < 0){
		perror("ERROR: setsockopt failed");
   
	}

	/* Bind */
    if(bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
      perror("ERROR: Socket binding failed");
     
    }

   /* Listen */
    if (listen(listenfd, 10) < 0) {
      perror("ERROR: Socket listening failed");
      
	}

	printf("=== WELCOME TO THE DWM(DISTANT WORK MANAGEMENT) Authentication===\n");

	while(1){
		socklen_t clilen = sizeof(cli_addr);
		connfd = accept(listenfd, (struct sockaddr*)&cli_addr, &clilen);

		/* Check if max clients is reached */
		if((cli_count + 1) == MAX_CLIENTS){
			printf("Max clients reached. Rejected: ");
			
			printf(":%d\n", cli_addr.sin_port);
			close(connfd);
			continue;
		}

		/* Client settings */
		client_t *cli = (client_t *)malloc(sizeof(client_t));
		cli->address = cli_addr;
		cli->sockfd = connfd;
		cli->uid = uid++;

		/* Add client to the queue and fork thread */
		queue_add(cli);
		pthread_create(&tid, NULL, &auth_client, (void*)cli);

		/* Reduce CPU usage */
		sleep(1);
	}


}

void *list(){
	pthread_t tid;
	//sockets

	char *ip = "192.168.43.28";
	int port = atoi("9999");
	int option = 1;
	int listenfd = 0, connfd = 0;
  	struct sockaddr_in serv_addr;
  	struct sockaddr_in cli_addr;


  	/* Socket settings */
  	listenfd = socket(AF_INET, SOCK_STREAM, 0);
  	serv_addr.sin_family = AF_INET;
  	serv_addr.sin_addr.s_addr = inet_addr(ip);
  	serv_addr.sin_port = htons(port);


  	signal(SIGPIPE, SIG_IGN);

	if(setsockopt(listenfd, SOL_SOCKET,(SO_REUSEPORT | SO_REUSEADDR),(char*)&option,sizeof(option)) < 0){
		perror("ERROR: setsockopt failed");
    
	}

	/* Bind */
    if(bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
      perror("ERROR: Socket binding failed");
    
    }

   /* Listen */
    if (listen(listenfd, 10) < 0) {
      perror("ERROR: Socket listening failed");
      
	}

	printf("=== WELCOME TO THE DWM(DISTANT WORK MANAGEMENT) Project Listing===\n");

	while(1){
		socklen_t clilen = sizeof(cli_addr);
		connfd = accept(listenfd, (struct sockaddr*)&cli_addr, &clilen);

		/* Check if max clients is reached */
		if((cli_count + 1) == MAX_CLIENTS){
			printf("Max clients reached. Rejected: ");
			
			printf(":%d\n", cli_addr.sin_port);
			close(connfd);
			continue;
		}

		/* Client settings */
		client_t *cli = (client_t *)malloc(sizeof(client_t));
		cli->address = cli_addr;
		cli->sockfd = connfd;
		cli->uid = uid++;

		/* Add client to the queue and fork thread */
		queue_add(cli);
		pthread_create(&tid, NULL, &list_it, (void*)cli);

		/* Reduce CPU usage */
		sleep(1);
	}

	
}

int main(){
	

	pthread_t thread1, thread2,thread3;

	pthread_create(&thread1,NULL, registration, NULL);

	pthread_create(&thread2,NULL, auth, NULL);

	pthread_create(&thread3,NULL, list, NULL);

	pthread_join (thread1, NULL );

	pthread_join (thread2, NULL );
	
	return 0;
}