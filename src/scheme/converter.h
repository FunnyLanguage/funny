/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2015 by fanguangping (fanguangping@163.com)
 * converter.h
 */

#ifndef CONVERTER_H_
#define CONVERTER_H_

#include "ds.h"

char* cell2str(Scheme *sc, Cell* cell);
Cell* op_atom2str(Scheme *sc);
Cell* op_char2int(Scheme *sc);
Cell* op_int2char(Scheme *sc);
Cell* op_char_upcase(Scheme *sc);
Cell* op_char_downcase(Scheme *sc);
Cell* op_str2sym(Scheme *sc);
Cell* op_str2atom(Scheme *sc);
Cell* op_sym2str(Scheme *sc);

#endif /* CONVERTER_H_ */
