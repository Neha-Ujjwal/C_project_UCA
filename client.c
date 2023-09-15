#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<mysql/mysql.h>
char username[20];
char password[20];


int createUser(){
	MYSQL *con = NULL;
 	// printf("initializing mysql ...\n");
  	con = mysql_init(NULL);
  	if(NULL == con){
    	//	printf("Failed to initialize mysql ...\n");
    	//	printf("Problem: %s\n", mysql_error(con));
    		return 0;
  	}
  //	printf("attempting to connect ...\n");
  	if(NULL == mysql_real_connect(
     		con,"localhost","root","12345678","client_server_project", 0, NULL, 0)){
    	//	printf("Failed to connect mysql ...\n");
    	//	printf("Problem: %s\n", mysql_error(con));
    		return 0;
  	}
 	// printf("connected to mysql ...\n");
  	char username[100];
 	 printf("enter username : ");
  	scanf("%s",username);
  	printf("\n");

  	char pwd[100];
  	printf("enter password : ");
  	scanf("%s",pwd);
  	printf("\n");


	char email[100];
        printf("enter emailId : ");
        scanf("%s",email);
         printf("\n");

  	char query[256];
  	snprintf(query,sizeof(query),"INSERT INTO authentication_data(username,password,email_id) VALUES ('%s','%s','%s')",username,pwd,email);

  	if(mysql_query(con,query)){
         // fprintf(stderr,"my sql query failed %s \n",mysql_error(con));
          return 0;
  	}



  	mysql_close(con);
 	 return 1;
}
int authenticate_user(int sockfd){
		char newUserAccount[10];
		int newUserCreation=0;

		int isAuthenticated=0;
                  //     printf("do you want to create a new user?(yes/no)");
                    //   scanf("%s",newUserAccount);

                 while(1){
                       printf("do you want to create a new user?(yes/no)");
                       scanf("%s",newUserAccount);

                       if(strcmp(newUserAccount,"yes")==0 || strcmp(newUserAccount,"Yes")==0 ||
                                       strcmp(newUserAccount,"Y")==0 || (strcmp(newUserAccount,"1")==0)){
                        newUserCreation=1;
                        break;

                       }else if(strcmp(newUserAccount,"no")==0 || strcmp(newUserAccount,"No")==0 ||
                                       strcmp(newUserAccount,"N")==0 || strcmp(newUserAccount,"NO")==0 || strcmp(newUserAccount,"0")==0){
                        newUserCreation=0;
                        break;
                       }else if(strcmp(newUserAccount,"\n")==0){
                               printf("enter a valid input ");
                       }else{
			       continue;
		       }

                 }

		 if(newUserCreation){
                      int createdUserSuccess=createUser(username,password);
		      if(createdUserSuccess){
                             printf("new user created succesfully \n");
                             isAuthenticated=1;
                         }else{
                              printf("error in creating new user \n");
                              isAuthenticated=0;
                         }

                 }else{
                            
		

        	printf("enter username: ");
      		  scanf("%s",username);
      		 //while(strcmp(fgets(username,sizeof(username),stdin),"\n")==0){
	      	//	 printf("invalid input\n");
	       	//	printf("enter username: ");
      		// }

       		//username[strlen(username)-1]='\0';
		//printf("%s",username);
        	send(sockfd,username,sizeof(username),0);

       		 printf("enter password: ");
        	scanf("%s",password);
	
	 	//while(strcmp(fgets(password,sizeof(password),stdin),"\n")==0){
              	//	 printf("invalid input\n");
               	//	printf("enter password: ");
      		 //}

      		 //password[strlen(password)-1]='\0';

        	//printf("%s",password);
		send(sockfd,password,sizeof(password),0);

        	char res[50];
        	recv(sockfd,res,sizeof(res),0);
		//printf("res is %d",strcmp(res,"authentication")==0);
		isAuthenticated=(strcmp(res,"authentication")==0);
		//printf("%d",isAuthenticated);
       		 //if(strcmp(res,"authentication")==0){
		//	isAuthenticated=1;

                // }else{
                  //       isAuthenticated=0;
                          //   send(client_sock,"fail",sizeof("fail"),0);
                  //}


                
        }
//	printf("value is : %d ",isAuthenticated);
	return isAuthenticated;

}


