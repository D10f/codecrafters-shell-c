#ifndef __TOKENIZER_H__
#define __TOKENIZER_H__

#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum TokenType {
    DOUBLE_QUOTED,
    EOL,
    ESCAPE,
    SINGLE_QUOTED,
    SPACE,
    UNDEF,
    UNQUOTED,
} TokenType;

typedef struct Token {
    char *value;
    struct Token *next;
} Token;

typedef struct Tokenizer {
    char *buffer;
    size_t cursor;
    Token *tokens;
    TokenType state;
    unsigned int arg_count;
} Tokenizer;

Token *new_token();

Tokenizer *new_tokenizer();

void clear_tokens(Tokenizer *tokenizer);

void clear_tokenizer(Tokenizer *tokenizer);

void insert_token(Tokenizer *tokenizer, Token *token);

TokenType get_token_type(char c);

void tokenize(Tokenizer *tokenizer);

#endif // !__TOKENIZER_H__
