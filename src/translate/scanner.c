/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2015 by fanguangping (fanguangping@163.com)
 * scanner.c
 */

#include <stdlib.h>
#include <ctype.h>
#include "scanner.h"


Input *alloc_input() {
	Input *result = (Input *) malloc(sizeof(Input));
	exit_when_out_of_memory(result);
	return result;
}

Input *file_input(FILE *file) {
	Input *input = alloc_input();
	input->isFile = TRUE;
	input->file = file;
	return input;
}

Input *string_input(String string) {
	Input *input = alloc_input();
	input->isFile = FALSE;
	input->string.buffer = string;
	input->string.pointer = 0;
	return input;
}

int get_char_from_input(Input *input) {
	int ret = 0;
	if (input->isFile == TRUE) {
		ret = getc(input->file);
	} else {
		ret = input->string.buffer[(input->string.pointer)++];
		if (ret == '\0')
			ret = EOF;
	}
	return ret;
}

int unget_char_from_input(int c, Input *input) {
	int ret = 0;
	if (input->isFile == TRUE) {
		ret = ungetc(c, input->file);
	} else {
		if (input->string.pointer == 0)
			ret = EOF;
		else
			ret = input->string.buffer[--(input->string.pointer)];
	}
	return ret;
}

// read_statement //

String char_to_string(int c) {
	String result = alloc_string(2);
	result[0] = c;
	result[1] = '\0';
	return result;
}

int has_next(Input *input) {
	int c = 0;
	while (TRUE) {
		c = get_char_from_input(input);

		switch (c) {
		case EOF:
			return FALSE;

		case COMMENT:
			while ('\n' != c && -1 != c)
				c = get_char_from_input(input);
			break;

		case DELIMITER:
			c = get_char_from_input(input);

			if (!isspace(c) && DELIMITER != c) {
				unget_char_from_input(c, input);
				return TRUE;
			}
			break;

		default:
			if (!isspace(c)) {
				unget_char_from_input(c, input);
				return TRUE;
			}
		}
	}
}

String next(Input *input) {
	int c = 0;

	while (TRUE) {
		c = get_char_from_input(input);

		switch (c) {
		case EOF:
			printf("Unexpected EOS\n");
			return NULL;

		case '\\':
			return read_symbol(c, input, TRUE);

		case PAREN_L:
			return char_to_string(PAREN_L);

		case PAREN_R:
			return char_to_string(PAREN_R);

		case LOOP_L:
			return char_to_string(LOOP_L);

		case LOOP_R:
			return char_to_string(LOOP_R);

		case DELIMITER:
			printf("Unexpected DELIMITER\n");
			return NULL;

		case '"':
			return read_string(c, input);

		default:
			return read_symbol(c, input, FALSE);
		}
	}

	return NULL;
}

String read_symbol(int c, Input *input, int escaped) {
	String buffer = alloc_string(MAX_WORD_LENGTH);
	int i = 0;
	if (escaped) {
		buffer[i++] = (char) c;
		c = get_char_from_input(input);
	}
	while (!isspace(c) && COMMENT != c && PAREN_L != c && PAREN_R != c
			&& LOOP_L != c && LOOP_R != c && DELIMITER != c && EOF != c) {
		if ('\\' == c) {
			c = get_char_from_input(input);
		}
		buffer[i++] = (char) c;
		c = get_char_from_input(input);
	}
	buffer[i] = '\0';

	unget_char_from_input(c, input);
	return buffer;
}

String read_string(int c, Input *input) {
	String buffer = alloc_string(MAX_STR_LENGTH);
	int i = 0;

	buffer[i++] = (char) c;
	c = get_char_from_input(input);
	while ('"' != c && EOF != c) {
		if ('\\' == c) {
			buffer[i++] = (char) c;
			c = get_char_from_input(input);
		}
		buffer[i++] = (char) c;
		c = get_char_from_input(input);
	}
	buffer[i++] = '"';
	buffer[i] = '\0';

	return buffer;
}

String next_token(Input *input) {
	if (!has_next(input)) {
		perror("Unexpected end of Object stream.\n");
		return NULL;
	}
	return next(input);
}
