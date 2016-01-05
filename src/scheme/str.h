/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2015 by fanguangping (fanguangping@163.com)
 * str.h
 */

#ifndef STR_H_
#define STR_H_

#include "ds.h"

Cell* op_strlen(Scheme *sc);
Cell* op_copystr(Scheme *sc);
Cell* op_strref(Scheme *sc);
Cell* op_strset(Scheme *sc);
Cell* op_strappend(Scheme *sc);
Cell* op_substr(Scheme *sc);

#endif /* STR_H_ */
