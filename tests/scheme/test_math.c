/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2015 by fanguangping (fanguangping@163.com)
 * test_math.c
 */

#include "test_math.h"

void test_math(Scheme *sc) {
	test_op(sc, "(inexact->exact 2.0)", "2");
	test_op(sc, "(exp 0)", "1.0");
	test_op(sc, "(exp 1)", "2.718281828");
	test_op(sc, "(log 1)", "0.0");
	test_op(sc, "(sin 0)", "0.0");
	test_op(sc, "(sin 1.57)", "0.9999996829");
	test_op(sc, "(cos 0)", "1.0");
	test_op(sc, "(cos 1.57)", "0.0007963267107");
	test_op(sc, "(tan 0)", "0.0");
	test_op(sc, "(tan 0.79)", "1.009246288");
	test_op(sc, "(asin 0)", "0.0");
	test_op(sc, "(asin 1)", "1.570796327");
	test_op(sc, "(acos 0)", "1.570796327");
	test_op(sc, "(acos 1)", "0.0");
	test_op(sc, "(atan 1)", "0.7853981634");
	test_op(sc, "(atan 2 2)", "0.7853981634");
	test_op(sc, "(sqrt 2)", "1.414213562");
	test_op(sc, "(expt 2 3)", "8");
	test_op(sc, "(floor 1.5)", "1.0");
	test_op(sc, "(ceiling 1.5)", "2.0");
	test_op(sc, "(truncate 1.5)", "1.0");
	test_op(sc, "(round 1.4)", "1.0");
	test_op(sc, "(round 1.5)", "2.0");
	test_op(sc, "(+ 1 2 3)", "6");
	test_op(sc, "(* 1 2 3)", "6");
	test_op(sc, "(- 1 2)", "-1");
	test_op(sc, "(/ 1 2)", "0.5");
	test_op(sc, "(quotient 4 3)", "1");
	test_op(sc, "(remainder 4 3)", "1");
	test_op(sc, "(modulo 4 3)", "1");
}
