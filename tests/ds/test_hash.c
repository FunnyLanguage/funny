/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2015 by fanguangping (fanguangping@163.com)
 * test_hash.c
 */

#include <assert.h>
#include <string.h>
#include "test_hash.h"
#include "../../src/ds/datastructure.h"

void test_alloc_hash_table() {
	HashTable table = alloc_hash_table();
	assert(table != NULL);
}

void test_alloc_hash_node() {
	HashNode *node = alloc_hash_node();
	assert(node != NULL);
}

void test_init_table() {
	HashTable table = init_table();
	assert(table != NULL);
	assert(table[0] == NULL);
	assert(table[HASHTABLE_SIZE - 1] == NULL);
}

void test_put_table_element() {
	HashTable table = init_table();
	put_table_element(NULL, NULL, NULL, NULL);
	put_table_element(table, "abc", "123", hash_code_string);
	put_table_element(table, "def", "456", hash_code_string);
	assert(strcmp(table[hash_code_string("abc")]->element, "123") == 0);
	assert(strcmp(table[hash_code_string("def")]->element, "456") == 0);
}

void test_get_table_element() {
	HashTable table = init_table();
	put_table_element(table, "abc", "123", hash_code_string);
	put_table_element(table, "def", "456", hash_code_string);
	put_table_element(table, "def", "789", hash_code_string);
	assert(strcmp(get_table_element(table, "abc", hash_code_string)->element, "123") == 0);
	assert(strcmp(get_table_element(table, "def", hash_code_string)->element, "456") == 0);
	assert(strcmp(get_table_element(table, "def", hash_code_string)->next->element, "789") == 0);
	assert(get_table_element(NULL, NULL, NULL) == NULL);
}

void test_clear_table() {
	clear_table(NULL);
	HashTable table = init_table();
	put_table_element(table, "abc", "123", hash_code_string);
	clear_table(table);
	assert(get_table_element(table, "abc", hash_code_string) == NULL);
}

void test_hash() {
	test_alloc_hash_table();
	test_alloc_hash_node();
	test_init_table();
	test_put_table_element();
	test_get_table_element();
	test_clear_table();
}
