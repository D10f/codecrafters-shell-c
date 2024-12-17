#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

/**
* strstarts - does @str start with @prefix?
* @str: string to examine
* @prefix: prefix to look for.
*/
bool strstarts(const char *str, const char *prefix)
{
     return strncmp(str, prefix, strlen(prefix)) == 0;
}

int main() {

    while ( true )
    {
        printf("$ ");
        fflush(stdout);

        // Wait for user input
        char input[100];
        fgets(input, 100, stdin);

        // Remove newline from fgets
        input[strcspn(input, "\n")] = '\0';

        if (strcmp(input, "exit 0") == 0) {
            exit(0);
        }

        if (strstarts(input, "echo")) {
            char *token = strtok(input, " ");
            token = strtok(NULL, "");
            printf("%s\n", token);
            continue;
        }

        if (strstarts(input, "type")) {
            char *token = strtok(input, " ");
            token = strtok(NULL, " "); // capture only until the next space
            /*printf("token: %s, %d\n", token, strncmp(token, "testing", strlen(token) - 1 ));*/

            if (strncmp(token, "echo", strlen(token) - 1) == 0 ||
                strncmp(token, "exit", strlen(token) - 1) == 0 ||
                strncmp(token, "type", strlen(token) - 1) == 0)
            {
                printf("%s is a shell builtin\n", token);
                continue;
            }

            printf("%s: not found\n", token);
            continue;
        }

        printf("%s: not found\n", input);
    }

    return 0;
}
