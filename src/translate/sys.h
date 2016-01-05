/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2015 by fanguangping (fanguangping@163.com)
 * sys.h
 */

#ifndef SYS_H_
#define SYS_H_

#include "../ds/datastructure.h"

String extract(Statement *s);
String arglist(Statement *s);
String funcname(Statement *s);
String target(Statement *s);

String eval_sys(Mapping *m, Statement *source, Queue *repQueue);
Statement *next_macro_statement(Queue *repQueue, String macro);
int is_macro_char(char c);
String macro_word(char c);
String parse_macro(Queue *repQueue, String macro);

#endif /* SYS_H_ */
