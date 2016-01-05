/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2015 by fanguangping (fanguangping@163.com)
 * mapping.h
 */

#ifndef MAPPING_H_
#define MAPPING_H_

#include "string.h"
#include "queue.h"
#include "statement.h"
#include "state.h"

typedef struct _SysBlock {
	String name;
	String prepare;
	String eval;
} SysBlock;

typedef struct _Mapping {
	SysBlock *sys;
	int isMacro;
	String source;
	String target;
	String sourceFunc;
	String targetFunc;
	Statement *sourceStatement;
	Statement *targetStatement;
	State *starter;
} Mapping;

SysBlock *alloc_sys_block();
Mapping *alloc_mapping();

Mapping *get_sys_mapping(Queue *mappings, String name);
char *calc_mapping_prefix(void *element);

#endif /* MAPPING_H_ */
