/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2015 by fanguangping (fanguangping@163.com)
 * statement.h
 */

#ifndef STATEMENT_H_
#define STATEMENT_H_

#include "string.h"
#include "queue.h"

typedef enum {
	SYM, ARG, LOOP
} StatementType;

typedef struct _Statement {
	String symbol;
	StatementType type;		//1-SYM, 2-ARG, 3-LOOP
	Queue *children;		//element is struct _Statement
} Statement;

Statement *alloc_statement();

Statement *init_sym(String symbol);
Statement *init_arg();
Statement *init_loop();
String get_arg_name(Statement *s);
void set_arg_name(Statement *s, String name);
int equal_sym(Statement *s, String sym);
int equal_arg(Statement *s, String sym);
int empty_statement(Statement *s);
int equal_statement(Statement *a, Statement *b);
String to_string_statement(Statement *s);
Statement *copy_statement(Statement *s);
char *calc_statement_prefix(void *element);
int match_statement(Statement *input, Statement *base);

#endif /* STATEMENT_H_ */
