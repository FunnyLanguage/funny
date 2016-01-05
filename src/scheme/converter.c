/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2015 by fanguangping (fanguangping@163.com)
 * converter.c
 */

#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "operator.h"

#define ATOM_STR_LENGTH 20

static long num2int(Num n) {
	return (n.isFix ? (n).longValue : (long) (n).doubleValue);
}

static const char *charnames[32] = { "nul", "soh", "stx", "etx", "eot", "enq",
		"ack", "bel", "bs", "ht", "lf", "vt", "ff", "cr", "so", "si", "dle",
		"dc1", "dc2", "dc3", "dc4", "nak", "syn", "etb", "can", "em", "sub",
		"esc", "fs", "gs", "rs", "us" };

static char* number2str(Num num, int print_flag) {
	char *p = (char *) malloc(ATOM_STR_LENGTH * sizeof(char));
	if (print_flag <= 1 || print_flag == 10) /* f is the base for numbers if > 1 */{
		if (num.isFix) {
			snprintf(p, ATOM_STR_LENGTH, "%ld", num.longValue);
		} else {
			snprintf(p, ATOM_STR_LENGTH, "%.10g", num.doubleValue);
			/* r5rs says there must be a '.' (unless 'e'?) */
			print_flag = strcspn(p, ".e");
			if (p[print_flag] == 0) {
				p[print_flag] = '.'; /* not found, so add '.0' at the end */
				p[print_flag + 1] = '0';
				p[print_flag + 2] = 0;
			}
		}
	} else {
		long v = num2int(num);
		if (print_flag == 16) {
			if (v >= 0)
				snprintf(p, ATOM_STR_LENGTH, "%lx", v);
			else
				snprintf(p, ATOM_STR_LENGTH, "-%lx", -v);
		} else if (print_flag == 8) {
			if (v >= 0)
				snprintf(p, ATOM_STR_LENGTH, "%lo", v);
			else
				snprintf(p, ATOM_STR_LENGTH, "-%lo", -v);
		} else if (print_flag == 2) {
			unsigned long b = (v < 0) ? -v : v;
			p = &p[ATOM_STR_LENGTH - 1];
			*p = 0;
			do {
				*--p = (b & 1) ? '1' : '0';
				b >>= 1;
			} while (b != 0);
			if (v < 0)
				*--p = '-';
		}
	}

	return p;
}

static char* char2str(int c, int print_flag) {
	char *p = (char *)malloc(ATOM_STR_LENGTH * sizeof(char));
	if (!print_flag) {
		p[0] = c;
		p[1] = 0;
	} else {
		switch (c) {
		case ' ':
			snprintf(p, ATOM_STR_LENGTH, "#\\space");
			break;
		case '\n':
			snprintf(p, ATOM_STR_LENGTH, "#\\newline");
			break;
		case '\r':
			snprintf(p, ATOM_STR_LENGTH, "#\\return");
			break;
		case '\t':
			snprintf(p, ATOM_STR_LENGTH, "#\\tab");
			break;
		default:
			if (c == 127) {
				snprintf(p, ATOM_STR_LENGTH, "#\\del");
				break;
			} else if (c < 32) {
				snprintf(p, ATOM_STR_LENGTH, "#\\%s", charnames[c]);
				break;
			}
			snprintf(p, ATOM_STR_LENGTH, "#\\%c", c);
			break;
		}
	}
	return p;
}

char* cell2str(Scheme *sc, Cell* cell) {
	String ret = init_string();
	Cell *x;

	if (is_vector(cell)) {
		ret = dyna_strcat(ret, "#(");
		int index = 0;
		int len = cell->_num.longValue;
		for (index = 0; index < len; index++) {
			Cell* elem = get_vector_item(cell, index);
			if (index > 0)
				ret = dyna_strcat(ret, " ");
			ret = dyna_strcat(ret, cell2str(sc, elem));
		}
		ret = dyna_strcat(ret, ")");
	} else if (is_environment(cell)) { //环境
		ret = dyna_strcat(ret, "#<ENVIRONMENT>");
	} else if (!is_pair(cell)) { //原子
		if (cell == &g_nil) {
			ret = "()";
		} else if (cell == &g_true) {
			ret = "#t";
		} else if (cell == &g_false) {
			ret = "#f";
		} else if (cell == &g_eof) {
			ret = "#<EOF>";
		} else if (is_symbol(cell)) {
			ret = symbol_name(cell);
		} else if (is_macro(cell)) {
			ret = "#<MACRO>";
		} else if (is_closure(cell)) {
			ret = "#<CLOSURE>";
		} else if (is_promise(cell)) {
			ret = "#<PROMISE>";
		} else if (is_proc(cell)) {
			ret = "#<PROCEDURE>";
		} else if (is_foreign(cell)) {
			ret = "#<FOREIGN PROCEDURE>";
		} else if (is_continuation(cell)) {
			ret = "#<CONTINUATION>";
		} else if (is_number(cell)) {
			ret = dyna_strcat(ret, number2str(cell->_num, 1));
		} else if (is_string(cell)) {
			ret = dyna_strcat(ret, cell->_string);
		} else if (is_character(cell)) {
			ret = dyna_strcat(ret, char2str(char_value(cell), 1));
		} else {
			ret = "#<ERROR>";
		}
	} else if (car(cell) == sc->sym_quote && (is_pair(cdr(cell)) && cdr(cdr(cell)) == &g_nil)) {
		ret = dyna_strcat(ret, "'");
		ret = dyna_strcat(ret, cell2str(sc, cadr(cell)));
	} else {
		ret = dyna_strcat(ret, "(");
		if (is_pair(cell)) {
			//ret = dyna_strcat(ret, cell2str(sc, car(cell)));
			//ret = dyna_strcat(ret, " . ");
			//ret = dyna_strcat(ret, cell2str(sc, cdr(cell)));

			for (x = cell; is_pair(x) && cdr(x) != &g_nil; x = cdr(x)) {
				if (is_pair(x)) {
					ret = dyna_strcat(ret, cell2str(sc, car(x)));
				} else {
					ret = dyna_strcat(ret, cell2str(sc, x));
				}
				if (is_pair(x)) {
					if (!is_pair(cdr(x))) {
						ret = dyna_strcat(ret, " . ");
					} else {
						ret = dyna_strcat(ret, " ");
					}
				}
			}
			if (is_pair(x)) {
				ret = dyna_strcat(ret, cell2str(sc, car(x)));
			} else {
				ret = dyna_strcat(ret, cell2str(sc, x));
			}
		}
		ret = dyna_strcat(ret, ")");
	}

	return ret;
}

