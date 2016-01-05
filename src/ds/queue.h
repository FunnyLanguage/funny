/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2015 by fanguangping (fanguangping@163.com)
 * queue.h
 */

#ifndef QUEUE_H_
#define QUEUE_H_

typedef struct _QueueNode {
	void *element;
	struct _QueueNode *next;
} QueueNode;

typedef struct _Queue {
	struct _QueueNode *head;
	struct _QueueNode *tail;
} Queue;

Queue *alloc_queue();
QueueNode *alloc_queue_node();

Queue *init_queue();
void enqueue(Queue *queue, void *element);
void *dequeue(Queue *queue);
void *get_queue_element(Queue *queue, int index);
int is_queue_empty(Queue *queue);
void clear_queue(Queue *queue);
void append_queue(Queue *q1, Queue *q2);
int contains_queue_element(Queue *queue, void *element, int (*equals)(void *e1, void *e2));
int get_queue_element_index(Queue *queue, void *element, int (*equals)(void *e1, void *e2));
int count_queue_element(Queue *queue);

#endif /* QUEUE_H_ */
