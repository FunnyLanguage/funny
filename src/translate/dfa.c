/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2015 by fanguangping (fanguangping@163.com)
 * dfa.c
 */

#include <stdlib.h>
#include <string.h>
#include "dfa.h"
#include "sys.h"
#include "init.h"

static long gensymId = 0;

String match_one_arg(Statement *source) {
	Queue *functions = get_functions();
	Mapping *mapping = NULL;
	String s = NULL;
	QueueNode *node = NULL;

    String symbol = get_arg_name(source);

	if (contains_queue_element(functions, symbol, equals_string) == TRUE) {
		return symbol;
	}

	Queue *mappings = get_mappings(source);
	if (mappings != NULL) {
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

			if (mapping->sourceFunc != NULL && strcmp(symbol,
					mapping->sourceFunc) == 0) {
				return mapping->targetFunc;
			}

			node = node->next;
		}
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

	Queue *mappings = get_mappings(source);
	Mapping *mapping = NULL;

	if (mapping != NULL) {
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

static int get_match_times_from_arg(Statement *arg, Queue *repQueue);
static Statement *expand(Statement *source, Queue *repQueue);

static int get_match_times(Statement *loop, Queue *repQueue) {
	int matchTimes = 0;
	QueueNode *node = NULL;
	Statement *s = NULL;

	if (loop->children != NULL) {
		node = loop->children->head;
		while (node != NULL) {
			s = (Statement *) node->element;
			if (s->type == ARG) {
				matchTimes = get_match_times_from_arg(s, repQueue);
				if (matchTimes != 0)
					break;
			}
			node = node->next;
		}
	}
	return matchTimes;
}

static int get_match_times_from_arg(Statement *arg, Queue *repQueue) {
	int matchTimes = 0;
	QueueNode *node = NULL;
	Statement *s = NULL;
	Queue *loopQueue = NULL;
	String argName = NULL;

	if (arg->type == ARG) {
		argName = get_arg_name(arg);
		loopQueue = find_loop_replace_statement(repQueue, argName);
		if (!is_queue_empty(loopQueue)) {
			matchTimes = count_queue_element(loopQueue);
		} else {
			if (arg->children != NULL) {
				node = arg->children->head;
				while (node != NULL) {
					s = (Statement *) node->element;
					if (s->type == ARG) {
						matchTimes = get_match_times_from_arg(s, repQueue);
						if (matchTimes != 0)
							break;
					}
					node = node->next;
				}
			}
		}
	}
	return matchTimes;
}

static Queue *expand_loop_statement(Statement *loop, Queue *repQueue, int matchTimes) {
	int i = 0;
	int index = 0;
	QueueNode *node = NULL;
	Statement *s = NULL;
	Statement *st = NULL;

	Queue *loopQueue = NULL;
	QueueNode *loopQueueNode = NULL;
	Queue *result = init_queue();
	String argName = NULL;

	for (i = 0; i < matchTimes; i++) {
		if (loop != NULL) {
			node = loop->children->head;
			while (node != NULL) {
				s = (Statement *) node->element;
				switch (s->type) {
				case SYM:
					enqueue(result, copy_statement(s));
					break;
				case ARG:
					argName = get_arg_name(s);
					loopQueue = find_loop_replace_statement(repQueue, argName);
					if (argName != NULL) {
						if (!is_queue_empty(loopQueue)) {
							index = 0;
							loopQueueNode = loopQueue->head;
							while (loopQueueNode != NULL) {
								if (index == i) {
									st = (Statement *) get_queue_element(loopQueue, index);
									enqueue(result, copy_statement(st));
									break;
								}
								index++;
								loopQueueNode = loopQueueNode->next;
							}
						} else {
							enqueue(result, copy_statement(s));
						}
					} else {
						enqueue(result, expand(s, repQueue));
					}
					break;
				case LOOP:
					fprintf(stderr, "Loop in loop not support!");
					break;
				}

				node = node->next;
			}
		}
	}

	return result;
}

static Statement *expand(Statement *source, Queue *repQueue) {
	String argName = NULL;
	Statement *s = NULL;
	QueueNode *node = NULL;
	Statement *result = NULL;
	int matchTimes = 0;

	switch (source->type) {
	case SYM:
		result = copy_statement(source);
		break;

	case ARG:
		argName = get_arg_name(source);
		s = find_replace_statement(repQueue, argName);
		if (argName != NULL) {
			if (s != NULL)
				result = copy_statement(s);
			else
				result = copy_statement(source);
		} else {
			result = init_arg();
			node = source->children->head;
			while (node != NULL) {
				s = (Statement *) node->element;
				if (s->type == LOOP) {
					matchTimes = get_match_times(s, repQueue);
					append_queue(result->children, expand_loop_statement(s, repQueue, matchTimes));
				} else {
					enqueue(result->children, expand(s, repQueue));
				}
				node = node->next;
			}
		}
		break;

	case LOOP:
		fprintf(stderr, "Not support!");
		break;
	}

	return result;
}

static String generate_symbol(long baseId, Queue *gensymHolder, String symbol) {
	String name = alloc_string(40);
	int index = get_queue_element_index(gensymHolder, symbol, equals_string);
	if (index == -1) {
		snprintf(name, 40, "gensym-%ld", gensymId);
		gensymId++;
		enqueue(gensymHolder, symbol);
	} else {
		snprintf(name, 40, "gensym-%ld", (baseId + index));
	}
	return name;
}

static void replace_with_gensym(Statement *source) {
	QueueNode *node = NULL;
	Statement *st = NULL;
	Queue *gensymHolder = init_queue();
	long baseId = gensymId;
	String symbol = NULL;

	node = source->children->head;
	while (node != NULL) {
		st = (Statement *) node->element;
		if (st->type == ARG) {
			symbol = get_arg_name(st);
			if (symbol != NULL && *symbol == '$') {
				String name = generate_symbol(baseId, gensymHolder, symbol);
				set_arg_name(st, name);
			} else {
				replace_with_gensym(st);
			}
		}
		node = node->next;
	}
}

Statement *expand_statement(Mapping *mapping, Queue *repQueue) {
	Statement *s = expand(mapping->targetStatement, repQueue);
	replace_with_gensym(s);
	return s;
}

String match(Statement *source) {
	QueueNode *node = NULL;
	Mapping *mapping = NULL;
	String result = NULL;
	State *start = NULL;
	Queue *repQueue = NULL;
	Statement *expanded = NULL;

	// match one arg
	if (source->children != NULL && source->children->head != NULL
			&& source->children->head == source->children->tail) {
		//temp = extract(source);
		result = match_one_arg(source);
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

	Queue *mappings = get_mappings(source);
	if (mappings != NULL) {
		node = mappings->head;
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
				if (mapping->isMacro) {
					expanded = expand_statement(mapping, repQueue);
					String string = to_string_statement(expanded);
					printf("expanded: %s\n", string);
					return match(expanded);
				}

				if (mapping->sys != NULL) {
					return eval_sys(mapping, source, repQueue);
				} else {
					result = eval_normal(mapping, repQueue);
				}
			}

			node = node->next;
		}
	}

	if (result == NULL) {
		printf("Cannot find mapping for statement: %s\n", to_string_statement(source));
	}

	return result;
}

static String match_loop_statement(Statement *loop, Queue *repQueue, int matchTimes) {
	int i = 0;
	int index = 0;
	QueueNode *node = NULL;
	Statement *s = NULL;
	Statement *st = NULL;
	String result = init_string();
	Queue *loopQueue = NULL;
	QueueNode *loopQueueNode = NULL;

	for (i = 0; i < matchTimes; i++) {
		if (loop != NULL) {
			node = loop->children->head;
			while (node != NULL) {
				result = dyna_strcat(result, " ");

				s = (Statement *) node->element;
				if (s->type == ARG) {
					index = 0;

					loopQueue = find_loop_replace_statement(repQueue, get_arg_name(s));
					loopQueueNode = loopQueue->head;
					while (loopQueueNode != NULL) {
						if (index == i) {
							st = (Statement *) get_queue_element(loopQueue, index);
							//if (strcmp("{}", to_string_statement(st)) == 0) {
							if (empty_statement(st)) {
								return NULL;
							}
							result = dyna_strcat(result, match(st));
							break;
						}
						index++;
						loopQueueNode = loopQueueNode->next;
					}
				} else {
					result = dyna_strcat(result, to_string_statement(s));
				}
				node = node->next;
			}
		}
	}

	return result;
}

String eval_normal(Mapping *mapping, Queue *repQueue) {
	String result = init_string();
	QueueNode *node = NULL;
	Statement *s = NULL;
	String temp = NULL;

	String loopBuffer = NULL;
	int matchTimes = 0;

	node = mapping->targetStatement->children->head;
	while (node != NULL) {
		s = (Statement *) node->element;
		temp = to_string_statement(s);

		switch (s->type) {
		case SYM:
			result = dyna_strcat(result, temp);
			result = dyna_strcat(result, " ");
			break;

		case ARG:
			result = dyna_strcat(result, match(find_replace_statement(repQueue, get_arg_name(s))));
			result = dyna_strcat(result, " ");
			break;

		case LOOP:
			matchTimes = get_match_times(s, repQueue);
			loopBuffer = match_loop_statement(s, repQueue, matchTimes);
			if (loopBuffer != NULL) {
				result = dyna_strcat(result, loopBuffer);
			}
			break;

		default:
			break;
		}

		node = node->next;
	}

	return result;
}
