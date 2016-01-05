/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2015 by fanguangping (fanguangping@163.com)
 * sys.c
 */

#include <stdio.h>
#include <string.h>
#include "scanner.h"
#include "parser.h"
#include "init.h"
#include "dfa.h"
#include "sys.h"

#define DEF_MACRO   "defmacro"

String extract(Statement *s) {
	if (s->type != ARG)
		return NULL;
	String str = substr_between(to_string_statement(s), PAREN_L, PAREN_R);
	return str;
}

String arglist(Statement *s) {
	QueueNode *node = NULL;
	Statement *st = NULL;
	String result = init_string();

	node = s->children->head;
	while (node != NULL) {
		st = (Statement *) node->element;
		if (st->type == ARG) {
			result = dyna_strcat(result, extract(st));
			result = dyna_strcat(result, " ");
		}
		node = node->next;
	}
	return trim_string(result);
}

String target(Statement *s) {
	QueueNode *node = NULL;
	Statement *st = NULL;
	String result = init_string();

	node = s->children->head;
	while (node != NULL) {
		st = (Statement *) node->element;
		if (st->type == ARG) {
			result = dyna_strcat(result, to_string_statement(st));
			result = dyna_strcat(result, " ");
		}
		node = node->next;
	}
	return trim_string(result);
}

String funcname(Statement *s) {
	String temp = copy_string(extract(s));
	char *p = strchr(temp, '(');
	if (p) {
		*p = ' ';
		p = trim_string(p);
		temp = p;
		p = strchr(temp, ' ');
		if (p)
			*p = '\0';
		return temp;
	} else {
		return NULL;
	}
}

String eval_sys(Mapping *m, Statement *source, Queue *repQueue) {
	String prepare = copy_string(m->sys->prepare);
	String eval = copy_string(m->sys->eval);
	Mapping *mapping = NULL;

	String* array = NULL;
	String* arraySource = NULL;
	String* arrayTarget = NULL;

	String prepareSource;
	String prepareTarget;
	String sourceFuncMacro;
	String sourceMacro;
	String targetFuncMacro;
	String targetMacro;

	if (!equals_string(prepare, "NIL")) {
		mapping = alloc_mapping();
		mapping->sys = NULL;
		if (strcmp(m->sys->name, DEF_MACRO) == 0) {
			mapping->isMacro = TRUE;
		} else {
			mapping->isMacro = FALSE;
		}

		array = split_string_once(prepare, '>');
		prepareSource = array[0];
		prepareTarget = array[1];

		arraySource = split_string_once(prepareSource, ',');
		sourceFuncMacro = trim_string(arraySource[0]);
		sourceMacro = trim_string(arraySource[1]);

		mapping->source = parse_macro(repQueue, sourceMacro);
		mapping->sourceFunc = parse_macro(repQueue, sourceFuncMacro);
		mapping->sourceStatement = parse(mapping->source);
		mapping->starter = to_dfa(mapping->sourceStatement);

		arrayTarget = split_string_once(prepareTarget, ',');
		targetFuncMacro = trim_string(arrayTarget[0]);
		targetMacro = trim_string(arrayTarget[1]);

		mapping->target = parse_macro(repQueue, targetMacro);
		mapping->targetFunc = parse_macro(repQueue, targetFuncMacro);
		mapping->targetStatement = parse(mapping->target);

		//enqueue(get_mappings(), mapping);
		add_mapping_to_trie(mapping);
	}

	if (!equals_string(eval, "NIL")) {
		return parse_macro(repQueue, eval);
	} else {
		return "";
	}
}

Statement *next_macro_statement(Queue *repQueue, String macro) {
	String str = alloc_string(20);
	Statement *statement = NULL;

	if (*macro == PAREN_L) {
		int i = 0;
		while (*macro != PAREN_R) {
			str[i++] = *macro;
			macro++;
		}
		str[i++] = *macro;
		macro++;
		str[i++] = '\0';

		statement = parse(str);
		return find_replace_statement(repQueue, get_arg_name(statement));
	}

	return NULL;
}

int is_macro_char(char c) {
	return (c == 'E' || c == 'F' || c == 'T' || c == 'M' || c == 'A');
}

String macro_word(char c) {
	switch (c) {
	case 'E':
		return "EXTRACT";
	case 'F':
		return "FUNCNAME";
	case 'T':
		return "TARGET";
	case 'M':
		return "MATCH";
	case 'A':
		return "ARGLIST";
	default:
		return 0;
	}
}

String parse_macro(Queue *repQueue, String macro) {
	String buf = init_string();
	Statement *s = NULL;
	String temp = NULL;
	char c;
	char *p;

	while (*macro) {
		c = *macro;

		if (c == PAREN_L) {
			s = next_macro_statement(repQueue, macro);
			p = strchr(macro, PAREN_R);
			if (!p)
				return NULL;
			macro = p + 1;

			temp = to_string_statement(s);
			buf = dyna_strcat(buf, temp);
		} else if (is_macro_char(c)) {
			macro += strlen(macro_word(c));

			p = strchr(macro, '[');
			if (!p)
				return NULL;
			macro = p + 1;

			s = next_macro_statement(repQueue, macro);

			p = strchr(macro, ']');
			if (!p)
				return NULL;
			macro = p + 1;

			switch (c) {
			case 'E':
				temp = extract(s);
				break;
			case 'F':
				temp = funcname(s);
				if (temp == NULL)
					return NULL;
				break;
			case 'T':
				temp = target(s);
				break;
			case 'M':
				temp = match(s);
				break;
			case 'A':
				temp = arglist(s);
				break;
			default:
				break;
			}

			buf = dyna_strcat(buf, temp);
		} else if (c == '\\') {
			macro++;
			buf = append_char(buf, *macro);
			if (*macro)
				macro++;
		} else {
			buf = append_char(buf, *macro);
			if (*macro)
				macro++;
		}
	}

	return buf;
}
