/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2015 by fanguangping (fanguangping@163.com)
 * func.h
 */

#ifndef FUNC_H_
#define FUNC_H_

#include "ds.h"

Cell* error_helper(Scheme *sc, const char * errorDesc, Cell* arg);
void s_save(Scheme *sc, OperatorFunc op, Cell* args, Cell* code);
Cell* s_return_helper(Scheme *sc, Cell* ret_value);

Cell* op_lambda(Scheme *sc);
Cell* op_lambda1(Scheme *sc);
Cell* op_get_closure(Scheme *sc);
Cell* op_def0(Scheme *sc);
Cell* op_def1(Scheme *sc);
Cell *op_toplevel(Scheme *sc);
Cell* op_eval_internal(Scheme *sc);
Cell* op_eval(Scheme *sc);
Cell* op_e0args(Scheme *sc);
Cell* op_e1args(Scheme *sc);
Cell* op_apply(Scheme *sc);
Cell* op_delay(Scheme *sc);
Cell* op_papply(Scheme *sc);
Cell* op_peval(Scheme *sc);
Cell* op_continuation(Scheme *sc);
Cell* op_quit(Scheme *sc);
Cell* op_gc(Scheme *sc);
Cell* op_gcverb(Scheme *sc);

#endif /* FUNC_H_ */
