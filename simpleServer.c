#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include <mysql/mysql.h>
#include "base64.h"



int authenticateUsingDB(char* username,char* password){

   int isAuthenticated=0;
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;

    // Initialize connection handler
    conn = mysql_init(NULL);

    if (conn == NULL) {
        fprintf(stderr, "mysql_init() failed\n");
        return 0;
    }

    // Connect to the MySQL database
    if (mysql_real_connect(conn, "localhost", "root", "12345678", "client_server_project", 0, NULL, 0) == NULL) {
        fprintf(stderr, "mysql_real_connect() failed: %s\n", mysql_error(conn));
        mysql_close(conn);
        return 0;
    }

 
    // Query the database for user information
    char query[100];
    snprintf(query, sizeof(query), "SELECT username, password FROM authentication_data WHERE username='%s'", username);

    if (mysql_query(conn, query)) {
        fprintf(stderr, "mysql_query() failed: %s\n", mysql_error(conn));
        mysql_close(conn);
        return 0;
    }

    res = mysql_store_result(conn);

    if (res == NULL) {
        fprintf(stderr, "mysql_store_result() failed: %s\n", mysql_error(conn));
        mysql_close(conn);
        return 0;
    }

    // Check if a user with the provided username exists
    if ((row = mysql_fetch_row(res))) {
        // Compare hashed password from the database with the user's input (you should use a hashing library)
		//printf("in mysql fn: %d \n ",strcmp(decode(row[1]),password));
		char* decodedPwd=decode(row[1]);
		decodedPwd[strlen(decodedPwd)-1]='\0';
	       if (strcmp(decodedPwd,(password)) == 0) {
            		printf("Authentication successful!\n");
	    		return 1;
        	} else {
            		printf("Authentication failed. Incorrect password.\n");
	    		return 0;
       		 }
    } else {
        printf("User not found.\n");
	return 0;
    }

    // Close the MySQL connection
    mysql_free_result(res);
    mysql_close(conn);

    return 1;
}



int authenticate_user(int client_sock){
          char username[20];
          char password[20];

           recv(client_sock,username,sizeof(username),0);
           recv(client_sock,password,sizeof(password),0);
		
	//	printf("%s",password);
          int isAuthenticated=authenticateUsingDB(username,password);

             

           if(isAuthenticated){
                //       printf("connected \n");
                send(client_sock,"authentication",sizeof("authentication"),0);
	    }else{
	        send(client_sock,"fail",sizeof("fail"),0);
	    }

          return isAuthenticated;
}

int receive_File(int client_sock){
	FILE *file=fopen("received_file.txt","wb");
	if(!file){
perror("file opening failed...");				
		return 0;
	}
	

	char buffer[4096];
	ssize_t bytes_received;

	while((bytes_received=recv(client_sock,buffer,sizeof(buffer),0))>0){
		fwrite(buffer,1,bytes_received,file);
	 }
	fclose(file);
	return 1;
}
	


int main(){

	int sockfd;
	 struct sockaddr_in client_addr,server_addr;
        char buffer[1024];

	
	printf("***************Welcome to TCP Client-server program ****************\n\n");
	

	//step1->creating socket
	sockfd=socket(AF_INET,SOCK_STREAM,0);
	if(sockfd<0){
		perror("error in creating socket...\n ");
		exit(1);
	}else{
		printf("Socket created successfully!!!!!!\n");
	}

	//struct sockaddr_in server_addr;


	

	//step 2: assigning port number,IP to socket
	memset(&server_addr,0,sizeof(server_addr));

	server_addr.sin_family=AF_INET;
	server_addr.sin_addr.s_addr=inet_addr("127.0.0.1");
	//server_addr.sin_addr.s_addr=INADDR_ANY;
	server_addr.sin_port=htons(8080);

	if(bind(sockfd,(struct sockaddr*)&server_addr,sizeof(server_addr))<0){
		perror("binding failed.... \n");
		close(sockfd);
		exit(0);
	}else{
		printf("socket binded successfully\n");
	}



	//step3 : listining for any request

	if(listen(sockfd,5)<0){
		perror("listening failed..\n");
		exit(0);
	}else{
		printf("server listening .... \n");
	}
	

	


	while(1){


		//creating client socket
		socklen_t addr_size=sizeof(client_addr);
		int client_sock=accept(sockfd,(struct sockaddr*)&client_addr,&addr_size);
	

		//validating user
		
		int isAuthenticated=authenticate_user(client_sock);
		
		if(isAuthenticated==1){
			//printf("\n authentication failed !!! this client dont have any account \n\n");
//	exit(1);		

//		}else{
		  	printf("client connected!!!!!!!!! \n");
			
			//file transfer
			char header[13];
			ssize_t receivedText=recv(client_sock,header,sizeof(header),0);
			header[receivedText]='\0';

			if(strcmp(header,"file")==0){
			   	receive_File(client_sock);
			}else{
				bzero(buffer,1024);
				recv(client_sock,buffer,sizeof(buffer),0);
				printf("Client : %s\n",buffer);
				
			
				bzero(buffer,1024);
				strcpy(buffer,"Hi this is server . Have a nice day!!!!!!\n");

				printf("Server : %s \n",buffer);
				send(client_sock,buffer,strlen(buffer),0);
			}

			close(client_sock);
			printf("client disconnected\n \n ");
            	}
	}
	return 0;
}
