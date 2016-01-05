/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2015 by fanguangping (fanguangping@163.com)
 * str.c
 */

#include <string.h>
#include "../ds/string.h"
#include "operator.h"

// (string-length x)
Cell* op_strlen(Scheme *sc) {
	return s_return_helper(sc,
			make_integer(sc, strlen(first(sc->args)->_string)));
}

// (copy-string str)
Cell* op_copystr(Scheme *sc) {
	char *str = first(sc->args)->_string;
	Cell* x = make_cell(sc, &g_nil, &g_nil);
	(x)->_flag = (T_STRING | T_ATOM);
	(x)->_string = copy_string(str);
	return s_return_helper(sc, x);
}

// (string-ref str index)
Cell* op_strref(Scheme *sc) {
	char *str = first(sc->args)->_string;
	int index = long_value(second(sc->args));
	if (index < 0 || index >= strlen(first(sc->args)->_string)) {
		return error_helper(sc, "string-ref: out of bounds:", second(sc->args));
	}
	return s_return_helper(sc,
			make_character(sc, ((unsigned char*) str)[index]));
}

// (string-set! str index c)
Cell* op_strset(Scheme *sc) {
	if (is_immutable(first(sc->args))) {
		return error_helper(sc,
				"string-set!: unable to alter immutable string:",
				first(sc->args));
	}
	char *str = (first(sc->args))->_string;
	int index = long_value(second(sc->args));
	if (index < 0 || index >= strlen(first(sc->args)->_string)) {
		return error_helper(sc, "string-set!: out of bounds:", second(sc->args));
	}
	int c = char_value(third(sc->args));
	str[index] = (char) c;
	return s_return_helper(sc, first(sc->args));
}

// (string-append x y ...)
Cell* op_strappend(Scheme *sc) {
	Cell* x;
	/* in 1.29 string-append was in Scheme in init.scm but was too slow */
	int len = 0;
	/* compute needed length for new string */
	for (x = sc->args; x != &g_nil; x = cdr(x)) {
		len += strlen(car(x)->_string);
	}
	Cell* newstr = make_empty_string(sc, len, ' ');
	/* store the contents of the argument strings into the new string */
	char *pos = (newstr)->_string;
	for (x = sc->args; x != &g_nil;
			pos += strlen(car(x)->_string), x = cdr(x)) {
		memcpy(pos, car(x)->_string, strlen(car(x)->_string));
	}
	return s_return_helper(sc, newstr);
}

// (substring str begin)
// (substring str begin end)
Cell* op_substr(Scheme *sc) {
	Cell* x;
	char *str = first(sc->args)->_string;
	int length = strlen(str);
	int begin = long_value(second(sc->args));
	if (begin > length) {
		return error_helper(sc, "substring: start out of bounds:",
				second(sc->args));
	}
	int end;
	if (cddr(sc->args) != &g_nil) {
		end = long_value(third(sc->args));
		if (end > length || end < begin) {
			return error_helper(sc, "substring: end out of bounds:",
					third(sc->args));
		}
	} else {
		end = length;
	}
	int len = end - begin;
	x = make_empty_string(sc, len, ' ');
	memcpy(x->_string, str + begin, len);
	(x->_string)[len] = 0;
	return s_return_helper(sc, x);
}
