/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2015 by fanguangping (fanguangping@163.com)
 * hash.h
 */

#ifndef HASH_H_
#define HASH_H_

#define HASHTABLE_SIZE 101

typedef struct _HashNode {
	void *element;
	struct _HashNode *next;
} HashNode;

typedef HashNode** HashTable;

HashTable alloc_hash_table();
HashNode *alloc_hash_node();

HashTable init_table();
void put_table_element(HashTable table, void *key, void *value,
		int (*HashCode)(void *key));
HashNode *get_table_element(HashTable table, void *key,
		int (*HashCode)(void *key));
void clear_table(HashTable table);

//========================

typedef int (*MyGEqualFunc) (const void *a, const void *b);
typedef unsigned int (*MyGHashFunc) (const void *key);
typedef void (*MyGHFunc) (void *key, void *value, void *user_data);

typedef struct _MyGHashNode MyGHashNode;

struct _MyGHashNode
{
  void *key;
  void *value;
  MyGHashNode *next;
  unsigned int key_hash;
};

typedef struct _MyGHashTable MyGHashTable;

struct _MyGHashTable
{
  int size;
  int nnodes;
  MyGHashNode **nodes;
  MyGHashFunc hash_func;
  MyGEqualFunc key_equal_func;
};

int my_g_str_equal(const void * v1, const void * v2);
unsigned int my_g_str_hash(const void * v);

MyGHashNode **my_g_hash_table_lookup_node(MyGHashTable *hash_table, const void *key, unsigned int *hash_return);
void my_g_hash_table_resize(MyGHashTable *hash_table);
void my_g_hash_table_maybe_resize(MyGHashTable *hash_table);

MyGHashTable* my_g_hash_table_new(MyGHashFunc hash_func, MyGEqualFunc key_equal_func);
void my_g_hash_table_insert(MyGHashTable *hash_table, void *key, void *value);
void my_g_hash_table_foreach(MyGHashTable *hash_table, MyGHFunc func, void *user_data);


#endif /* HASH_H_ */
