const char *username;

#include <stdlib.h>
#include <stdio.h>
#include <gtk/gtk.h>
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

volatile sig_atomic_t flag = 0;
int sockfd = 0;
char name[32];
char query[1024]="INSERT INTO users values (NULL, \"";
time_t start;
time_t stop;
int diff;
char *ip ;
#include "getStyle.h"
#include "showCallBacks.h"




void onToReg(GtkButton *b,GtkWidget *w){
	gtk_widget_hide(w);
	showReg();
}

void onBackToLogin(GtkButton *b,GtkWidget *w){
	gtk_widget_hide(w);
	showAuth();
}

void onLogin(GtkButton *b,GtkWidget *w){
	GtkLabel *suckLabel;
	char  auth[255];char payload[255];
	bzero(payload,255);

	GtkEntry *usernameEntry;
	GtkEntry *passwordEntry;

	usernameEntry=GTK_ENTRY(gtk_builder_get_object(signInBuilder,"usernameEntry"));
	passwordEntry=GTK_ENTRY(gtk_builder_get_object(signInBuilder,"passwordEntry"));
	suckLabel=GTK_LABEL(gtk_builder_get_object(signInBuilder,"suckLabel"));
	
	username=gtk_entry_get_text (usernameEntry);
	const char *password=gtk_entry_get_text (passwordEntry);
	
	printf("%s\n",username );
	printf("%s\n",password );


	
	int port = atoi("9899");

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

	char salt[]=" ";

	strcat(payload, username);
	strcat(payload, salt);
	strcat(payload,password);
	purify(payload);
	printf("%s\n",payload );
	// Send query
	send(sockfd, payload, sizeof(payload), 0);
	int receive = recv(sockfd, auth, 255, 0);

	if (receive > 0){
			if(strlen(auth) > 0){
				if(strcmp(auth,"A")==0){
					printf("%s\n", "AUTHENTICATION PASSED");

					gtk_widget_hide(w);
					showListClient();
				}else{
					printf("%s\n", "AUTHENTICATION FAILED, PLEASE TRY AGAIN");
					gtk_label_set_text (suckLabel,"Authentication failed.\n Please try again");
				}
			}
		} else {
			printf("ERROR: -1\n");
			gtk_label_set_text (suckLabel,"Connection error");
		
		}

    close(sockfd);
	// gtk_widget_hide(w);
	// showListClient();
}

void onNewProject(GtkButton *b,GtkWidget *w){
	showCreateProject();
	gtk_widget_hide(w);
}

void onBackToClientProList(GtkButton *b,GtkWidget *w){
	showListClient();
	gtk_widget_hide(w);
}

void purify(char *s){
	for(int i=0;i<=255;i++){
		if(s[i]=='\n'){
			s[i]='\0';
			
		}
	}
	
}
void onRegClicked(GtkButton *b){
	char query[1024]="INSERT INTO users values (NULL, \"";
	char querymid[]="\",\"";
	char queryend[]="\")";

	GtkEntry *nameEntry;
	GtkEntry *usernameEntry;
	GtkEntry *passwordEntry;
	GtkEntry *roleEntry;
	GtkLabel *suckLabel;
	nameEntry=GTK_ENTRY(gtk_builder_get_object(regBuilder,"nameEntry"));
	usernameEntry=GTK_ENTRY(gtk_builder_get_object(regBuilder,"usernameEntry"));
	passwordEntry=GTK_ENTRY(gtk_builder_get_object(regBuilder,"passwordEntry"));
	roleEntry=GTK_ENTRY(gtk_builder_get_object(regBuilder,"roleEntry"));
	suckLabel=GTK_LABEL(gtk_builder_get_object(regBuilder,"suckLabel"));

	const char *name=gtk_entry_get_text (nameEntry);
	const char *username=gtk_entry_get_text (usernameEntry);
	const char *password=gtk_entry_get_text (passwordEntry);
	const char *role=gtk_entry_get_text (roleEntry);

	strcat(query, name);
	strcat(query, querymid);
	strcat(query, username);
	strcat(query, querymid);
	strcat(query, password);
	strcat(query, querymid);
	strcat(query, role);
	strcat(query, queryend);
	printf("%s\n", query);

	int port = atoi("9898");

	struct sockaddr_in server_addr;


	
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ip);
    server_addr.sin_port = htons(port);

    int err = connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
  
    if (err == -1) {
		printf("ERROR: connect\n");
		
	}

	// Send query
	if(send(sockfd, query, sizeof(query), 0)<0){
		printf("ERROR: connect\n");
	}else{
		gtk_entry_set_text (nameEntry,"");
		gtk_entry_set_text (usernameEntry,"");
		gtk_entry_set_text (passwordEntry,"");
		gtk_entry_set_text (roleEntry,"");
		
		gtk_label_set_text (suckLabel,"successfully registered");
                    
	}

	close(sockfd);
}
void onCreateProject(GtkButton *b, GtkButton *backButton){
	
	char status[]="not_accepted";char workedHours[]="0";
	
	char pQuery[1024]="INSERT INTO project values (NULL, \"";

	GtkEntry *projectNameEntry;
	GtkEntry *clientNameEntry;
	GtkEntry *workerNameEntry;
	GtkEntry *priceEntry;
	GtkLabel *suckLabel;

	projectNameEntry=GTK_ENTRY(gtk_builder_get_object(createProBuilder,"projectName"));
	clientNameEntry=GTK_ENTRY(gtk_builder_get_object(createProBuilder,"clientName"));
	workerNameEntry=GTK_ENTRY(gtk_builder_get_object(createProBuilder,"workerName"));
	priceEntry=GTK_ENTRY(gtk_builder_get_object(createProBuilder,"price"));
	suckLabel=GTK_LABEL(gtk_builder_get_object(createProBuilder,"suckLabel"));

	const char *projectName=gtk_entry_get_text (projectNameEntry);
	const char *clientName=gtk_entry_get_text (clientNameEntry);
	const char *workerName=gtk_entry_get_text (workerNameEntry);
	const char *price=gtk_entry_get_text (priceEntry);

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

	printf("%s\n",pQuery );


	
	int port = atoi("9898");

	
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
	if(send(sockfd, pQuery, sizeof(pQuery), 0)>0){
		gtk_entry_set_text (projectNameEntry,"");
		gtk_entry_set_text (clientNameEntry,"");
		gtk_entry_set_text (workerNameEntry,"");
		gtk_entry_set_text (priceEntry,"");
		gtk_button_set_label (backButton,"back");
		gtk_label_set_text (suckLabel,"Created successfully! Press back to go to your projects list");
	}else{
		gtk_label_set_text (suckLabel,"Unknown error occured");
	}	

	close(sockfd);

}

int main(int argc, char *argv[]){
	
	gtk_init(NULL,NULL);
	getCSS();
	if (argc < 2) {
       fprintf(stderr,"error no host");
       exit(0);
    }

	ip = argv[1];

	showAuth();
	
	gtk_main();

	return 0;
}


