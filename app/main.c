#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/param.h>

/**
* strstarts - does @str start with @prefix?
* @str: string to examine
* @prefix: prefix to look for.
*/
bool strstarts(const char *str, const char *prefix);

/**
 * Returns the full path of @cmd if found at one of the path locations.
 * @cmd: string to look for inside the directories defined by PATH env.
 */
char *find_in_path(const char* cmd);

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
            char *path = find_in_path(token);

            if ( path == NULL )
                printf("%s: not found\n", token);
            else
                printf("%s is %s/%s\n", token, path, token);

            continue;
        }

        printf("%s: not found\n", input);
    }

    return 0;
}

bool strstarts(const char *str, const char *prefix)
{
     return strncmp(str, prefix, strlen(prefix)) == 0;
}

bool process_command(const char command[], char input[])
{
    if ( strncmp(input, command, strlen(command)) )
    {
        char *token = strtok(input, " ");
        token = strtok(NULL, "");
        return true;
    }

    return false;
}

char *find_in_path(const char* cmd)
{
    char *path_var = getenv("PATH");
    char *path_env = strndup(path_var, strlen(path_var));

    path_env = strtok(path_env, "=");
    path_env = strtok(path_env, ":");

    struct dirent *directory;

    while ( (path_env = strtok(NULL, ":")) )
    {
        DIR *dir = opendir( path_env );

        if (dir == NULL) continue; // can't open it

        while ( (directory = readdir(dir)) )
        {
            if (strncmp(directory->d_name, cmd, strlen(cmd)) == 0)
            {
                closedir(dir);
                return path_env;
            }
        }

        closedir(dir);
    }

    return NULL;
}
