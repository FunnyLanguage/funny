/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2015 by fanguangping (fanguangping@163.com)
 * vec.c
 */

#include "operator.h"

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
	Cell* p = vec + 1 + n;
	if (index % 2 == 0) {
		p->_pair._car = a;
		return (p->_pair._car);
	} else {
		p->_pair._cdr = a;
		return (p->_pair._cdr);
	}
}

//填充数组
void fill_vector(Cell* vec, Cell* fill_obj) {
	int num = long_value(vec) / 2 + long_value(vec) % 2;
	Cell* p = NULL;
	int i;
	for (i = 0; i < num; i++) {
		p = vec + 1 + i;
		p->_flag = T_PAIR;
		set_immutable(p);		//不可变的
		p->_pair._car = fill_obj;		//填充数组的内容
		p->_pair._cdr = fill_obj;		//填充数组的内容
	}
}

// (vector x y ...)
Cell* op_vector(Scheme *sc) {
	Cell* x;
	int len = ls_length(sc, sc->args);
	if (len < 0)
		return error_helper(sc, "vector: not a proper list:", sc->args);
	Cell* vec = make_vector(sc, len);
	int index = 0;
	for (x = sc->args; is_pair(x); x = cdr(x), index++) {
		set_vector_item(vec, index, car(x));
	}
	return s_return_helper(sc, vec);
}

// (make-vector n e)
Cell* op_makevector(Scheme *sc) {
	int len = long_value(first(sc->args));
	Cell* vec = make_vector(sc, len);
	if (rest(sc->args) != &g_nil) {
		fill_vector(vec, second(sc->args));
	}
	return s_return_helper(sc, vec);
}

// (vector-length x)
Cell* op_veclen(Scheme *sc) {
	return s_return_helper(sc, make_integer(sc, long_value(first(sc->args))));
}

// (vector-ref x index)
Cell* op_vecref(Scheme *sc) {
	int index = long_value(second(sc->args));
	if (index < 0 || index >= long_value(first(sc->args))) {
		return error_helper(sc, "vector-ref: out of bounds:", second(sc->args));
	}
	return s_return_helper(sc, get_vector_item(first(sc->args), index));
}

// (vector-set! x index e)
Cell* op_vecset(Scheme *sc) {
	if (is_immutable(first(sc->args))) {
		return error_helper(sc,
				"vector-set!: unable to alter immutable vector:",
				first(sc->args));
	}
	int index = long_value(second(sc->args));
	if (index < 0 || index >= long_value(first(sc->args))) {
		return error_helper(sc, "vector-set!: out of bounds:", second(sc->args));
	}
	set_vector_item(first(sc->args), index, third(sc->args));
	return s_return_helper(sc, first(sc->args));
}
