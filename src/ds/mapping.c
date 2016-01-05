/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2015 by fanguangping (fanguangping@163.com)
 * mapping.c
 */

#include <stdlib.h>
#include <string.h>
#include "datastructure.h"

Mapping *alloc_mapping() {
	Mapping *result = (Mapping *) malloc(sizeof(Mapping));
	exit_when_out_of_memory(result);
	return result;
}

SysBlock *alloc_sys_block() {
	SysBlock *result = (SysBlock *) malloc(sizeof(SysBlock));
	exit_when_out_of_memory(result);
	return result;
}

Mapping *get_sys_mapping(Queue *mappings, String name) {
	if (is_queue_empty(mappings) || name == NULL)
		return NULL;
	Mapping *mapping = NULL;
	QueueNode *node = mappings->head;
	while (node != NULL) {
		mapping = (Mapping *) node->element;
		if (mapping != NULL && mapping->sys != NULL && equals_string(mapping->sys->name, name)) {
			return mapping;
		}
		node = node->next;
	}
	return NULL;
}

char *calc_mapping_prefix(void *element) {
	if (element == NULL)
		return NULL;
	Mapping *mapping = (Mapping *)element;
	return calc_statement_prefix(mapping->sourceStatement);
}