// (atom->string x)
// (atom->string num base)
Cell* op_atom2str(Scheme *sc) {
	Cell* x = first(sc->args);

	long print_flag = 0;
	if (rest(sc->args) != &g_nil) {
		/* we know cadr(sc->args) is a natural number */
		/* see if it is 2, 8, 10, or 16, or error */
		print_flag = second(sc->args)->_num.longValue;
		if (is_number(x)
				&& (print_flag == 16 || print_flag == 10 || print_flag == 8
						|| print_flag == 2)) {
			/* base is OK */
		} else
			print_flag = -1;
	}
	if (print_flag < 0) {
		return error_helper(sc, "atom->string: bad base:", second(sc->args));
	} else if (is_number(x)) {
		char *p = number2str(x->_num, print_flag);
		return s_return_helper(sc, make_string_n(sc, p, strlen(p)));
	} else if (is_character(x)) {
		int c = char_value(x);
		char *p = char2str(c, print_flag);
		return s_return_helper(sc, make_string_n(sc, p, strlen(p)));
	} else if (is_string(x)) {
		char *p = x->_string;
		return s_return_helper(sc, make_string_n(sc, p, strlen(p)));
	} else if (is_symbol(x)) {
		char *p = symbol_name(x);
		return s_return_helper(sc, make_string_n(sc, p, strlen(p)));
	} else {
		return error_helper(sc, "atom->string: not an atom:", x);
	}
}

// (char->integer x)
Cell* op_char2int(Scheme *sc) {
	char c = (char) long_value(first(sc->args));
	return s_return_helper(sc, make_integer(sc, (unsigned char) c));
}

// (integer->char x)
Cell* op_int2char(Scheme *sc) {
	unsigned char c = (unsigned char) long_value(first(sc->args));
	return s_return_helper(sc, make_character(sc, (char) c));
}

// (char-upcase x)
Cell* op_char_upcase(Scheme *sc) {
	unsigned char c = (unsigned char) long_value(first(sc->args));
	c = toupper(c);
	return s_return_helper(sc, make_character(sc, (char) c));
}

// (char-downcase x)
Cell* op_char_downcase(Scheme *sc) {
	unsigned char c = (unsigned char) long_value(first(sc->args));
	c = tolower(c);
	return s_return_helper(sc, make_character(sc, (char) c));
}

// (string->symbol x)
Cell* op_str2sym(Scheme *sc) {
	return s_return_helper(sc, make_symbol(sc, first(sc->args)->_string));
}

// (string->atom str)
// (string->atom numstr base)
Cell* op_str2atom(Scheme *sc) {
	char *s = first(sc->args)->_string;
	long print_flag = 0;
	if (rest(sc->args) != &g_nil) {
		/* we know cadr(sc->args) is a natural number */
		/* see if it is 2, 8, 10, or 16, or error */
		print_flag = second(sc->args)->_num.longValue;
		if (print_flag == 16 || print_flag == 10 || print_flag == 8
				|| print_flag == 2) {
			/* base is OK */
		} else {
			print_flag = -1;
		}
	}
	if (print_flag < 0) {
		return error_helper(sc, "string->atom: bad base:", second(sc->args));
	} else if (*s == '#') /* no use of base! */{
		return s_return_helper(sc, make_sharp_const(sc, s + 1));
	} else {
		if (print_flag == 0 || print_flag == 10) {
			return s_return_helper(sc, make_atom_from_string(sc, s));
		} else {
			char *ep;
			long iv = strtol(s, &ep, (int) print_flag);
			if (*ep == 0) {
				return s_return_helper(sc, make_integer(sc, iv));
			} else {
				return s_return_helper(sc, &g_false);
			}
		}
	}
}

// (symbol->string x)
Cell* op_sym2str(Scheme *sc) {
	Cell* x = make_string(sc, symbol_name(first(sc->args)));
	set_immutable(x);
	return s_return_helper(sc, x);
}
