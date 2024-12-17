#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

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

LOOP: while ( true )
    {
        printf("$ ");
        fflush(stdout);

        // Wait for user input
        char input[100];
        fgets(input, 100, stdin);

        // Remove newline from fgets
        input[strcspn(input, "\n")] = '\0';

        if (strstarts(input, "exit"))
        {
            char *token = strtok(input, " ");
            token = strtok(NULL, "");

            if (token == NULL) // no exit code was provided
                exit(0);

            // strtol returns a "long" but it gets cast to int
            char *endptr;
            int exit_status = strtol(token, &endptr, 10);

            exit(exit_status);
        }

        if (strstarts(input, "echo"))
        {
            char *token = strtok(input, " ");
            token = strtok(NULL, "");

            if (token == NULL) // no other arguments were provided
            {
                printf("\n");
                continue;
            }

            printf("%s\n", token);
            continue;
        }

        if (strstarts(input, "type"))
        {
            char *token = strtok(input, " ");
            token = strtok(NULL, " "); // capture only until the next space

            if (token == NULL) // no other arguments were provided
                continue;

            if (strncmp(token, "echo", strlen(token)) == 0 ||
                strncmp(token, "exit", strlen(token)) == 0 ||
                strncmp(token, "type", strlen(token)) == 0)
            {
                printf("%s is a shell builtin\n", token);
                continue;
            }

            // Try to find in PATH
            char *path_env = getenv("PATH");
            char *pathenv = strndup(path_env, strlen(path_env));

            pathenv = strtok(pathenv, "=");
            pathenv = strtok(pathenv, ":");

            while ( (pathenv = strtok(NULL, ":")) )
            {
                if ( strncmp(pathenv, "/usr/bin", strlen("/usr/bin")) == 0 )
                    continue;

                struct dirent *directory;
                DIR *dir = opendir( pathenv );

                if ( dir == NULL )
                    continue;
                    /*fprintf(stderr, "Unable to open directory: %s\n", pathenv);*/

                while ( (directory = readdir(dir) ) )
                {
                    if (strncmp(directory->d_name, token, strlen(token)) == 0)
                    {
                        printf("%s is %s/%s\n", token, pathenv, token);
                        closedir(dir);
                        goto LOOP;
                    }
                }

                closedir(dir);
            }

            printf("%s: not found\n", token);
            continue;
        }

        printf("%s: not found\n", input);
    }

    return 0;
}
