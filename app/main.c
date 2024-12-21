#include <ctype.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/wait.h>
#include <unistd.h>

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
 * @cmd: string representing the command to run.
 */
/*void run_command(char* cmd_buffer, char *args_buffer);*/
void run_command(char *argv[]);

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

void parse_input(char *buffer, char *argv[]);

int main()
{
    while ( true )
    {
        printf("$ ");
        fflush(stdout);

        char cmd_buffer[1024];
        char *arg_buffer = NULL;
        char *argv[10];
        int argc = 0;

        fgets(cmd_buffer, 1024, stdin);
        cmd_buffer[strcspn(cmd_buffer, "\n")] = '\0';

        parse_input(cmd_buffer, argv);

        for ( int i = 0; i < 10; i++ )
        {
            if ( argv[i] == NULL )
                break;
            argc++;
        }

        /*continue;*/
        /**/
        /*process_input(cmd_buffer, &arg_buffer);*/

        if ( strncmp(argv[0], "exit", 4) == 0 )
        {
            if ( argv[1] == NULL )
            {
                printf("%s\n", "Exiting...");
                exit(0);
            }

            // strtol returns a "long" but it gets cast to int
            char *endptr;
            int exit_status = strtol(argv[1], &endptr, 10);
            exit(exit_status);
        }

        if ( strncmp(argv[0], "echo", 4) == 0 )
        {
            for (int i = 1; i < argc; i++)
                printf("%s ", argv[i]);

            printf("\n");
            continue;
        }

        if ( strncmp(argv[0], "type", 4) == 0 )
        {
            if ( argv[1] == NULL )
                continue;

            /*char *token = strtok(arg_buffer, " ");*/
            /**/
            /*if ( token == NULL )*/
            /*    continue;*/

            if (strncmp(argv[1], "echo", 4) == 0 ||
                strncmp(argv[1], "exit", 4) == 0 ||
                strncmp(argv[1], "type", 4) == 0)
            {
                printf("%s is a shell builtin\n", argv[1]);
                continue;
            }

            char *filepath = find_command(argv[1]);

            if ( filepath == NULL )
                fprintf(stderr, "%s: not found\n", argv[1]);
            else
                printf("%s is %s/%s\n", argv[1], filepath, argv[1]);

            continue;
        }

        char *filepath = find_command(argv[0]);

        if ( filepath == NULL )
            fprintf(stderr, "%s: command not found\n", argv[0]);
        else
        {
            char cmd_path[1024];
            sprintf(cmd_path, "%s/%s", filepath, argv[0]);
            argv[0] = cmd_path;
            run_command(argv);
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
        if ( strncmp(path_env, "/usr/bin", strlen("/usr/bin")) == 0 )
            continue;

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

void parse_input(char *buffer, char *argv[])
{
    if ( buffer == NULL )
        return;

    char *curr_ptr = buffer;
    char *prev_ptr = buffer;
    int argc = 0;

    while ( *curr_ptr != '\0' )
    {
        if ( isspace(*curr_ptr) )
        {
            // Trim leading spaces
            if ( isspace(*prev_ptr) )
            {
                while ( isspace(*(++curr_ptr)) );
                prev_ptr = curr_ptr;
                continue;
            }

            *curr_ptr = '\0';

            int new_buffer_size = (curr_ptr - prev_ptr) + 1;
            argv[argc] = malloc(new_buffer_size);
            strncpy(argv[argc], prev_ptr, new_buffer_size);
            argc++;

            // Consume any remaning whitespace
            while ( isspace(*(++curr_ptr)) );

            prev_ptr = curr_ptr;
            continue;
        }

        if ( *curr_ptr == '\'' )
        {
            prev_ptr = curr_ptr + 1;

            while ( (*(++curr_ptr)) != '\'' && *curr_ptr != '\0' );

            *curr_ptr = '\0';

            int new_buffer_size = (curr_ptr - prev_ptr) + 1;
            argv[argc] = malloc(new_buffer_size);
            strncpy(argv[argc], prev_ptr, new_buffer_size);
            argc++;

            prev_ptr = ++curr_ptr;
            continue;
        }

        if ( *curr_ptr == '"' )
        {
            prev_ptr = curr_ptr + 1;

            while ( (*(++curr_ptr)) != '"' && *curr_ptr != '\0' );

            *curr_ptr = '\0';

            int new_buffer_size = (curr_ptr - prev_ptr) + 1;
            argv[argc] = malloc(new_buffer_size);
            strncpy(argv[argc], prev_ptr, new_buffer_size);
            argc++;

            prev_ptr = ++curr_ptr;
            continue;
        }

        if ( *curr_ptr == '\\' )
        {
            *curr_ptr = '\0';

            int new_buffer_size = (curr_ptr - prev_ptr) + 1;
            argv[argc] = malloc(256);
            strncpy(argv[argc], prev_ptr, new_buffer_size);

            int i = 0;
            char tmp[256];
            tmp[i++] = *(++curr_ptr);

            while ( *(++curr_ptr) == '\\' )
                tmp[i++] = *(++curr_ptr);

            while ( ! isspace(*curr_ptr) && *curr_ptr != '\0' )
            {
                if ( *curr_ptr != '\\' )
                    tmp[i++] = *curr_ptr;

                curr_ptr++;
            }

            /*if ( isspace(*curr_ptr) )*/
            /*    strncat(argv[argc++], tmp, i);*/
            strncat(argv[argc++], tmp, i);

            prev_ptr = curr_ptr;
            continue;
        }

        curr_ptr++;
    }


    if ( ! isspace(*prev_ptr) && *prev_ptr != '\0' )
    {
        int new_buffer_size = (curr_ptr - prev_ptr) + 1;
        argv[argc] = malloc(new_buffer_size);
        strncpy(argv[argc], prev_ptr, new_buffer_size);
        argc++;
    }

    argv[argc] = NULL;
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

void run_command(char *argv[])
{
    pid_t pid = fork();

    if ( pid == 0 )
    {
        execv(argv[0], argv);
        perror("execv");
        exit(1);
    }
    else if ( pid < 0 )
        perror("fork");
    else
    {
        int status;
        waitpid( pid, &status, 0 );
    }
}
