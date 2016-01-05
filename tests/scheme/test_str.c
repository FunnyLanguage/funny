/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2015 by fanguangping (fanguangping@163.com)
 * test_str.c
 */

#include "test_str.h"

void test_str(Scheme *sc) {
	test_op(sc, "(string-length \"abc\")", "3");
	test_op(sc, "(string-ref \"abc\" 2)", "#\\c");
	test_op(sc, "(string-set! (copy-string \"abc\") 2 #\\d)", "abd");
	test_op(sc, "(string-append \"abc\" \"abc\")", "abcabc");
	test_op(sc, "(substring \"abc\" 1)", "bc");
	test_op(sc, "(substring \"abcd\" 1 3)", "bc");
}
