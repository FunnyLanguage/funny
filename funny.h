/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2014 by fanguangping (fanguangping@163.com)
 *
 */

#ifndef FUNNY_H_
#define FUNNY_H_

#include "ds.h"

Input *file_input(FILE *file);
Input *string_input(String string);
int get_char_from_input(Input *input);
int unget_char_from_input(int c, Input *input);
String char_to_string(int c);
int has_next(Input *input);
String next(Input *input);
String read_symbol(int c, Input *input, int escaped);
String read_string(int c, Input *input);
String next_token(Input *input);
Statement *parse_statement(String token, Input *input);
Statement *parse_expression(String token, Input *input);

void eat_whitespace(FILE *in);
int peek(FILE *in);
int has_next_statement(FILE *in);
Statement *next_statement(FILE *in);
Statement *read_statement(FILE *in);

Statement *parse(String funny);
Statement *parse_regex(String funny);

String extract(Statement *s);
String arglist(Statement *s);
String funcname(Statement *s);
String target(Statement *s);

String match_one_arg(String symbol);
int orignal_statement(Statement *source);
String match_orignal(Statement *source);
State *test_match(Mapping *mapping, Statement *source, Queue *repQueue);
String match(Statement *source);

String eval_sys(Mapping *m, Statement *source, Queue *repQueue);
Statement *next_macro_statement(Queue *repQueue, String macro);
int is_macro_char(char c);
String macro_word(char c);
String parse_macro(Queue *repQueue, String macro);

Queue *mappings;
Queue *functions;

void load_core_template();
void load();
Queue *get_mappings();
Queue *get_functions();
String get_sys_macro_string(String s, FILE *fp);
Queue *parse_sys_macro();
void match_template(Statement *source);
void load_template(String fileName);

int funny_init();
void wait_to_exit(Statement *st);

#endif /* FUNNY_H_ */
