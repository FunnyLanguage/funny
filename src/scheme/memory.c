/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2015 by fanguangping (fanguangping@163.com)
 * memory.c
 */

#include <stdio.h>
#include <stdlib.h>
#include "memory.h"
#include "predicate.h"

void finalize_cell(Scheme *sc, Cell* a) {
	if (is_string(a)) {
		free(a->_string);
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

Cell *alloc_cell() {
	Cell *result = (Cell *) malloc(sizeof(Cell));
	exit_when_out_of_memory(result);
	return result;
}
