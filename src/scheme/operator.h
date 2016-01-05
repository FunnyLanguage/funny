/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2015 by fanguangping (fanguangping@163.com)
 * operator.h
 */

#ifndef OPERATOR_H_
#define OPERATOR_H_

#include "../ds/datastructure.h"
#include "converter.h"
#include "ds.h"
#include "env.h"
#include "func.h"
#include "let.h"
#include "list.h"
#include "logic.h"
#include "macro.h"
#include "maker.h"
#include "math.h"
#include "memory.h"
#include "other.h"
#include "predicate.h"
#include "prog.h"
#include "read.h"
#include "str.h"
#include "vec.h"
#include "write.h"

MyGHashTable *operators;

void init_core_operators(MyGHashTable *table);
Cell* eval(Scheme *sc, String statement);
int scheme_init(Scheme *sc);
void add_constant(Scheme *sc, char *name, Cell *value);
void add_int_constant(Scheme *sc, char *name, long value);

#endif /* OPERATOR_H_ */
