/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2015 by fanguangping (fanguangping@163.com)
 * statement.c
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "datastructure.h"

Statement *alloc_statement() {
	Statement *result = (Statement *) malloc(sizeof(Statement));
	exit_when_out_of_memory(result);
	return result;
}

Statement *init_sym(String symbol) {
	Statement *s = alloc_statement();
	s->symbol = symbol;
	s->type = SYM;
	s->children = NULL;
	return s;
}

Statement *init_arg() {
	Statement *s = alloc_statement();
	s->symbol = NULL;
	s->type = ARG;
	s->children = init_queue();
	return s;
}

Statement *init_loop() {
	Statement *s = alloc_statement();
	s->symbol = NULL;
	s->type = LOOP;
	s->children = init_queue();
	return s;
}

String get_arg_name(Statement *s) {
	if (s == NULL)
		return NULL;
	if (s->type != ARG) {
		fprintf(stderr, "Not an ARG!\n");
		return NULL;
	}
	if ((!is_queue_empty(s->children)) && s->children->head
			== s->children->tail) {
		Statement * arg = (Statement *) (s->children->head->element);
		if (arg != NULL && arg->type == SYM && arg->symbol != NULL)
			return arg->symbol;
	}
	return NULL;
}

void set_arg_name(Statement *s, String name) {
	if (s == NULL || name == NULL)
		return;
	if (s->type != ARG) {
		fprintf(stderr, "Not an ARG!\n");
		return;
	}
	if ((!is_queue_empty(s->children)) && s->children->head
			== s->children->tail) {
		Statement * arg = (Statement *) (s->children->head->element);
		if (arg != NULL && arg->type == SYM)
			arg->symbol = name;
	}
}

int equal_sym(Statement *s, String sym) {
	if (s != NULL && s->type == SYM && equals_string(s->symbol, sym)) {
		return TRUE;
	} else {
		return FALSE;
	}
}

int equal_arg(Statement *s, String sym) {
	if (s != NULL && s->type == ARG && equals_string(get_arg_name(s), sym)) {
		return TRUE;
	} else {
		return FALSE;
	}
}

int empty_statement(Statement *s) {
	return (s == NULL || (s->type == ARG && s->children != NULL
			&& s->children->head == NULL));
}

int equal_statement(Statement *a, Statement *b) {
	Queue *qa = NULL, *qb = NULL;
	QueueNode *na = NULL, *nb = NULL;

	if (a == NULL && b == NULL)
		return TRUE;
	if (a == NULL || b == NULL)
		return FALSE;

	if (a->type != b->type)
		return FALSE;
	if (a->type == SYM && strcmp(a->symbol, b->symbol) == 0)
		return TRUE;

	if (a->type == ARG || a->type == LOOP) {
		qa = a->children;
		qb = b->children;

		if (qa->head == NULL && qb->head == NULL)
			return TRUE;
		if (qa->head != NULL && qb->head != NULL) {
			na = qa->head;
			nb = qb->head;

			while (na != NULL && nb != NULL) {
				if (!equal_statement((Statement*) na->element,
						(Statement*) nb->element)) {
					return FALSE;
				}
				na = na->next;
				nb = nb->next;
			}

			if (na == NULL && nb == NULL)
				return TRUE;
		}
	}
	return FALSE;
}

static String strcat_statement(String buf, Statement *s) {
	QueueNode *node = NULL;
	Statement *st = NULL;

	if (s == NULL)
		return "";
	switch (s->type) {
	case SYM:
		buf = dyna_strcat(buf, s->symbol);
		break;

	case ARG:
		if (s->children != NULL) {
			buf = dyna_strcat(buf, "{");
			node = s->children->head;
			while (node != NULL) {
				st = (Statement *) node->element;
				buf = strcat_statement(buf, st);
				if (node != s->children->tail)
					buf = dyna_strcat(buf, " ");
				node = node->next;
			}
			buf = dyna_strcat(buf, "}");
		}
		break;

	case LOOP:
		if (s->children != NULL) {
			buf = dyna_strcat(buf, "[");
			node = s->children->head;
			while (node != NULL) {
				st = (Statement *) node->element;
				buf = strcat_statement(buf, st);
				if (node != s->children->tail)
					buf = dyna_strcat(buf, " ");
				node = node->next;
			}
			buf = dyna_strcat(buf, "]");
		}
		break;
	}

	return buf;
}

String to_string_statement(Statement *s) {
	String str = init_string();
	str = strcat_statement(str, s);
	return str;
}

Statement *copy_statement(Statement *s) {
	Statement *result = NULL;
	QueueNode *node = NULL;
	Statement *st = NULL;
	if (s == NULL)
		return NULL;
	switch (s->type) {
	case SYM:
		result = init_sym(s->symbol);
		break;
	case ARG:
		result = init_arg();
		if (s->children != NULL) {
			node = s->children->head;
			while (node != NULL) {
				st = (Statement *) node->element;
				enqueue(result->children, copy_statement(st));
				node = node->next;
			}
		}
		break;
	case LOOP:
		result = init_loop();
		if (s->children != NULL) {
			node = s->children->head;
			while (node != NULL) {
				st = (Statement *) node->element;
				enqueue(result->children, copy_statement(st));
				node = node->next;
			}
		}
		break;
	}

	return result;
}

char *calc_statement_prefix(void *element) {
	if (element == NULL)
		return NULL;
	Statement *s = (Statement *) element;
	Statement *st = NULL;
	QueueNode *node = NULL;
	char *prefix = alloc_string(MAX_TRIE_HEIGHT + 1);
	char *p = NULL;

	int i = 0;
	if (s->children != NULL) {
		node = s->children->head;
		while (node != NULL && i < MAX_TRIE_HEIGHT) {
			st = (Statement *) node->element;
			switch (st->type) {
			case SYM:
				p = st->symbol;
				while (i < MAX_TRIE_HEIGHT && (prefix[i] = *p)) {
					p++;
					i++;
				}
				break;

			case ARG:
				prefix[i] = BASE_CHAR;
				i++;
				break;

			case LOOP:
				fprintf(
						stderr,
						"Syntax error, cannot calculate prefix with statement: %s",
						to_string_statement(s));
				return NULL;
			}
			node = node->next;
		}
		prefix[i] = '\0';
	}
	return prefix;
}

int match_statement(Statement *input, Statement *base) {
	if (input == NULL || base == NULL)
		return FALSE;
	switch (input->type) {
	case SYM:
		if (base->type == SYM && equals_string(input->symbol, base->symbol))
			return TRUE;
		else
			return FALSE;
	case ARG:
		if (base->type == ARG)
			return TRUE;
		else
			return FALSE;
	case LOOP:
		fprintf(stderr, "Input statement type cannot be LOOP!\n");
		return FALSE;
	}
	return FALSE;
}
