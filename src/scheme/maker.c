/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2015 by fanguangping (fanguangping@163.com)
 * maker.c
 */

#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>
#include "operator.h"
#include "memory.h"

#define STR_BUFF_SIZE 1024

static Num* initial_num_long(Num* self, long longValue) {
	self->isFix = TRUE;
	self->longValue = longValue;
	return self;
}

static Num* initial_num_double(Num* self, double doubleValue) {
	self->isFix = FALSE;
	self->doubleValue = doubleValue;
	return self;
}

Cell* make_cell(Scheme *sc, Cell* a, Cell* b) {
	Cell* p = create_cell_helper(sc, a, b);
	/* For right now, include "a" and "b" in "cell" so that gc doesn't think they are garbage. */
	/* Tentatively record it as a pair so gc understands it. */
	(p)->_flag = T_PAIR;
	(p)->_pair._car = a;
	(p)->_pair._cdr = b;
	return p;
}

/* get new cons cell */
Cell* make_pair(Scheme *sc, Cell* a, Cell* b, int immutable) {
	Cell* x = make_cell(sc, a, b);
	if (immutable)
		set_immutable(x);
//	(x)->_flag = T_PAIR;
//	(x)->_pair._car = a;
//	(x)->_pair._cdr = b;
	return (x);
}

/* get number atom (integer) */
Cell* make_integer(Scheme *sc, long num) {
	Cell* x = make_cell(sc, &g_nil, &g_nil);
	(x)->_flag = (T_NUMBER | T_ATOM);
	initial_num_long(&(x->_num), num);
	return (x);
}

Cell* make_real(Scheme *sc, double num) {
	Cell* x = make_cell(sc, &g_nil, &g_nil);
	(x)->_flag = (T_NUMBER | T_ATOM);
	initial_num_double(&(x->_num), num);
	return (x);
}

Cell* make_number(Scheme *sc, Num n) {
	if (n.isFix)
		return make_integer(sc, n.longValue);
	else
		return make_real(sc, n.doubleValue);
}

static int count_consecutive_cells(Cell* x, int needed) {
	int n = 1;
	//如果是连续的cell ， cdr(x)与x+1 必然相等
	while (cdr(x) == x + 1) {
		x = cdr(x);
		n++;
		if (n > needed)
			return n;
	}
	return n;
}

static Cell* find_consecutive_cells(Scheme *sc, int n) {
	Cell* *pp = &sc->freeCell;
	while (*pp != &g_nil) {
		int count = count_consecutive_cells(*pp, n);
		if (count >= n) {
			Cell* x = *pp;
			*pp = (*pp + n - 1)->_pair._cdr;
			sc->freeCellCount -= n;
			return x;
		}
		pp = &((*pp + count - 1)->_pair._cdr);
	}
	return &g_nil;
}

//获取n个连续的cell
static Cell* get_consecutive_cells(Scheme *sc, int n) {
	/* Are there any cells available? */
	Cell* x = find_consecutive_cells(sc, n);
	if (x != &g_nil)
		return x;

	/* If not, try gc'ing some */
	gc(sc, &g_nil, &g_nil);
	x = find_consecutive_cells(sc, n);
	if (x != &g_nil)
		return x;

	/* If there still aren't, try getting more heap */
	if (alloc_cellseg(sc, 1)) {
		x = find_consecutive_cells(sc, n);
		if (x != &g_nil)
			return x;
	}

	printf("No Memory ! ! !\n");
	while (TRUE)
		getchar();
	return &g_nil;
}

//创建数组
Cell* make_vector(Scheme *sc, int len) {
	//一个cell中的 _cons可以保存两个元素，所以只要len/2+len%2个cell就可以保存len个元素
	// 必能分配成功，失败不会返回
	Cell* cells = get_consecutive_cells(sc, len / 2 + len % 2 + 1);
	//cells[0]用于保存数组控制信息
	(cells)->_flag = (T_VECTOR | T_ATOM);	    //是数组，并是一个原子
	initial_num_long(&(cells->_num), (long) len);	//设置数组的长度
	fill_vector(cells, &g_nil);
	return cells;
}
// 创建字符
Cell* make_character(Scheme *sc, int c) {
	Cell* x = make_cell(sc, &g_nil, &g_nil);
	(x)->_flag = (T_CHAR | T_ATOM);
	initial_num_long(&(x->_num), (long) c);
	return (x);
}

/* allocate name to string area */
static char *store_string(int len, const char *str, char fill) {
	char *buf = (char*) malloc(len + 1);
	if (str != 0) {
		snprintf(buf, len + 1, "%s", str);
	} else {
		memset(buf, fill, len);
		buf[len] = 0;
	}
	return buf;
}

