/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2014 by fanguangping (fanguangping@163.com)
 *
 */

#ifndef _DS_H_
#define _DS_H_

#include <stdio.h>

#ifdef TRUE
#undef TRUE
#endif
#define TRUE 1

#ifdef FALSE
#undef FALSE
#endif
#define FALSE 0

/****************** String ******************/

typedef char* String;

String init_string();
String copy_string(String str);
String trim_string(String str);
String substr_before(String str, char c);
String substr_between(String str, char a, char b);
String substr_after(String str, char c);
String dyna_strcat(String str, char *append);
String append_char(String str, char c);
String* split_string(String string, const char delimiter);
String* split_string_once(String string, const char delimiter);
int equals_string(void *a, void *b);
int hash_code_string(void *key);
char *strlwr(char *str);

/****************** Queue ******************/

typedef struct _QueueNode {
	void *element;
	struct _QueueNode *next;
} QueueNode;

typedef struct _Queue {
	struct _QueueNode *head;
	struct _QueueNode *tail;
} Queue;

Queue *init_queue();
void enqueue(Queue *queue, void *element);
void *dequeue(Queue *queue);
void *get_queue_element(Queue *queue, int index);
int is_queue_empty(Queue *queue);
void clear_queue(Queue *queue);
void append_queue(Queue *q1, Queue *q2);
int contains_queue_element(Queue *queue, void *element, int (*equals)(void *e1, void *e2));
int count_queue_element(Queue *queue);

/****************** HashTable ******************/

#define HASHTABLE_SIZE 101

typedef struct _HashNode {
	void *element;
	struct _HashNode *next;
} HashNode;

typedef HashNode** HashTable;

HashTable init_table();
void put_table_element(HashTable table, void *key, void *value,
		int (*HashCode)(void *key));
HashNode *get_table_element(HashTable table, void *key,
		int (*HashCode)(void *key));
void clear_table(HashTable table);

/****************** Scheme ******************/
typedef struct _Num {
	int isFix;
	union {
		long longValue;
		double doubleValue;
	};
} Num;

struct _Cell;
struct _Scheme;
typedef struct _Cell* (*ForeignFunc)(struct _Scheme *, struct _Cell *);
typedef struct _Cell* (*OperatorFunc)(struct _Scheme *);

typedef struct _Pair {
	struct _Cell *_car;
	struct _Cell *_cdr;
} Pair;

typedef struct _Port {
	unsigned char kind;
	union {
		struct {
			FILE *file;
			char *filename;
			int curLine;
			int closed;
		} filePort;
		struct {
			char *start;
			char *end;
			char *cur;
		} stringPort;
	};
} Port;

enum PortSubclass {
	PORT_FREE = 0,
	PORT_FILE = 1,
	PORT_STRING = 2,
	PORT_CAN_REALLOC = 4,
	PORT_INPUT = 16,
	PORT_OUTPUT = 32,
	PORT_EOF = 64
};

typedef struct _Cell {
	unsigned int _flag;
	union {
		Num _num;
		char *_string;
		OperatorFunc _op;
		ForeignFunc _func;
		Pair _pair;
		Port *_port;
	};
} Cell;

#define CELL_SEGSIZE 5000	 /* # of cells in one segment */
#define CELL_NSEGMENT 100    /* # of segments for cells */
#define STR_BUFF_SIZE 1024

typedef struct _Scheme {
	Cell* cellSeg[CELL_NSEGMENT];
	int lastCellSeg;
	Cell* freeCell; /* cell* to top of free cells */
	long freeCellCount; /* # of free cells */

	/* We use 4 registers. */
	OperatorFunc op;							//当前处理方法
	Cell* args; /* register for arguments of function */
	Cell* env;  /* stack register for current environment */
	Cell* code; /* register for current code */
	Cell* callStack; /* stack register for next evaluation */
	Cell* returnValue;
	//int nesting;

	//端口管理
	Cell* inPort;
	Cell* outPort;
	Port* curPort;

	Cell* objectList; /* cell* to symbol table *///管理所有的符号，确保了所有相同名字的符号是同一个
	Cell* globalEnv; /* cell* to global environment */

	int token;						//保持词法分析获取的单词
	char stringBuffer[STR_BUFF_SIZE];

	/* global cell*s to special symbols */
	Cell* sym_lambda; /* cell* to syntax lambda */
	Cell* sym_quote; /* cell* to syntax quote */			//引用    '
	Cell* sym_qquote; /* cell* to symbol quasiquote */			//准引用 `
	Cell* sym_unquote; /* cell* to symbol unquote */			//解引用 ,
	Cell* sym_unquote_sp; /* cell* to symbol unquote-splicing *///解引用 ,@
	Cell* sym_feed_to; /* => */							// cond中有用到
	Cell* sym_colon_hook; /* *colon-hook* */
	Cell* sym_error_hook; /* *error-hook* */
	Cell* sym_sharp_hook; /* *sharp-hook* */
	Cell* sym_compile_hook; /* *compile-hook* */
} Scheme;

/****************** Input ******************/

typedef struct _Input {
	int isFile;
	union {
		FILE *file;
		struct {
			String buffer;
			int pointer;
		} string;
	};
} Input;

/****************** StatementType ******************/

typedef enum {
	SYM, ARG, LOOP
} StatementType;

/****************** Statement ******************/

typedef struct _Statement {
	String symbol;
	StatementType type;		//1-SYM, 2-ARG, 3-LOOP
	Queue *children;		//element is struct _Statement
} Statement;

Statement *init_sym(String symbol);
Statement *init_arg();
Statement *init_loop();
int is_sym(String token);
int equal_sym(Statement *s, String sym);
int equal_arg(Statement *s, String sym);
int empty_statement(Statement *s);
int equal_statement(Statement *a, Statement *b);
String to_string_statement(Statement *s);

/****************** ReplaceQueue ******************/

typedef struct _ReplaceNode {
	Statement *source;
	Statement *target;
} ReplaceNode;

void add_replace_statement(Queue *queue, Statement *source, Statement *target);
Statement *find_replace_statement(Queue *queue, Statement *source);
Queue *find_loop_replace_statement(Queue *queue, Statement *source);

/****************** State ******************/

typedef struct _State {
	int id;
	int accept;
} State;

typedef struct _TransitNode {
	State *starter;
	Statement *input;
	State *next;
} TransitNode;

State *to_dfa(Statement *s);
State *transit(State *starter, State *start, Statement *input, Queue *repQueue);
int equals_state(void *a, void *b);
int hash_code_state(void *s);

/****************** Mapping ******************/

typedef struct _SysBlock {
	String name;
	String prepare;
	String eval;
} SysBlock;

typedef struct _Mapping {
	SysBlock *sys;
	String source;
	String target;
	String sourceFunc;
	String targetFunc;
	Statement *sourceStatement;
	Statement *targetStatement;
	State *starter;
} Mapping;

Mapping *get_sys_mapping(Queue *mappings, String name);

#endif // _DS_H_
