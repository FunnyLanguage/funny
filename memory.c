/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2014 by fanguangping (fanguangping@163.com)
 *
 * Funny is based on TinyScheme which is now maintained by Dimitrios Souflis (dsouflis@acm.org)
 * This is a revised and modified version of TinyScheme
 */

#include <stdio.h>
#include <stdlib.h>
#include "memory.h"
#include "basic.h"

void init_constants() {
	(&g_nil)->_flag = (T_ATOM | REF_MARK);
	(&g_nil)->_pair._car = (&g_nil)->_pair._cdr = &g_nil;
	(&g_true)->_flag = (T_ATOM | REF_MARK);
	(&g_true)->_pair._car = (&g_true)->_pair._cdr = &g_true;
	(&g_false)->_flag = (T_ATOM | REF_MARK);
	(&g_false)->_pair._car = (&g_false)->_pair._cdr = &g_false;
	(&g_eof)->_flag = (T_ATOM | REF_MARK);
	(&g_eof)->_pair._car = (&g_eof)->_pair._cdr = &g_eof;
}

void finalize_cell(Scheme *sc, Cell* a) {
	if (is_string(a)) {
		free(a->_string);
	} else if (is_port(a)) {
		if ((a->_port->kind & PORT_FILE) && a->_port->filePort.closed) {
			port_close(a, PORT_INPUT | PORT_OUTPUT);
		}
		free(a->_port);
	}
}

/* garbage collector  //标记-清除算法
 *  We use algorithm E (Knuth, The Art of Computer Programming Vol.1, sec. 2.3.5),
 *  the Schorr-Deutsch-Waite link-inversion algorithm, for marking.
 */
static void mark(Cell* a) {
	Cell* t = (Cell*) 0;
	Cell* p = a;
	E2: set_mark(p);
	if (is_vector(p)) {
		int i;
		int num = ((p)->_num.longValue) / 2 + ((p)->_num.longValue) % 2;
		for (i = 0; i < num; i++) {
			/* Vector cells will be treated like ordinary cells */
			mark(p + 1 + i);
		}
	}
	if (is_atom(p))
		goto E6;
	/* E4: down car */
	Cell* q = (p)->_pair._car;
	if (q && !is_mark(q)) {
		set_atom(p); /* a note that we have moved car */
		(p)->_pair._car = t;
		t = p;
		p = q;
		goto E2;
	}
	E5: q = (p)->_pair._cdr; /* down cdr */
	if (q && !is_mark(q)) {
		(p)->_pair._cdr = t;
		t = p;
		p = q;
		goto E2;
	}
	E6: /* up.  Undo the link switching from steps E4 and E5. */
	if (!t)
		return;
	q = t;
	if (is_atom(q)) {
		clear_atom(q);
		t = (q)->_pair._car;
		(q)->_pair._car = p;
		p = q;
		goto E5;
	} else {
		t = (q)->_pair._cdr;
		(q)->_pair._cdr = p;
		p = q;
		goto E6;
	}
}

/* garbage collection. parameter a, b is marked. */
void gc(Scheme *sc, Cell* a, Cell* b) {
	//int b_out = sc->gc_verbose && (sc->outport != &g_nil);
	int b_out = TRUE;
	//if (b_out)
	//	write_string(sc, "gc start ...");

	/* mark system globals */
	mark(sc->objectList);
	mark(sc->globalEnv);

	/* mark current registers */
	mark(sc->args);
	mark(sc->env);
	mark(sc->code);
	mark(sc->callStack);
//	mark(sc->ret_value);
//	mark(sc->outport);
//	mark(sc->inport);
//	mark(sc->save_inport);
//	mark(sc->loadport);

	/* mark variables a, b */
	mark(a);
	mark(b);

	/* garbage collect */
	clear_mark(&g_nil);
	sc->freeCellCount = 0;
	sc->freeCell = &g_nil;
	/* free-list is kept sorted by address so as to maintain consecutive
	 ranges, if possible, for use with vectors. Here we scan the cells
	 (which are also kept sorted by address) downwards to build the
	 free-list in sorted order.
	 */
	int i;
	for (i = sc->lastCellSeg; i >= 0; i--) {
		Cell* p = sc->cellSeg[i] + CELL_SEGSIZE;
		while (--p >= sc->cellSeg[i]) {
			if (is_mark(p)) {  //如果已经标记清除标记位
				clear_mark(p);
			} else {  //如果未标记，释放该节点
				/* reclaim cell */
				if (typeflag(p) != 0) {
					finalize_cell(sc, p);
					(p)->_flag = 0;
					p->_pair._car = &g_nil;
				}
				++sc->freeCellCount;
				(p)->_pair._cdr = sc->freeCell;
				sc->freeCell = p;
			}
		}
	}

	if (b_out) {
		char msg[80];
		snprintf(msg, 80, "done: %ld cells were recovered.\n", sc->freeCellCount);
		//write_string(sc, msg);
	}
}

