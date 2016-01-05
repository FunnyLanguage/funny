/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2015 by fanguangping (fanguangping@163.com)
 * list.c
 */

#include "operator.h"

//将链表反序
Cell* reverse(Scheme *sc, Cell* old_list) {
	Cell* new_list = &g_nil;
	for (; is_pair(old_list); old_list = cdr(old_list)) {
		new_list = cons(sc, car(old_list), new_list);
	}
	return new_list;
}

//list* ,例子 (list* 1 2 3) => (1  2 . 3)
Cell* list_star(Scheme *sc, Cell* old_list) {
	old_list = reverse(sc, old_list);
	Cell* new_list = car(old_list);
	old_list = cdr(old_list);
	while (is_pair(old_list)) {
		new_list = cons(sc, car(old_list), new_list);
		old_list = cdr(old_list);
	}
	return new_list;
}

/* append list -- produce new list (in reverse order) */
//如果acc 为 (4 3) ,  list 为 (5 6) , 则最后返回结果应当为 (6 5 4 3)
Cell* revappend(Scheme *sc, Cell* acc, Cell* list) {
	while (is_pair(list)) {
		acc = cons(sc, car(list), acc);
		list = cdr(list);
	}
	if (list == &g_nil)
		return acc;
	return &g_false; /* signal an error */
}

/* Result is:
 proper list: length
 circular list: -1			//环行链表
 not even a pair: -2	//如果是这样的形式(a . b) （b不是pair）
 dotted list: -2 minus length before dot	//pair 链表的最后一个cell不是nil
 */
int ls_length(Scheme *sc, Cell* a) {
	int len = 0;
	Cell* slow = a;
	Cell* fast = a;
	while (TRUE) {
		if (fast == &g_nil)
			return len;
		if (!is_pair(fast))
			return -2 - len;
		fast = cdr(fast);
		++len;
		if (fast == &g_nil)
			return len;
		if (!is_pair(fast))
			return -2 - len;
		fast = cdr(fast);
		++len;
		slow = cdr(slow);
		//如果a中有一个环 fast 和slow都会最终进入这个环中，并且有于fast是slow的速度的两倍，索引fast最终能赶上slow
		if (fast == slow)
			return -1;
	}
}

int is_list(Scheme *sc, Cell* a) {
	return ls_length(sc, a) >= 0;
}


// (car x)
Cell* op_car(Scheme *sc) {
	return s_return_helper(sc, caar(sc->args));
}

// (cdr x)
Cell* op_cdr(Scheme *sc) {
	return s_return_helper(sc, cdar(sc->args));
}

// (cons x y) => (x . y) ：x不变，y=car(y)
Cell* op_cons(Scheme *sc) {
	CDR(sc->args) = cadr(sc->args);
	return s_return_helper(sc, sc->args);
}

// (set-car! x y)
Cell* op_setcar(Scheme *sc) {
	if (!is_immutable(car(sc->args))) {
		CAAR(sc->args) = cadr(sc->args);
		return s_return_helper(sc, car(sc->args));
	} else {
		return error_helper(sc, "set-car!: unable to alter immutable pair",
		NULL);
	}
}

// (set-cdr! x y)
Cell* op_setcdr(Scheme *sc) {
	if (!is_immutable(car(sc->args))) {
		CDAR(sc->args) = cadr(sc->args);
		return s_return_helper(sc, car(sc->args));
	} else {
		return error_helper(sc, "set-cdr!: unable to alter immutable pair",
		NULL);
	}
}


// (reverse LIST)
Cell* op_reverse(Scheme *sc) {
	return s_return_helper(sc, reverse(sc, first(sc->args)));
}

// (list* 1 2 3) => (1  2 . 3)
Cell* op_list_star(Scheme *sc) {
	return s_return_helper(sc, list_star(sc, sc->args));
}

// (append LIST LIST)
Cell* op_append(Scheme *sc) {
	Cell* x, *y;
	x = &g_nil;
	y = sc->args;
	while (y != &g_nil) {
		x = revappend(sc, x, car(y));
		y = cdr(y);
		if (x == &g_false) {
			return error_helper(sc, "non-list argument to append", NULL);
		}
	}
	return s_return_helper(sc, reverse(sc, x));
}


// (length LIST)
Cell* op_list_length(Scheme *sc) {
	long len = ls_length(sc, first(sc->args));
	if (len < 0)
		return error_helper(sc, "length: not a list:", car(sc->args));
	return s_return_helper(sc, make_integer(sc, len));
}

// (assq object alist)
// (define e '((a 1) (b 2) (c 3)))
// (assq 'a e)  =>  (a 1)
Cell* op_assq(Scheme *sc) {
	Cell* x, *y;
	x = car(sc->args);
	for (y = cadr(sc->args); is_pair(y); y = cdr(y)) {
		if (!is_pair(car(y)))
			return error_helper(sc, "unable to handle non pair element", NULL);
		if (x == caar(y))
			break;
	}
	if (is_pair(y))
		return s_return_helper(sc, car(y));
	else
		return s_return_helper(sc, &g_false);
}



// (cons-stream expr-arg expr-code)
// 例子 (cons-stream 1 1)   => (1 . #<PROMISE>)
Cell* op_c0stream(Scheme *sc) {
	s_save(sc, op_c1stream, &g_nil, cdr(sc->code));
	sc->code = car(sc->code);
	sc->op = op_eval;
	return &g_true;
}

Cell* op_c1stream(Scheme *sc) {
	Cell *x;
	sc->args = sc->returnValue; /* save sc->value to register sc->args for gc */
	x = make_closure(sc, cons(sc, &g_nil, sc->code), sc->env);
	(x)->_flag = T_PROMISE;
	return s_return_helper(sc, cons(sc, sc->args, x));
}

Cell* op_quote(Scheme *sc) {
	return s_return_helper(sc, car(sc->code));
}

