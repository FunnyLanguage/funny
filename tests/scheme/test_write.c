/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2015 by fanguangping (fanguangping@163.com)
 * test_write.c
 */

#include "test_write.h"

void test_write(Scheme *sc) {
	test_op(sc, "(print \"message \" \"from \" \"print.\")", "#t");
	test_op(sc, "(print-line \"message \" \"from \" \"print-line.\")", "#t");
	test_op(sc, "(error \"message \" \"from \" \"error.\")", "#t");
}

