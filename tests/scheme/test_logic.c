/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2015 by fanguangping (fanguangping@163.com)
 * test_logic.c
 */

#include "test_logic.h"

void test_logic(Scheme *sc) {
	test_op(sc, "(and #t #t)", "#t");
	test_op(sc, "(and #t #f)", "#f");
	test_op(sc, "(and #f #t)", "#f");
	test_op(sc, "(and #f #f)", "#f");
	test_op(sc, "(or #t #t)", "#t");
	test_op(sc, "(or #t #f)", "#t");
	test_op(sc, "(or #f #t)", "#t");
	test_op(sc, "(or #f #f)", "#f");
	test_op(sc, "(not #t)", "#f");
	test_op(sc, "(not #f)", "#t");
	test_op(sc, "(= 1 1 1)", "#t");
	test_op(sc, "(< 1 2 3)", "#t");
	test_op(sc, "(> 3 2 1)", "#t");
	test_op(sc, "(<= 1 1 2)", "#t");
	test_op(sc, "(>= 2 2 1)", "#t");
}
