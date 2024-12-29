#include "command.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static char *builtins[] = {
    "cd",
    "echo",
    "exit",
    "pwd",
    "type",
};

static int total_builtins = sizeof(builtins) / sizeof(char *);

void run_builtin(char *argv[])
{
    if ( strncmp(argv[0], "cd", strlen(argv[0])) == 0 )
    {
        if ( ! argv[1] )
            return;

        if ( chdir(argv[1]) == -1 )
            printf("cd: %s: No such file or directory\n", argv[1]);
    }
    else if ( strncmp(argv[0], "echo", strlen(argv[0])) == 0 )
    {
        if ( ! argv[1] )
            printf("\n");
        else
        {
            char *arg;
            int i = 1;
            while ( (arg = argv[i++]) )
                printf("%s ", arg);

            printf("\n");
        }
    }
    else if ( strncmp(argv[0], "exit", strlen(argv[0])) == 0 )
    {
        exit_code = argv[1] ? atoi(argv[1]) : 0;
        is_running = false;
        return;
    }
    else if ( strncmp(argv[0], "pwd", strlen(argv[0])) == 0 )
    {
        char cwd[1024];
        getcwd(cwd, 1024);
        printf("%s\n", cwd);
    }
    else if ( strncmp(argv[0], "type", strlen(argv[0])) == 0 )
    {
        if ( ! argv[1] )
            return;
        else if ( is_builtin(argv[1] ))
            printf("%s is a shell builtin\n", argv[1]);
        else
        {
            char *filepath;
            if ( (filepath = find_command(argv[1])) )
            {
                printf("%s is %s/%s\n", argv[1], filepath, argv[1]);
                free(filepath);
            }
            else
                fprintf(stderr, "%s: not found\n", argv[1]);
        }

        return;
    }
}

bool is_builtin(char *cmd)
{
    for ( int i = 0; i < total_builtins; i++ )
        if ( strncmp(cmd, builtins[i], strlen(cmd)) == 0 )
            return true;

    return false;
}

void build_argument_list(Tokenizer *tokenizer, char *argv[])
{
    Token *token = tokenizer->tokens;

    int i = 0;
    while ( token )
    {
        argv[i++] = token->value;
        token = token->next;
    }

    argv[i] = NULL;
}

char *find_command(char *cmd)
{
    char *path = getenv("PATH");
    path = strndup(path, strlen(path));
    path = strtok(path, ":");

    struct dirent *dirent;

    do {
        if ( strncmp(path, "/usr/bin", strlen("/usr/bin")) == 0 )
            continue;

        DIR *directory = opendir( path );

        if ( ! directory )
            continue;

        while ( ( dirent = readdir( directory ) ))
        {
            if ( strncmp(cmd, dirent->d_name, strlen(cmd)) == 0 )
            {
                closedir(directory);
                return path;
            }
        }

        closedir(directory);

    } while ( (path = strtok(NULL, ":")) );

    return NULL;
}

void exec_command(char *argv[])
{
    pid_t pid = fork();

    if ( pid == 0 )
    {
        execv(argv[0], argv);
        perror("execv");
        exit(1);
    }
    else if ( pid < 0 )
        perror("Error forking child process.");
    else
    {
        int status;
        waitpid( pid, &status, 0 );
    }
}

void run_command(char *argv[])
{
    if ( is_builtin(argv[0]) )
    {
        run_builtin(argv);
        return;
    }

    char *filepath;
    if ( (filepath = find_command(argv[0])) )
    {
        char cmd_path[256];
        sprintf(cmd_path, "%s/%s", filepath, argv[0]);
        strncpy(argv[0], cmd_path, 256);
        exec_command(argv);
        return;
    }

    fprintf(stderr, "%s: command not found\n", argv[0]);
}
