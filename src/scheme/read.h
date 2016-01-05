/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2015 by fanguangping (fanguangping@163.com)
 * read.h
 */

#ifndef READ_H_
#define READ_H_

#include "ds.h"

Cell* op_read_internal(Scheme *sc);
Cell* op_read_sexpr(Scheme *sc);
Cell* op_read_list(Scheme *sc);
Cell* op_read_vec(Scheme *sc);
Cell* op_read_dot(Scheme *sc);
Cell* op_read_quote(Scheme *sc);

#endif /* READ_H_ */
