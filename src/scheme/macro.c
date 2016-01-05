/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2015 by fanguangping (fanguangping@163.com)
 * macro.c
 */

#include "operator.h"

Cell* op_domacro(Scheme *sc) {
	sc->code = sc->returnValue;
	sc->op = op_eval;
	return &g_true;
}

//形式1 (macro (macro-name var-name ...) body ...)
//形式2 (macro macro-name (lambda ....))
Cell* op_macro0(Scheme *sc) {
	Cell *x;
	if (is_pair(car(sc->code))) {			                //处理形式1
		x = caar(sc->code);			                        //macro-name
		sc->code = cons(sc, sc->sym_lambda,
				cons(sc, cdar(sc->code), cdr(sc->code)));	//lambda表达式 （宏代码）
	} else {			                                    //处理形式2
		x = car(sc->code);			                        //macro-name
		sc->code = cadr(sc->code);			                //lambda表达式 （宏代码）
	}
	if (!is_symbol(x))
		return error_helper(sc, "variable is not a symbol", NULL);
	s_save(sc, op_macro1, &g_nil, x);
	sc->op = op_eval;                                       //为这个宏生成一个闭包
	return &g_true;
}

Cell* op_macro1(Scheme *sc) {
	Cell *x;
	(sc->returnValue)->_flag = T_MACRO;		                //sc->returnValue是一个闭包，这里吧类型改为宏
	x = find_slot_in_env(sc, sc->env, sc->code, FALSE);
	if (x != &g_nil)
		set_slot_in_env(sc, x, sc->returnValue);			//将宏和他的名字关联起来
	else
		new_slot_in_env(sc, sc->code, sc->returnValue);
	return s_return_helper(sc, sc->code);
}
