#include <gtk/gtk.h>


GtkBuilder *regBuilder;
GtkBuilder *signInBuilder;
GtkBuilder *proListWorkerBuilder;
GtkBuilder *proListClientBuilder;
GtkBuilder *createProBuilder;

GtkWidget *regWin;
GtkWidget *signInWin;
GtkWidget *proListWorkerWin;
GtkWidget *proListClientWin;
GtkWidget *createProWin;

void showReg(){
	regBuilder=gtk_builder_new_from_file("templates/register.glade");
	regWin=GTK_WIDGET(gtk_builder_get_object(regBuilder,"regWindow"));	
	gtk_builder_connect_signals(regBuilder, NULL);
	// g_signal_connect(regBuilder, "delete_event", gtk_main_quit, NULL);
	gtk_widget_show(regWin);
}
void showAuth(){
	signInBuilder=gtk_builder_new_from_file("templates/signin.glade");
	signInWin=GTK_WIDGET(gtk_builder_get_object(signInBuilder,"signInWindow"));	
	gtk_builder_connect_signals(signInBuilder, NULL);
	gtk_widget_show(signInWin);
}
void showListClient(){
	proListClientBuilder=gtk_builder_new_from_file("templates/clientProjectList.glade");
	gtk_builder_connect_signals(proListClientBuilder, NULL);

	// dynamic render here
	GtkGrid *myGrid;
	GtkWidget *newLabel;
	
	proListClientWin=GTK_WIDGET(gtk_builder_get_object(proListClientBuilder,"proListClientWin"));	
	myGrid=GTK_GRID(gtk_builder_get_object(proListClientBuilder,"projectGrid"));

	gtk_widget_show(proListClientWin);

	


	char  clientName[255], workerName[255], projectName[255];char price[255];
	char status[]="not_accepted";char workedHours[]="0";char buff_in[1024];
	bzero(clientName, 255);bzero(workerName, 255);bzero(projectName, 255);bzero(buff_in, 1024);
	char pQuery[1024]="SELECT * FROM project where client_name=\"";

	
	printf("LISTING PROJECTS\n");
	

	strcpy(clientName, username);
	
	
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

	
	int port = atoi("9999");

		
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
	int i=-1;
	int r=1;
	while(1){
		int receive = recv(sockfd, buff_in, 1024, 0);
		if (receive > 0){
			printf("%s\n", buff_in);
			if(i!=-1){
				newLabel=gtk_label_new (buff_in);
				gtk_grid_attach(myGrid, newLabel,i,r, 1, 1);
				gtk_widget_show(newLabel);
			}
			if(i==5){
				r++;
				i=-2;
			}
			
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



	
}
void showListWorker(){
	
	proListWorkerBuilder=gtk_builder_new_from_file("templates/workerProjectList.glade");
	proListWorkerWin=GTK_WIDGET(gtk_builder_get_object(proListWorkerBuilder,"proListWorkerWin"));	
	gtk_builder_connect_signals(proListWorkerBuilder, NULL);
	gtk_widget_show(proListWorkerWin);


}
void showCreateProject(){
	createProBuilder=gtk_builder_new_from_file("templates/createProject.glade");
	createProWin=GTK_WIDGET(gtk_builder_get_object(createProBuilder,"createProWin"));	
	gtk_builder_connect_signals(createProBuilder, NULL);
	gtk_widget_show(createProWin);
}