Cell* make_string_n(Scheme *sc, const char *str, int len) {
	Cell* x = make_cell(sc, &g_nil, &g_nil);
	(x)->_flag = (T_STRING | T_ATOM);
	(x)->_string = store_string(len, str, 0);
	return (x);
}

// 创建字符串
Cell* make_string(Scheme *sc, const char *str) {
	return make_string_n(sc, str, strlen(str));
}

// 创建空字符串
Cell* make_empty_string(Scheme *sc, int len, char fill) {
	Cell* x = make_cell(sc, &g_nil, &g_nil);
	(x)->_flag = (T_STRING | T_ATOM);
	(x)->_string = store_string(len, 0, fill);
	return (x);
}

/* make symbol or number atom from string */
Cell* make_atom_from_string(Scheme *sc, char *q) {
	char *p = q;
	int has_dec_point = 0;
	int has_fp_exp = 0;
	char c = *p++;
	if ((c == '+') || (c == '-'))
		c = *p++;
	if (c == '.') {
		has_dec_point = 1;
		c = *p++;
	}
	if (!isdigit(c)) {
		return (make_symbol(sc, strlwr(q)));
	}
	for (; (c = *p) != 0; ++p) {
		//判断是不是符合数字格式的标准
		if (!isdigit(c)) {
			if (c == '.') {
				if (!has_dec_point) {
					has_dec_point = 1;
					continue;
				}
			} else if ((c == 'e') || (c == 'E')) {
				if (!has_fp_exp) {
					has_dec_point = 1; /* decimal point illegal from now on */
					p++;
					if ((*p == '-') || (*p == '+') || isdigit(*p)) {
						continue;
					}
				}
			}
			return (make_symbol(sc, strlwr(q)));
		}
	}
	if (has_dec_point) {
		return make_real(sc, atof(q));
	}
	return (make_integer(sc, atol(q)));
}

static const char *charnames[32] = { "nul", "soh", "stx", "etx", "eot", "enq",
		"ack", "bel", "bs", "ht", "lf", "vt", "ff", "cr", "so", "si", "dle",
		"dc1", "dc2", "dc3", "dc4", "nak", "syn", "etb", "can", "em", "sub",
		"esc", "fs", "gs", "rs", "us" };

static int is_ascii_name(const char *name, int *pc) {
	int i;
	for (i = 0; i < 32; i++) {
		if (strcasecmp(name, charnames[i]) == 0) {
			*pc = i;
			return 1;
		}
	}
	if (strcasecmp(name, "del") == 0) {
		*pc = 127;
		return 1;
	}
	return 0;
}

//将字符串形式的二进制数据转化为整数
static long binary_decode(const char *s) {
	long x = 0;
	while (*s != 0 && (*s == '1' || *s == '0')) {
		x <<= 1;
		x += *s - '0';
		s++;
	}
	return x;
}

//解析 # 字符开头的字符串 用于生成常量（不包括数组）
Cell* make_sharp_const(Scheme *sc, char *name) {
	char tmp[STR_BUFF_SIZE];
	long x = 0;
	if (strcmp(name, "t") == 0)
		return (&g_true);
	else if (strcmp(name, "f") == 0)
		return (&g_false);
	else if (*name == 'o') {/* #o (octal) */
		snprintf(tmp, STR_BUFF_SIZE, "0%s", name + 1);
		sscanf(tmp, "%lo", (long unsigned *) &x);
		return (make_integer(sc, x));
	} else if (*name == 'd') { /* #d (decimal) */
		sscanf(name + 1, "%ld", (long int *) &x);
		return (make_integer(sc, x));
	} else if (*name == 'x') { /* #x (hex) */
		snprintf(tmp, STR_BUFF_SIZE, "0x%s", name + 1);
		sscanf(tmp, "%lx", (long unsigned *) &x);
		return (make_integer(sc, x));
	} else if (*name == 'b') { /* #b (binary) */
		x = binary_decode(name + 1);
		return (make_integer(sc, x));
	} else if (*name == '\\') { /* #\w (character) */
		int c = 0;
		if (strcasecmp(name + 1, "space") == 0) {
			c = ' ';
		} else if (strcasecmp(name + 1, "newline") == 0) {
			c = '\n';
		} else if (strcasecmp(name + 1, "return") == 0) {
			c = '\r';
		} else if (strcasecmp(name + 1, "tab") == 0) {
			c = '\t';
		} else if (name[1] == 'x' && name[2] != 0) {
			int c1 = 0;
			if (sscanf(name + 2, "%x", (unsigned int *) &c1) == 1 && c1 < UCHAR_MAX) {
				c = c1;
			} else {
				return &g_nil;
			}
		} else if (is_ascii_name(name + 1, &c)) {

		} else if (name[2] == 0) {
			c = name[1];
		} else {
			return &g_nil;
		}
		return make_character(sc, c);
	} else
		return (&g_nil);
}

