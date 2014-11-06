/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2014 by fanguangping. Email: fguangping@gmail.com
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Affero General Public
 * License version 3 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License version 3 for more details.
 *
 * You should have received a copy of the GNU Affero General Public
 * License version 3 along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */

#ifndef _DS_H_
#define _DS_H_

#define TRUE    1
#define FALSE   0

void exit_when_out_of_memory(void *element);

/****************** String ******************/

typedef char* String;

char *init_string();
char *copy_string(char *str);
char *trim_string(char *str);
char *substr_before(char *str, char c);
char *substr_between(char *str, char a, char b);
char *substr_after(char *str, char c);
char *dyna_strcat(char *str, char *append);
char *append_char(char *str, char c);
int equals_string(void *a, void *b);
int hash_code_string(void *key);

/****************** Queue ******************/

typedef struct _QueueNode
{
	void *element;
	struct _QueueNode *next;
} QueueNode;

typedef struct _Queue
{
	struct _QueueNode *head;
	struct _QueueNode *tail;
} Queue;

Queue *init_queue();
void enqueue(Queue *queue, void *element);
void *dequeue(Queue *queue);
void *get_queue_element(Queue *queue, int index);
int  is_queue_empty(Queue *queue);
void clear_queue(Queue *queue);
void append_queue(Queue *q1, Queue *q2);

/****************** HashTable ******************/

#define HASHTABLE_SIZE 101

typedef struct _HashNode
{
	void *element;
	struct _HashNode *next;
} HashNode;

typedef HashNode** HashTable;

HashTable init_table();
void put_table_element(HashTable table, void *key, void *value, int (*hash_code)(void *key));
HashNode *get_table_element(HashTable table, void *key, int (*hash_code)(void *key));
void clear_table(HashTable table);

/****************** ObjectType ******************/

typedef enum 
{
	THE_EMPTY_LIST, 
	BOOLEAN, 
	SYMBOL, 
	FIXNUM,
	CHARACTER, 
	STRING, 
	PAIR, 
	PRIMITIVE_PROC,
	COMPOUND_PROC, 
	INPUT_PORT, 
	OUTPUT_PORT,
	EOF_OBJECT
} ObjectType;

/****************** Object ******************/

typedef struct _Object 
{
    ObjectType type;
    union 
	{
        struct 
		{
            int value;
        } boolean;
        struct 
		{
            String value;
        } symbol;
        struct 
		{
            long value;
        } fixnum;
        struct 
		{
            char value;
        } character;
        struct 
		{
            String value;
        } string;
        struct 
		{
            struct _Object *car;
            struct _Object *cdr;
        } pair;
        struct 
		{
            struct _Object *(*fn)(struct _Object *arguments);
        } primitive_proc;
        struct 
		{
            struct _Object *parameters;
            struct _Object *body;
            struct _Object *env;
        } compound_proc;
        struct 
		{
            FILE *stream;
        } input_port;
        struct 
		{
            FILE *stream;
        } output_port;
    } data;
} Object;

Object *init_object();

/****************** StatementType ******************/

typedef enum 
{
	SYM,
	ARG,
	LOOP
}StatementType;

/****************** Statement ******************/

typedef struct _Statement
{
	String symbol;
	StatementType type;		//1-SYM, 2-ARG, 3-LOOP, 4-EPSILON
	Queue *children;		//element is struct _Statement
}Statement;

Statement *init_sym(String symbol);
Statement *init_arg();
Statement *init_loop();
int is_sym(String token);
int empty_statement(Statement *s);
int equal_statement(Statement *a, Statement *b);
String to_string_statement(Statement *s);

/****************** ReplaceQueue ******************/

typedef struct _ReplaceNode
{
	Statement *source;
	Statement *target;
}ReplaceNode;

void add_replace_statement(Queue *queue, Statement *source, Statement *target);
Statement *find_replace_statement(Queue *queue, Statement *source);
Queue *find_loop_replace_statement(Queue *queue, Statement *source);

/****************** State ******************/

typedef struct _State
{
	int id;
	int accept;
}State;

typedef struct _TransitNode
{
	State *starter;
	Statement *input;
	State *next;
}TransitNode;

State *to_dfa(Statement *s);
State *transit(State *starter, State *start, Statement *input, Queue *repQueue);
int equals_state(void *a, void *b);
int hash_code_state(void *s);

/****************** Mapping ******************/

typedef struct _SysBlock
{
	String prepare;
	String eval;
}SysBlock;

typedef struct _Mapping
{
	SysBlock *sys;
	String source;
	String target;
	String sourceFunc;
	String targetFunc;
	Statement *sourceStatement;
	Statement *targetStatement;
	State *starter;
}Mapping;

#endif // _DS_H_
