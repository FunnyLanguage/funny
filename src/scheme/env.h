/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2015 by fanguangping (fanguangping@163.com)
 * env.h
 */

#ifndef ENV_H_
#define ENV_H_

#include "ds.h"

// 环境
void new_frame_in_env(Scheme *sc, Cell* oldEnv);
void new_slot_in_env(Scheme *sc, Cell* sym, Cell* value);
Cell* find_slot_in_env(Scheme *sc, Cell* env, Cell* sym, int all);
void set_slot_in_env(Scheme *sc, Cell* symbol, Cell* value);
Cell* slot_value_in_env(Cell* symbol);

Cell* op_int_env(Scheme *sc);
Cell* op_curr_env(Scheme *sc);

#endif /* ENV_H_ */
