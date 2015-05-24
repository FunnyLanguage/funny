/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2014 by fanguangping (fanguangping@163.com)
 *
 * Funny is based on TinyScheme which is now maintained by Dimitrios Souflis (dsouflis@acm.org)
 * This is a revised and modified version of TinyScheme
 */

#ifndef MEMORY_H_
#define MEMORY_H_

#include "ds.h"

/*
 * 常数
 * */
Cell g_nil;
Cell g_true;
Cell g_false;
Cell g_eof;
void init_constants();

void gc(Scheme *sc, Cell* a, Cell* b);
int alloc_cellseg(Scheme *sc, int n);
Cell* create_cell_helper(Scheme *sc, Cell* a, Cell* b);

/*
#define MAX_STR_SIZE  102400
#define MAX_SIZE      512

typedef struct _MemoryPool {
	String strings[MAX_STR_SIZE];
	int stringIndex;
	Queue *queues[MAX_SIZE];
	int queueIndex;
	HashTable tables[MAX_SIZE];
	int tableIndex;
	Statement *statements[MAX_SIZE];
	int statementIndex;
	Mapping *mappings[MAX_SIZE];
	int mappingIndex;
} MemoryPool;

MemoryPool memoryPool;
*/

void exit_when_out_of_memory(void *element);

String alloc_string(int size);
String *alloc_string_array(int size);
String realloc_string(String str, int size);
Queue *alloc_queue();
QueueNode *alloc_queue_node();
HashTable alloc_hash_table();
HashNode *alloc_hash_node();
Scheme *alloc_scheme();
Cell *alloc_cell();

Input *alloc_input();
Statement *alloc_statement();
Mapping *alloc_mapping();
SysBlock *alloc_sys_block();
ReplaceNode *alloc_replace_node();
State *alloc_state();
TransitNode *alloc_transit_node();

#endif /* MEMORY_H_ */
