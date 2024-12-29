#include "tokenizer.h"

Token *new_token()
{
    Token *token = malloc(sizeof(Token));
    token->value = malloc(sizeof(char) * 256);
    token->next = NULL;
    return token;
}

Tokenizer *new_tokenizer()
{
    Tokenizer *tokenizer = malloc(sizeof(Tokenizer));
    tokenizer->buffer = NULL;
    tokenizer->cursor = 0;
    tokenizer->state = UNDEF;
    tokenizer->tokens = NULL;
    return tokenizer;
}

void clear_tokens(Tokenizer *tokenizer)
{
    Token *prev = NULL;
    Token *curr = tokenizer->tokens;

    while ( curr )
    {
        prev = curr;
        curr = curr->next;
        free(prev->value);
        free(prev);
    }

    tokenizer->tokens = NULL;
    tokenizer->arg_count = 0;
}

void clear_tokenizer(Tokenizer *tokenizer)
{
    clear_tokens(tokenizer);
    free(tokenizer->buffer);
    free(tokenizer);
}

void insert_token(Tokenizer *tokenizer, Token *token)
{
    Token *head = tokenizer->tokens;

    if ( head == NULL )
    {
        tokenizer->tokens = token;
        tokenizer->arg_count++;
        return;
    }

    while ( head->next )
        head = head->next;

    head->next = token;
    tokenizer->arg_count++;
}

TokenType get_token_type(char c)
{
    switch (c) {
        case ' ':
        case '\t':
        case '\n':
        case '\r':
            return SPACE;
        case '\\':
            return ESCAPE;
        case '\'':
            return SINGLE_QUOTED;
        case '"':
            return DOUBLE_QUOTED;
        case '\0':
            return EOL;
        default:
            return UNQUOTED;

    }
}

void tokenize(Tokenizer *tokenizer)
{
    Token *token = new_token();
    size_t cursor = 0;

    char current_char;
    TokenType current_char_type;

    tokenizer->state = get_token_type(tokenizer->buffer[0]);

    int i = 0;

    if (tokenizer->state == SINGLE_QUOTED ||
        tokenizer->state == DOUBLE_QUOTED)
        i++;

    for ( ; i <= strlen(tokenizer->buffer); ++i )
    {
        current_char = tokenizer->buffer[i];
        current_char_type = get_token_type(current_char);

        switch ( tokenizer->state )
        {
            case SPACE:
                if ( current_char_type == SPACE )
                    continue;

                if ( current_char_type == EOL )
                {
                    free(token);
                    continue;
                }

                if ( current_char_type == UNQUOTED )
                {
                    token->value[cursor++] = current_char;
                    tokenizer->state = UNQUOTED;
                    continue;
                }

                if ( current_char_type == ESCAPE )
                {
                    token->value[cursor++] = tokenizer->buffer[++i];
                    tokenizer->state = UNQUOTED;
                    continue;
                }

                if (current_char_type == SINGLE_QUOTED ||
                    current_char_type == DOUBLE_QUOTED)
                {
                    tokenizer->state = current_char_type;
                    continue;
                }

            case UNQUOTED:
                if (current_char_type == SPACE ||
                    current_char_type == SINGLE_QUOTED ||
                    current_char_type == DOUBLE_QUOTED)
                {
                    token->value[cursor] = '\0';
                    insert_token(tokenizer, token);
                    cursor = 0;
                    token = new_token();
                    tokenizer->state = current_char_type;
                }
                else if ( current_char_type == EOL )
                {
                    token->value[cursor] = '\0';
                    insert_token(tokenizer, token);
                }
                else if ( current_char_type == ESCAPE )
                {
                    token->value[cursor++] = tokenizer->buffer[++i];
                }
                else if ( current_char_type == UNQUOTED )
                {
                    token->value[cursor++] = current_char;
                }
                continue;

            case SINGLE_QUOTED:
                if ( current_char_type != SINGLE_QUOTED )
                {
                    token->value[cursor++] = current_char;
                    continue;
                }

                char next = tokenizer->buffer[i + 1];
                TokenType next_type = get_token_type(next);

                switch (next) {
                    case ' ':
                        token->value[cursor] = '\0';
                        insert_token(tokenizer, token);
                        cursor = 0;
                        token = new_token();
                        tokenizer->state = SPACE;
                        continue;
                    case '\0':
                        token->value[cursor] = '\0';
                        insert_token(tokenizer, token);
                        continue;
                    case '\'':
                    case '"':
                        i++;
                        break;
                }

                tokenizer->state = next_type;
                continue;

            case DOUBLE_QUOTED:
                if ( current_char_type == ESCAPE )
                {
                    char next = tokenizer->buffer[i + 1];
                    switch (next) {
                        case '"':
                        case '\\':
                        case '$':
                            token->value[cursor++] = tokenizer->buffer[++i];
                            continue;
                        default:
                            token->value[cursor++] = current_char;
                            continue;
                    }
                }

                if ( current_char_type != DOUBLE_QUOTED )
                {
                    token->value[cursor++] = current_char;
                    continue;
                }

                next = tokenizer->buffer[i + 1];
                next_type = get_token_type(next);

                switch (next) {
                    case ' ':
                        token->value[cursor] = '\0';
                        insert_token(tokenizer, token);
                        cursor = 0;
                        token = new_token();
                        tokenizer->state = SPACE;
                        break;
                    case '\0':
                        token->value[cursor] = '\0';
                        insert_token(tokenizer, token);
                        break;
                    case '\'':
                    case '"':
                        i++;
                        break;
                }

                tokenizer->state = next_type;
                continue;

            case EOL:
                break;

            default:
               printf("This shouldn't happen\n");
                exit(1);
        }
    }
}