static int hash_fn(const char *key, int table_size) {
	const char *c;
	unsigned int hashed = 0;
	for (c = key; *c; c++) {
		int bits_per_int = sizeof(unsigned int) * 8;
		/* letters have about 5 bits in them */
		hashed = (hashed << 5) | (hashed >> (bits_per_int - 5));
		hashed ^= *c;
	}
	return hashed % table_size;
}

//查找一个符号
static Cell* oblist_find_by_name(Scheme *sc, const char *name) {
	int location = hash_fn(name, sc->objectList->_num.longValue);
	Cell* sym_list;
	for (sym_list = get_vector_item(sc->objectList, location);
			sym_list != &g_nil; sym_list = cdr(sym_list)) {
		char *s = symbol_name(car(sym_list));
		if (strcasecmp(name, s) == 0)
			return car(sym_list); //返回一个符号
	}
	return &g_nil;
}

// 创建一个新的符号
static Cell* oblist_add_by_name(Scheme *sc, const char *name) {
	//符号内部像这样("name1")
	Cell* sym = immutable_cons(sc, make_string(sc, name), &g_nil);
	(sym)->_flag = T_SYMBOL; //类型为符号
	set_immutable(car(sym));
	int location = hash_fn(name, sc->objectList->_num.longValue);
	//get_vector_item(sc->oblist, location) 等价于  (vector-ref sc->oblist location)    求值结果是一个符号链表 (symbol ...)
	set_vector_item(sc->objectList, location,
			immutable_cons(sc, sym, get_vector_item(sc->objectList, location))); //将新的符号加入链表头部
	return sym;
}

//添加一个符号，如果有这个符号，就返回已有的那个
Cell* make_symbol(Scheme *sc, const char *name) {
	/* first check oblist */
	Cell* sym = oblist_find_by_name(sc, name);
	if (sym != &g_nil)
		return sym;
	return oblist_add_by_name(sc, name);
}

// 创建过程
Cell* make_func(Scheme *sc, OperatorFunc f) {
	Cell* x = make_cell(sc, &g_nil, &g_nil);
	(x)->_flag = (T_PROC | T_ATOM);
	x->_op = f;
	return (x);
}

// 创建扩展过程
Cell* make_foreign_func(Scheme *sc, ForeignFunc f) {
	Cell* x = make_cell(sc, &g_nil, &g_nil);
	(x)->_flag = (T_FOREIGN | T_ATOM);
	x->_func = f;
	return (x);
}

/* make closure. c is code. e is environment */
//创建闭包
Cell* make_closure(Scheme *sc, Cell* code, Cell* env) {
	Cell* p = make_cell(sc, code, env);
	(p)->_flag = T_CLOSURE;
	return p;
}

/* make continuation. */
//创建延续
Cell* make_continuation(Scheme *sc, Cell* d) {
	Cell* p = make_cell(sc, &g_nil, d);
	(p)->_flag = T_CONTINUATION;
	return p;
}

// (make-closure closure)
Cell* op_make_closure(Scheme *sc) {
	Cell *x, *y;

	//形式1 (make-clourse '(lambda? var-name body ...) envir?)      lambda 和 envir是可有可无的
	//形式2 (make-clourse '(lambda? (var-name ...) body ...) envir?)      lambda 和 envir是可有可无的
	//例子 (define f (make-closure '(plus ((car plus) 3 4))))  结果为7
	x = car(sc->args);
	if (car(x) == sc->sym_lambda)
		x = cdr(x);
	if (cdr(sc->args) == &g_nil)
		y = sc->env;
	else
		y = cadr(sc->args);
	return s_return_helper(sc, make_closure(sc, x, y));
}

// (make-string n)
// (make-string n c)
Cell* op_makestring(Scheme *sc) {
	int len = long_value(first(sc->args));
	int fill = ' ';
	if (rest(sc->args) != &g_nil) {
		fill = char_value(second(sc->args));
	}
	return s_return_helper(sc, make_empty_string(sc, len, (char) fill));
}

// (new-segment num)
Cell* op_newsegment(Scheme *sc) {
	if (!is_pair(sc->args) || !is_number(first(sc->args))) {
		return error_helper(sc, "new-segment: argument must be a number", NULL);
	}
	alloc_cellseg(sc, (int) long_value(first(sc->args)));
	return s_return_helper(sc, &g_true);
}
