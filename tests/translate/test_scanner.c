/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2015 by fanguangping (fanguangping@163.com)
 * test_scanner.c
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "../../src/translate/scanner.h"
#include "test_scanner.h"

#define STATEMENT "{define function {add}: {add {a} {b}} as {{a} + {b}}}"

void test_alloc_input() {
	Input *input = alloc_input();
	assert(input != NULL);
}

void test_file_input() {
	char *funnyHome = getenv("FUNNY_HOME");
	String fileName = dyna_strcat(copy_string(funnyHome), "/test/test.fn");
	FILE *file = fopen( fileName, "rb");
	if (!file)
		perror(fileName), exit(1);
	Input *input = file_input(file);
	assert(input != NULL);
	assert(input->isFile == TRUE);
	assert(input->file != NULL);
	fclose(file);
}

void test_string_input() {
	Input *input = string_input("test");
	assert(input != NULL);
	assert(input->isFile == FALSE);
	assert(input->string.buffer != NULL);
	assert(input->string.pointer == 0);
}

void test_char_to_string() {
	String str = char_to_string('a');
	assert(strcmp(str, "a") == 0);
}

//String read_symbol

void test_read_symbol() {
	;
}

void test_next() {
	Input *input = string_input(copy_string(STATEMENT));
	int hasNext = 0;
	String nextString = NULL;
	hasNext = has_next(input);
	nextString = next(input);
	assert(hasNext == TRUE);
	assert(strcmp(nextString, "{") == 0);
	hasNext = has_next(input);
	nextString = next(input);
	assert(hasNext == TRUE);
	assert(strcmp(nextString, "define") == 0);
	hasNext = has_next(input);
	nextString = next(input);
	assert(hasNext == TRUE);
	assert(strcmp(nextString, "function") == 0);
	hasNext = has_next(input);
	nextString = next(input);
	assert(hasNext == TRUE);
	assert(strcmp(nextString, "{") == 0);
	hasNext = has_next(input);
	nextString = next(input);
	assert(hasNext == TRUE);
	assert(strcmp(nextString, "add") == 0);
	hasNext = has_next(input);
	nextString = next(input);
	assert(hasNext == TRUE);
	assert(strcmp(nextString, "}") == 0);
	hasNext = has_next(input);
	nextString = next(input);
	assert(hasNext == TRUE);
	assert(strcmp(nextString, ":") == 0);
	hasNext = has_next(input);
	nextString = next(input);
	assert(hasNext == TRUE);
	assert(strcmp(nextString, "{") == 0);
	hasNext = has_next(input);
	nextString = next(input);
	assert(hasNext == TRUE);
	assert(strcmp(nextString, "add") == 0);
	hasNext = has_next(input);
	nextString = next(input);
	assert(hasNext == TRUE);
	assert(strcmp(nextString, "{") == 0);
	hasNext = has_next(input);
	nextString = next(input);
	assert(hasNext == TRUE);
	assert(strcmp(nextString, "a") == 0);
	hasNext = has_next(input);
	nextString = next(input);
	assert(hasNext == TRUE);
	assert(strcmp(nextString, "}") == 0);
	hasNext = has_next(input);
	nextString = next(input);
	assert(hasNext == TRUE);
	assert(strcmp(nextString, "{") == 0);
	hasNext = has_next(input);
	nextString = next(input);
	assert(hasNext == TRUE);
	assert(strcmp(nextString, "b") == 0);
	hasNext = has_next(input);
	nextString = next(input);
	assert(hasNext == TRUE);
	assert(strcmp(nextString, "}") == 0);
	hasNext = has_next(input);
	nextString = next(input);
	assert(hasNext == TRUE);
	assert(strcmp(nextString, "}") == 0);
	hasNext = has_next(input);
	nextString = next(input);
	assert(hasNext == TRUE);
	assert(strcmp(nextString, "as") == 0);
	hasNext = has_next(input);
	nextString = next(input);
	assert(hasNext == TRUE);
	assert(strcmp(nextString, "{") == 0);
	hasNext = has_next(input);
	nextString = next(input);
	assert(hasNext == TRUE);
	assert(strcmp(nextString, "{") == 0);
	hasNext = has_next(input);
	nextString = next(input);
	assert(hasNext == TRUE);
	assert(strcmp(nextString, "a") == 0);
	hasNext = has_next(input);
	nextString = next(input);
	assert(hasNext == TRUE);
	assert(strcmp(nextString, "}") == 0);
	hasNext = has_next(input);
	nextString = next(input);
	assert(hasNext == TRUE);
	assert(strcmp(nextString, "+") == 0);
	hasNext = has_next(input);
	nextString = next(input);
	assert(hasNext == TRUE);
	assert(strcmp(nextString, "{") == 0);
	hasNext = has_next(input);
	nextString = next(input);
	assert(hasNext == TRUE);
	assert(strcmp(nextString, "b") == 0);
	hasNext = has_next(input);
	nextString = next(input);
	assert(hasNext == TRUE);
	assert(strcmp(nextString, "}") == 0);
	hasNext = has_next(input);
	nextString = next(input);
	assert(hasNext == TRUE);
	assert(strcmp(nextString, "}") == 0);
	hasNext = has_next(input);
	nextString = next(input);
	assert(hasNext == TRUE);
	assert(strcmp(nextString, "}") == 0);
	hasNext = has_next(input);
	nextString = next(input);
	assert(hasNext == FALSE);
	assert(nextString == NULL);
}

void test_scanner() {
 test_alloc_input();
 test_file_input();
 test_string_input();
 test_char_to_string();
 test_next();
}
