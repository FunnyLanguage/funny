/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2015 by fanguangping (fanguangping@163.com)
 * let.h
 */

#ifndef LET_H_
#define LET_H_

#include "ds.h"

Cell* op_let0(Scheme *sc);
Cell* op_let1(Scheme *sc);
Cell* op_let2(Scheme *sc);
Cell* op_letstar0(Scheme *sc);
Cell* op_letstar1(Scheme *sc);
Cell* op_letstar2(Scheme *sc);
Cell* op_let0rec(Scheme *sc);
Cell* op_let1rec(Scheme *sc);
Cell* op_let2rec(Scheme *sc);

#endif /* LET_H_ */
