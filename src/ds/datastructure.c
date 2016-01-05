/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2015 by fanguangping (fanguangping@163.com)
 * datastructure.c
 */

#include <stdio.h>
#include <stdlib.h>
#include "datastructure.h"

void exit_when_out_of_memory(void *element) {
	if (element == NULL) {
		fprintf(stderr, "out of memory!\n");
		exit(1);
	}
}
