/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2014 by fanguangping (fanguangping@163.com)
 *
 * Funny is based on TinyScheme which is now maintained by Dimitrios Souflis (dsouflis@acm.org)
 * This is a revised and modified version of TinyScheme
 */

#include <math.h>
#include <limits.h>
#include <float.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "ds.h"
#include "memory.h"
#include "basic.h"

/*
 * 数操作函数
 *
 * */

static long num2int(Num n) {
	return (n.isFix ? (n).longValue : (long) (n).doubleValue);
}

static double num2float(Num n) {
	return (!n.isFix ? (n).doubleValue : (double) (n).longValue);
}

// 两数相加
Num num_add(Num a, Num b) {
	Num result;
	result.isFix = a.isFix && b.isFix;
	if (result.isFix) {
		result.longValue = a.longValue + b.longValue;
	} else {
		result.doubleValue = num2float(a) + num2float(b);
	}
	return result;
}

// 两数相减
Num num_sub(Num a, Num b) {
	Num result;
	result.isFix = a.isFix && b.isFix;
	if (result.isFix) {
		result.longValue = a.longValue - b.longValue;
	} else {
		result.doubleValue = num2float(a) - num2float(b);
	}
	return result;
}

// 两数相乘
Num num_mul(Num a, Num b) {
	Num result;
	result.isFix = a.isFix && b.isFix;
	if (result.isFix) {
		result.longValue = a.longValue * b.longValue;
	} else {
		result.doubleValue = num2float(a) * num2float(b);
	}
	return result;
}

// 两数相除
Num num_div(Num a, Num b) {
	Num result;
	result.isFix = a.isFix && b.isFix && (a.longValue % b.longValue == 0);
	if (result.isFix) {
		result.longValue = a.longValue / b.longValue;
	} else {
		result.doubleValue = num2float(a) / num2float(b);
	}
	return result;
}

// 两数整除
Num num_intdiv(Num a, Num b) {
	Num result;
	result.isFix = a.isFix && b.isFix;
	if (result.isFix) {
		result.longValue = a.longValue / b.longValue;
	} else {
		result.doubleValue = num2float(a) / num2float(b);
	}
	return result;
}

// 两数求余
Num num_rem(Num a, Num b) {
	long e1 = num2int(a);
	long e2 = num2int(b);
	long result = e1 % e2;
	//余数必须和第一个操作数符号相同
	if ((result > 0) && (e1 < 0))
		result -= labs(e2);
	else if ((result < 0) && (e1 > 0))
		result += labs(e2);
	Num r;
	r.isFix = a.isFix && b.isFix;
	r.longValue = result;
	return r;
}

// 两数求模
Num num_mod(Num a, Num b) {
	long e1 = num2int(a);
	long e2 = num2int(b);
	long result = e1 % e2;
	//模数必须和第二个操作数符号相同
	if (result * e2 < 0) {
		result += e2;
	}
	Num r;
	r.isFix = a.isFix && b.isFix;
	r.longValue = result;
	return r;
}

// 两数是否相等
int num_eq(Num a, Num b) {
	int result;
	int is_fixnum = a.isFix && b.isFix;
	if (is_fixnum) {
		result = (a.longValue == b.longValue);
	} else {
		result = (num2float(a) == num2float(b));
	}
	return result;
}

// 大于
int num_gt(Num a, Num b) {
	int result;
	int is_fixnum = a.isFix && b.isFix;
	if (is_fixnum) {
		result = (a.longValue > b.longValue);
	} else {
		result = (num2float(a) > num2float(b));
	}
	return result;
}

// 小于
int num_lt(Num a, Num b) {
	int result;
	int is_fixnum = a.isFix && b.isFix;
	if (is_fixnum) {
		result = (a.longValue < b.longValue);
	} else {
		result = (num2float(a) < num2float(b));
	}
	return result;
}

// 大于等于
int num_ge(Num a, Num b) {
	return !num_lt(a, b);
}

// 小于等于
int num_le(Num a, Num b) {
	return !num_gt(a, b);
}

/* Round to nearest. Round to even if midway */
// 将浮点数四舍五入到整数
double round_per_r5rs(double x) {
	double fl = floor(x);
	double ce = ceil(x);
	double dfl = x - fl;
	double dce = ce - x;
	if (dfl > dce) {
		return ce;
	} else if (dfl < dce) {
		return fl;
	} else {
		if (fmod(fl, 2.0) == 0.0) { /* I imagine this holds */
			return fl;
		} else {
			return ce;
		}
	}
}

// 判断一个浮点数是否近似为0
int is_zero_double(double x) {
	return x < DBL_MIN && x > -DBL_MIN;
}

