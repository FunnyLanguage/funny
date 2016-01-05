/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2015 by fanguangping (fanguangping@163.com)
 * ds.h
 */

#ifndef DS_H_
#define DS_H_

#include <stdio.h>

#ifdef TRUE
#undef TRUE
#endif
#define TRUE 1

#ifdef FALSE
#undef FALSE
#endif
#define FALSE 0

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

typedef struct _Cell {
	unsigned int _flag;
	union {
		Num _num;
		char *_string;
		OperatorFunc _op;
		ForeignFunc _func;
		Pair _pair;
	};
} Cell;

#define CELL_SEGSIZE 5000	 /* # of cells in one segment */
#define CELL_NSEGMENT 100    /* # of segments for cells */

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

	char* input;
	int inputPointer;

	Cell* objectList; /* cell* to symbol table *///管理所有的符号，确保了所有相同名字的符号是同一个
	Cell* globalEnv; /* cell* to global environment */

	int token;						//保持词法分析获取的单词

	/* global cell*s to special symbols */
	Cell* sym_lambda; /* cell* to syntax lambda */
	Cell* sym_quote; /* cell* to syntax quote */			//引用    '
	Cell* sym_feed_to; /* => */							// cond中有用到
	Cell* sym_colon_hook; /* *colon-hook* */
	Cell* sym_error_hook; /* *error-hook* */
	Cell* sym_sharp_hook; /* *sharp-hook* */
	Cell* sym_compile_hook; /* *compile-hook* */
} Scheme;

/*
 * 常数
 * */
Cell g_nil;
Cell g_true;
Cell g_false;
Cell g_eof;
void init_constants();

/*
 * 宏
 * */

// 特征标记
#define TYPE_MASK       31		/* 0000000000011111 */	// 5位 可以表示32个类型
#define T_SYNTAX        4096	/* 0001000000000000 */  // 语法符号
#define T_IMMUTABLE     8192	/* 0010000000000000 */	// 标记一个cell为不可改变的
#define T_ATOM          16384	/* 0100000000000000 */	// 标记一个cell为原子
#define REF_MARK        32768	/* 1000000000000000 */	/* 仅用于gc */

// 词法标记
#define TOK_EOF     -1				// 输入结束
#define TOK_LPAREN  0				// (
#define TOK_RPAREN  1				// )
#define TOK_DOT     2				// .
#define TOK_ATOM    3				// 原子 （number id symbol ）
#define TOK_QUOTE   4				// '
#define TOK_COMMENT 5				// ;
#define TOK_DQUOTE  6				// "
#define TOK_SHARP   7				// #
#define TOK_SHARP_CONST 8			// #  常量 (例如 #t #f #\a)
#define TOK_VECTOR  9				// #( 数组

#define DELIMITERS  "()\";\f\t\v\n\r "	// 分隔符


/* () is #t in R5RS */ //不为#f的都是true
#define is_true(p)       ((p) != &g_false)
#define is_false(p)      ((p) == &g_false)

/*
 * 类型
 * */
enum SchemeType {
	T_PAIR = 1,					// 点对
	T_NUMBER = 2,				// 数字
	T_CHAR = 3,					// 字符
	T_STRING = 4,				// 字符串
	T_SYMBOL = 5,				// 符号
	T_VECTOR = 6,				// 数组 vector : pair<any,any>[num]
	T_PROC = 7,					// 过程
	T_FOREIGN = 8,				// 扩展过程
	T_CLOSURE = 9,				// 闭包
	T_PROMISE = 10,				//
	T_MACRO = 11,				// 宏
	T_CONTINUATION = 12,		// 延续
	T_ENVIRONMENT = 13,			// 环境
	T_LAST_TYPE = 14			// 最大编号
};


/*
 * 特征标记函数
 * */
