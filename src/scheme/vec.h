/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2015 by fanguangping (fanguangping@163.com)
 * vec.h
 */

#ifndef VEC_H_
#define VEC_H_

#include "ds.h"

Cell* get_vector_item(Cell* vec, int index);
Cell* set_vector_item(Cell* vec, int index, Cell* a);
void fill_vector(Cell* vec, Cell* fill_obj);

Cell* op_vector(Scheme *sc);
Cell* op_makevector(Scheme *sc);
Cell* op_veclen(Scheme *sc);
Cell* op_vecref(Scheme *sc);
Cell* op_vecset(Scheme *sc);

#endif /* VEC_H_ */
