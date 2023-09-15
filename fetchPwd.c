#include <mysql/mysql.h>
#include <stdio.h>
#include <string.h>


int authenticate(char* username,char* password){
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;

    // Initialize connection handler
    conn = mysql_init(NULL);

    if (conn == NULL) {
        fprintf(stderr, "mysql_init() failed\n");
        return 1;
    }

    // Connect to the MySQL database
    if (mysql_real_connect(conn, "hostname", "username", "password", "database_name", 0, NULL, 0) == NULL) {
        fprintf(stderr, "mysql_real_connect() failed: %s\n", mysql_error(conn));
        mysql_close(conn);
        return 1;
    }

    // User input
   // char username[50];
   // char password[50];

    //printf("Enter username: ");
   // scanf("%s", username);
   // printf("Enter password: ");
   // scanf("%s", password);

    // Query the database for user information
    char query[100];
    snprintf(query, sizeof(query), "SELECT username, password FROM users WHERE username='%s'", username);

    if (mysql_query(conn, query)) {
        fprintf(stderr, "mysql_query() failed: %s\n", mysql_error(conn));
        mysql_close(conn);
        return 1;
    }

    res = mysql_store_result(conn);

    if (res == NULL) {
        fprintf(stderr, "mysql_store_result() failed: %s\n", mysql_error(conn));
        mysql_close(conn);
        return 1;
    }

    // Check if a user with the provided username exists
    if ((row = mysql_fetch_row(res))) {
        // Compare hashed password from the database with the user's input (you should use a hashing library)
        if (strcmp(row[1], hashed_password(password)) == 0) {
            printf("Authentication successful!\n");
        } else {
            printf("Authentication failed. Incorrect password.\n");
        }
    } else {
        printf("User not found.\n");
    }

    // Close the MySQL connection
    mysql_free_result(res);
    mysql_close(conn);

    return 0;

}

int main() {
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;

    // Initialize connection handler
    conn = mysql_init(NULL);

    if (conn == NULL) {
        fprintf(stderr, "mysql_init() failed\n");
        return 1;
    }

    // Connect to the MySQL database
    if (mysql_real_connect(conn, "localhost", "root", "12345678", "client_server_project", 0, NULL, 0) == NULL) {
        fprintf(stderr, "mysql_real_connect() failed: %s\n", mysql_error(conn));
        mysql_close(conn);
        return 1;
    }

    // User input
    char username[50];
    char password[50];

    printf("Enter username: ");
    scanf("%s", username);
    printf("Enter password: ");
    scanf("%s", password);

    // Query the database for user information
    char query[100];
    snprintf(query, sizeof(query), "SELECT username, password FROM authentication_data WHERE username='%s'", username);

    if (mysql_query(conn, query)) {
        fprintf(stderr, "mysql_query() failed: %s\n", mysql_error(conn));
        mysql_close(conn);
        return 1;
    }

    res = mysql_store_result(conn);

    if (res == NULL) {
        fprintf(stderr, "mysql_store_result() failed: %s\n", mysql_error(conn));
        mysql_close(conn);
        return 1;
    }

    // Check if a user with the provided username exists
    if ((row = mysql_fetch_row(res))) {
        // Compare hashed password from the database with the user's input (you should use a hashing library)
        if (strcmp(row[1], password) == 0) {
            printf("Authentication successful!\n");
        } else {
            printf("Authentication failed. Incorrect password.\n");
        }
    } else {
        printf("User not found.\n");
    }

    // Close the MySQL connection
    mysql_free_result(res);
    mysql_close(conn);

    return 0;
}
