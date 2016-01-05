/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2015 by fanguangping (fanguangping@163.com)
 * trie.h
 */

#ifndef TRIE_H_
#define TRIE_H_

#include "queue.h"
#include "hash.h"

#define TRIE_NODE_SIZE    27
#define MAX_TRIE_HEIGHT   3
#define BASE_CHAR         96

typedef char * (*CalcPrefixFunc)(void *);

typedef struct _TrieNode {
	Queue *queue;
	struct _TrieNode **next;
} TrieNode;

typedef struct _Trie {
	TrieNode *root;
	HashTable table;
	CalcPrefixFunc addPrefixFunc;
	CalcPrefixFunc findPrefixFunc;
} Trie;

Trie *alloc_trie();
TrieNode *alloc_trie_node();

Trie *init_trie(CalcPrefixFunc addPrefixFunc, CalcPrefixFunc findPrefixFunc);
int prefix_out_of_range(char *prefix);
void trie_add(Trie *trie, void *element);
Queue *trie_find(Trie *trie, void *element);

#endif /* TRIE_H_ */
