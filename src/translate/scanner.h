/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2015 by fanguangping (fanguangping@163.com)
 * scanner.h
 */

#ifndef SCANNER_H_
#define SCANNER_H_

#include <stdio.h>
#include "../ds/datastructure.h"


#define MAX_WORD_LENGTH 128
#define MAX_STR_LENGTH  1024
#define PAREN_L         '{'
#define PAREN_R         '}'
#define PAREN_L_STR     "{"
#define PAREN_R_STR     "}"
#define LOOP_L			'['
#define LOOP_R			']'
#define LOOP_L_STR      "["
#define LOOP_R_STR      "]"
#define COMMENT         ';'
#define DELIMITER       ','

typedef struct _Input {
	int isFile;
	union {
		FILE *file;
		struct {
			String buffer;
			int pointer;
		} string;
	};
} Input;

Input *alloc_input();
Input *file_input(FILE *file);
Input *string_input(String string);
int get_char_from_input(Input *input);
int unget_char_from_input(int c, Input *input);
String char_to_string(int c);
int has_next(Input *input);
String next(Input *input);
String read_symbol(int c, Input *input, int escaped);
String read_string(int c, Input *input);
String next_token(Input *input);

#endif /* SCANNER_H_ */
