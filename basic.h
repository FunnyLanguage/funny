/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2014 by fanguangping (fanguangping@163.com)
 *
 * Funny is based on TinyScheme which is now maintained by Dimitrios Souflis (dsouflis@acm.org)
 * This is a revised and modified version of TinyScheme
 */

#ifndef BASIC_H_
#define BASIC_H_

#include "ds.h"

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
#define TOK_EOF     -1					// 输入结束
#define TOK_LPAREN  0				// (
#define TOK_RPAREN  1				// )
#define TOK_DOT     2					// .
#define TOK_ATOM    3				// 原子 （number id symbol ）
#define TOK_QUOTE   4				// '
#define TOK_COMMENT 5			// ,
#define TOK_DQUOTE  6				// "
#define TOK_BQUOTE  7				// `
#define TOK_COMMA   8				// ,
#define TOK_ATMARK  9				// ,@
#define TOK_SHARP 10					// #
#define TOK_SHARP_CONST 11		// #  常量 (例如 #t #f #\a)
#define TOK_VECTOR 12				// #( 数组

#define DELIMITERS  "()\";\f\t\v\n\r "	// 分隔符


/* () is #t in R5RS */ //不为#f的都是true
#define is_true(p)       ((p) != &g_false)
#define is_false(p)      ((p) == &g_false)

#define long_value_unchecked(p)       ((p)->_num.longValue)
// #define string_value(p)           (p)->_string._str	    //获取字符串的值
// #define string_len(p)             ((p)->_string._len)	//字符串的长度
// #define sym_value(sym_kv)         cdr(sym_kv)
// #define proc_value(p)             long_value(p)

/*
 * 数操作函数
 * */
Num num_add(Num a, Num b);
Num num_sub(Num a, Num b);
Num num_mul(Num a, Num b);
Num num_div(Num a, Num b);
Num num_intdiv(Num a, Num b);
Num num_rem(Num a, Num b);
Num num_mod(Num a, Num b);
int num_eq(Num a, Num b);
int num_gt(Num a, Num b);
int num_lt(Num a, Num b);
int num_ge(Num a, Num b);
int num_le(Num a, Num b);
double round_per_r5rs(double x);
int is_zero_double(double x);

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
	T_PORT = 7,					// 端口
	T_PROC = 8,					// 过程
	T_FOREIGN = 9,				// 扩展过程
	T_CLOSURE = 10,				// 闭包
	T_PROMISE = 11,				//
	T_MACRO = 12,				// 宏
	T_CONTINUATION = 13,		// 延续
	T_ENVIRONMENT = 14,			// 环境
	T_LAST_TYPE = 14			// 最大编号
};

Num num_value(Cell* p);
long long_value(Cell* p);
double double_value(Cell* p);
Num* initial_num_long(Num* self, long longValue);
Num* initial_num_double(Num* self, double doubleValue);
long char_value(Cell* p);
char *symbol_name(Cell* sym);
Cell* closure_code(Cell* p);
Cell* closure_env(Cell* p);
void set_immutable(Cell* p);
//String func_name(Cell* p);
//OperatorFunc func_value(Cell* p);

/*
 * is 函数
 * */
int is_pair(Cell* p);
int is_number(Cell* p);
int is_integer(Cell* p);
int is_real(Cell* p);
int is_character(Cell* p);
int is_string(Cell* p);
int is_symbol(Cell* sym);
int is_syntax(Cell* p);
int is_vector(Cell* p);
int is_port(Cell* p);
int is_inport(Cell* p);
int is_outport(Cell* p);
int is_proc(Cell* p);
int is_foreign(Cell* p);
int is_closure(Cell* p);
int is_promise(Cell* p);
int is_macro(Cell* p);
int is_continuation(Cell* p);
int is_environment(Cell* p);
int is_immutable(Cell* p);

/*
 * make 函数
 * */
Cell* make_cell(Scheme *sc, Cell* a, Cell* b);
Cell* make_pair(Scheme *sc, Cell* a, Cell* b, int immutable);
Cell* make_integer(Scheme *sc, long num);
Cell* make_real(Scheme *sc, double num);
Cell* make_number(Scheme *sc, Num n);
Cell* make_vector(Scheme *sc, int len);
Cell* make_character(Scheme *sc, int c);
Cell* make_string_n(Scheme *sc, const char *str, int len);
Cell* make_string(Scheme *sc, const char *str);
Cell* make_empty_string(Scheme *sc, int len, char fill);
Cell* make_atom_from_string(Scheme *sc, char *q);
Cell* make_sharp_const(Scheme *sc, char *name);
Cell* make_symbol(Scheme *sc, const char *name);
Cell* make_func(Scheme *sc, OperatorFunc f);
Cell* make_foreign_func(Scheme *sc, ForeignFunc f);
Cell* make_closure(Scheme *sc, Cell* code, Cell* env);
Cell* make_continuation(Scheme *sc, Cell* d);
Cell* make_port(Scheme *sc, Port *p);

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
//#define cons(sc,a,b)           make_pair(sc,a,b,0)
//#define immutable_cons(sc,a,b) make_pair(sc,a,b,1)

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

#define first(p)  car(p)
#define second(p) cadr(p)
#define third(p)  caddr(p)
#define fourth(p) cadddr(p)
#define rest(p)   cdr(p)


// 其他函数
Cell* reverse(Scheme *sc, Cell* old_list);
Cell* list_star(Scheme *sc, Cell* old_list);
Cell* revappend(Scheme *sc, Cell* acc, Cell* list);
int list_length(Scheme *sc, Cell* a);
int is_list(Scheme *sc, Cell* a);
Cell* get_vector_item(Cell* vec, int index);
Cell* set_vector_item(Cell* vec, int index, Cell* a);
void fill_vector(Cell* vec, Cell* fill_obj);
int eqv(Cell* a, Cell* b);
long binary_decode(const char *s);
int hash_fn(const char *key, int table_size);

int file_interactive(Scheme *sc);
void port_close(Cell* p, int flag);
//int push_load_file(Scheme *sc, const char *fname);
//void pop_load_file(Scheme *sc);
Cell* port_from_file(Scheme *sc, FILE *f, int prop);
Cell* port_from_filename(Scheme *sc, const char *file_name, int prop);
int realloc_port_string(Scheme *sc, Port *p);
Cell* port_from_string(Scheme *sc, char *start, char *end, int prop);
Cell* oblist_initial_value(Scheme *sc);
Cell* oblist_all_symbols(Scheme *sc);
Cell* gensym(Scheme *sc);
int is_ascii_name(const char *name, int *pc);
void add_syntax_symbol(Scheme *sc, String name);

// 读取函数
int get_char(Scheme *sc);
void unget_char(Scheme *sc, int c);
char *readstr_upto(Scheme *sc, char *delim);
Cell* readstrexp(Scheme *sc);
int token(Scheme *sc);

// 写入函数
void write_char(Scheme *sc, int c);
void write_string(Scheme *sc, const char *s);
void atom2str(Scheme *sc, Cell* cell, int flag, char **pp, int *plen);
void printatom(Scheme *sc, Cell* cell, int flag);

// 环境
void new_frame_in_env(Scheme *sc, Cell* oldEnv);
void new_slot_in_env(Scheme *sc, Cell* sym, Cell* value);
Cell* find_slot_in_env(Scheme *sc, Cell* env, Cell* sym, int all);
void set_slot_in_env(Scheme *sc, Cell* symbol, Cell* value);
Cell* slot_value_in_env(Cell* symbol);
void scheme_define(Scheme *sc, Cell* envir, Cell* symbol, Cell* value);

#endif /* BASIC_H_ */
