/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2015 by fanguangping (fanguangping@163.com)
 * queue.c
 */

#include <stdlib.h>
#include "datastructure.h"

Queue *alloc_queue() {
	Queue *result = (Queue *) malloc(sizeof(Queue));
	exit_when_out_of_memory(result);
	return result;
}

QueueNode *alloc_queue_node() {
	QueueNode *result = (QueueNode *) malloc(sizeof(QueueNode));
	exit_when_out_of_memory(result);
	return result;
}

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

int contains_queue_element(Queue *queue, void *element,
		int(*equals)(void *e1, void *e2)) {
	return get_queue_element_index(queue, element, equals) == -1;
}

int get_queue_element_index(Queue *queue, void *element,
		int(*equals)(void *e1, void *e2)) {
	if (queue == NULL || element == NULL || equals == NULL)
		return -1;
	int index = 0;
	QueueNode *node = queue->head;
	while (node != NULL) {
		if (equals(element, node->element) == TRUE) {
			return index;
		}
		node = node->next;
		index++;
	}
	return -1;
}

int count_queue_element(Queue *queue) {
	if (queue == NULL)
		return -1;
	int count = 0;

	QueueNode *node = queue->head;
	while (node != NULL) {
		count++;
		node = node->next;
	}
	return count;
}

