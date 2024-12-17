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

        printf("%s: command not found\n", input);
    }

    return 0;
}
