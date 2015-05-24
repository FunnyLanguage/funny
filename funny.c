/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2014 by fanguangping (fanguangping@163.com)
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "funny.h"
#include "memory.h"
#include "scheme.h"

#define MAX_WORD_LENGTH 128
#define MAX_STR_LENGTH  1024
#define PAREN_L         '{'
#define PAREN_R         '}'
#define PAREN_L_STR     "{"
#define PAREN_R_STR     "}"
#define LOOP_L			'['
#define LOOP_R			']'
#define COMMENT         ';'
#define DELIMITER       ','

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
	while (!isspace(c) && COMMENT != c && PAREN_L != c && PAREN_R != c && DELIMITER != c && EOF != c) {
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

Statement *parse_statement(String token, Input *input) {
	Statement *root = NULL, *s = NULL;
	String the_next_token = NULL;

	root = init_arg();
	if (strcmp(PAREN_L_STR, token) == 0) {
		the_next_token = next_token(input);
		while (strcmp(PAREN_R_STR, the_next_token) != 0) {
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

// DFA  //

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

String match_one_arg(String symbol) {
	Queue *functions = get_functions();
	Queue *mappings = get_mappings();
	Mapping *mapping = NULL;
	String s = NULL;
	QueueNode *node = NULL;

	if (contains_queue_element(functions, symbol, equals_string) == TRUE) {
		return symbol;
	}

	//MAPPINGS
	node = mappings->head;
	while (node != NULL) {
		mapping = (Mapping *) node->element;
		if (mapping->sys != NULL) {
			node = node->next;
			continue;
		}

		if (equal_arg(mapping->sourceStatement, symbol)) {
			s = extract(mapping->targetStatement);
			return s;
		}

		if (mapping->sourceFunc != NULL && strcmp(symbol, mapping->sourceFunc) == 0) {
			return mapping->targetFunc;
		}

		node = node->next;
	}

	return NULL;
}

int orignal_statement(Statement *source) {
	QueueNode *node = NULL;
	Statement *s = NULL;
	if (source->children != NULL && source->children->head != NULL) {
		node = source->children->head;
		while (node != NULL) {
			s = (Statement *) node->element;
			if (s->type != SYM) {
				return FALSE;
			}
			node = node->next;
		}
	}

	return TRUE;
}

String match_orignal(Statement *source) {
	int orignal = TRUE;
	Queue *queue = NULL;
	QueueNode *node = NULL;
	Statement *s = NULL;
	String result = init_string();

	Queue *mappings = get_mappings();
	Mapping *mapping = NULL;

	//MAPPINGS
	node = mappings->head;
	while (node != NULL) {
		mapping = (Mapping *) node->element;
		if (mapping->sys != NULL) {
			node = node->next;
			continue;
		}

		if (equal_statement(mapping->sourceStatement, source)) {
			return extract(mapping->targetStatement);
		}

		node = node->next;
	}

	queue = source->children;
	node = queue->head;
	while (node != NULL) {
		s = (Statement *) node->element;
		if (s->type == SYM) {
			result = dyna_strcat(result, s->symbol);
			result = dyna_strcat(result, " ");
		} else {
			orignal = FALSE;
			break;
		}

		node = node->next;
	}

	if (orignal) {
		return result;
	} else {
		free(result);
		return NULL;
	}
}

State *test_match(Mapping *mapping, Statement *source, Queue *repQueue) {
	State *start = mapping->starter;
	State *next = NULL;
	QueueNode *node = NULL;
	Statement *s = NULL;

	if (source->children != NULL) {
		node = source->children->head;
		while (node != NULL) {
			s = (Statement *) node->element;
			next = transit(mapping->starter, start, s, repQueue);
			start = next;
			if (start == NULL) {
				break;
			}
			node = node->next;
		}
	}

	return start;
}

String match(Statement *source) {
	QueueNode *node = NULL;
	QueueNode *stnode = NULL;
	Mapping *mapping = NULL;
	String temp = NULL;
	String loopBuffer = NULL;
	String result = NULL;
	State *start = NULL;
	Statement *s = NULL;
	Statement *st = NULL;
	Queue *repQueue = NULL;
	int emptyLoop = FALSE;

	int matchTimes = 0;
	Queue *loop = NULL;
	QueueNode *loopNode = NULL;
	Statement *loopSt = NULL;
	int i = 0;
	int index = 0;

	// match one arg
	if (source->children != NULL && source->children->head != NULL
			&& source->children->head == source->children->tail) {
		temp = extract(source);
		result = match_one_arg(temp);
		if (result != NULL) {
			return result;
		}
	}

	// match orignal
	if (orignal_statement(source)) {
		result = match_orignal(source);
		if (result != NULL) {
			return result;
		}
	}

	node = get_mappings()->head;
	while (node != NULL) {
		mapping = (Mapping *) node->element;
		repQueue = init_queue();

#ifdef DEBUG
		if (mapping->targetFunc != NULL && strcmp(mapping->targetFunc, "cond") == 0) {
			//printf("debug\n");
		}
#endif

		start = test_match(mapping, source, repQueue);
		if (start == NULL) {
			node = node->next;
			continue;
		}
		if (start->accept) {
			if (mapping->sys != NULL) {
				return eval_sys(mapping, source, repQueue);
			}

			result = init_string();
			stnode = mapping->targetStatement->children->head;
			while (stnode != NULL) {
				s = (Statement *) stnode->element;
				temp = to_string_statement(s);

				switch (s->type) {
				case SYM:
					result = dyna_strcat(result, temp);
					result = dyna_strcat(result, " ");
					break;

				case ARG:
					result = dyna_strcat(result, match(find_replace_statement(repQueue, s)));
					result = dyna_strcat(result, " ");
					break;

				case LOOP:
					matchTimes = 0;
					loop = s->children;
					loopNode = NULL;
					loopSt = NULL;
					i = 0;
					index = 0;

					if (loop != NULL) {
						loopNode = loop->head;
						while (loopNode != NULL) {
							loopSt = (Statement *) loopNode->element;
							if (loopSt->type == ARG) {
								Queue *loopQueue = find_loop_replace_statement(repQueue, loopSt);
								matchTimes = count_queue_element(loopQueue);
								break;
							}
							loopNode = loopNode->next;
						}
					}

					loopBuffer = init_string();

					for (i = 0; i < matchTimes; i++) {
						if (loop != NULL) {
							loopNode = loop->head;
							while (loopNode != NULL) {
								loopBuffer = dyna_strcat(loopBuffer, " ");

								loopSt = (Statement *) loopNode->element;
								if (loopSt->type == ARG) {
									index = 0;

									Queue *loopQueue = find_loop_replace_statement(repQueue, loopSt);
									QueueNode *loopQueueNode = loopQueue->head;
									while (loopQueueNode != NULL) {
										if (index == i) {
											st = (Statement *) get_queue_element(loopQueue, index);
											if (strcmp("{}", to_string_statement(st)) == 0) {
												emptyLoop = TRUE;
												break;
											}
											loopBuffer = dyna_strcat(loopBuffer, match(st));
											break;
										}
										index++;
										loopQueueNode = loopQueueNode->next;
									}
								} else {
									loopBuffer = dyna_strcat(loopBuffer, to_string_statement(loopSt));
								}
								if (emptyLoop)
									break;
								loopNode = loopNode->next;
							}
						}
					}

					if (!emptyLoop) {
						result = dyna_strcat(result, loopBuffer);
					} else {
						free(loopBuffer);
					}

					break;

				default:
					break;
				}

				stnode = stnode->next;
			}
		}

		node = node->next;
	}

	if (result == NULL) {
		printf("Cannot find mapping for statement: %s\n", to_string_statement(source));
	}

	return result;
}

// TODO eval_sys  //

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

		array = split_string_once(prepare, '>');
		prepareSource = array[0];
		prepareTarget = array[1];

		arraySource = split_string_once(prepareSource, ',');
		sourceFuncMacro = trim_string(arraySource[0]);
		sourceMacro = trim_string(arraySource[1]);

		mapping->source = parse_macro(repQueue, sourceMacro);
		mapping->sourceFunc = parse_macro(repQueue, sourceFuncMacro);
		mapping->sourceStatement = parse_regex(mapping->source);
		mapping->starter = to_dfa(mapping->sourceStatement);

		arrayTarget = split_string_once(prepareTarget, ',');
		targetFuncMacro = trim_string(arrayTarget[0]);
		targetMacro = trim_string(arrayTarget[1]);

		mapping->target = parse_macro(repQueue, targetMacro);
		mapping->targetFunc = parse_macro(repQueue, targetFuncMacro);
		mapping->targetStatement = parse_regex(mapping->target);

		enqueue(get_mappings(), mapping);
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
		return find_replace_statement(repQueue, statement);
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

/****************** init_sys ******************/

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

#define SYS_MACRO      "sys.macro"
#define CORE_TEMPLATE  "core.fn"
#define CMD_TEMPLATE   "command.fn"

void load() {
	mappings = parse_sys_macro();
	load_template(CORE_TEMPLATE);
	load_template(CMD_TEMPLATE);
	functions = init_queue();
}

Queue *get_mappings() {
	if (mappings == NULL) {
		load();
	}
	return mappings;
}

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

Queue *parse_sys_macro() {
	String source = NULL;
	Queue *queue = init_queue();
	Mapping *mapping = NULL;

	FILE *fp = NULL;
	int c = 0;

	fp = fopen( SYS_MACRO, "rb");
	if (!fp)
		perror(SYS_MACRO), exit(1);

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
			enqueue(queue, mapping);
			continue;
		} else {
			while (c != '\n')
				c = getc(fp);
			continue;
		}
	}

	fclose(fp);

	return queue;
}

void match_template(Statement *source) {
	State *start = NULL;
	Queue *repQueue = NULL;
	Mapping *mapping = get_sys_mapping(get_mappings(), DEF_TEMPLATE);

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

/////////////////////////////////////

int funny_init() {
	load();
	return TRUE;
}

void wait_to_exit(Statement *st) {
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

//#ifdef DEBUG
//		printf("TARGET: %s\n", scheme);
//#endif

		eval(sc, scheme);

		printf("\n");
	}

	return 0;
}

