/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2015 by fanguangping (fanguangping@163.com)
 * test_prog.c
 */

#include "test_prog.h"

void test_prog(Scheme *sc) {
  test_op(sc, "(begin (+ 1 2) (* 2 3))", "6");
  test_op(sc, "(if #t 3)", "3");
  test_op(sc, "(if #f 3)", "()");
  test_op(sc, "(if #t 3 4)", "3");
  test_op(sc, "(if #f 3 4)", "4");
  test_op(sc, "(cond ((> 1 0) 10))", "10");
  test_op(sc, "(cond ((> 0 1) 10) (else 0))", "0");
  test_op(sc, "(case (* 2 3) ((2 3 5 7) 'prime) ((1 4 6 8 9) 'composite))", "composite");
  test_op(sc, "(force (delay (+ 1 2)))", "3");
}
