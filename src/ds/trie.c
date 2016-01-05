/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2015 by fanguangping (fanguangping@163.com)
 * trie.c
 */

#include <stdlib.h>
#include "datastructure.h"

Trie *alloc_trie() {
	Trie *result = (Trie *) malloc(sizeof(Trie));
	exit_when_out_of_memory(result);
	return result;
}

TrieNode *alloc_trie_node() {
	TrieNode *result = (TrieNode *) malloc(sizeof(TrieNode));
	exit_when_out_of_memory(result);
	result->next = (TrieNode **) calloc(TRIE_NODE_SIZE, sizeof(TrieNode*));
	result->queue = init_queue();
	return result;
}

Trie *init_trie(CalcPrefixFunc addPrefixFunc, CalcPrefixFunc findPrefixFunc) {
	Trie *trie = alloc_trie();
	trie->root = alloc_trie_node();
	trie->table = init_table();
	trie->addPrefixFunc = addPrefixFunc;
	trie->findPrefixFunc = findPrefixFunc;
	return trie;
}

int prefix_out_of_range(char *prefix) {
	if (prefix == NULL)
		return TRUE;
	char *p = prefix;
	while (*p) {
		if (*p < BASE_CHAR || *p > 'z') {
			return TRUE;
		}
		p++;
	}
	return FALSE;
}

void trie_add(Trie *trie, void *element) {
	if (trie == NULL || element == NULL)
		return;
	TrieNode *node = trie->root;
	char *p = trie->addPrefixFunc(element);
	if (prefix_out_of_range(p)) {
		put_table_element(trie->table, p, element, hash_code_string);
		return;
	}

	int index;
	while (*p) {
		index = *p - BASE_CHAR;
		if (node->next[index] == NULL) {
			node->next[index] = alloc_trie_node();
		}
		node = node->next[index];
		p++;
	}
	enqueue(node->queue, element);
}

Queue *trie_find(Trie *trie, void *element) {
	TrieNode *node = trie->root;
	char *p = trie->findPrefixFunc(element);
	if (prefix_out_of_range(p)) {
		HashNode *hashNode = get_table_element(trie->table, p, hash_code_string);
		Queue *queue = init_queue();
		while (hashNode != NULL) {
			enqueue(queue, hashNode->element);
			hashNode = hashNode->next;
		}
		return queue;
	}

	int index;
	while (*p && node) {
		index = *p - BASE_CHAR;
		node = node->next[index];
		p++;
	}
	if (node == NULL)
		return NULL;
	return node->queue;
}
