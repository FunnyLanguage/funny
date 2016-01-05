/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2015 by fanguangping (fanguangping@163.com)
 * list.h
 */

#ifndef LIST_H_
#define LIST_H_

#include "ds.h"

Cell* reverse(Scheme *sc, Cell* old_list);
Cell* list_star(Scheme *sc, Cell* old_list);
Cell* revappend(Scheme *sc, Cell* acc, Cell* list);
int ls_length(Scheme *sc, Cell* a);
int is_list(Scheme *sc, Cell* a);

Cell* op_car(Scheme *sc);
Cell* op_cdr(Scheme *sc);
Cell* op_cons(Scheme *sc);
Cell* op_setcar(Scheme *sc);
Cell* op_setcdr(Scheme *sc);
Cell* op_reverse(Scheme *sc);
Cell* op_list_star(Scheme *sc);
Cell* op_append(Scheme *sc);
Cell* op_list_length(Scheme *sc);
Cell* op_assq(Scheme *sc);
Cell* op_c0stream(Scheme *sc);
Cell* op_c1stream(Scheme *sc);
Cell* op_quote(Scheme *sc);

#endif /* LIST_H_ */
