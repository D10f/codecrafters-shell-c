#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "command.h"
#include "tokenizer.h"

/**
 * Global variables declared in command.h
 */
bool is_running = true;
unsigned short exit_code = 0;

int main(int argc, char *argv[])
{
    setbuf(stdout, NULL);
    setbuf(stderr, NULL);

    size_t len = 0;
    ssize_t nread;
    Tokenizer *tokenizer = new_tokenizer();

    while ( is_running )
    {
        printf("$ ");

        if ( (nread = getline(&tokenizer->buffer, &len, stdin)) < 0 )
        {
            clear_tokenizer(tokenizer);
            exit(1);
        }

        // If nothing is entered
        if ( nread == 1 )
            continue;

        tokenizer->buffer[nread - 1] = '\0';

        Token *token = tokenizer->tokens;
        while ( token )
        {
            printf("%s\n", token->value);
            token = token->next;
        }

        tokenize(tokenizer);

        // If no tokens are found e.g, only spaces
        if ( tokenizer->arg_count == 0 )
            continue;

        char *argv[tokenizer->arg_count];
        build_argument_list(tokenizer, argv);
        run_command(argv);
        clear_tokens(tokenizer);
    }

    clear_tokenizer(tokenizer);

    return exit_code;
}
