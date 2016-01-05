/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2015 by fanguangping (fanguangping@163.com)
 * other.c
 */

#include <string.h>
#include <limits.h>
#include "operator.h"

Cell* op_set0(Scheme *sc) {
	//形式 (set! var_name expr)
	if (is_immutable(car(sc->code)))
		return error_helper(sc, "set!: unable to alter immutable variable",
				car(sc->code));
	s_save(sc, op_set1, &g_nil, car(sc->code));
	sc->code = cadr(sc->code);				//sc->code结果为expr
	//对expr进行求值
	sc->op = op_eval;
	return &g_true;
}

Cell* op_set1(Scheme *sc) {
	Cell *y;

	//sc->code为 var_name
	y = find_slot_in_env(sc, sc->env, sc->code, TRUE);
	if (y != &g_nil) {
		set_slot_in_env(sc, y, sc->returnValue);
		return s_return_helper(sc, sc->returnValue);
	} else {
		return error_helper(sc, "set!: unbound variable:", sc->code);
	}
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

//生成一个名字唯一的符号
static Cell* gensym(Scheme *sc) {
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

// (gensym)
Cell* op_gensym(Scheme *sc) {
	return s_return_helper(sc, gensym(sc));
}


//返回所有符号的列表
static Cell* oblist_all_symbols(Scheme *sc) {
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

// (oblist)
Cell* op_oblist(Scheme *sc) {
	return s_return_helper(sc, oblist_all_symbols(sc));
}
