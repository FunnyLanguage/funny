/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2014 by fanguangping (fanguangping@163.com)
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "ds.h"
#include "memory.h"

/****************** String ******************/

String init_string() {
	String result = alloc_string(1);
	result[0] = '\0';
	return result;
}

String copy_string(String str) {
	String result = alloc_string(strlen(str) + 1);
	strcpy(result, str);
	return result;
}

String trim_string(String str) {
	char *end;

	while (isspace(*str))
		str++;

	if (*str == '\0')
		return str;

	end = str + strlen(str) - 1;
	while (end > str && isspace(*end))
		end--;

	*(end + 1) = '\0';

	return str;
}

String substr_before(String str, char c) {
	char *p = strchr(str, c);
	if (p) {
		*p = '\0';
		return str;
	} else {
		return "";
	}
}

String substr_between(String str, char a, char b) {
	char *s = strchr(str, a);
	char *e = strchr(str, b);

	if (s && e && s < e) {
		*e = '\0';
		return s + 1;
	} else {
		return "";
	}
}

String substr_after(String str, char c) {
	char *p = strchr(str, c);
	if (p)
		return p + 1;
	else
		return "";
}

String dyna_strcat(String str, String append) {
	int length = strlen(str) + strlen(append) + 1;
	str = realloc_string(str, length);
	str = strcat(str, append);
	return str;
}

String append_char(String str, char c) {
	char *p = NULL;
	int length = strlen(str) + 2;
	str = realloc_string(str, length);
	p = str;
	while (*p)
		p++;
	*p++ = c;
	*p++ = '\0';
	return str;
}

String* split_string(String string, const char delimiter) {
	char** result = 0;
	int count = 0;
	char* tmp = string;
	char* last_comma = 0;
	char delim[2];
	delim[0] = delimiter;
	delim[1] = 0;

	/* Count how many elements will be extracted. */
	while (*tmp) {
		if (delimiter == *tmp) {
			count++;
			last_comma = tmp;
		}
		tmp++;
	}

	/* Add space for trailing token. */
	count += last_comma < (string + strlen(string) - 1);

	/* Add space for terminating null string so caller
	 knows where the list of returned strings ends. */
	count++;

	result = malloc(sizeof(char*) * count);

	if (result) {
		int index = 0;
		char* token = strtok(string, delim);

		while (token) {
			//assert(index < count);
			*(result + index++) = strdup(token);
			token = strtok(0, delim);
		}
		//assert(index == count - 1);
		*(result + index) = 0;
	}

	return result;
}

String* split_string_once(String string, const char delimiter) {
	char** result = NULL;
	char *p = strchr(string, delimiter);
	if (p) {
		result = alloc_string_array(2);
		*p = '\0';
		*result = copy_string(string);
		*(result + 1) = copy_string(p + 1);
	}

	return result;
}

int equals_string(void *a, void *b) {
	if (strcmp(a, b) == 0) {
		return TRUE;
	} else {
		return FALSE;
	}
}

int hash_code_string(void *key) {
	int hash = 0;
	char *p = key;
	while (*p++) {
		hash += *p;
	}
	hash = hash % HASHTABLE_SIZE;
	return hash;
}

char *strlwr(char *str) {
  size_t i;
  size_t len = strlen(str);

  for(i=0; i<len; i++)
	  str[i] = tolower((unsigned char)str[i]);

  return str;
}

/****************** Queue ******************/

Queue *init_queue() {
	Queue *queue = alloc_queue();
	queue->head = NULL;
	queue->tail = NULL;
	return queue;
}

void enqueue(Queue *queue, void *element) {
	QueueNode *node = NULL;
	if (queue == NULL)
		return;

	node = alloc_queue_node();
	node->element = element;
	node->next = NULL;

	if (queue->head == NULL) {
		queue->tail = queue->head = node;
	} else {
		queue->tail->next = node;
		queue->tail = node;
	}
}

void *dequeue(Queue *queue) {
	QueueNode *node = NULL;
	void *element = NULL;

	if (queue == NULL || queue->head == NULL) {
		return NULL;
	}

	element = queue->head->element;
	if (queue->head == queue->tail) {
		free(queue->head);
		queue->head = queue->tail = NULL;
	} else {
		node = queue->head;
		queue->head = queue->head->next;
		free(node);
	}

	return element;
}

