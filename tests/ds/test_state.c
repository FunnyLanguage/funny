/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2015 by fanguangping (fanguangping@163.com)
 * test_state.c
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "test_datastructure.h"
#include "test_state.h"
#include "../../src/ds/datastructure.h"

void test_alloc_state() {
	State *state = alloc_state();
	assert(state != NULL);
}

void test_alloc_transit_node() {
	TransitNode *node = alloc_transit_node();
	assert(node != NULL);
}

static State * goto_next(State *state) {
	HashTable stateTable = getStateTable();
	HashNode *node = (HashNode *) get_table_element(stateTable, state, hash_code_state);
	return ((State *) (((TransitNode *)(node->element))->next));
}

static State * exit_loop(State *state) {
	HashTable stateTable = getStateTable();
	HashNode *node = (HashNode *) get_table_element(stateTable, state, hash_code_state);
	return ((State *) (((TransitNode *)(node->next->element))->next));
}

void test_to_dfa() {
	assert(to_dfa(NULL) == NULL);

	// {define function {add}: {add {a} {b}} as {{a} + {b}}}
	State *state1 = to_dfa(create_statement());
	// {for the conditions: [when {a} then {b} ,]}
	State *state2 = to_dfa(create_loop_statement());
	// {let [{a}={b},] eval: [{c}]}
	State *state3 = to_dfa(create_loop_statement1());

	State *state = state1;          //[start]
	assert(state->accept == FALSE);
	state = goto_next(state);       //define
	assert(state->accept == FALSE);
	state = goto_next(state);       //function
	assert(state->accept == FALSE);
	state = goto_next(state);       //{add}
	assert(state->accept == FALSE);
	state = goto_next(state);       //:
	assert(state->accept == FALSE);
	state = goto_next(state);       //{add {a} {b}}
	assert(state->accept == FALSE);
	state = goto_next(state);       //as
	assert(state->accept == FALSE);
	state = goto_next(state);       //{{a} + {b}}
	assert(state->accept == TRUE);

	state = state2;                 //[start]
	assert(state->accept == FALSE);
	state = goto_next(state);       //for
	assert(state->accept == FALSE);
	state = goto_next(state);       //the
	assert(state->accept == FALSE);
	state = goto_next(state);       //conditions:
	assert(state->accept == TRUE);
	state = goto_next(state);       //when
	assert(state->accept == FALSE);
	state = goto_next(state);       //{a}
	assert(state->accept == FALSE);
	state = goto_next(state);       //then
	assert(state->accept == FALSE);
	state = goto_next(state);       //{b}
	assert(state->accept == FALSE);
	state = goto_next(state);       //,
	assert(state->accept == TRUE);
	state = goto_next(state);       //when
	assert(state->accept == FALSE);
	state = goto_next(state);       //{a}
	assert(state->accept == FALSE);
	state = goto_next(state);       //then
	assert(state->accept == FALSE);
	state = goto_next(state);       //{b}
	assert(state->accept == FALSE);
	state = goto_next(state);       //,
	assert(state->accept == TRUE);

	state = state3;                 //[start]
	assert(state->accept == FALSE);
	state = goto_next(state);       //let
	assert(state->accept == FALSE);
	state = goto_next(state);       //{a}
	assert(state->accept == FALSE);
	state = goto_next(state);       //=
	assert(state->accept == FALSE);
	state = goto_next(state);       //{b}
	assert(state->accept == FALSE);
	state = goto_next(state);       //,
	assert(state->accept == FALSE);
	state = goto_next(state);       //{a}
	assert(state->accept == FALSE);
	state = goto_next(state);       //=
	assert(state->accept == FALSE);
	state = goto_next(state);       //{b}
	assert(state->accept == FALSE);
	state = goto_next(state);       //,
	assert(state->accept == FALSE);
	state = exit_loop(state);       //eval:
	assert(state->accept == TRUE);
	state = goto_next(state);       //{c}
	assert(state->accept == TRUE);
	state = goto_next(state);       //{c}
	assert(state->accept == TRUE);
}

void test_transit() {
	// {{x} or {y}}
	State *starter = to_dfa(create_source_statement1());
	Statement *s1 = create_source_statement1();
	Statement *s2 = create_source_statement1();
	State *state = starter;
	Queue *repQueue = init_queue();

	state = transit(starter, state, s1, repQueue);
	state = transit(starter, state, init_sym("or"), repQueue);
	state = transit(starter, state, s2, repQueue);
	assert(state->accept == TRUE);
	ReplaceNode *node1 = get_queue_element(repQueue, 0);
	ReplaceNode *node2 = get_queue_element(repQueue, 1);
	assert(strcmp("x", node1->argName) == 0);
	assert(node1->replacement == s1);
	assert(strcmp("y", node2->argName) == 0);
	assert(node2->replacement == s2);
}

void test_equals_state() {
	State *state = alloc_state();
	state->id = 123;
	assert(equals_state(state, state));
	assert(!equals_state(state, NULL));
	assert(equals_state(NULL, NULL));
}

void test_hash_code_state() {
	State *state = alloc_state();
	state->id = 123;
	assert(hash_code_state(state) == hash_code_state(state));
}

void test_get_transit_node_from_table() {
	// {let [{a}={b},] eval: [{c}]}
	State *state1 = to_dfa(create_loop_statement1());
	State *state = state1;
	TransitNode *node = NULL;
	node = get_transit_node_from_table(state1, state, init_sym("let"));
	assert(node != NULL);
	state = goto_next(state);
	node = get_transit_node_from_table(state1, state, init_arg());
	assert(node != NULL);
	state = goto_next(state);
	node = get_transit_node_from_table(state1, state, init_sym("="));
	assert(node != NULL);
	state = goto_next(state);
	node = get_transit_node_from_table(state1, state, init_arg());
	assert(node != NULL);
	state = goto_next(state);
	node = get_transit_node_from_table(state1, state, init_sym(","));
	assert(node != NULL);
	state = goto_next(state);
	node = get_transit_node_from_table(state1, state, init_arg());
	assert(node != NULL);
	state = goto_next(state);
	node = get_transit_node_from_table(state1, state, init_sym("="));
	assert(node != NULL);
	state = goto_next(state);
	node = get_transit_node_from_table(state1, state, init_arg());
	assert(node != NULL);
	state = goto_next(state);
	node = get_transit_node_from_table(state1, state, init_sym(","));
	assert(node != NULL);
	state = goto_next(state);
	node = get_transit_node_from_table(state1, state, init_sym("eval:"));
	assert(node != NULL);
	state = exit_loop(state);
	node = get_transit_node_from_table(state1, state, init_arg());
	assert(node != NULL);
	state = goto_next(state);
	node = get_transit_node_from_table(state1, state, init_arg());
	assert(node != NULL);
	state = goto_next(state);
	node = get_transit_node_from_table(state1, state, init_arg());
	assert(node != NULL);
}

void test_state() {
  test_alloc_state();
  test_alloc_transit_node();
  test_to_dfa();
  test_transit();
  test_equals_state();
  test_hash_code_state();
  test_get_transit_node_from_table();
}
