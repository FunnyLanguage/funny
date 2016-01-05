/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2015 by fanguangping (fanguangping@163.com)
 * logic.h
 */

#ifndef LOGIC_H_
#define LOGIC_H_

#include "ds.h"

int eqv(Cell* a, Cell* b);

Cell* op_and0(Scheme *sc);
Cell* op_and1(Scheme *sc);
Cell* op_or0(Scheme *sc);
Cell* op_or1(Scheme *sc);
Cell* op_not(Scheme *sc);
Cell* op_numeq(Scheme *sc);
Cell* op_less(Scheme *sc);
Cell* op_gre(Scheme *sc);
Cell* op_leq(Scheme *sc);
Cell* op_geq(Scheme *sc);

#endif /* LOGIC_H_ */
