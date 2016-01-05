/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2015 by fanguangping (fanguangping@163.com)
 * parser.h
 */

#ifndef PARSER_H_
#define PARSER_H_

#include <stdio.h>
#include "../ds/datastructure.h"
#include "scanner.h"

Statement *parse_statement(String token, Input *input);
Statement *parse_expression(String token, Input *input);

void eat_whitespace(FILE *in);
int peek(FILE *in);
int has_next_statement(FILE *in);
Statement *next_statement(FILE *in);
Statement *read_statement(FILE *in);

Statement *parse(String funny);
Statement *parse_regex(String funny);


#endif /* PARSER_H_ */
