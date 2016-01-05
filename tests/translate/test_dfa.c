/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2015 by fanguangping (fanguangping@163.com)
 * test_dfa.c
 */

#include <assert.h>
#include <string.h>
#include "../ds/test_datastructure.h"
#include "../../src/translate/dfa.h"
#include "../../src/translate/parser.h"
#include "../../src/translate/init.h"
#include "test_dfa.h"

void test_match_one_arg() {
	load();
	assert(match_one_arg(NULL) == NULL);
	assert(strcmp(match_one_arg(parse("{+}")), "+") == 0);
	assert(strcmp(match_one_arg(parse("{true}")), "true") == 0);
	assert(strcmp(match_one_arg(parse("{false}")), "false") == 0);
}

void test_orignal_statement() {
	load();
	assert(!orignal_statement(NULL));
	assert(orignal_statement(parse("{abc}")));
	assert(orignal_statement(parse("{a b c}")));
	assert(orignal_statement(parse("{'(a b c)}")));
	assert(!orignal_statement(parse("{a {b} c}")));
}

void test_match_orignal() {
	load();
	assert(match_orignal(NULL) == NULL);
	assert(strcmp(match_orignal(parse("{abc}")), "abc") == 0);
	assert(strcmp(match_orignal(parse("{a b c}")), "a b c") == 0);
	assert(strcmp(match_orignal(parse("{'(a b c)}")), "'(a b c)") == 0);
	assert(match_orignal(parse("{a {b} c}")) == NULL);
}

void test_test_match() {
	;
}

void test_match4test() {
	load();
	assert(match(NULL) == NULL);
	assert(strcmp(match(parse("{abc}")), "abc") == 0);
	assert(strcmp(match(parse("{a b c}")), "a b c") == 0);
	assert(strcmp(match(parse("{'(a b c)}")), "'(a b c)") == 0);
	assert(match(parse("{a {b} c}")) == NULL);
}

void test_dfa() {
	test_match_one_arg();
	test_orignal_statement();
	test_match_orignal();
	test_test_match();
	test_match4test();
}
