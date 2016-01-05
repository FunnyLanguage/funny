/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2015 by fanguangping (fanguangping@163.com)
 * dfa.h
 */

#ifndef DFA_H_
#define DFA_H_

#include "../ds/datastructure.h"

String match_one_arg(Statement *source);
int orignal_statement(Statement *source);
String match_orignal(Statement *source);
State *test_match(Mapping *mapping, Statement *source, Queue *repQueue);
Statement *expand_statement(Mapping *mapping, Queue *repQueue);
String match(Statement *source);
String eval_normal(Mapping *mapping, Queue *repQueue);

//String gmatch(Statement *source);

#endif /* DFA_H_ */
