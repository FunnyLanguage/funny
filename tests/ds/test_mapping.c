/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2015 by fanguangping (fanguangping@163.com)
 * test_mapping.c
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "test_datastructure.h"
#include "test_mapping.h"
#include "../../src/ds/datastructure.h"

void test_alloc_sys_block() {
	SysBlock *sys = alloc_sys_block();
	assert(sys != NULL);
}

void test_alloc_mapping() {
	Mapping *mapping = alloc_mapping();
	assert(mapping != NULL);
}

void test_get_sys_mapping() {
	Mapping *mapping = create_sys_mapping();
	Queue *queue = init_queue();
	enqueue(queue, mapping);
	assert(mapping == get_sys_mapping(queue, SYS_TEMPLATE_NAME));
	assert(get_sys_mapping(queue, NULL) == NULL);
	assert(get_sys_mapping(NULL, SYS_TEMPLATE_NAME) == NULL);
	assert(get_sys_mapping(NULL, NULL) == NULL);
}

void test_calc_mapping_prefix() {
	Mapping *mapping = create_sys_mapping();
	assert(strcmp(calc_mapping_prefix(mapping), "def") == 0);
	assert(calc_mapping_prefix(NULL) == NULL);
}

void test_mapping() {
	test_alloc_sys_block();
	test_alloc_mapping();
	test_get_sys_mapping();
	test_calc_mapping_prefix();
}
