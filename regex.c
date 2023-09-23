#include <stdio.h>
#include <regex.h>

int main() {
    regex_t regex;
    int reti;
    char email[100];
    printf("enter email address  :");
    scanf("%s",email);
    // Change this to your email address

    // Compile the regular expression
    reti = regcomp(&regex, "^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$", REG_EXTENDED);

    if (reti) {
        fprintf(stderr, "Could not compile regex\n");
        return 1;
    }

    // Execute the regular expression
    reti = regexec(&regex, email, 0, NULL, 0);

    if (!reti) {
        printf("Valid email address\n");
    } else if (reti == REG_NOMATCH) {
        printf("Invalid email address\n");
    } else {
        char error_message[100];
        regerror(reti, &regex, error_message, sizeof(error_message));
        fprintf(stderr, "Regex match failed: %s\n", error_message);
        return 1;
    }

    // Free the compiled regular expression
    regfree(&regex);

    return 0;
}
