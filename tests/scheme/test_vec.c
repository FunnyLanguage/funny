/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2015 by fanguangping (fanguangping@163.com)
 * test_vec.c
 */

#include <assert.h>
#include <string.h>
#include "test_vec.h"

void test_vec(Scheme *sc) {
	test_op(sc, "(vector 1 2 3)", "#(1 2 3)");
	test_op(sc, "(make-vector 3 2)", "#(2 2 2)");
	test_op(sc, "(vector-length (vector 1 2 3))", "3");
	test_op(sc, "(vector-ref (vector 1 2 3) 2)", "3");
	test_op(sc, "(vector-set! (vector 1 2 3) 2 4)", "#(1 2 4)");
}
