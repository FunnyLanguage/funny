/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2015 by fanguangping (fanguangping@163.com)
 * parser.c
 */

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "scanner.h"
#include "parser.h"

static int is_sym(String token) {
	if (strcmp(PAREN_L_STR, token) == 0 || strcmp(PAREN_R_STR, token) == 0
			|| strcmp(LOOP_L_STR, token) == 0 || strcmp(LOOP_R_STR, token) == 0) {
		return FALSE;
	} else {
		return TRUE;
	}
}

Statement *parse_statement(String token, Input *input) {
	Statement *root = NULL, *s = NULL;
	String the_next_token = NULL;

	if (strcmp(PAREN_L_STR, token) == 0) {
		root = init_arg();
		the_next_token = next_token(input);
		while (strcmp(PAREN_R_STR, the_next_token) != 0) {
			s = parse_expression(the_next_token, input);
			enqueue(root->children, s);
			the_next_token = next_token(input);
		}
	} else if (strcmp(LOOP_L_STR, token) == 0) {
		root = init_loop();
		the_next_token = next_token(input);
		while (strcmp(LOOP_R_STR, the_next_token) != 0) {
			s = parse_expression(the_next_token, input);
			enqueue(root->children, s);
			the_next_token = next_token(input);
		}
	}

	return root;
}

Statement *parse_expression(String token, Input *input) {
	if (is_sym(token)) {
		return init_sym(token);
	} else {
		return parse_statement(token, input);
	}
}

// parse from file //

void eat_whitespace(FILE *in) {
	int c;

	while ((c = getc(in)) != EOF) {
		if (isspace(c)) {
			continue;
		}
		/* comments are whitespace also */
		else if (c == COMMENT) {
			while (((c = getc(in)) != EOF) && (c != '\n'))
				;
			continue;
		}
		ungetc(c, in);
		break;
	}
}

int peek(FILE *in) {
	int c = getc(in);
	ungetc(c, in);
	return c;
}

int has_next_statement(FILE *in) {
	Input *input = file_input(in);
	return has_next(input);
}

Statement *next_statement(FILE *in) {
	Input *input = file_input(in);
	String the_next_token = next(input);
	Statement *s = parse_statement(the_next_token, input);
	return s;
}

Statement *read_statement(FILE *in) {
	Statement *s = NULL;
	eat_whitespace(in);
	s = next_statement(in);

	while (empty_statement(s) && peek(in) != '\n') {
		s = next_statement(in);
	}
	return s;
}

// parse from string //

Statement *parse(String funny) {
	Input *input = string_input(funny);
	String next_token = NULL;
	next_token = next(input);
	return parse_statement(next_token, input);
}

Statement *parse_regex(String funny) {
	Statement *s = NULL;
	Statement *temp = NULL;

	String before = NULL;
	String between = NULL;
	String after = copy_string(funny);
	String* array = NULL;

	s = init_arg();
	while (strchr(after, LOOP_L) != NULL) {
		array = split_string_once(after, LOOP_L);

		before = array[0];
		trim_string(before);
		before = strcat(before, PAREN_R_STR);
		if (!equals_string("{}", before)) {
			temp = parse(before);
			append_queue(s->children, temp->children);
			free(temp);
		}

		after = array[1];
		array = split_string_once(after, LOOP_R);
		trim_string(array[0]);
		between = char_to_string(PAREN_L);
		between = dyna_strcat(between, array[0]);
		between = dyna_strcat(between, PAREN_R_STR);

		if (!equals_string("{}", between)) {
			temp = parse(between);
			temp->type = LOOP;
			enqueue(s->children, temp);
		}

		after = char_to_string(PAREN_L);
		trim_string(array[1]);
		after = dyna_strcat(after, array[1]);
	}

	if (!equals_string("{}", after)) {
		temp = parse(after);
		append_queue(s->children, temp->children);
		free(temp);
	}

	return s;
}

