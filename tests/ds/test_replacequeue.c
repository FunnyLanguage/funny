/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2015 by fanguangping (fanguangping@163.com)
 * test_replacequeue.c
 */

#include <assert.h>
#include <string.h>
#include "test_datastructure.h"
#include "test_replacequeue.h"
#include "../../src/ds/datastructure.h"

void test_alloc_replace_node() {
	ReplaceNode *node = alloc_replace_node();
	assert(node != NULL);
}

void test_add_replace_statement() {
	add_replace_statement(NULL, NULL, NULL);
}

void test_find_replace_statement() {
	String argName1 = "a";
	String argName2 = "b";
	Statement *replacement1 = create_source_statement1();
	Statement *replacement2 = create_target_statement1();

	Queue *queue = init_queue();
	add_replace_statement(queue, argName1, replacement1);
	add_replace_statement(queue, argName2, replacement2);

	assert(find_replace_statement(queue, argName1) == replacement1);
	assert(find_replace_statement(queue, argName2) == replacement2);
	assert(find_replace_statement(queue, NULL) == NULL);
	assert(find_replace_statement(NULL, argName1) == NULL);
	assert(find_replace_statement(NULL, NULL) == NULL);
}

void test_find_loop_replace_statement() {
	String argName1 = "a";
	String argName2 = "b";
	Statement *replacement1 = create_replacement1();
	Statement *replacement2 = create_replacement2();
	Statement *replacement3 = create_replacement3();
	Statement *replacement4 = create_replacement4();

	Queue *queue = init_queue();
	add_replace_statement(queue, argName1, replacement1);
	add_replace_statement(queue, argName2, replacement2);
	add_replace_statement(queue, argName1, replacement3);
	add_replace_statement(queue, argName2, replacement4);

	Queue *replacements_for_arg1 = find_loop_replace_statement(queue, argName1);
	Queue *replacements_for_arg2 = find_loop_replace_statement(queue, argName2);

	assert(replacements_for_arg1->head->element == replacement1);
	assert(replacements_for_arg1->head->next->element == replacement3);
	assert(replacements_for_arg2->head->element == replacement2);
	assert(replacements_for_arg2->head->next->element == replacement4);
	assert(find_replace_statement(queue, NULL) == NULL);
	assert(find_replace_statement(NULL, argName1) == NULL);
	assert(find_replace_statement(NULL, NULL) == NULL);
}

void test_replacequeue() {
	test_alloc_replace_node();
	test_add_replace_statement();
	test_find_replace_statement();
	test_find_loop_replace_statement();
}