int send_File(int client_socket){
        char fileName[50];
        printf("enter the file name you want to send : ");
       // scanf("%s",&fileName);

	//if(send(client_socket,fileName,sizeof(fileName),0)==-1){
	//	perror("error in sending file name ");
	//	return 0;
	//}
	
	 while(strcmp(fgets(fileName,sizeof(fileName),stdin),"\n")==0){
               printf("invalid input\n");
               printf("enter fileName: ");
       }

       fileName[strlen(fileName)-1]='\0';
	//printf("%s",fileName);


        FILE *file=fopen(fileName,"rb");
        if(!file){
                perror("error in opening file");
		//fclose(file);
                return 0;
        }

	//if(send(client_socket,fileName,sizeof(fileName),0)==-1){

	//	perror("error in sending file name ");
	//	fclose(file);
          //      return 0;
        //}


        char buffer[4096];
        size_t bytes_read;
        while((bytes_read=fread(buffer,1,sizeof(buffer),file))>0){
                send(client_socket,buffer,bytes_read,0);
		
        }
	
        fclose(file);
        return 1;
	
	
}


int main(){

	//step1:creating client socket
	int sockfd=socket(AF_INET,SOCK_STREAM,0);
	if(sockfd<0){
		perror("socket creation failed \n");
		exit(1);
	}else{
		printf("socket created successfully\n");
	}

	//step2:assigning IP,port to client socket
	struct sockaddr_in client_addr;

	memset(&client_addr,0,sizeof(client_addr));

	client_addr.sin_family=AF_INET;
	client_addr.sin_addr.s_addr=inet_addr("127.0.0.1");
        client_addr.sin_port=htons(8080);
	
	

	//step3:connecting with server
	if(connect(sockfd,(struct sockaddr*)&client_addr,sizeof(client_addr))!=0){
                exit(1);
        }
	
	int isAuthenticated=authenticate_user(sockfd);

	if(isAuthenticated==0){
		printf("\n authentication fails!! Try again with correct username and password \n \n ");
	}else{
	


         printf("connection created with server successfully..!!!!\n");
        
	
	 // file transfer 

	int fileToBeSent=0;
	char isFile[50];
	printf("do you want to send file to server?(yes/no)");
//	scanf("%s",&isFile);
	
	 while(strcmp(fgets(isFile,sizeof(isFile),stdin),"\n")==0){
               printf("invalid input\n");
               printf("do you want to send file to server?(yes/no): ");
       }

       isFile[strlen(isFile)-1]='\0';

	if(strcmp(isFile,"yes")==0 || strcmp(isFile,"y")==0 || strcmp(isFile,"1")==0 || 
			strcmp(isFile,"Y")==0 || strcmp(isFile,"Yes")==0){
		fileToBeSent=1;
	}

	if(fileToBeSent==1){

		char header[]="file";
		send(sockfd,header,strlen(header),0);
		if(send_File(sockfd)==1){
			printf("file sent successFully \n");
		}else{
			printf("file sending failed \n");
			exit(1);
		}
	
	}else{

	
	
		char buffer[1024];

		bzero(buffer,1024);

		char str[]="hello this is ";
		strcat(str,username);
		strcpy(buffer,str);
		printf("Client : %s \n",buffer);
		send(sockfd,buffer,strlen(buffer),0);

		bzero(buffer,1024);
		recv(sockfd,buffer,sizeof(buffer),0);
		printf("Server : %s\n",buffer);
	}

	close(sockfd);

	printf("disconnected from the server \n");

}
	return 0;



}