/* allocate new cell segment */
int alloc_cellseg(Scheme *sc, int n) {
	int k;
	Cell* p;

	for (k = 0; k < n; k++) {
		if (sc->lastCellSeg >= CELL_NSEGMENT - 1)
			return k;
		sc->cellSeg[sc->lastCellSeg + 1] = (Cell*) malloc((CELL_SEGSIZE + 1) * sizeof(Cell));
		long i = ++sc->lastCellSeg;
		Cell* p_new = sc->cellSeg[i];
		while (i > 0 && sc->cellSeg[i - 1] > sc->cellSeg[i]) { //插入排序，sc->cell_seg中的元素安装指针值的从小大排序
			sc->cellSeg[i] = sc->cellSeg[i - 1];
			sc->cellSeg[--i] = p_new;
		}
		sc->freeCellCount += CELL_SEGSIZE; //增加空闲单元的计数
		//所有的空闲cell构成一个链表，并且依照位置值从小到大排列
		Cell* last = p_new + CELL_SEGSIZE - 1;
		for (p = p_new; p <= last; p++) {
			(p)->_flag = 0;
			p->_pair._cdr = p + 1;
			p->_pair._car = &g_nil;
		}
		/* insert new cells in address order on free list */
		if (sc->freeCell == &g_nil || last < sc->freeCell) {
			last->_pair._cdr = sc->freeCell;
			sc->freeCell = p_new;
		} else {
			Cell* p = sc->freeCell;
			while ((p)->_pair._cdr != &g_nil && p_new > (p)->_pair._cdr)
				p = (p)->_pair._cdr;
			last->_pair._cdr = p->_pair._cdr;
			p->_pair._cdr = p_new;
		}
	}
	return n;
}

/* get new cell.  parameter a, b is marked by gc. */
Cell* create_cell_helper(Scheme *sc, Cell* a, Cell* b) {		//分配一个cell
	if (sc->freeCell == &g_nil) {
		gc(sc, a, b);
		if (sc->freeCell == &g_nil) {
			if (!alloc_cellseg(sc, 1) && sc->freeCell == &g_nil) {
				printf("No Memory ! ! !\n");
				while (1)
					getchar();
			}
		}
	}
	Cell* p = sc->freeCell;
	sc->freeCell = (p)->_pair._cdr;
	--sc->freeCellCount;
	return p;
}

void exit_when_out_of_memory(void *element) {
	if (element == NULL) {
		fprintf(stderr, "out of memory!\n");
		exit(1);
	}
}

String alloc_string(int size) {
	String result = (String) malloc(sizeof(char) * size);
	exit_when_out_of_memory(result);
	return result;
}

String *alloc_string_array(int size) {
	String *result = (String*) malloc(sizeof(String) * size);
	exit_when_out_of_memory(result);
	return result;
}

String realloc_string(String str, int size) {
	str = (String) realloc(str, size * sizeof(char));
	exit_when_out_of_memory(str);
	return str;
}

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

HashTable alloc_hash_table() {
	HashTable result = (HashTable) malloc(HASHTABLE_SIZE * sizeof(HashNode *));
	exit_when_out_of_memory(result);
	return result;
}

HashNode *alloc_hash_node() {
	HashNode *result = (HashNode *) malloc(sizeof(HashNode));
	exit_when_out_of_memory(result);
	return result;
}

Scheme *alloc_scheme() {
	Scheme *result = (Scheme *) malloc(sizeof(Scheme));
	exit_when_out_of_memory(result);
	return result;
}

Cell *alloc_cell() {
	Cell *result = (Cell *) malloc(sizeof(Cell));
	exit_when_out_of_memory(result);
	return result;
}

Input *alloc_input() {
	Input *result = (Input *) malloc(sizeof(Input));
	exit_when_out_of_memory(result);
	return result;
}

Statement *alloc_statement() {
	Statement *result = (Statement *) malloc(sizeof(Statement));
	exit_when_out_of_memory(result);
	return result;
}

Mapping *alloc_mapping() {
	Mapping *result = (Mapping *) malloc(sizeof(Mapping));
	exit_when_out_of_memory(result);
	return result;
}

SysBlock *alloc_sys_block() {
	SysBlock *result = (SysBlock *) malloc(sizeof(SysBlock));
	exit_when_out_of_memory(result);
	return result;
}

ReplaceNode *alloc_replace_node() {
	ReplaceNode *result = (ReplaceNode *) malloc(sizeof(ReplaceNode));
	exit_when_out_of_memory(result);
	return result;
}

State *alloc_state() {
	State *result = (State *) malloc(sizeof(State));
	exit_when_out_of_memory(result);
	return result;
}

TransitNode *alloc_transit_node() {
	TransitNode *result = (TransitNode *) malloc(sizeof(TransitNode));
	exit_when_out_of_memory(result);
	return result;
}
