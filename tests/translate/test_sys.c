/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2015 by fanguangping (fanguangping@163.com)
 * test_sys.c
 */

#include <assert.h>
#include <string.h>
#include "../ds/test_datastructure.h"
#include "../../src/translate/sys.h"
#include "test_sys.h"

void test_extract() {
	//assert(strcmp(extract ("{abc}"), "abc") == 0);
	//assert(strcmp(extract ("{a b c}"), "a b c") == 0);
	//assert(strcmp(extract ("{a {b} c}"), "a {b} c") == 0);
}

void test_arglist() {
	//assert(strcmp(arglist ("{abc}"), "abc") == 0);
	//assert(strcmp(arglist ("{{a} {b} {c}}"), "a b c") == 0);
}

void test_target() {
	;
}

void test_funcname() {
	;
}

void test_sys() {
	test_extract();
	test_arglist();
	test_target();
	test_funcname();
}

