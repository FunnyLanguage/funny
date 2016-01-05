/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2015 by fanguangping (fanguangping@163.com)
 * ds.c
 */

#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "ds.h"
#include "maker.h"
#include "predicate.h"
#include "math.h"

void init_constants() {
	(&g_nil)->_flag = (T_ATOM | REF_MARK);
	(&g_nil)->_pair._car = (&g_nil)->_pair._cdr = &g_nil;
	(&g_true)->_flag = (T_ATOM | REF_MARK);
	(&g_true)->_pair._car = (&g_true)->_pair._cdr = &g_true;
	(&g_false)->_flag = (T_ATOM | REF_MARK);
	(&g_false)->_pair._car = (&g_false)->_pair._cdr = &g_false;
	(&g_eof)->_flag = (T_ATOM | REF_MARK);
	(&g_eof)->_pair._car = (&g_eof)->_pair._cdr = &g_eof;
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

static long num2int(Num n) {
	return (n.isFix ? (n).longValue : (long) (n).doubleValue);
}

static double num2float(Num n) {
	return (!n.isFix ? (n).doubleValue : (double) (n).longValue);
}

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

// 设为不可改变
void set_immutable(Cell* p) {
	p->_flag |= T_IMMUTABLE;
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
