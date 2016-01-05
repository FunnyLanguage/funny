/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2015 by fanguangping (fanguangping@163.com)
 * datastructure.h
 */

#ifndef DATASTRUCTURE_H_
#define DATASTRUCTURE_H_

#ifdef TRUE
#undef TRUE
#endif
#define TRUE 1

#ifdef FALSE
#undef FALSE
#endif
#define FALSE 0

void exit_when_out_of_memory(void *element);


#include "hash.h"
#include "mapping.h"
#include "queue.h"
#include "replacequeue.h"
#include "state.h"
#include "statement.h"
#include "string.h"
#include "trie.h"

#endif /* DATASTRUCTURE_H_ */
