/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2015 by fanguangping (fanguangping@163.com)
 * predicate.h
 */

#ifndef PREDICATE_H_
#define PREDICATE_H_

#include "ds.h"

int is_pair(Cell* p);
int is_number(Cell* p);
int is_integer(Cell* p);
int is_real(Cell* p);
int is_character(Cell* p);
int is_string(Cell* p);
int is_symbol(Cell* sym);
int is_syntax(Cell* p);
int is_vector(Cell* p);
int is_proc(Cell* p);
int is_foreign(Cell* p);
int is_closure(Cell* p);
int is_promise(Cell* p);
int is_macro(Cell* p);
int is_continuation(Cell* p);
int is_environment(Cell* p);
int is_immutable(Cell* p);

Cell* op_defp(Scheme *sc);
Cell* op_boolp(Scheme *sc);
Cell* op_nullp(Scheme *sc);
Cell* op_symbolp(Scheme *sc);
Cell* op_numberp(Scheme *sc);
Cell* op_stringp(Scheme *sc);
Cell* op_integerp(Scheme *sc);
Cell* op_realp(Scheme *sc);
Cell* op_charp(Scheme *sc);
Cell* op_charap(Scheme *sc);
Cell* op_charnp(Scheme *sc);
Cell* op_charwp(Scheme *sc);
Cell* op_charup(Scheme *sc);
Cell* op_charlp(Scheme *sc);
Cell* op_procp(Scheme *sc);
Cell* op_pairp(Scheme *sc);
Cell* op_listp(Scheme *sc);
Cell* op_envp(Scheme *sc);
Cell* op_vectorp(Scheme *sc);
Cell* op_eq(Scheme *sc);
Cell* op_eqv(Scheme *sc);
Cell* op_closurep(Scheme *sc);
Cell* op_macrop(Scheme *sc);

#endif /* PREDICATE_H_ */