/*
 * 特征标记函数
 *
 * */

unsigned int typeflag(Cell* p) {
	return p->_flag;
}

int type(Cell* p) {
	return (p->_flag) & TYPE_MASK;
}

// 是否atom
int is_atom(Cell* p) {
	return (p->_flag) & T_ATOM;
}

// 设置atom
void set_atom(Cell* p) {
	(p->_flag) |= T_ATOM;
}

// 清除atom
void clear_atom(Cell* p) {
	(p->_flag) &= ~T_ATOM;
}

// 是否mark
int is_mark(Cell* p) {
	return (p->_flag) & REF_MARK;
}

// 设置mark
void set_mark(Cell* p) {
	(p->_flag) |= REF_MARK;
}

// 清除mark
void clear_mark(Cell* p) {
	(p->_flag) &= ~REF_MARK;
}

/*
 * 类型
 * */

//从cell中提取数字
Num num_value(Cell* p) {
	return ((p)->_num);
}
//将一个数字转换为整数返回
long long_value(Cell* p) {
	return num2int(p->_num);
}
//将一个数字转换为浮点数返回
double double_value(Cell* p) {
	return num2float(p->_num);
}
Num* initial_num_long(Num* self, long longValue) {
	self->isFix = TRUE;
	self->longValue = longValue;
	return self;
}
Num* initial_num_double(Num* self, double doubleValue) {
	self->isFix = FALSE;
	self->doubleValue = doubleValue;
	return self;
}

long char_value(Cell* p) {
	return p->_num.longValue;
}

char *symbol_name(Cell* sym) {
	return car(sym)->_string;
}

// 闭包code
Cell* closure_code(Cell* p) {
	return car(p);
}
// 闭包env
Cell* closure_env(Cell* p) {
	return cdr(p);
}

int is_pair(Cell* p) {
	return (type(p) == T_PAIR);
}

//判断是不是数字
int is_number(Cell* p) {
	return (type(p) == T_NUMBER);
}
//判断是不是整数,或者是一个可以无损转换为整数的浮点数
int is_integer(Cell* p) {
	if (!is_number(p))
		return FALSE;
	if (p->_num.isFix || (double) long_value(p) == double_value(p))
		return TRUE;
	return FALSE;
}
//判断是不是浮点数
int is_real(Cell* p) {
	return is_number(p) && !(p->_num.isFix);
}
//判断是不是字符
int is_character(Cell* p) {
	return (type(p) == T_CHAR);
}

//是否是字符串
int is_string(Cell* p) {
	return (type(p) == T_STRING);
}
//是否是符号
int is_symbol(Cell* sym) {
	return (type(sym) == T_SYMBOL);
}

// 是否SYNTAX
int is_syntax(Cell* p) {
	return (typeflag(p) & T_SYNTAX);
}
// 是否是数组
int is_vector(Cell* p) {
	return (type(p) == T_VECTOR);
}
// 是否port
int is_port(Cell* p) {
	return (type(p) == T_PORT);
}
// 是否inport
int is_inport(Cell* p) {
	return is_port(p) && (p->_port->kind & PORT_INPUT);
}
// 是否outport
int is_outport(Cell* p) {
	return is_port(p) && (p->_port->kind & PORT_OUTPUT);
}
// 是否过程
int is_proc(Cell* p) {
	return (type(p) == T_PROC);
}
// 是否扩展过程
int is_foreign(Cell* p) {
	return (type(p) == T_FOREIGN);
}
// 是否闭包
int is_closure(Cell* p) {
	return (type(p) == T_CLOSURE);
}

// 是否promise
int is_promise(Cell* p) {
	return (type(p) == T_PROMISE);
}
// 是否宏
int is_macro(Cell* p) {
	return (type(p) == T_MACRO);
}
// 是否延续
int is_continuation(Cell* p) {
	return (type(p) == T_CONTINUATION);
}
// 是否环境
int is_environment(Cell* p) {
	return (type(p) == T_ENVIRONMENT);
}
// 是否不可改变
int is_immutable(Cell* p) {
	return (typeflag(p) & T_IMMUTABLE);
}
// 设为不可改变
void set_immutable(Cell* p) {
	p->_flag |= T_IMMUTABLE;
}

/*
 * make 函数
 *
 * */

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
	initial_num_long(&x->_num, num);
	return (x);
}

Cell* make_real(Scheme *sc, double num) {
	Cell* x = make_cell(sc, &g_nil, &g_nil);
	(x)->_flag = (T_NUMBER | T_ATOM);
	initial_num_double(&x->_num, num);
	return (x);
}

