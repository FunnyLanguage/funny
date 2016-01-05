/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2015 by fanguangping (fanguangping@163.com)
 * memory.h
 */

#ifndef MEMORY_H_
#define MEMORY_H_

#include "ds.h"

void gc(Scheme *sc, Cell* a, Cell* b);
int alloc_cellseg(Scheme *sc, int n);
Cell* create_cell_helper(Scheme *sc, Cell* a, Cell* b);

void exit_when_out_of_memory(void *element);

Cell *alloc_cell();

#endif /* MEMORY_H_ */
