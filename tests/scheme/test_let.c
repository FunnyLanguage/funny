/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2015 by fanguangping (fanguangping@163.com)
 * test_let.c
 */


//http://blog.csdn.net/baisung/article/details/7650488 分类
//http://www.cnblogs.com/xiangnan/p/3387146.html

#include "test_let.h"

void test_let(Scheme *sc) {
	test_op(sc, "(let ((x 2) (y 5)) (* x y))", "10");
	test_op(sc, "(let* ((x 1) (y (+ x 1))) (cons y x))", "(2 . 1)");
	test_op(sc, "(letrec ((x (lambda () (+ y y))) (y 100)) (+ (x) y))", "300");
}


