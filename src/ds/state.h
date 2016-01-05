/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2015 by fanguangping (fanguangping@163.com)
 * state.h
 */

#ifndef STATE_H_
#define STATE_H_

#include "queue.h"
#include "statement.h"

typedef struct _State {
	int id;
	int accept;
} State;

typedef struct _TransitNode {
	State *starter;
	Statement *input;
	State *next;
} TransitNode;

State *alloc_state();
TransitNode *alloc_transit_node();

State *to_dfa(Statement *s);
State *transit(State *starter, State *start, Statement *input, Queue *repQueue);
int equals_state(void *a, void *b);
int hash_code_state(void *s);
HashTable getStateTable();
TransitNode *get_transit_node_from_table(State *starter, State *start, Statement *input);

#endif /* STATE_H_ */
