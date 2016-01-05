/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2015 by fanguangping (fanguangping@163.com)
 * prog.h
 */

#ifndef PROG_H_
#define PROG_H_

#include "ds.h"

Cell* op_begin(Scheme *sc);
Cell* op_if0(Scheme *sc);
Cell* op_if1(Scheme *sc);
Cell* op_cond0(Scheme *sc);
Cell* op_cond1(Scheme *sc);
Cell* op_case0(Scheme *sc);
Cell* op_case1(Scheme *sc);
Cell* op_force(Scheme *sc);
Cell* op_save_forced(Scheme *sc);

#endif /* PROG_H_ */
