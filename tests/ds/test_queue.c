/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2015 by fanguangping (fanguangping@163.com)
 * test_queue.c
 */

#include <assert.h>
#include <string.h>
#include "test_queue.h"
#include "../../src/ds/datastructure.h"

void test_alloc_queue() {
	Queue *queue = alloc_queue();
	assert(queue != NULL);
}

void test_alloc_queue_node() {
	QueueNode *node = alloc_queue_node();
	assert(node != NULL);
}

void test_init_queue() {
	Queue *queue = init_queue();
	assert(queue != NULL);
	assert(queue->head == NULL);
	assert(queue->tail == NULL);
}

void test_enqueue() {
	Queue *queue = init_queue();
	enqueue(queue, "abc");
	assert(queue->head == queue->tail);
	assert(strcmp(queue->head->element, "abc") == 0);
	enqueue(queue, "def");
	assert(strcmp(queue->head->next->element, "def") == 0);
	assert(strcmp(queue->tail->element, "def") == 0);
}

void test_dequeue() {
	Queue *queue = init_queue();
	enqueue(queue, "abc");
	enqueue(queue, "def");
	char *str = dequeue(queue);
	assert(strcmp(str, "abc") == 0);
	assert(queue->head == queue->tail);
	str = dequeue(queue);
	assert(strcmp(str, "def") == 0);
	assert(queue->head == NULL);
	assert(queue->tail == NULL);
	assert(dequeue(queue) == NULL);
}

void test_get_queue_element() {
	Queue *queue = init_queue();
	enqueue(queue, "abc");
	enqueue(queue, "def");
	assert(strcmp(get_queue_element(queue, 0), "abc") == 0);
	assert(strcmp(get_queue_element(queue, 1), "def") == 0);
	assert(get_queue_element(queue, 2) == NULL);
	assert(get_queue_element(queue, -1) == NULL);
}

void test_is_queue_empty() {
	assert(is_queue_empty(NULL));
	Queue *queue = init_queue();
	assert(is_queue_empty(queue));
	enqueue(queue, "abc");
	dequeue(queue);
	assert(is_queue_empty(queue));
}

void test_clear_queue() {
	clear_queue(NULL);
	Queue *queue = init_queue();
	clear_queue(queue);
	assert(is_queue_empty(queue));
	enqueue(queue, "abc");
	enqueue(queue, "def");
	clear_queue(queue);
	assert(is_queue_empty(queue));
}

void test_append_queue() {
	Queue *q1 = init_queue();
	enqueue(q1, "abc");
	Queue *q2 = init_queue();
	enqueue(q2, "def");
	append_queue(q1, q2);
	append_queue(q1, NULL);
	append_queue(NULL, NULL);
	assert(strcmp(q1->head->next->element, "def") == 0);
}

void test_contains_queue_element() {
	Queue *queue = init_queue();
	enqueue(queue, "abc");
	enqueue(queue, "def");
	assert(contains_queue_element(queue, "abc", equals_string));
	assert(contains_queue_element(queue, "def", equals_string));
	assert(!contains_queue_element(queue, "ghi", equals_string));
	assert(!contains_queue_element(queue, NULL, equals_string));
	assert(!contains_queue_element(NULL, NULL, equals_string));
}

void test_get_queue_element_index() {
	Queue *queue = init_queue();
	enqueue(queue, "abc");
	enqueue(queue, "def");
	assert(get_queue_element_index(queue, "abc", equals_string) == 0);
	assert(get_queue_element_index(queue, "def", equals_string) == 1);
	assert(get_queue_element_index(queue, "ghi", equals_string) == -1);
	assert(get_queue_element_index(queue, NULL, equals_string) == -1);
	assert(get_queue_element_index(NULL, NULL, equals_string) == -1);
}

void test_count_queue_element() {
	assert(count_queue_element(NULL) == -1);
	Queue *queue = init_queue();
	assert(count_queue_element(queue) == 0);
	enqueue(queue, "abc");
	assert(count_queue_element(queue) == 1);
	enqueue(queue, "def");
	assert(count_queue_element(queue) == 2);
}

void test_queue() {
	test_alloc_queue();
	test_alloc_queue_node();
	test_init_queue();
	test_enqueue();
	test_dequeue();
	test_get_queue_element();
	test_is_queue_empty();
	test_clear_queue();
	test_append_queue();
	test_contains_queue_element();
	test_get_queue_element_index();
	test_count_queue_element();
}
