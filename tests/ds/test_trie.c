/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2015 by fanguangping (fanguangping@163.com)
 * test_trie.c
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "test_datastructure.h"
#include "test_trie.h"
#include "../../src/ds/datastructure.h"

void test_alloc_trie() {
	Trie *trie = alloc_trie();
	assert(trie != NULL);
}

void test_alloc_trie_node() {
	TrieNode *node = alloc_trie_node();
	assert(node != NULL);
}

void test_init_trie() {
	Trie *trie = init_trie(NULL, NULL);
	assert(trie != NULL);
	assert(trie->root != NULL);
	assert(trie->table != NULL);
}

void test_prefix_out_of_range() {
	assert(prefix_out_of_range(NULL));
	assert(!prefix_out_of_range("def"));
	assert(!prefix_out_of_range("`d"));
	assert(prefix_out_of_range("`+`"));
	assert(prefix_out_of_range("!@#"));
}

void test_trie_add() {
	Mapping *mapping1 = create_sys_mapping(); //def
	Trie *trie = init_trie(calc_mapping_prefix, calc_statement_prefix);
	trie_add(trie, mapping1);
	Queue *queue = NULL;
	queue = trie->root->next['d' - BASE_CHAR]->next['e' - BASE_CHAR]->next['f' - BASE_CHAR]->queue;
	assert(queue->head->element == mapping1);

	Mapping *mapping2 = create_mapping(); //`+`
	trie_add(trie, mapping2);
	HashTable table = trie->table;
	HashNode *node = get_table_element(table, "`+`", hash_code_string);
	assert(node->element == mapping2);

	trie_add(NULL, mapping1);
	trie_add(trie, NULL);
	trie_add(NULL, NULL);
}

// {define template {exit}: {exit} as {(exit)}}
static Statement *create_statement1() {
	Statement *s = (Statement *)malloc(sizeof(Statement));
	Statement *arg1 = NULL;
	Statement *arg2 = NULL;
	Statement *arg3 = NULL;
	s->symbol = NULL;
	s->type = ARG;
	s->children = init_queue();
	add_statement(s, "define", SYM);
	add_statement(s, "template", SYM);
	arg1 = add_statement(s, NULL, ARG);
	add_statement(arg1, "exit", SYM);
	add_statement(s, ":", SYM);
	arg2 = add_statement(s, NULL, ARG);
	add_statement(arg2, "exit", SYM);
	add_statement(s, "as", SYM);
	arg3 = add_statement(s, NULL, ARG);
	add_statement(arg3, "(exit)", SYM);
	return s;
}

// {{a} + {b}}
Statement *create_statement2() {
	Statement *s = (Statement *)malloc(sizeof(Statement));
	Statement *arg1 = NULL;
	Statement *arg2 = NULL;
	s->symbol = NULL;
	s->type = ARG;
	s->children = init_queue();
	arg1 = add_statement(s, NULL, ARG);
	add_statement(arg1, "a", SYM);
	add_statement(s, "+", SYM);
	arg2 = add_statement(s, NULL, ARG);
	add_statement(arg2, "b", SYM);
	return s;
}

void test_trie_find() {
	Mapping *mapping1 = create_sys_mapping(); //def
	Trie *trie = init_trie(calc_mapping_prefix, calc_statement_prefix);
	trie_add(trie, mapping1);
	Mapping *mapping2 = create_mapping(); //`+`
	trie_add(trie, mapping2);

	Queue *queue = NULL;
	queue = trie_find(trie, create_statement1());
	assert(queue->head->element == mapping1);
	queue = trie_find(trie, create_statement2());
	assert(queue->head->element == mapping2);
	queue = trie_find(trie, create_source_statement1());
	assert(queue == NULL);
}

void test_trie() {
  test_alloc_trie();
  test_alloc_trie_node();
  test_init_trie();
  test_prefix_out_of_range();
  test_trie_add();
  test_trie_find();
}
