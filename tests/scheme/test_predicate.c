/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2015 by fanguangping (fanguangping@163.com)
 * test_predicate.c
 */

#include "test_predicate.h"

void test_predicate(Scheme *sc) {
	test_op(sc, "(defined? abc)", "#f");
	test_op(sc, "(defined? abc '())", "#f");
	test_op(sc, "(boolean? #f)", "#t");
	test_op(sc, "(null? '())", "#t");
	test_op(sc, "(symbol? 'abc)", "#t");
	test_op(sc, "(number? 123.123)", "#t");
	test_op(sc, "(string? \"asfd\")", "#t");
	test_op(sc, "(integer? 123)", "#t");
	test_op(sc, "(real? 123.123)", "#t");
	test_op(sc, "(char? #\\a)", "#t");
	test_op(sc, "(char-alphabetic? #\\a)", "#t");
	test_op(sc, "(char-numeric? #\\2)", "#t");
	test_op(sc, "(char-whitespace? #\\space)", "#t");
	test_op(sc, "(char-upper-case? #\\A)", "#t");
	test_op(sc, "(char-lower-case? #\\a)", "#t");
	test_op(sc, "(procedure? +)", "#t");
	test_op(sc, "(pair? '(a . b))", "#t");
	test_op(sc, "(list? '(a b))", "#t");
	test_op(sc, "(environment? '(a b))", "#f");
	test_op(sc, "(vector? (vector 1 2 3))", "#t");
	test_op(sc, "(eq? 1 1)", "#f");
	test_op(sc, "(eqv? 1 1)", "#t");
	test_op(sc, "(closure? (lambda () ()))", "#t");
	test_op(sc, "(macro? (lambda () ()))", "#f");
}
