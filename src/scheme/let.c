/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2015 by fanguangping (fanguangping@163.com)
 * let.c
 */

#include "operator.h"

//let的形式 (let let_name? ((var_name init_expr) ...) body_expr ...) 。let_name字段可有可无
Cell* op_let0(Scheme *sc) {
	sc->args = &g_nil;
	sc->returnValue = sc->code;	//sc->returnValue形式 (let_name? ((var_name init_expr) ...) body_expr ...)
	//let分为命名的let 和未命名的let
	sc->code = is_symbol(first(sc->code)) ? second(sc->code) : first(sc->code); //sc->code形式 ((var_name init_expr) ...)
	sc->op = op_let1;
	return &g_true;
}

Cell* op_let1(Scheme *sc) {
	sc->args = cons(sc, sc->returnValue, sc->args); //这里的结果为 (init_result ...  (let_name? ((var_name init_expr) ...) body_expr ...) )
	if (is_pair(sc->code)) { /* continue */
		if (!is_pair(car(sc->code)) || !is_pair(cdar(sc->code))) {
			return error_helper(sc, "Bad syntax of binding spec in let :",
					car(sc->code));
		}
		s_save(sc, op_let1, sc->args, cdr(sc->code));
		sc->code = cadar(sc->code);  //sc->code形式 init_expr
		sc->args = &g_nil;
		//对init_expr求值 ，假设其返回值为 init_result
		sc->op = op_eval;
		return &g_true;
	} else { /* end */
		sc->args = reverse(sc, sc->args); //反序 结果为  ( (let_name? ((var_name init_expr) ...) body_expr ...) init_result ... )
		sc->code = car(sc->args); //结果为(let_name? ((var_name init_expr) ...) body_expr ...)
		sc->args = cdr(sc->args);  //结果为(init_result ... )
		sc->op = op_let2;
		return &g_true;
	}
}

Cell* op_let2(Scheme *sc) {
	Cell *x, *y;

	new_frame_in_env(sc, sc->env);  //为let创建一个新的环境
	x = is_symbol(car(sc->code)) ? cadr(sc->code) : car(sc->code); //如果第一个节点就是符号，这个符号是let_name，要跳过 ，最后x的结果为((var_name init_expr) ...)
	for (y = sc->args; y != &g_nil; x = cdr(x), y = cdr(y)) { //y为(init_result ...)
		new_slot_in_env(sc, caar(x), car(y)); //caar(x)为var_name , y为 init_result
	}
	if (is_symbol(car(sc->code))) { /* named let */ //命名let
		//这里将命名let转化为一个闭包，这里没有尾递归优化
		for (x = cadr(sc->code), sc->args = &g_nil; x != &g_nil; x = cdr(x)) {//x为((var_name init_expr) ...)
			//这个循环的目的就是吧var_name都抽取出来
			if (!is_pair(x))
				return error_helper(sc, "Bad syntax of binding in let :", x);
			if (!is_list(sc, car(x)))
				return error_helper(sc, "Bad syntax of binding in let :",
						car(x));
			sc->args = cons(sc, caar(x), sc->args);	//sc->args为(var_name ...) ,这里var_name的顺序和sc->code中对应的var_name的顺序是相反的
		}
		x = make_closure(sc, cons(sc, reverse(sc, sc->args), cddr(sc->code)),
				sc->env);					//创建闭包
		new_slot_in_env(sc, car(sc->code), x);			//为新创建的闭包定义一个名字
		sc->code = cddr(sc->code);//跳过let_name和var_name初始化部分 ，最后 sc->code 是 ( body_expr ...)
		sc->args = &g_nil;
	} else {
		sc->code = cdr(sc->code);//跳过var_name初始化部分 ，最后 sc->code 是 ( body_expr ...)
		sc->args = &g_nil;
	}
	//对( body_expr ...)部分进行求值
	sc->op = op_begin;
	return &g_true;
}

Cell* op_letstar0(Scheme *sc) {
	//let*的形式是这样的 (let* ((var_name init_expr) ...) body_expr ...)
	if (car(sc->code) == &g_nil) {
		new_frame_in_env(sc, sc->env);
		sc->code = cdr(sc->code);
		sc->op = op_begin;
		return &g_true;
	}
	if (!is_pair(car(sc->code)) || !is_pair(caar(sc->code))
			|| !is_pair(cdaar(sc->code))) {
		return error_helper(sc, "Bad syntax of binding spec in let* :",
				car(sc->code));
	}
	s_save(sc, op_letstar1, cdr(sc->code), car(sc->code));// cdr(sc->code)为 (body_expr ...)  。 car(sc->code)为 ((var_name init_expr) ...)
	sc->code = cadaar(sc->code);//sc->code最后为init_expr 这个代码要使用let*外层的环境（特别的一个情形是，如果这是个lambda表达式，就会绑定到外层的环境上）
	sc->op = op_eval;
	return &g_true;			//对第一个var_name的初始化表达式求值 ，求值的结果假设为init_result
}

Cell* op_letstar1(Scheme *sc) {
	new_frame_in_env(sc, sc->env);					//为let*创建一个环境
	sc->op = op_letstar2;
	return &g_true;
}

Cell* op_letstar2(Scheme *sc) {
	// sc->args为 (body_expr ...)  。 sc->code为 ((var_name init_expr) ...)
	new_slot_in_env(sc, caar(sc->code), sc->returnValue);// caar(sc->code)为var_name  sc->returnValue为init_result
	sc->code = cdr(sc->code);
	if (is_pair(sc->code)) { /* continue */
		s_save(sc, op_letstar2, sc->args, sc->code);
		sc->code = cadar(sc->code);					//sc->code最后为init_expr
		sc->args = &g_nil;
		//对init_expr求值
		sc->op = op_eval;
		return &g_true;
	} else { /* end */
		sc->code = sc->args; //sc->code最后为(body_expr ...)
		sc->args = &g_nil;
		//对(body_expr ...)求值
		sc->op = op_begin;
		return &g_true;
	}
}


Cell* op_let0rec(Scheme *sc) {
	//形式 (letrec ((var_name init_expr) ... ) body)
	new_frame_in_env(sc, sc->env);
	sc->args = &g_nil;
	sc->returnValue = sc->code;
	sc->code = car(sc->code);
	sc->op = op_let1rec;
	return &g_true;
}

Cell* op_let1rec(Scheme *sc) {
	sc->args = cons(sc, sc->returnValue, sc->args);
	if (is_pair(sc->code)) { /* continue */
		if (!is_pair(car(sc->code)) || !is_pair(cdar(sc->code))) {
			return error_helper(sc, "Bad syntax of binding spec in letrec :",
					car(sc->code));
		}
		s_save(sc, op_let1rec, sc->args, cdr(sc->code));
		sc->code = cadar(sc->code);
		sc->args = &g_nil;
		sc->op = op_eval;
		return &g_true;
	} else { /* end */
		sc->args = reverse(sc, sc->args);
		sc->code = car(sc->args);
		sc->args = cdr(sc->args);
		sc->op = op_let2rec;
		return &g_true;
	}
}

Cell* op_let2rec(Scheme *sc) {
	Cell *x, *y;

	for (x = car(sc->code), y = sc->args; y != &g_nil; x = cdr(x), y = cdr(y)) {
		new_slot_in_env(sc, caar(x), car(y));
	}
	sc->code = cdr(sc->code);
	sc->args = &g_nil;
	sc->op = op_begin;
	return &g_true;
}
