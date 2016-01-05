/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2015 by fanguangping (fanguangping@163.com)
 * test_statement.c
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "test_datastructure.h"
#include "test_statement.h"
#include "../../src/ds/datastructure.h"

void test_alloc_statement() {
	Statement *s = alloc_statement();
	assert(s != NULL);
}

void test_init_sym() {
	Statement *s = init_sym("define");
	assert(s != NULL);
	assert(strcmp(s->symbol, "define") == 0);
	assert(s->type == SYM);
	assert(s->children == NULL);
}

void test_init_arg() {
	Statement *s = init_arg();
	assert(s != NULL);
	assert(s->symbol == NULL);
	assert(s->type == ARG);
	assert(s->children != NULL);
}

void test_init_loop() {
	Statement *s = init_loop();
	assert(s != NULL);
	assert(s->symbol == NULL);
	assert(s->type == LOOP);
	assert(s->children != NULL);
}

void test_get_arg_name() {
	assert(get_arg_name(NULL) == NULL);
	Statement *s = init_arg();
	add_statement(s, "A_ARG", SYM);
	assert(strcmp(get_arg_name(s), "A_ARG") == 0);
	add_statement(s, "TWO_ARG", SYM);
	assert(get_arg_name(s) == NULL);
	s = init_sym("A_SYM");
	assert(get_arg_name(s) == NULL);
}

void test_set_arg_name() {
	set_arg_name(NULL, NULL);
	Statement *s = init_arg();
	add_statement(s, "A_ARG", SYM);
	set_arg_name(s, "TWO_ARG");
	assert(strcmp(get_arg_name(s), "TWO_ARG") == 0);
}

void test_equal_sym() {
	Statement *s = init_sym("A_SYM");
	assert(!equal_sym(NULL, NULL));
	assert(!equal_sym(NULL, "A_SYM"));
	assert(!equal_sym(s, NULL));
	assert(equal_sym(s, "A_SYM"));
}

void test_equal_arg() {
	Statement *s = init_arg();
	add_statement(s, "A_ARG", SYM);
	assert(!equal_arg(NULL, NULL));
	assert(!equal_arg(NULL, "A_ARG"));
	assert(!equal_arg(s, NULL));
	assert(equal_arg(s, "A_ARG"));
}

void test_empty_statement() {
	Statement *s = init_arg();
	assert(empty_statement(NULL));
	assert(empty_statement(s));
	s = init_sym("");
	assert(!empty_statement(s));
	s = init_loop();
	assert(!empty_statement(s));
}

void test_equal_statement() {
	Statement *a = create_statement();
	Statement *b = create_statement();
	assert(equal_statement(a, b));
}

void test_to_string_statement() {
	assert(strcmp(to_string_statement(create_statement()), STATEMENT_STRING) == 0);
	assert(strcmp(to_string_statement(create_loop_statement()), LOOP_STATEMENT_STRING) == 0);
	assert(strcmp(to_string_statement(NULL), "") == 0);
}

void test_calc_statement_prefix() {
	Statement *s = create_statement();
	assert(strcmp(calc_statement_prefix(s), "def") == 0);
	assert(calc_statement_prefix(NULL) == NULL);
	Statement *oneArg = init_arg();
	add_statement(oneArg, "define", SYM);
	assert(strcmp(calc_statement_prefix(oneArg), "def") == 0);
}

void test_match_statement() {
	Statement *s1 = init_sym("abc");
	Statement *s2 = init_sym("abc");
	Statement *s3 = init_sym("def");
	Statement *s4 = init_arg();
	add_statement(s4, "abc", SYM);
	Statement *s5 = init_arg();
	add_statement(s5, "abc", SYM);
	Statement *s6 = init_arg();
	add_statement(s6, "def", SYM);

	assert(match_statement(NULL, NULL) == FALSE);
	assert(match_statement(s1, NULL) == FALSE);
	assert(match_statement(NULL, s1) == FALSE);
	assert(match_statement(s1, s2) == TRUE);
	assert(match_statement(s1, s3) == FALSE);
	assert(match_statement(s4, s5) == TRUE);
	assert(match_statement(s4, s6) == TRUE);
	assert(match_statement(s1, s4) == FALSE);
}

void test_statement() {
	test_alloc_statement();
	test_init_sym();
	test_init_arg();
	test_init_loop();
	test_get_arg_name();
	test_set_arg_name();
	test_equal_sym();
	test_equal_arg();
	test_empty_statement();
	test_equal_statement();
	test_to_string_statement();
	test_calc_statement_prefix();
	test_match_statement();
}