void *get_queue_element(Queue *queue, int index) {
	int i = 0;
	QueueNode *node = NULL;
	if (queue == NULL)
		return NULL;

	node = queue->head;
	while (node != NULL) {
		if (i == index) {
			return node->element;
		}
		node = node->next;
		i++;
	}
	return NULL;
}

int is_queue_empty(Queue *queue) {
	return (queue == NULL || queue->head == NULL) ? TRUE : FALSE;
}

void clear_queue(Queue *queue) {
	QueueNode *node = NULL, *p = NULL;
	if (queue == NULL)
		return;

	p = queue->head;
	while (TRUE) {
		if (p == NULL)
			break;

		node = p;
		p = node->next;
		free(node);
	}

	queue->head = NULL;
	queue->tail = NULL;
}

void append_queue(Queue *q1, Queue *q2) {
	void *temp = NULL;
	while ((temp = dequeue(q2))) {
		enqueue(q1, temp);
	}
	free(q2);
}

int contains_queue_element(Queue *queue, void *element, int (*equals)(void *e1, void *e2)) {
	QueueNode *node = queue->head;
	while (node != NULL) {
		if (equals(element, node->element) == TRUE) {
			return TRUE;
		}
		node = node->next;
	}
	return FALSE;
}

int count_queue_element(Queue *queue) {
	int count = 0;

	QueueNode *node = queue->head;
	while (node != NULL) {
		count++;
		node = node->next;
	}
	return count;
}

/****************** HashTable ******************/

HashTable init_table() {
	HashTable table = alloc_hash_table();
	int i;
	for (i = 0; i < HASHTABLE_SIZE; i++) {
		table[i] = NULL;
	}
	return table;
}

void put_table_element(HashTable table, void *key, void *value,
		int (*hash_code)(void *key)) {
	HashNode *valueNode = NULL;
	int hash = 0;
	HashNode *node = NULL;
	if (table == NULL)
		return;

	valueNode = alloc_hash_node();
	valueNode->element = value;
	valueNode->next = NULL;
	hash = hash_code(key);
	node = table[hash];

	if (node == NULL) {
		table[hash] = valueNode;
	} else {
		while (node->next != NULL)
			node = node->next;
		node->next = valueNode;
	}
}

HashNode *get_table_element(HashTable table, void *key,
		int (*hash_code)(void *key)) {
	if (table == NULL)
		return NULL;
	else
		return table[hash_code(key)];
}

void clear_table(HashTable table) {
	HashNode *node = NULL, *p = NULL;
	int i;
	if (table == NULL)
		return;

	for (i = 0; i < HASHTABLE_SIZE; i++) {
		p = table[i];
		while (p != NULL) {
			node = p;
			p = node->next;
			free(node);
		}
		table[i] = NULL;
	}
}

/****************** Scheme ******************/

/****************** Statement ******************/

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

int is_sym(String token) {
	if (strcmp("{", token) == 0 || strcmp("}", token) == 0) {
		return FALSE;
	} else {
		return TRUE;
	}
}

int equal_sym(Statement *s, String sym) {
	if (s->type == SYM && strcmp(s->symbol, sym) == 0) {
		return TRUE;
	} else {
		return FALSE;
	}
}

int equal_arg(Statement *s, String sym) {
	if (s->type == ARG && s->children->head == s->children->tail
			&& strcmp(((Statement *) (s->children->head->element))->symbol, sym)
					== 0) {
		return TRUE;
	} else {
		return FALSE;
	}
}

