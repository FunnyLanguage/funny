/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2015 by fanguangping (fanguangping@163.com)
 * test_parser.c
 */

#include <assert.h>
#include <string.h>
#include "../ds/test_datastructure.h"
#include "../../src/translate/parser.h"
#include "test_parser.h"

void test_parse() {
	assert(parse(NULL) == NULL);
	Statement *s = create_statement();
	assert(equal_statement(parse(STATEMENT_STRING), s));
	assert(strcmp(to_string_statement(parse(STATEMENT_STRING)), STATEMENT_STRING) == 0);
}

// {(lambda () {let {gensym-0} = {p} [ {start program {q}} ]})}
void test_parse_regex() {
	assert(parse(NULL) == NULL);
	Statement *s = create_loop_statement();
	assert(equal_statement(parse(LOOP_STATEMENT_STRING), s));
	assert(strcmp(to_string_statement(parse(LOOP_STATEMENT_STRING)), LOOP_STATEMENT_STRING) == 0);
}

void test_parser() {
	test_parse();
	test_parse_regex();
}
