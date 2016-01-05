/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2015 by fanguangping (fanguangping@163.com)
 * maker.h
 */

#ifndef MAKER_H_
#define MAKER_H_

#include "ds.h"

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

Cell* op_make_closure(Scheme *sc);
Cell* op_makestring(Scheme *sc);
Cell* op_newsegment(Scheme *sc);

#endif /* MAKER_H_ */