Cell* make_number(Scheme *sc, Num n) {
	if (n.isFix)
		return make_integer(sc, n.longValue);
	else
		return make_real(sc, n.doubleValue);
}

/*
 * 创建数组
 * */

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

//数组操作
void fill_vector(Cell* vec, Cell* fill_obj) {
	int num = long_value(vec) / 2 + long_value(vec) % 2;
	int i;
	for (i = 0; i < num; i++) {
		(vec + 1 + i)->_flag = T_PAIR;
		set_immutable(vec + 1 + i);		//不可变的
		(vec + 1 + i)->_pair._car = fill_obj;		//填充数组的内容
		(vec + 1 + i)->_pair._cdr = fill_obj;		//填充数组的内容
	}
}

//创建数组
Cell* make_vector(Scheme *sc, int len) {
	//一个cell中的 _cons可以保存两个元素，所以只要len/2+len%2个cell就可以保存len个元素
	// 必能分配成功，失败不会返回
	Cell* cells = get_consecutive_cells(sc, len / 2 + len % 2 + 1);
	//cells[0]用于保存数组控制信息
	(cells)->_flag = (T_VECTOR | T_ATOM);	//是数组，并是一个原子
	initial_num_long(&cells->_num, (long) len);	//设置数组的长度
	fill_vector(cells, &g_nil);
	return cells;
}
// 创建字符
Cell* make_character(Scheme *sc, int c) {
	Cell* x = make_cell(sc, &g_nil, &g_nil);
	(x)->_flag = (T_CHAR | T_ATOM);
	initial_num_long(&x->_num, (long) c);
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

#define STR_BUFF_SIZE 1024

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
			if (sscanf(name + 2, "%x", (unsigned int *) &c1)
					== 1&& c1 < UCHAR_MAX) {
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

// 创建port
Cell* make_port(Scheme *sc, Port *p) {
	Cell* x = make_cell(sc, &g_nil, &g_nil);
	x->_flag = T_PORT | T_ATOM;
	x->_port = p;
	return (x);
}

/*
 * car/cdr/cons函数
 * */
Cell* car(Cell* p) {
	return p->_pair._car;
}

Cell* cdr(Cell* p) {
	return p->_pair._cdr;
}

Cell* cons(Scheme *sc, Cell* a, Cell* b) {
	return make_pair(sc, a, b, FALSE);
}

Cell* immutable_cons(Scheme *sc, Cell* a, Cell* b) {
	return make_pair(sc, a, b, TRUE);
}

/*
 * 其他函数
 *
 * */

//将链表反序
Cell* reverse(Scheme *sc, Cell* old_list) {
	Cell* new_list = &g_nil;
	for (; is_pair(old_list); old_list = cdr(old_list)) {
		new_list = cons(sc, car(old_list), new_list);
	}
	return new_list;
}

//list* ,例子 (list* 1 2 3) => (1  2 . 3)
Cell* list_star(Scheme *sc, Cell* old_list) {
	old_list = reverse(sc, old_list);
	Cell* new_list = car(old_list);
	old_list = cdr(old_list);
	while (is_pair(old_list)) {
		new_list = cons(sc, car(old_list), new_list);
		old_list = cdr(old_list);
	}
	return new_list;
}

/* append list -- produce new list (in reverse order) */
//如果acc 为 (4 3) ,  list 为 (5 6) , 则最后返回结果应当为 (6 5 4 3)
Cell* revappend(Scheme *sc, Cell* acc, Cell* list) {
	while (is_pair(list)) {
		acc = cons(sc, car(list), acc);
		list = cdr(list);
	}
	if (list == &g_nil)
		return acc;
	return &g_false; /* signal an error */
}

/* Result is:
 proper list: length
 circular list: -1			//环行链表
 not even a pair: -2	//如果是这样的形式(a . b) （b不是pair）
 dotted list: -2 minus length before dot	//pair 链表的最后一个cell不是nil
 */
int list_length(Scheme *sc, Cell* a) {
	int len = 0;
	Cell* slow = a;
	Cell* fast = a;
	while (TRUE) {
		if (fast == &g_nil)
			return len;
		if (!is_pair(fast))
			return -2 - len;
		fast = cdr(fast);
		++len;
		if (fast == &g_nil)
			return len;
		if (!is_pair(fast))
			return -2 - len;
		fast = cdr(fast);
		++len;
		slow = cdr(slow);
		//如果a中有一个环 fast 和slow都会最终进入这个环中，并且有于fast是slow的速度的两倍，索引fast最终能赶上slow
		if (fast == slow)
			return -1;
	}
}

int is_list(Scheme *sc, Cell* a) {
	return list_length(sc, a) >= 0;
}

//取数组中的第index个元素
Cell* get_vector_item(Cell* vec, int index) {
	int n = index / 2;
	if (index % 2 == 0) {
		return car(vec + 1 + n);
	} else {
		return cdr(vec + 1 + n);
	}
}
//设置数组中的第index个元素的值
Cell* set_vector_item(Cell* vec, int index, Cell* a) {
	int n = index / 2;
	if (index % 2 == 0) {
		((vec + 1 + n)->_pair._car) = a;
		return ((vec + 1 + n)->_pair._car);
	} else {
		((vec + 1 + n)->_pair._cdr) = a;
		return ((vec + 1 + n)->_pair._cdr);
	}
}

/* equivalence of atoms */
int eqv(Cell* a, Cell* b) {
	if (is_string(a)) {
		if (is_string(b))
			return ((a)->_string == (b)->_string);
		else
			return FALSE;
	} else if (is_number(a)) {
		if (is_number(b)) {
			if (a->_num.isFix == b->_num.isFix)
				return num_eq(num_value(a), num_value(b));
		}
		return FALSE;
	} else if (is_character(a)) {
		if (is_character(b))
			return char_value(a) == char_value(b);
		else
			return FALSE;
	} else if (is_port(a)) {
		if (is_port(b))
			return a == b;
		else
			return FALSE;
	} else if (is_proc(a)) {
		if (is_proc(b))
			//return long_value(a) == long_value(b);
			return a->_op == b->_op;
		else
			return FALSE;
	} else {
		return (a == b);
	}
}

//将字符串形式的二进制数据转化为整数
long binary_decode(const char *s) {
	long x = 0;
	while (*s != 0 && (*s == '1' || *s == '0')) {
		x <<= 1;
		x += *s - '0';
		s++;
	}
	return x;
}

int hash_fn(const char *key, int table_size) {
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

//端口
int file_interactive(Scheme *sc) {
	//return sc->top_file_index == 0 && (sc->load_stack[0].kind & port_file)
	//		&& sc->load_stack[0].f.file == stdin;
	return (sc->curPort->kind & PORT_FILE)
			&& (sc->curPort->filePort.file == stdin);
}

void port_close(Cell* p, int flag) {
	Port *pt = p->_port;
	pt->kind &= ~flag;
	if ((pt->kind & (PORT_INPUT | PORT_OUTPUT)) == 0) {
		if (pt->kind & PORT_FILE) {
			if (pt->filePort.filename)
				free(pt->filePort.filename);
			fclose(pt->filePort.file);
		}
		pt->kind = PORT_FREE;
	}
}

#define MAX_FILES 256
#define BLOCK_SIZE 256

/*
 int push_load_file(Scheme *sc, const char *fname) {
 if (sc->top_file_index == MAX_FILES - 1)
 return 0;
 FILE *fin = fopen(fname, "r");
 if (fin != 0) {
 sc->top_file_index++;
 sc->load_stack[sc->top_file_index].kind = port_file | port_input;
 sc->load_stack[sc->top_file_index].f.file = fin;
 sc->load_stack[sc->top_file_index].f.closeit = 1;
 sc->load_stack[sc->top_file_index].f.curr_line = 0;
 if (fname)
 sc->load_stack[sc->top_file_index].f.filename = store_string(
 strlen(fname), fname, 0);
 sc->nesting_stack[sc->top_file_index] = 0;
 sc->loadport->_port = sc->load_stack + sc->top_file_index;
 }
 return fin != 0;
 }

 void pop_load_file(Scheme *sc) {
 if (sc->top_file_index != 0) {
 sc->nesting = sc->nesting_stack[sc->top_file_index];
 port_close(sc->loadport, port_input);
 sc->top_file_index--;
 sc->loadport->_port = sc->load_stack + sc->top_file_index;
 }
 }
 */

Cell* port_from_file(Scheme *sc, FILE *file, int prop) {
	Port *pt = (Port *) malloc(sizeof *pt);
	pt->kind = PORT_FILE | prop;
	pt->filePort.file = file;
	pt->filePort.closed = FALSE;
	return make_port(sc, pt);
}

Cell* port_from_filename(Scheme *sc, const char *file_name, int prop) {
	char *mode = "r";
	if (prop == (PORT_INPUT | PORT_OUTPUT))
		mode = "a+";
	else if (prop == PORT_OUTPUT)
		mode = "w";
	FILE *f = fopen(file_name, mode);
	if (f == 0)
		return 0;
	Port *pt = (Port *) malloc(sizeof *pt);
	pt->kind = PORT_FILE | prop;
	pt->filePort.file = f;
	pt->filePort.closed = TRUE;
	pt->filePort.curLine = 0;
	if (file_name)
		pt->filePort.filename = store_string(strlen(file_name), file_name, 0);
	return make_port(sc, pt);
}

//为字符串类型的端口重新分配内存
int realloc_port_string(Scheme *sc, Port *p) {
	char *data = p->stringPort.start;
	size_t data_size = p->stringPort.end - p->stringPort.start;
	size_t new_size = data_size + BLOCK_SIZE + 1;
	p->stringPort.start = (char*) malloc(new_size);
	memset(p->stringPort.start, 0, new_size);
	if (data)
		strcpy(p->stringPort.start, data);
	p->stringPort.end = p->stringPort.start + new_size;
	p->stringPort.cur = p->stringPort.start + data_size;
	if (data)
		free(data);
	return TRUE;
}

Cell* port_from_string(Scheme *sc, char *start, char *end, int prop) {
	Port *pt = (Port*) malloc(sizeof(Port));
	pt->kind = PORT_STRING | prop;
	pt->stringPort.start = start;
	pt->stringPort.end = end;
	if (!start)
		realloc_port_string(sc, pt);
	pt->stringPort.cur = pt->stringPort.start;
	return make_port(sc, pt);
}

//符号列表初始化
Cell* oblist_initial_value(Scheme *sc) {
	return make_vector(sc, 461); /* probably should be bigger */
}

//返回所有符号的列表
Cell* oblist_all_symbols(Scheme *sc) {
	Cell* ob_list = &g_nil;
	int i;
	Cell* sym_list;
	for (i = 0; i < sc->objectList->_num.longValue; i++) {
		for (sym_list = get_vector_item(sc->objectList, i); sym_list != &g_nil;
				sym_list = cdr(sym_list)) {
			ob_list = cons(sc, car(sym_list), ob_list);
		}
	}
	return ob_list;
}

//生成一个名字唯一的符号
Cell* gensym(Scheme *sc) {
	static long s_gensym_cnt = 0;
	for (; s_gensym_cnt < LONG_MAX; s_gensym_cnt++) {
		char name[40];
		snprintf(name, 40, "gensym-%ld", s_gensym_cnt);
		Cell* sym = oblist_find_by_name(sc, name);
		if (sym != &g_nil)
			continue;
		return oblist_add_by_name(sc, name);
	}
	return &g_nil;
}

static const char *charnames[32] = { "nul", "soh", "stx", "etx", "eot", "enq",
		"ack", "bel", "bs", "ht", "lf", "vt", "ff", "cr", "so", "si", "dle",
		"dc1", "dc2", "dc3", "dc4", "nak", "syn", "etb", "can", "em", "sub",
		"esc", "fs", "gs", "rs", "us" };

int is_ascii_name(const char *name, int *pc) {
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

void add_syntax_symbol(Scheme *sc, String name) {
	Cell* sym = make_symbol(sc, name);
	sym->_flag |= T_SYNTAX;
}

/*
 * 读取函数
 *
 * */

/* get new character from input file */
int get_char(Scheme *sc) {
	Port *port = sc->inPort->_port;
	if (port->kind & PORT_EOF)
		return EOF;
	int c = 0;
	if (port->kind & PORT_FILE) {
		c = fgetc(port->filePort.file);
	} else {
		if (*port->stringPort.cur == 0
				|| port->stringPort.cur == port->stringPort.end) {
			c = EOF;
		} else {
			c = *port->stringPort.cur++;
		}
	}
	if (c == EOF)
		port->kind |= PORT_EOF;
	return c;
}

/* back character to input buffer */
void unget_char(Scheme *sc, int c) {
	if (c == EOF)
		return;
	Port *port = sc->inPort->_port;
	if (port->kind & PORT_FILE) {
		ungetc(c, port->filePort.file);
	} else {
		if (port->stringPort.cur != port->stringPort.start)
			port->stringPort.cur--;
		//else 0;//error ！！！
	}
}

/* check c is in chars */
int find_char_in_string(char *s, int c) {
	if (c == EOF)
		return TRUE;
	while (*s) {
		if (*s++ == c)
			return TRUE;
	}
	return FALSE;
}

/* skip white characters */
int skipspace(Scheme *sc) {
	int c = 0, curr_line = 0;
	do {
		c = get_char(sc);
		if (c == '\n')
			curr_line++;
	} while (isspace(c));
	if (sc->inPort->_port->kind & PORT_FILE)
		sc->inPort->_port->filePort.curLine += curr_line;
	if (c != EOF) {
		unget_char(sc, c);
		return 1;
	} else
		return EOF;
}

/* read characters up to delimiter, but cater to character constants */
//读取一个字符串直到遇到分隔符
char *readstr_upto(Scheme *sc, char *delim) {
	char *p = sc->stringBuffer;
	while ((p - sc->stringBuffer < sizeof(sc->stringBuffer))
			&& !find_char_in_string(delim, (*p++ = get_char(sc))))
		; //读取字符串，直到遇到一个分隔符
	unget_char(sc, p[-1]);
	*--p = '\0';
	return sc->stringBuffer;
}

Cell* readstrexp(Scheme *sc) {
	char *p = sc->stringBuffer;

	int c1 = 0;
	enum {
		st_ok, st_bsl, st_x1, st_x2, st_oct1, st_oct2
	} state = st_ok;

	while (TRUE) {
		char c = get_char(sc);
		if (c == EOF || p - sc->stringBuffer > sizeof(sc->stringBuffer) - 1) {
			return &g_false;
		}
		switch (state) {
		case st_ok:
			switch (c) {
			case '\\':
				state = st_bsl;
				break;
			case '"':
				*p = 0;
				return make_string_n(sc, sc->stringBuffer, p - sc->stringBuffer);
			default:
				*p++ = c;
				break;
			}
			break;
		case st_bsl:
			switch (c) {
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
				state = st_oct1;
				c1 = c - '0';
				break;
			case 'x':
			case 'X':
				state = st_x1;
				c1 = 0;
				break;
			case 'n':
				*p++ = '\n';
				state = st_ok;
				break;
			case 't':
				*p++ = '\t';
				state = st_ok;
				break;
			case 'r':
				*p++ = '\r';
				state = st_ok;
				break;
			case '"':
				*p++ = '"';
				state = st_ok;
				break;
			default:
				*p++ = c;
				state = st_ok;
				break;
			}
			break;
		case st_x1:
		case st_x2:
			c = toupper(c);
			if (c >= '0' && c <= 'F') {
				if (c <= '9') {
					c1 = (c1 << 4) + c - '0';
				} else {
					c1 = (c1 << 4) + c - 'A' + 10;
				}
				if (state == st_x1) {
					state = st_x2;
				} else {
					*p++ = c1;
					state = st_ok;
				}
			} else {
				return &g_false;
			}
			break;
		case st_oct1:
		case st_oct2:
			if (c < '0' || c > '7') {
				*p++ = c1;
				unget_char(sc, c);
				state = st_ok;
			} else {
				if (state == st_oct2 && c1 >= 32)
					return &g_false;

				c1 = (c1 << 3) + (c - '0');

				if (state == st_oct1)
					state = st_oct2;
				else {
					*p++ = c1;
					state = st_ok;
				}
			}
			break;
		}
	}
}

/* get token */
int token(Scheme *sc) {
	int c = skipspace(sc);
	if (c == EOF)
		return (TOK_EOF);
	switch (c = get_char(sc)) {
	case EOF:
		return (TOK_EOF);
	case '(':
		return (TOK_LPAREN);
	case ')':
		return (TOK_RPAREN);
	case '.':
		c = get_char(sc);
		if (find_char_in_string(" \n\t", c)) {
			return (TOK_DOT);
		} else {
			unget_char(sc, c);
			unget_char(sc, '.');
			return TOK_ATOM;
		}
	case '\'': //引用
		return (TOK_QUOTE);
	case ';': //注释
		while ((c = get_char(sc)) != '\n' && c != EOF)
			; //跳过注释
		if (c == '\n' && (sc->inPort->_port->kind & PORT_FILE))
			sc->inPort->_port->filePort.curLine++;
		if (c == EOF)
			return TOK_EOF;
		else
			return token(sc);
	case '"': //双引号
		return (TOK_DQUOTE);
	case '`': //准引用
		return (TOK_BQUOTE);
	case ',':
		if ((c = get_char(sc)) == '@') {
			return (TOK_ATMARK);
		} else {
			unget_char(sc, c);
			return (TOK_COMMA);
		}
	case '#':
		c = get_char(sc);
		if (c == '(')
			return TOK_VECTOR;
		else if (c == '!') { //#!开头的注释
			while ((c = get_char(sc)) != '\n' && c != EOF)
				;
			if (c == '\n' && (sc->inPort->_port->kind & PORT_FILE))
				sc->inPort->_port->filePort.curLine++;
			if (c == EOF)
				return TOK_EOF;
			return token(sc);
		} else {
			unget_char(sc, c);
			if (find_char_in_string(" tfodxb\\", c))
				return TOK_SHARP_CONST;
			else
				return (TOK_SHARP);
		}
	default:
		unget_char(sc, c);
		return (TOK_ATOM);
	}
}

/*
 * 写入函数
 *
 * */

void write_char(Scheme *sc, int c) {
	Port *port = sc->outPort->_port;
	if (port->kind & PORT_FILE) {
		fputc(c, port->filePort.file);
	} else {
		if (port->stringPort.cur != port->stringPort.end) {
			*port->stringPort.cur++ = c;
		} else if ((port->kind & PORT_CAN_REALLOC)
				&& realloc_port_string(sc, port)) {
			*port->stringPort.cur++ = c;
		}
	}
}

void write_string(Scheme *sc, const char *s) {
	Port *port = sc->outPort->_port;
	int len = strlen(s);
	if (port->kind & PORT_FILE) {
		fwrite(s, 1, len, port->filePort.file);
	} else {
		for (; len; len--) {
			if (port->stringPort.cur != port->stringPort.end) {
				*port->stringPort.cur++ = *s++;
			} else if ((port->kind & PORT_CAN_REALLOC)
					&& realloc_port_string(sc, port)) {
				*port->stringPort.cur++ = *s++;
			}
		}
	}
}

//打印一个字符串为字符串字面量
static void printslashstring(Scheme *sc, char *p, int len) {
	unsigned char *s = (unsigned char*) p;
	write_char(sc, '"');
	int i;
	for (i = 0; i < len; i++) {
		if (*s == 0xff || *s == '"' || *s == '\\' || *s < ' ') {
			write_char(sc, '\\');
			switch (*s) {
			case '"':
				write_char(sc, '"');
				break;
			case '\n':
				write_char(sc, 'n');
				break;
			case '\t':
				write_char(sc, 't');
				break;
			case '\r':
				write_char(sc, 'r');
				break;
			case '\\':
				write_char(sc, '\\');
				break;
			default: {
				int d = *s / 16;
				write_char(sc, 'x');
				if (d < 10) {
					write_char(sc, d + '0');
				} else {
					write_char(sc, d - 10 + 'A');
				}
				d = *s % 16;
				if (d < 10) {
					write_char(sc, d + '0');
				} else {
					write_char(sc, d - 10 + 'A');
				}
			}
			}
		} else {
			write_char(sc, *s);
		}
		s++;
	}
	write_char(sc, '"');
}

/* Uses internal buffer unless string cell* is already available */
void atom2str(Scheme *sc, Cell* cell, int flag, char **pp,
		int *plen) {
	char *p = sc->stringBuffer;
	if (cell == &g_nil) {
		p = "()";
	} else if (cell == &g_true) {
		p = "#t";
	} else if (cell == &g_false) {
		p = "#f";
	} else if (cell == &g_eof) {
		p = "#<EOF>";
	} else if (is_port(cell)) {
		p = "#<PORT>";
	} else if (is_symbol(cell)) {
		p = symbol_name(cell);
	} else if (is_macro(cell)) {
		p = "#<MACRO>";
	} else if (is_closure(cell)) {
		p = "#<CLOSURE>";
	} else if (is_promise(cell)) {
		p = "#<PROMISE>";
	} else if (is_proc(cell)) {
		snprintf(p, STR_BUFF_SIZE, "#<PROCEDURE>");
		//snprintf(p, STR_BUFF_SIZE, "#<%s PROCEDURE %ld>", proc_name(cell), proc_value(cell));
	} else if (is_foreign(cell)) {
		snprintf(p, STR_BUFF_SIZE, "#<FOREIGN PROCEDURE>");
		//snprintf(p, STR_BUFF_SIZE, "#<FOREIGN PROCEDURE %ld>", proc_value(cell));
	} else if (is_continuation(cell)) {
		p = "#<CONTINUATION>";
	} else if (is_number(cell)) {
		p = sc->stringBuffer;
		if (flag <= 1 || flag == 10) /* f is the base for numbers if > 1 */{
			if (cell->_num.isFix) {
				snprintf(p, STR_BUFF_SIZE, "%ld", cell->_num.longValue);
			} else {
				snprintf(p, STR_BUFF_SIZE, "%.10g", cell->_num.doubleValue);
				/* r5rs says there must be a '.' (unless 'e'?) */
				flag = strcspn(p, ".e");
				if (p[flag] == 0) {
					p[flag] = '.'; /* not found, so add '.0' at the end */
					p[flag + 1] = '0';
					p[flag + 2] = 0;
				}
			}
		} else {
			long v = long_value(cell);
			if (flag == 16) {
				if (v >= 0)
					snprintf(p, STR_BUFF_SIZE, "%lx", v);
				else
					snprintf(p, STR_BUFF_SIZE, "-%lx", -v);
			} else if (flag == 8) {
				if (v >= 0)
					snprintf(p, STR_BUFF_SIZE, "%lo", v);
				else
					snprintf(p, STR_BUFF_SIZE, "-%lo", -v);
			} else if (flag == 2) {
				unsigned long b = (v < 0) ? -v : v;
				p = &p[STR_BUFF_SIZE - 1];
				*p = 0;
				do {
					*--p = (b & 1) ? '1' : '0';
					b >>= 1;
				} while (b != 0);
				if (v < 0)
					*--p = '-';
			}
		}
	} else if (is_string(cell)) {
		if (!flag) {
			p = cell->_string;
		} else { /* Hack, uses the fact that printing is needed */
			*pp = sc->stringBuffer;
			*plen = 0;
			printslashstring(sc, cell->_string, strlen(cell->_string));
			return;
		}
	} else if (is_character(cell)) {
		int c = char_value(cell);
		if (!flag) {
			p[0] = c;
			p[1] = 0;
		} else {
			switch (c) {
			case ' ':
				snprintf(p, STR_BUFF_SIZE, "#\\space");
				break;
			case '\n':
				snprintf(p, STR_BUFF_SIZE, "#\\newline");
				break;
			case '\r':
				snprintf(p, STR_BUFF_SIZE, "#\\return");
				break;
			case '\t':
				snprintf(p, STR_BUFF_SIZE, "#\\tab");
				break;
			default:
				if (c == 127) {
					snprintf(p, STR_BUFF_SIZE, "#\\del");
					break;
				} else if (c < 32) {
					snprintf(p, STR_BUFF_SIZE, "#\\%s", charnames[c]);
					break;
				}
				snprintf(p, STR_BUFF_SIZE, "#\\%c", c);
				break;
				break;
			}
		}
	} else {
		p = "#<ERROR>";
	}
	*pp = p;
	*plen = strlen(p);
}

/* print atoms */
void printatom(Scheme *sc, Cell* cell, int flag) {
	char *p;
	int len;
	atom2str(sc, cell, flag, &p, &len);
	write_string(sc, p);
}

/*
 * 环境
 *
 * */

void new_frame_in_env(Scheme *sc, Cell* oldEnv) {
	Cell* newFrame = &g_nil;
	if (oldEnv == &g_nil)
		newFrame = make_vector(sc, 461);
	sc->env = immutable_cons(sc, newFrame, oldEnv);
	(sc->env)->_flag = T_ENVIRONMENT;
}

void new_slot_in_environment(Scheme *sc, Cell* env, Cell* sym, Cell* value) {
	Cell* slot = immutable_cons(sc, sym, value);
	if (is_vector(car(env))) {
		int location = hash_fn(symbol_name(sym), long_value_unchecked(car(env)));
		set_vector_item(car(env), location,
				immutable_cons(sc, slot, get_vector_item(car(env), location)));
	} else {
		env->_pair._car = immutable_cons(sc, slot, car(env));
	}
}

void new_slot_in_env(Scheme *sc, Cell* sym, Cell* value) {
	new_slot_in_environment(sc, sc->env, sym, value);
}

Cell* find_slot_in_env(Scheme *sc, Cell* env, Cell* sym, int all) {
	for (; env != &g_nil; env = cdr(env)) {
		Cell* item = NULL; /*symbol_kv_list*/
		if (is_vector(car(env))) {
			int location = hash_fn(symbol_name(sym), long_value_unchecked(car(env)));
			item = get_vector_item(car(env), location);
		} else {
			item = car(env);
		}
		for (; item != &g_nil; item = cdr(item)) {
			if (caar(item) == sym)
				return car(item);
		}
		if (!all)
			return &g_nil;
	}
	return &g_nil;
}

void set_slot_in_env(Scheme *sc, Cell* symbol, Cell* value) {
	symbol->_pair._cdr = value;
}

Cell* slot_value_in_env(Cell* symbol) {
	return cdr(symbol);
}

void scheme_define(Scheme *sc, Cell* envir, Cell* symbol, Cell* value) {
	Cell* p = find_slot_in_env(sc, envir, symbol, FALSE);
	if (p != &g_nil)
		set_slot_in_env(sc, p, value);
	else
		new_slot_in_environment(sc, envir, symbol, value);
}

