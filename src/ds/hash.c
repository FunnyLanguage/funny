/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2015 by fanguangping (fanguangping@163.com)
 * hash.c
 */

#include <stdlib.h>
#include <string.h>
#include "datastructure.h"

HashTable alloc_hash_table() {
	HashTable result = (HashTable) malloc(HASHTABLE_SIZE * sizeof(HashNode *));
	exit_when_out_of_memory(result);
	return result;
}

HashNode *alloc_hash_node() {
	HashNode *result = (HashNode *) malloc(sizeof(HashNode));
	exit_when_out_of_memory(result);
	return result;
}

HashTable init_table() {
	HashTable table = alloc_hash_table();
	int i;
	for (i = 0; i < HASHTABLE_SIZE; i++) {
		table[i] = NULL;
	}
	return table;
}

void put_table_element(HashTable table, void *key, void *value,
		int (*hash_code)(void *key)) {
	HashNode *valueNode = NULL;
	int hash = 0;
	HashNode *node = NULL;
	if (table == NULL || key == NULL || value == NULL || hash_code == NULL)
		return;

	valueNode = alloc_hash_node();
	valueNode->element = value;
	valueNode->next = NULL;
	hash = hash_code(key);
	node = table[hash];

	if (node == NULL) {
		table[hash] = valueNode;
	} else {
		while (node->next != NULL)
			node = node->next;
		node->next = valueNode;
	}
}

HashNode *get_table_element(HashTable table, void *key,
		int (*hash_code)(void *key)) {
	if (table == NULL || key == NULL || hash_code == NULL)
		return NULL;
	else
		return table[hash_code(key)];
}

void clear_table(HashTable table) {
	HashNode *node = NULL, *p = NULL;
	int i;
	if (table == NULL)
		return;

	for (i = 0; i < HASHTABLE_SIZE; i++) {
		p = table[i];
		while (p != NULL) {
			node = p;
			p = node->next;
			free(node);
		}
		table[i] = NULL;
	}
}

//================================

#define MY_HASH_TABLE_MIN_SIZE 11
#define MY_HASH_TABLE_MAX_SIZE 13845163

MyGHashTable* my_g_hash_table_new(MyGHashFunc hash_func, MyGEqualFunc key_equal_func) {
	MyGHashTable *hash_table;
	hash_table = (MyGHashTable *) malloc(sizeof(MyGHashTable));
	exit_when_out_of_memory(hash_table);
	hash_table->size = MY_HASH_TABLE_MIN_SIZE;
	hash_table->nnodes = 0;
	hash_table->hash_func = hash_func;
	hash_table->key_equal_func = key_equal_func;
	hash_table->nodes = (MyGHashNode **) calloc(hash_table->size, sizeof(MyGHashNode*));
	exit_when_out_of_memory(hash_table->nodes);
	return hash_table;
}

MyGHashNode **my_g_hash_table_lookup_node(MyGHashTable *hash_table, const void *key,
		unsigned int *hash_return) {
	MyGHashNode **node_ptr, *node;
	unsigned int hash_value;

	hash_value = (*hash_table->hash_func)(key);
	node_ptr = &hash_table->nodes[hash_value % hash_table->size];

	if (hash_return)
		*hash_return = hash_value;

	if (hash_table->key_equal_func) {
		while ((node = *node_ptr)) {
			if (node->key_hash == hash_value && hash_table->key_equal_func(
					node->key, key))
				break;
			node_ptr = &(*node_ptr)->next;
		}
	} else {
		while ((node = *node_ptr)) {
			if (node->key == key)
				break;

			node_ptr = &(*node_ptr)->next;
		}
	}

	return node_ptr;
}

static const unsigned int g_primes[] = { 11, 19, 37, 73, 109, 163, 251, 367,
		557, 823, 1237, 1861, 2777, 4177, 6247, 9371, 14057, 21089, 31627,
		47431, 71143, 106721, 160073, 240101, 360163, 540217, 810343, 1215497,
		1823231, 2734867, 4102283, 6153409, 9230113, 13845163, };

static const unsigned int g_nprimes = sizeof(g_primes) / sizeof(g_primes[0]);

static unsigned int g_spaced_primes_closest(unsigned int num) {
	int i;

	for (i = 0; i < g_nprimes; i++)
		if (g_primes[i] > num)
			return g_primes[i];

	return g_primes[g_nprimes - 1];
}

#define MY_CLAMP(x, low, high)  (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))

void my_g_hash_table_resize(MyGHashTable *hash_table) {
	MyGHashNode **new_nodes;
	MyGHashNode *node;
	MyGHashNode *next;
	unsigned int hash_val;
	int new_size;
	int i;

	new_size = g_spaced_primes_closest(hash_table->nnodes);
	new_size = MY_CLAMP (new_size, MY_HASH_TABLE_MIN_SIZE, MY_HASH_TABLE_MAX_SIZE);

	new_nodes = (MyGHashNode **) calloc(new_size, sizeof(MyGHashNode*));

	for (i = 0; i < hash_table->size; i++)
		for (node = hash_table->nodes[i]; node; node = next) {
			next = node->next;

			hash_val = node->key_hash % new_size;

			node->next = new_nodes[hash_val];
			new_nodes[hash_val] = node;
		}

	free(hash_table->nodes);
	hash_table->nodes = new_nodes;
	hash_table->size = new_size;
}

void my_g_hash_table_maybe_resize(MyGHashTable *hash_table) {
	int nnodes = hash_table->nnodes;
	int size = hash_table->size;

	if ((size >= 3 * nnodes && size > MY_HASH_TABLE_MIN_SIZE) || (3 * size
			<= nnodes && size < MY_HASH_TABLE_MAX_SIZE))
		my_g_hash_table_resize(hash_table);
}

void my_g_hash_table_insert(MyGHashTable *hash_table, void * key, void * value) {
	MyGHashNode **node_ptr, *node;
	unsigned int key_hash;

	node_ptr = my_g_hash_table_lookup_node(hash_table, key, &key_hash);

	if ((node = *node_ptr)) {
		node->value = value;
	} else {
		node = (MyGHashNode *) malloc(sizeof(MyGHashNode));

		node->key = key;
		node->value = value;
		node->key_hash = key_hash;
		node->next = NULL;

		*node_ptr = node;
		hash_table->nnodes++;
		my_g_hash_table_maybe_resize(hash_table);
	}
}

void my_g_hash_table_foreach(MyGHashTable *hash_table, MyGHFunc func, void * user_data) {
	MyGHashNode *node;
	int i;

	for (i = 0; i < hash_table->size; i++)
		for (node = hash_table->nodes[i]; node; node = node->next)
			(*func)(node->key, node->value, user_data);
}

int my_g_str_equal(const void * v1, const void * v2) {
	const char *string1 = (const char *) v1;
	const char *string2 = (const char *) v2;

	return strcmp(string1, string2) == 0;
}

unsigned int my_g_str_hash(const void * v) {
	/* 31 bit hash function */
	const signed char *p = v;
	unsigned int h = *p;

	if (h)
		for (p += 1; *p != '\0'; p++)
			h = (h << 5) - h + *p;

	return h;
}

