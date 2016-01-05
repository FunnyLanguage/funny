/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2015 by fanguangping (fanguangping@163.com)
 * main.c
 */

#include <stdlib.h>
#include <stdio.h>
#include "ds/datastructure.h"
#include "scheme/ds.h"
#include "scheme/operator.h"
#include "translate/parser.h"
#include "translate/dfa.h"
#include "translate/init.h"

static Scheme *alloc_scheme() {
	Scheme *result = (Scheme *) malloc(sizeof(Scheme));
	exit_when_out_of_memory(result);
	return result;
}

static int funny_init() {
	load();
	return TRUE;
}

static void wait_to_exit(Statement *st) {
	if (equal_arg(st, "exit")) {
		printf("Goodbye!\n");
		exit(0);
	}
}

int main(int argc, char **argv) {
	Scheme *sc = alloc_scheme();
	Statement *st = NULL;
	String scheme = NULL;

	if (!scheme_init(sc)) {
		fprintf(stderr, "Initialize scheme environment error!\n");
		return 1;
	}

	if (!funny_init()) {
		fprintf(stderr, "Initialize funny environment error!\n");
		return 1;
	}

	printf("Welcome to FUNNY programming world. Type {exit} to exit.\n");

	while (TRUE) {
		printf("> ");
		st = read_statement(stdin);

		if (empty_statement(st)) {
			printf("can't parse the statement.\n");
			continue;
		}

		wait_to_exit(st);

		scheme = match(st);

		if (equals_string(scheme, ""))
			continue;

//#ifdef DEBUG
		printf("TARGET: %s\n", scheme);
//#endif

		Cell* result = eval(sc, scheme);
		printf("%s\n", cell2str(sc, result));
	}

	return 0;
}
