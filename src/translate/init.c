/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2015 by fanguangping (fanguangping@163.com)
 * init.c
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "parser.h"
#include "dfa.h"
#include "sys.h"
#include "init.h"

#define MAX_SIZE       512

#define COMMENT_CHAR   '$'
#define STATEMENT_CHAR 's'
#define STATEMENT_STR  "statement:"
#define NAME_CHAR      'n'
#define NAME_STR       "name:"
#define PREPARE_CHAR   'p'
#define PREPARE_STR    "prepare:"
#define EVAL_CHAR      'e'
#define EVAL_STR       "eval:"
#define DEF_TEMPLATE   "deftpl"

#define SYS_MACRO      "/sys.macro"
#define CORE_TEMPLATE  "/templates/core.fn"
#define SYS_TEMPLATE   "/templates/sys.fn"
#define CMD_TEMPLATE   "/templates/command.fn"
#define GUI_TEMPLATE   "/templates/gui.fn"
#define DB_TEMPLATE    "/templates/db.fn"

static int loaded = FALSE;

void load() {
	if (loaded)
		return;
	char *funnyHome = getenv("FUNNY_HOME");
	char *sysMacro = dyna_strcat(copy_string(funnyHome), SYS_MACRO);
	char *coreTemplate = dyna_strcat(copy_string(funnyHome), CORE_TEMPLATE);
	char *sysTemplate = dyna_strcat(copy_string(funnyHome), SYS_TEMPLATE);

	parse_sys_macro(sysMacro);
	load_template(coreTemplate);
	load_template(sysTemplate);
	//load_template(CMD_TEMPLATE);
	//load_template(GUI_TEMPLATE);
	//load_template(DB_TEMPLATE);
	functions = init_queue();

	loaded = TRUE;
}

void add_mapping_to_trie(Mapping *mapping) {
	trie_add(mappingTrie, mapping);
}

Queue *get_mappings(Statement *s) {
	if (s == NULL)
		return NULL;
	return trie_find(mappingTrie, s);
}

/*
Queue *get_mappings() {
	if (mappings == NULL) {
		load();
	}
	return mappings;
}
*/

Queue *get_functions() {
	if (functions == NULL) {
		functions = init_queue();
	}
	return functions;
}

String get_sys_macro_string(String s, FILE *fp) {
	int c = 0, i = 0;
	String macro = NULL;

	i = strlen(s);
	while (i-- > 0)
		c = getc(fp);

	i = 0;
	macro = alloc_string(MAX_SIZE);
	while (c != '\n') {
		macro[i++] = c = getc(fp);
	}
	macro[--i] = '\0';

	return trim_string(macro);
}

void parse_sys_macro(String fileName) {
	String source = NULL;
	//Queue *queue = init_queue();
	mappingTrie = init_trie(calc_mapping_prefix, calc_statement_prefix);
	Mapping *mapping = NULL;

	FILE *fp = NULL;
	int c = 0;

	fp = fopen( fileName, "rb");
	if (!fp)
		perror(fileName), exit(1);

	while (TRUE) {
		c = getc(fp);

		if (c == -1)
			break;

		if (c == COMMENT_CHAR) {
			while (c != '\n')
				c = getc(fp);
			continue;
		}

		if (c == STATEMENT_CHAR) {
			source = get_sys_macro_string(STATEMENT_STR, fp);

			mapping = alloc_mapping();
			mapping->sys = alloc_sys_block();
			mapping->source = source;
			mapping->sourceStatement = parse(source);
			mapping->sourceFunc = NULL;
			mapping->starter = to_dfa(mapping->sourceStatement);

			mapping->target = NULL;
			mapping->targetStatement = NULL;
			mapping->targetFunc = NULL;

			continue;
		} else if (c == NAME_CHAR) {
			mapping->sys->name = get_sys_macro_string(NAME_STR, fp);
			continue;
		} else if (c == PREPARE_CHAR) {
			mapping->sys->prepare = get_sys_macro_string(PREPARE_STR, fp);
			continue;
		} else if (c == EVAL_CHAR) {
			mapping->sys->eval = get_sys_macro_string(EVAL_STR, fp);
			//enqueue(queue, mapping);
			trie_add(mappingTrie, mapping);
			continue;
		} else {
			while (c != '\n')
				c = getc(fp);
			continue;
		}
	}

	fclose(fp);

	// return queue;
}

void match_template(Statement *source) {
	State *start = NULL;
	Queue *repQueue = NULL;
	Mapping *mapping = get_sys_mapping(get_mappings(source), DEF_TEMPLATE);

	if (repQueue == NULL) {
		repQueue = init_queue();
	}
	start = test_match(mapping, source, repQueue);
	if (start->accept) {
		eval_sys(mapping, source, repQueue);
	}
}

void load_template(String fileName) {
	Statement *s = NULL;
	FILE *fp = NULL;

	fp = fopen( fileName, "rb");
	if (!fp)
		perror(fileName), exit(1);

	while (has_next_statement(fp)) {
		s = next_statement(fp);
		match_template(s);
	}

	fclose(fp);
}
