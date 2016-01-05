/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2015 by fanguangping (fanguangping@163.com)
 * test_env.c
 */

#include "test_env.h"

void test_env(Scheme *sc) {
	test_op(sc, "(interaction-environment)", "#<ENVIRONMENT>");
	test_op(sc, "(current-environment)", "#<ENVIRONMENT>");
}
