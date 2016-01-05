/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2015 by fanguangping (fanguangping@163.com)
 * test_main.c
 */

#include <stdio.h>
#include <stdlib.h>
#include "ds/test_string.h"
#include "ds/test_queue.h"
#include "ds/test_hash.h"
#include "ds/test_statement.h"
#include "ds/test_mapping.h"
#include "ds/test_replacequeue.h"
#include "ds/test_state.h"
#include "ds/test_trie.h"
#include "../src/scheme/ds.h"
#include "../src/scheme/operator.h"
#include "scheme/test_converter.h"
#include "scheme/test_env.h"
#include "scheme/test_func.h"
#include "scheme/test_let.h"
#include "scheme/test_list.h"
#include "scheme/test_logic.h"
#include "scheme/test_math.h"
#include "scheme/test_predicate.h"
#include "scheme/test_prog.h"
#include "scheme/test_str.h"
#include "scheme/test_vec.h"
#include "scheme/test_write.h"
#include "translate/test_scanner.h"
#include "translate/test_parser.h"

void ds_test() {
	test_string();
	test_queue();
	test_hash();
	test_statement();
	test_mapping();
	test_replacequeue();
	test_state();
	test_trie();
}


static Scheme *alloc_scheme() {
	Scheme *result = (Scheme *) malloc(sizeof(Scheme));
	exit_when_out_of_memory(result);
	return result;
}

void scheme_test() {
	Scheme *sc = alloc_scheme();
	if (!scheme_init(sc)) {
		fprintf(stderr, "Initialize scheme environment error!\n");
		return ;
	}

	test_math(sc);
	test_str(sc);
	test_vec(sc);
	test_logic(sc);
	test_predicate(sc);
	test_list(sc);
	test_converter(sc);
	test_let(sc);
	test_env(sc);
	test_write(sc);
	test_func(sc);
	test_prog(sc);
}

void translate_test() {
	test_scanner();
	test_parser();
}

void system_test() {
	;
}

void extend_test() {
	;
}

int main(int argc, char **argv) {
	ds_test();
	scheme_test();
	translate_test();
	system_test();
	extend_test();
	return 0;
}
