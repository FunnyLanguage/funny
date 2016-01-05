/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2015 by fanguangping (fanguangping@163.com)
 * test_list.c
 */

#include "test_list.h"

void test_list(Scheme *sc) {
	test_op(sc, "(car '(1 2 3))", "1");
	test_op(sc, "(cdr '(1 2 3))", "(2 3)");
	test_op(sc, "'(1 2 3 . 4)", "(1 2 3 . 4)");
	test_op(sc, "(cons 1 2)", "(1 . 2)");
	test_op(sc, "(set-car! '(1 2 3) 5)", "(5 2 3)");
	test_op(sc, "(set-cdr! '(1 2 3) 5)", "(1 . 5)");
	test_op(sc, "(reverse '(1 2 3))", "(3 2 1)");
	test_op(sc, "(append '(1 2 3) '(4 5))", "(1 2 3 4 5)");
	test_op(sc, "(length '(1 2 3))", "3");
	test_op(sc, "(cons-stream 1 1)", "(1 . #<PROMISE>)");
}
