/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2015 by fanguangping (fanguangping@163.com)
 * replacequeue.h
 */

#ifndef REPLACEQUEUE_H_
#define REPLACEQUEUE_H_

#include "string.h"
#include "queue.h"
#include "statement.h"

typedef struct _ReplaceNode {
	String argName;
	Statement *replacement;
} ReplaceNode;

ReplaceNode *alloc_replace_node();

void add_replace_statement(Queue *queue, String argName, Statement *replacement);
Statement *find_replace_statement(Queue *queue, String argName);
Queue *find_loop_replace_statement(Queue *queue, String argName);

#endif /* REPLACEQUEUE_H_ */
