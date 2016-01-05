/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2015 by fanguangping (fanguangping@163.com)
 * env.c
 */

#include "operator.h"

/*
 * 环境
 *
 * */

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
		int location = hash_fn(symbol_name(sym), car(env)->_num.longValue);
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
	if (!is_symbol(sym)) {
		return &g_nil;
	}

	for (; env != &g_nil; env = cdr(env)) {
		Cell* item = NULL; /*symbol_kv_list*/
		if (is_vector(car(env))) {
			int location = hash_fn(symbol_name(sym), car(env)->_num.longValue);
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

// (interaction-environment)
Cell* op_int_env(Scheme *sc) {
	return s_return_helper(sc, sc->globalEnv);
}

// (current-environment)
Cell* op_curr_env(Scheme *sc) {
	return s_return_helper(sc, sc->env);
}
