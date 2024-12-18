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
bool strstarts(const char *str, const char *prefix);

/**
 * Searches for an executable by the name of @cmd inside the directories
 * specified by the PATH environment variable. Returns the absolute path
 * of the executable found.
 * @cmd: string representing the command to find.
 */
char *find_command(const char* cmd);

/**
 * Reads user input from stdin and breaks it down into the command and
 * the rest of the arguments.
 */
int process_input(char* buffer, char **arg_buffer);

/**
 * Prints an error message to stderr indicating that the command @cmd
 * was not found in the PATH variable. It returns 127 exit code.
 * @cmd: string to look for inside the directories defined by PATH env.
 */
int command_not_found_err(const char* cmd);

int main()
{
    while ( true )
    {
        printf("$ ");
        fflush(stdout);

        char cmd_buffer[1024];
        char *arg_buffer = NULL;

        process_input(cmd_buffer, &arg_buffer);

        if ( strncmp(cmd_buffer, "exit", 4) == 0 )
        {
            if ( arg_buffer == NULL )
            {
                printf("%s\n", "Exiting...");
                exit(0);
            }

            // strtol returns a "long" but it gets cast to int
            char *endptr;
            int exit_status = strtol(arg_buffer, &endptr, 10);
            exit(exit_status);
        }

        if ( strncmp(cmd_buffer, "echo", 4) == 0 )
        {
            if ( arg_buffer == NULL )
                printf("\n");
            else
                printf("%s\n", arg_buffer);

            continue;
        }

        if ( strncmp(cmd_buffer, "type", 4) == 0 )
        {
            if ( arg_buffer == NULL )
                continue;

            char *token = strtok(arg_buffer, " ");

            if ( token == NULL )
                continue;

            if (strncmp(token, "echo", 4) == 0 ||
                strncmp(token, "exit", 4) == 0 ||
                strncmp(token, "type", 4) == 0)
            {
                printf("%s is a shell builtin\n", token);
                continue;
            }

            char *filepath = find_command(token);

            if ( filepath == NULL )
                command_not_found_err(token);
            else
                printf("%s is %s/%s\n", token, filepath, token);

            continue;
        }
    }

    return 0;
}

bool strstarts(const char *str, const char *prefix)
{
     return strncmp(str, prefix, strlen(prefix)) == 0;
}

int command_not_found_err(const char* cmd)
{
    fprintf(stderr, "%s: not found\n", cmd);
    return 127;
}

char *find_command(const char* cmd)
{
    char *paths = getenv("PATH");
    char *path_env = strndup(paths, strlen(paths));
    path_env = strtok(path_env, ":");

    struct dirent *dirent;

    do {
        DIR *directory = opendir( path_env );

        if ( directory == NULL )
            continue;

        while ( (dirent = readdir(directory) ))
        {
            if ( strncmp(dirent->d_name, cmd, strlen(cmd)) == 0 )
            {
                closedir(directory);
                return path_env;
            }
        }

        closedir(directory);

    } while ( (path_env = strtok(NULL, ":")) );

    return NULL;
}

int process_input(char* buffer, char **arg_buffer)
{
    fgets(buffer, 1024, stdin);
    buffer[strcspn(buffer, "\n")] = '\0';

    if ( buffer == NULL )
        return -1;

    char *c = buffer;

    // Read until the end of the user input
    while ( *(++c) != '\0' )
    {
        // Upon finding the first space, set the null character at that
        // position, and point the argument buffer one position further.
        // Otherwise, there are simply no arguments provided.
        if ( *c == ' ' )
        {
            *c = '\0';
            *arg_buffer = &*(c + 1);
            break;
        }
    }

    return 0;
}
