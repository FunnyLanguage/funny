/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2015 by fanguangping (fanguangping@163.com)
 * other.h
 */

#ifndef OTHER_H_
#define OTHER_H_

#include "ds.h"

Cell* op_set0(Scheme *sc);
Cell* op_set1(Scheme *sc);
Cell* op_gensym(Scheme *sc);
Cell* op_oblist(Scheme *sc);

#endif /* OTHER_H_ */
