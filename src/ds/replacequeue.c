/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2015 by fanguangping (fanguangping@163.com)
 * replacequeue.c
 */

#include <stdlib.h>
#include "datastructure.h"

ReplaceNode *alloc_replace_node() {
	ReplaceNode *result = (ReplaceNode *) malloc(sizeof(ReplaceNode));
	exit_when_out_of_memory(result);
	return result;
}

void add_replace_statement(Queue *queue, String argName, Statement *replacement) {
	if (queue == NULL || argName == NULL || replacement == NULL)
		return;

	ReplaceNode *node = alloc_replace_node();
	node->argName = argName;
	node->replacement = replacement;
	enqueue(queue, node);
}

Statement *find_replace_statement(Queue *queue, String argName) {
	if (queue == NULL || argName == NULL)
		return NULL;

	ReplaceNode *repNode = NULL;
	QueueNode *node = queue->head;

	while (node != NULL) {
		repNode = (ReplaceNode *) (node->element);
		if (equals_string(argName, repNode->argName)) {
			return repNode->replacement;
		}
		node = node->next;
	}
	return NULL;
}

Queue *find_loop_replace_statement(Queue *queue, String argName) {
	if (queue == NULL || argName == NULL)
		return NULL;

	ReplaceNode *repNode = NULL;
	Queue *result = init_queue();
	QueueNode *node = queue->head;

	while (node != NULL) {
		repNode = (ReplaceNode *) (node->element);
		if (equals_string(argName, repNode->argName)) {
			enqueue(result, repNode->replacement);
		}
		node = node->next;
	}
	return result;
}
