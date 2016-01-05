/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2015 by fanguangping (fanguangping@163.com)
 * test_init.c
 */

#include <assert.h>
#include <string.h>
#include "../ds/test_datastructure.h"
#include "../../src/translate/init.h"
#include "test_init.h"

//Queue *get_mappings(Statement *s)

void test_get_mappings() {
	load();
	assert(get_mappings(NULL) == NULL);
	//assert(get_mappings(create_statement1()) == NULL);
	//assert(get_mappings(create_statement2()) != NULL);
}

void test_load() {
	load();
	//assert(get_mappings(create_statement2()) != NULL);
	//assert(get_mappings(create_statement3()) != NULL);
	//assert(get_mappings(create_statement4()) != NULL);
	//assert(get_mappings(create_statement5()) != NULL);
}

void test_init() {
	test_get_mappings();
	test_load();
}
