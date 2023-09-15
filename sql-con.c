#include<stdio.h>
#include "mysql/mysql.h"
int main(){
  MYSQL *con = NULL;
  printf("initializing mysql ...\n");
  con = mysql_init(NULL);
  if(NULL == con){
    printf("Failed to initialize mysql ...\n");
    printf("Problem: %s\n", mysql_error(con));
    return 0;
  }
  printf("attempting to connect ...\n");
  if(NULL == mysql_real_connect(
     con,"localhost","root","12345678","client_server_project", 0, NULL, 0)){
    printf("Failed to connect mysql ...\n");
    printf("Problem: %s\n", mysql_error(con));
    return 0;
  }
  printf("connected to mysql ...\n");
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
	  fprintf(stderr,"my sql query failed %s \n",mysql_error(con));
	  return 1;
  }


	
  mysql_close(con);
  return 0;
}
