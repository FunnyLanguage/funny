/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2015 by fanguangping (fanguangping@163.com)
 * write.c
 */

#include <stdlib.h>
#include <stdio.h>
#include "operator.h"

// (print x y z ...)
Cell* op_print(Scheme *sc) {
	Cell *x;

	for (x = sc->args; x != &g_nil; x = cdr(x)) {
		printf("%s", cell2str(sc, car(x)));
	}

	return s_return_helper(sc, &g_true);
}

// (print-line x y z ...)
Cell* op_print_line(Scheme *sc) {
	Cell *x;

	for (x = sc->args; x != &g_nil; x = cdr(x)) {
		printf("%s", cell2str(sc, car(x)));
	}
	printf("\n");

	return s_return_helper(sc, &g_true);
}

// (error x y z ...)
Cell* op_err(Scheme *sc) {
	Cell *x;

	if (!is_string(car(sc->args))) {
		sc->args = cons(sc, make_string(sc, " -- "), sc->args);
		set_immutable(sc->args);
	}
	fprintf(stderr, "Error: ");
	fprintf(stderr, car(sc->args)->_string);
	sc->args = cdr(sc->args);

	for (x = sc->args; x != &g_nil; x = cdr(x)) {
		fprintf(stderr, cell2str(sc, car(x)));
	}
	fprintf(stderr, "\n");

	return s_return_helper(sc, &g_true);
}
