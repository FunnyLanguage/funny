/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2015 by fanguangping (fanguangping@163.com)
 * logic.c
 */

#include "operator.h"

static double num2float(Num n) {
	return (!n.isFix ? (n).doubleValue : (double) (n).longValue);
}

// 两数是否相等
static int num_eq(Num a, Num b) {
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
static int num_gt(Num a, Num b) {
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
static int num_lt(Num a, Num b) {
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
static int num_ge(Num a, Num b) {
	return !num_lt(a, b);
}

// 小于等于
static int num_le(Num a, Num b) {
	return !num_gt(a, b);
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
	} else if (is_proc(a)) {
		if (is_proc(b))
			return a->_op == b->_op;
		else
			return FALSE;
	} else {
		return (a == b);
	}
}

Cell* op_and0(Scheme *sc) {
	if (sc->code == &g_nil)
		return s_return_helper(sc, &g_true);
	s_save(sc, op_and1, &g_nil, cdr(sc->code));
	sc->code = car(sc->code);
	sc->op = op_eval;
	return &g_true;
}

Cell* op_and1(Scheme *sc) {
	if (is_false(sc->returnValue))
		return s_return_helper(sc, sc->returnValue);
	else if (sc->code == &g_nil)
		return s_return_helper(sc, sc->returnValue);
	else {
		s_save(sc, op_and1, &g_nil, cdr(sc->code));
		sc->code = car(sc->code);
		sc->op = op_eval;
		return &g_true;
	}
}

Cell* op_or0(Scheme *sc) {
	if (sc->code == &g_nil)
		return s_return_helper(sc,&g_false);
	s_save(sc, op_or1, &g_nil, cdr(sc->code));
	sc->code = car(sc->code);
	sc->op = op_eval;
	return &g_true;
}

Cell* op_or1(Scheme *sc) {
	if (is_true(sc->returnValue))
		return s_return_helper(sc, sc->returnValue);
	else if (sc->code == &g_nil)
		return s_return_helper(sc, sc->returnValue);
	else {
		s_save(sc, op_or1, &g_nil, cdr(sc->code));
		sc->code = car(sc->code);
		sc->op = op_eval;
		return &g_true;
	}
}

// (not x)
Cell* op_not(Scheme *sc) {
	int istrue = (is_false(first(sc->args)));
	return s_return_helper(sc, (istrue) ? &g_true : &g_false);
}

// (= x y ...)
Cell* op_numeq(Scheme *sc) {
	Cell* x = sc->args;
	Num value;
	value = num_value(car(x));
	for (x = cdr(x); x != &g_nil; x = cdr(x)) {
		if (!num_eq(value, num_value(car(x)))) {
			return s_return_helper(sc, &g_false);
		}
		value = num_value(car(x));
	}
	return s_return_helper(sc, &g_true);
}

// (< x y ...)
Cell* op_less(Scheme *sc) {
	Cell* x = sc->args;
	Num value;
	value = num_value(car(x));
	for (x = cdr(x); x != &g_nil; x = cdr(x)) {
		if (!num_lt(value, num_value(car(x)))) {
			return s_return_helper(sc, &g_false);
		}
		value = num_value(car(x));
	}
	return s_return_helper(sc, &g_true);
}

// (> x y ...)
Cell* op_gre(Scheme *sc) {
	Cell* x = sc->args;
	Num value;
	value = num_value(car(x));
	for (x = cdr(x); x != &g_nil; x = cdr(x)) {
		if (!num_gt(value, num_value(car(x)))) {
			return s_return_helper(sc, &g_false);
		}
		value = num_value(car(x));
	}
	return s_return_helper(sc, &g_true);
}

// (<= x y ...)
Cell* op_leq(Scheme *sc) {
	Cell* x = sc->args;
	Num value;
	value = num_value(car(x));
	for (x = cdr(x); x != &g_nil; x = cdr(x)) {
		if (!num_le(value, num_value(car(x)))) {
			return s_return_helper(sc, &g_false);
		}
		value = num_value(car(x));
	}
	return s_return_helper(sc, &g_true);
}

// (>= x y ...)
Cell* op_geq(Scheme *sc) {
	Cell* x = sc->args;
	Num value;
	value = num_value(car(x));
	for (x = cdr(x); x != &g_nil; x = cdr(x)) {
		if (!num_ge(value, num_value(car(x)))) {
			return s_return_helper(sc, &g_false);
		}
		value = num_value(car(x));
	}
	return s_return_helper(sc, &g_true);
}