int empty_statement(Statement *s) {
	return (s == NULL || (s->type == ARG && s->children->head == NULL));
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

String strcat_statement(String buf, Statement *s) {
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

/****************** ReplaceQueue ******************/

void add_replace_statement(Queue *queue, Statement *source, Statement *target) {
	if (queue == NULL)
		return;

	ReplaceNode *node = alloc_replace_node();
	node->source = source;
	node->target = target;
	enqueue(queue, node);
}

Statement *find_replace_statement(Queue *queue, Statement *source) {
	QueueNode *node = NULL;
	ReplaceNode *repNode = NULL;
	if (queue == NULL)
		return NULL;

	node = queue->head;
	while (node != NULL) {
		repNode = (ReplaceNode *) (node->element);
		if (equal_statement(source, repNode->source)) {
			return repNode->target;
		}
		node = node->next;
	}
	return NULL;
}

Queue *find_loop_replace_statement(Queue *queue, Statement *source) {
	Queue *result = NULL;
	QueueNode *node = NULL;
	ReplaceNode *repNode = NULL;
	if (queue == NULL)
		return NULL;

	result = init_queue();
	node = queue->head;
	while (node != NULL) {
		repNode = (ReplaceNode *) (node->element);
		if (equal_statement(source, repNode->source)) {
			enqueue(result, repNode->target);
		}
		node = node->next;
	}
	return result;
}

/****************** State ******************/

static int stateId = 0;
static HashTable stateTable = NULL;

State *new_state() {
	State *s = alloc_state();
	s->id = stateId++;
	s->accept = FALSE;
	return s;
}

void set_transition(State *starter, State *current, State *next,
		Statement *loop) {
	QueueNode *node = NULL;
	TransitNode *tnode = NULL;
	State *currentState = current;

	if (stateTable == NULL) {
		stateTable = init_table();
	}

	if (loop->children != NULL) {
		node = loop->children->head;
		while (node != loop->children->tail) {
			State *nextState = new_state();
			tnode = alloc_transit_node();
			tnode->starter = starter;
			tnode->input = (Statement *) node->element;
			tnode->next = nextState;
			put_table_element(stateTable, currentState, tnode, hash_code_state);
			currentState = nextState;

			node = node->next;
		}

		tnode = alloc_transit_node();
		tnode->starter = starter;
		tnode->input = (Statement *) loop->children->tail->element;
		tnode->next = next;
		put_table_element(stateTable, currentState, tnode, hash_code_state);
	}
}

State *to_dfa(Statement *s) {
	State *current = new_state();
	State *starter = current;
	QueueNode *node = NULL;
	Statement *st = NULL;
	TransitNode *tnode = NULL;

	if (stateTable == NULL) {
		stateTable = init_table();
	}

	if (s->children != NULL) {
		node = s->children->head;
		while (node != NULL) {
			st = (Statement *) node->element;
			switch (st->type) {
			case SYM:
			case ARG:
				tnode = alloc_transit_node();
				tnode->starter = starter;
				tnode->input = st;
				tnode->next = new_state();
				put_table_element(stateTable, current, tnode, hash_code_state);
				current = tnode->next;
				break;

			case LOOP:
				set_transition(starter, current, current, st);
				break;
			}
			node = node->next;
		}
		current->accept = TRUE;
	}

	return starter;
}

State *transit(State *starter, State *start, Statement *input, Queue *repQueue) {
	State *result = NULL;
	HashNode *node = NULL;
	TransitNode *tnode = NULL;

	if (start == NULL || input == NULL)
		return NULL;
	switch (input->type) {
	case SYM:
		node = (HashNode *) get_table_element(stateTable, start,
				hash_code_state);
		while (node != NULL) {
			tnode = (TransitNode *) node->element;
			if (equals_state(starter, tnode->starter)
					&& equal_statement(input, tnode->input)) {
				result = tnode->next;
				break;
			}
			node = node->next;
		}
		break;

	case ARG:
		node = (HashNode *) get_table_element(stateTable, start,
				hash_code_state);
		while (node != NULL) {
			tnode = (TransitNode *) node->element;
			if (equals_state(starter, tnode->starter)
					&& tnode->input->type == ARG) {
				add_replace_statement(repQueue, tnode->input, input);
				result = tnode->next;
				break;
			}
			node = node->next;
		}
		break;

	case LOOP:
		fprintf(stderr, "transit error!\n");
		break;
	}

	return result;
}

int equals_state(void *a, void *b) {
	if (a == NULL && b == NULL)
		return TRUE;
	if (a == NULL || b == NULL)
		return FALSE;
	return ((State *) a)->id == ((State *) b)->id;
}

int hash_code_state(void *s) {
	if (s == NULL)
		return -1;
	return (((State *) s)->id) % HASHTABLE_SIZE;
}

/****************** Mapping ******************/
Mapping *get_sys_mapping(Queue *mappings, String name) {
	Mapping *mapping = NULL;
	QueueNode *node = mappings->head;
	while (node != NULL) {
		mapping = (Mapping *) node->element;
		if (mapping->sys != NULL && strcmp(mapping->sys->name, name) == 0) {
			return mapping;
		}
		node = node->next;
	}
	return NULL;
}
