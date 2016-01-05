/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2015 by fanguangping (fanguangping@163.com)
 * test_func.c
 */


//http://blog.csdn.net/xiao_wanpeng/article/details/8466745

#include "test_func.h"

void test_func(Scheme *sc) {
  test_op(sc, "(lambda () ())", "#<CLOSURE>");
  test_op(sc, "(lambda (a b) (+ a b))", "#<CLOSURE>");
  test_op(sc, "(define x 123)", "x");
  test_op(sc, "(define add5 (lambda (x) (+ x 5)))", "add5");
  test_op(sc, "(define (add6 x) (+ x 6))", "add6");
  test_op(sc, "(define fix (lambda (x y z) (define add (lambda (a b) (+ a b))) (- x (add y z))))", "fix");
  test_op(sc, "(eval '(+ 1 2))", "3");
  test_op(sc, "(eval '(+ 1 2) (interaction-environment))", "3");
  test_op(sc, "(apply + '(1 2))", "3");
  test_op(sc, "(gc)", "#t");
}
