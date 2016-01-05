/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2015 by fanguangping (fanguangping@163.com)
 * state.c
 */

#include <stdlib.h>
#include <stdio.h>
#include "datastructure.h"

static int stateId = 0;
static HashTable stateTable = NULL;

State *alloc_state() {
	State *result = (State *) malloc(sizeof(State));
	exit_when_out_of_memory(result);
	return result;
}

TransitNode *alloc_transit_node() {
	TransitNode *result = (TransitNode *) malloc(sizeof(TransitNode));
	exit_when_out_of_memory(result);
	return result;
}

static State *new_state() {
	State *s = alloc_state();
	s->id = stateId++;
	s->accept = FALSE;
	return s;
}

static void set_transition(State *starter, State *current, State *next,
		Statement *loop) {
	QueueNode *node = NULL;
	TransitNode *tnode = NULL;
	State *currentState = current;

	if (stateTable == NULL) {
		stateTable = init_table();
	}

	if (loop->children != NULL) {
		node = loop->children->head;
		while (node != loop->children->tail) {
			State *nextState = new_state();
			tnode = alloc_transit_node();
			tnode->starter = starter;
			tnode->input = (Statement *) node->element;
			tnode->next = nextState;
			put_table_element(stateTable, currentState, tnode, hash_code_state);
			currentState = nextState;

			node = node->next;
		}

		tnode = alloc_transit_node();
		tnode->starter = starter;
		tnode->input = (Statement *) loop->children->tail->element;
		tnode->next = next;
		put_table_element(stateTable, currentState, tnode, hash_code_state);
	}
}

State *to_dfa(Statement *s) {
	if (s == NULL)
		return NULL;
	State *current = new_state();
	State *starter = current;
	QueueNode *node = NULL;
	Statement *st = NULL;
	TransitNode *tnode = NULL;

	if (stateTable == NULL) {
		stateTable = init_table();
	}

	if (s->children != NULL) {
		node = s->children->head;
		while (node != NULL) {
			st = (Statement *) node->element;
			switch (st->type) {
			case SYM:
			case ARG:
				tnode = alloc_transit_node();
				tnode->starter = starter;
				tnode->input = st;
				tnode->next = new_state();
				put_table_element(stateTable, current, tnode, hash_code_state);
				current = tnode->next;
				break;

			case LOOP:
				set_transition(starter, current, current, st);
				break;
			}
			node = node->next;
		}
		current->accept = TRUE;
	}

	return starter;
}

State *transit(State *starter, State *state, Statement *input, Queue *repQueue) {
	State *result = NULL;
	TransitNode *tnode = NULL;

	if (state == NULL || input == NULL)
		return NULL;
	switch (input->type) {
	case SYM:
		tnode = get_transit_node_from_table(starter, state, input);
		if (tnode != NULL) {
			result = tnode->next;
		}
		break;

	case ARG:
		tnode = get_transit_node_from_table(starter, state, input);
		if (tnode != NULL) {
			add_replace_statement(repQueue, get_arg_name(tnode->input), input);
			result = tnode->next;
		}
		break;

	case LOOP:
		fprintf(stderr, "transit error!\n");
		break;
	}

	return result;
}

int equals_state(void *a, void *b) {
	if (a == NULL && b == NULL)
		return TRUE;
	if (a == NULL || b == NULL)
		return FALSE;
	return ((State *) a)->id == ((State *) b)->id;
}

int hash_code_state(void *s) {
	if (s == NULL)
		return -1;
	return (((State *) s)->id) % HASHTABLE_SIZE;
}

HashTable getStateTable() {
	return stateTable;
}

TransitNode *get_transit_node_from_table(State *starter, State *state,
		Statement *input) {
	TransitNode *tnode = NULL;
	HashNode *node = (HashNode *) get_table_element(stateTable, state,
			hash_code_state);
	while (node != NULL) {
		tnode = (TransitNode *) node->element;
		if (equals_state(starter, tnode->starter) && match_statement(input,
				tnode->input)) {
			return tnode;
		}
		node = node->next;
	}
	return NULL;
}