unsigned int typeflag(Cell* p);
int type(Cell* p);
int is_atom(Cell* p);
void set_atom(Cell* p);
void clear_atom(Cell* p);
int is_mark(Cell* p);
void set_mark(Cell* p);
void clear_mark(Cell* p);

Num num_value(Cell* p);
long long_value(Cell* p);
double double_value(Cell* p);
long char_value(Cell* p);
char *symbol_name(Cell* sym);
Cell* closure_code(Cell* p);
Cell* closure_env(Cell* p);
void set_immutable(Cell* p);

/*
 * car/cdr/cons函数
 * */
Cell* car(Cell* p);
Cell* cdr(Cell* p);
Cell* cons(Scheme *sc, Cell* a, Cell* b);
Cell* immutable_cons(Scheme *sc, Cell* a, Cell* b);

#define CAR(p)           ((p)->_pair._car)
#define CDR(p)           ((p)->_pair._cdr)
#define CAAR(p)          CAR(CAR(p))
#define CADR(p)          CAR(CDR(p))
#define CDAR(p)          CDR(CAR(p))
#define CDDR(p)          CDR(CDR(p))

#define caar(p)   car(car(p))
#define cadr(p)   car(cdr(p))
#define cdar(p)   cdr(car(p))
#define cddr(p)   cdr(cdr(p))
#define caaar(p)  car(car(car(p)))
#define caadr(p)  car(car(cdr(p)))
#define cadar(p)  car(cdr(car(p)))
#define caddr(p)  car(cdr(cdr(p)))
#define cdaar(p)  cdr(car(car(p)))
#define cdadr(p)  cdr(car(cdr(p)))
#define cddar(p)  cdr(cdr(car(p)))
#define cdddr(p)  cdr(cdr(cdr(p)))
#define caaaar(p) car(car(car(car(p))))
#define caaadr(p) car(car(car(cdr(p))))
#define caadar(p) car(car(cdr(car(p))))
#define caaddr(p) car(car(cdr(cdr(p))))
#define cadaar(p) car(cdr(car(car(p))))
#define cadadr(p) car(cdr(car(cdr(p))))
#define caddar(p) car(cdr(cdr(car(p))))
#define cadddr(p) car(cdr(cdr(cdr(p))))
#define cdaaar(p) cdr(car(car(car(p))))
#define cdaadr(p) cdr(car(car(cdr(p))))
#define cdadar(p) cdr(car(cdr(car(p))))
#define cdaddr(p) cdr(car(cdr(cdr(p))))
#define cddaar(p) cdr(cdr(car(car(p))))
#define cddadr(p) cdr(cdr(car(cdr(p))))
#define cdddar(p) cdr(cdr(cdr(car(p))))
#define cddddr(p) cdr(cdr(cdr(cdr(p))))

#define first(p)  	car(p)
#define second(p) 	car(cdr(p))
#define third(p)  	car(cdr(cdr(p)))
#define fourth(p) 	car(cdr(cdr(cdr(p))))
#define fifth(p)  	car(cdr(cdr(cdr(cdr(p)))))
#define sixth(p)  	car(cdr(cdr(cdr(cdr(cdr(p))))))
#define seventh(p)  car(cdr(cdr(cdr(cdr(cdr(cdr(p)))))))
#define eighth(p)  	car(cdr(cdr(cdr(cdr(cdr(cdr(cdr(p))))))))
#define ninth(p)  	car(cdr(cdr(cdr(cdr(cdr(cdr(cdr(cdr(p)))))))))
#define tenth(p)  	car(cdr(cdr(cdr(cdr(cdr(cdr(cdr(cdr(cdr(p))))))))))
#define eleventh(p) car(cdr(cdr(cdr(cdr(cdr(cdr(cdr(cdr(cdr(cdr(p)))))))))))
#define twelfth(p)  car(cdr(cdr(cdr(cdr(cdr(cdr(cdr(cdr(cdr(cdr(cdr(p))))))))))))

#define rest(p)   	cdr(p)

#endif /* DS_H_ */
