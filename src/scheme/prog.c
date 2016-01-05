/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2015 by fanguangping (fanguangping@163.com)
 * prog.c
 */

#include <string.h>
#include "operator.h"

Cell* op_begin(Scheme *sc) {
	//形式 (begin body ...)
	if (!is_pair(sc->code))
		return s_return_helper(sc, sc->code);		        //这里这样的代码可以通过(begin . 1)
	if (cdr(sc->code) != &g_nil) {
		s_save(sc, op_begin, &g_nil, cdr(sc->code));		//保存剩余的未求值的部分
	}
	sc->code = car(sc->code);
	sc->op = op_eval;
	return &g_true;
}

//if的形式 (if condition true_body false_body?) ， false_body可有可无
Cell* op_if0(Scheme *sc) {
	s_save(sc, op_if1, &g_nil, cdr(sc->code));   //这里cdr(sc->code) 结果是(true_body false_body?)
	sc->code = car(sc->code);				     //最后 sc->code 是条件
	sc->op = op_eval;                            //对条件求值
	return &g_true;
}

Cell* op_if1(Scheme *sc) {
	if (is_true(sc->returnValue))
		sc->code = first(sc->code);    //最后sc->code为true_body
	else
		sc->code = second(sc->code);   //最后sc->code为false_body
	sc->op = op_eval;                  //对true_body或者false_body求值
	return &g_true;
}

Cell* op_cond0(Scheme *sc) {
	//形式1 (cond (cond_expr ) ...)
	//形式2 (cond (cond_expr  body) ...)
	//形式3 (cond (cond_expr => body) ...)
	if (!is_pair(sc->code))
		return error_helper(sc, "syntax error in cond", NULL);
	s_save(sc, op_cond1, &g_nil, sc->code);
	sc->code = caar(sc->code);			    //sc->code 变为 cond_expr
	sc->op = op_eval;
	return &g_true;
}

Cell* op_cond1(Scheme *sc) {
	Cell *x;
	if (is_true(sc->returnValue)) {
		if ((sc->code = cdar(sc->code)) == &g_nil)
			return s_return_helper(sc, sc->returnValue);     //形式1   这里 sc->code 被改变了
		if (car(sc->code) == sc->sym_feed_to) {	             //形式3 这里如果通过 sc->code 应当为  (=> body)
			if (!is_pair(cdr(sc->code)))
				return error_helper(sc, "syntax error in cond", NULL);
			//x 为  '(sc->returnValue)  之所以这个样子
			//1 参数要放到列表里面，2 这里body是一个处理过程，这个形式确保sc->returnValue不被进一步求值直接交给body
			//例子 (cond ('(1 2) => list))   结果 ((1 2))
			x = cons(sc, sc->sym_quote, cons(sc, sc->returnValue, &g_nil));
			sc->code = cons(sc, cadr(sc->code), cons(sc, x, &g_nil));
			sc->op = op_eval;
			return &g_true;
		}
		//形式2  如果到这里 sc->code 应当为  (body)
		sc->op = op_begin;
		return &g_true;
	} else {					//循环求值
		if ((sc->code = cdr(sc->code)) == &g_nil) {
			return s_return_helper(sc, &g_nil);
		} else {
			s_save(sc, op_cond1, &g_nil, sc->code);
			sc->code = caar(sc->code);
			sc->op = op_eval;
			return &g_true;
		}
	}
}

Cell* op_case0(Scheme *sc) {
	//形式 (case cond_expr (test_expr body) ...)
	s_save(sc, op_case1, &g_nil, cdr(sc->code));
	sc->code = car(sc->code);					//cond_expr
	//对条件进行求值
	sc->op = op_eval;
	return &g_true;
}

Cell* op_case1(Scheme *sc) {
	Cell *x, *y;
	//sc->code为((test_expr body) ...)
	for (x = sc->code; x != &g_nil; x = cdr(x)) {
		if (!is_pair(y = caar(x))) {
			if (eqv(y, sc->returnValue))
				break;
		}
		for (; y != &g_nil; y = cdr(y)) {
			if (eqv(car(y), sc->returnValue))
				break;
		}
		if (y != &g_nil)
			break;
	}
	if (x != &g_nil) {
		sc->code = cdar(x);					//sc->code 变为 (body)
		sc->op = op_begin;
		return &g_true;
	}
	return s_return_helper(sc, &g_nil);
}


// (force (delay (+ 1 2)))
Cell* op_force(Scheme *sc) {
	sc->code = first(sc->args);
	if (is_promise(sc->code)) {
		/* Should change type to closure here */
		s_save(sc, op_save_forced, &g_nil, sc->code);
		sc->args = &g_nil;
		sc->op = op_apply;
		return &g_true;
	} else
		return s_return_helper(sc, sc->code);
}

Cell* op_save_forced(Scheme *sc) {
	/* Save forced value replacing promise */
	memcpy(sc->code, sc->returnValue, sizeof(Cell));
	return s_return_helper(sc, sc->returnValue);
}
