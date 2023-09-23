#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <mysql/mysql.h>
#include <pthread.h>

struct User {
    char username[50];
    char password[50];
};

struct User users[] = {
    {"user1", "pass1"},
    {"user2", "pass2"},
    {"user3", "pass3"}
};

int num_users = sizeof(users) / sizeof(users[0]);

int num_clients = 0;
int client_sockets[1024];

// Function to authenticate a user from the database
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

    // User input
   // char username[50];
    //char password[50];

   // printf("Enter username: ");
    //scanf("%s", username);
   // printf("Enter password: ");
   // scanf("%s", password);

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
        if (strcmp(row[1],(password)) == 0) {
            printf("Authentication successful!\n");
   return 1;
        } else {
            printf("Authentication failed. Incorrect password.\n");
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

// Function to handle a single client
void *clientHandler(void *arg) {
    int client_sock = *((int *)arg);
    char buffer[1024];

    // Authenticate the client
    int isAuthenticated = authenticate_user(client_sock);

    if (isAuthenticated == 1) {
        printf("Client connected!!!!!!!!!\n");

        // Receive the client's request and handle accordingly
        char header[13];
        ssize_t receivedText = recv(client_sock, header, sizeof(header), 0);
        header[receivedText] = '\0';

        if (strcmp(header, "file") == 0) {
            receive_File(client_sock);
        } else {
            bzero(buffer, 1024);
            recv(client_sock, buffer, sizeof(buffer), 0);
            printf("Client : %s\n", buffer);

            bzero(buffer, 1024);
            strcpy(buffer, "Hi, this is server. Have a nice day!\n");
            printf("Server : %s \n", buffer);
            send(client_sock, buffer, strlen(buffer), 0);
        }

        close(client_sock);
        printf("Client disconnected\n\n");
    }

    // Remove the client socket descriptor from the global array
    for (int i = 0; i < num_clients; i++) {
        if (client_sockets[i] == client_sock) {
            for (int j = i; j < num_clients - 1; j++) {
                client_sockets[j] = client_sockets[j + 1];
            }
            num_clients--;
            break;
        }
    }

    free(arg); // Free the memory allocated for the client_sock
    pthread_exit(NULL);
}

// Function to authenticate a user based on username and password
int authenticate_user(int client_sock) {
    char username[20];
    char password[20];

    recv(client_sock, username, sizeof(username), 0);
    recv(client_sock, password, sizeof(password), 0);
	
    printf("%s\n",password);
    int isAuthenticated = authenticateUsingDB(username, password);

    if (isAuthenticated) {
        send(client_sock, "authentication", sizeof("authentication"), 0);
    } else {
        send(client_sock, "fail", sizeof("fail"), 0);
    }

    return isAuthenticated;
}

// Function to receive a file from the client
int receive_File(int client_sock) {
    FILE *file = fopen("received_file.txt", "wb");
    if (!file) {
        perror("file opening failed...");
        return 0;
    }

    char buffer[4096];
    ssize_t bytes_received;

    while ((bytes_received = recv(client_sock, buffer, sizeof(buffer), 0)) > 0) {
        fwrite(buffer, 1, bytes_received, file);
    }

    fclose(file);
    return 1;
}

int main() {
    int sockfd;
    struct sockaddr_in client_addr, server_addr;
    char buffer[1024];
    pid_t childpid;

    printf("***************Welcome to TCP Client-server program ****************\n\n");

    // Create a socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Error in creating socket...\n");
        exit(1);
    } else {
        printf("Socket created successfully!!!!!!\n");
    }

    // Initialize server_addr
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(8080);

    // Bind the socket
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Binding failed....\n");
        close(sockfd);
        exit(1);
    } else {
        printf("Socket binded successfully\n");
    }

    // Listen for incoming connections
    if (listen(sockfd, 5) < 0) {
        perror("Listening failed..\n");
        exit(1);
    } else {
        printf("Server listening ....\n");
    }

    while (1) {
        // Accept incoming connections
        socklen_t addr_size = sizeof(client_addr);
        int client_sock = accept(sockfd, (struct sockaddr *)&client_addr, &addr_size);

        if (client_sock < 0) {
            perror("Accept failed\n");
            continue;
        }

        // Add the client socket descriptor to the global array
        if (num_clients < 1024) {
            client_sockets[num_clients++] = client_sock;
        } else {
            printf("Too many clients. Rejecting the connection.\n");
            close(client_sock);
            continue;
        }

        // Create a new thread to handle the client
        pthread_t tid;
        int *client_sock_arg = (int *)malloc(sizeof(int));
        *client_sock_arg = client_sock;

        if (pthread_create(&tid, NULL, clientHandler, client_sock_arg) != 0) {
            perror("Failed to create thread\n");
            continue;
        }
    }

    // Close the socket and clean up resources
    close(sockfd);

    return 0;
}

	

