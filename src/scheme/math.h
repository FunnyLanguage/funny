/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2015 by fanguangping (fanguangping@163.com)
 * math.h
 */

#ifndef MATH_H_
#define MATH_H_

#include "ds.h"

Cell* op_inexact2exact(Scheme *sc);
Cell* op_exp(Scheme *sc);
Cell* op_log(Scheme *sc);
Cell* op_sin(Scheme *sc);
Cell* op_cos(Scheme *sc);
Cell* op_tan(Scheme *sc);
Cell* op_asin(Scheme *sc);
Cell* op_acos(Scheme *sc);
Cell* op_atan(Scheme *sc);
Cell* op_sqrt(Scheme *sc);
Cell* op_expt(Scheme *sc);
Cell* op_floor(Scheme *sc);
Cell* op_ceiling(Scheme *sc);
Cell* op_truncate(Scheme *sc);
Cell* op_round(Scheme *sc);
Cell* op_add(Scheme *sc);
Cell* op_mul(Scheme *sc);
Cell* op_sub(Scheme *sc);
Cell* op_div(Scheme *sc);
Cell* op_intdiv(Scheme *sc);
Cell* op_rem(Scheme *sc);
Cell* op_mod(Scheme *sc);
void init_math_constants(Scheme *sc);

#endif /* MATH_H_ */
