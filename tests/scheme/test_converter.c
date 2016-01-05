/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2015 by fanguangping (fanguangping@163.com)
 * test_converter.c
 */

#include "test_converter.h"

void test_converter(Scheme *sc) {
  test_op(sc, "(atom->string 3)", "3");
  test_op(sc, "(atom->string 5 2)", "101");
  test_op(sc, "(char->integer #\\0)", "48");
  test_op(sc, "(integer->char 48)", "#\\0");
  test_op(sc, "(char-upcase #\\a)", "#\\A");
  test_op(sc, "(char-downcase #\\A)", "#\\a");
  test_op(sc, "(string->symbol \"abc\")", "abc");
  test_op(sc, "(string->atom \"3\")", "3");
  test_op(sc, "(string->atom \"101\" 2)", "5");
  test_op(sc, "(symbol->string 'abc)", "abc");
}

void test_cell2str() {
  ;
}

