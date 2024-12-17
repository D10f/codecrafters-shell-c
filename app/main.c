#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

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

        printf("%s: command not found\n", input);
    }

    return 0;
}
