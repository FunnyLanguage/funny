/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2015 by fanguangping (fanguangping@163.com)
 * func.c
 */

#include <string.h>
#include "operator.h"

Cell* error_helper(Scheme *sc, const char * errorDesc, Cell* arg) {
	//查找释放有处理错误的钩子函数
	Cell* symbol = find_slot_in_env(sc, sc->env, sc->sym_error_hook, TRUE);
	//如果找到了钩子函数，由钩子函数来处理这个错误
	if (symbol != &g_nil) {
		if (arg != NULL) {
			sc->code = cons(sc, cons(sc, sc->sym_quote, cons(sc, arg, &g_nil)),
					&g_nil);
			//结果（用lisp表示）类似这样：(sc->sym_quote arg)
		} else {
			sc->code = &g_nil;
		}
		sc->code = cons(sc, make_string(sc, errorDesc), sc->code);
		//结果（用lisp表示）类似这样：(error_desc sc->sym_quote arg)
		set_immutable(car(sc->code));
		sc->code = cons(sc, slot_value_in_env(symbol), sc->code);
		//结果（用lisp表示）类似这样：(slot_value_in_env(symbol)  error_desc sc->sym_quote arg)
		sc->op = op_eval;
		return &g_true;
	} else {
		if (arg != NULL) {
			sc->args = cons(sc, arg, &g_nil);
		} else {
			sc->args = &g_nil;
		}
		sc->args = cons(sc, make_string(sc, errorDesc), sc->args);
		//结果（用lisp表示）类似这样：(error_desc arg)
		set_immutable(car(sc->args));
		sc->op = op_err;
		return &g_true;
	}
}

void s_save(Scheme *sc, OperatorFunc op, Cell* args, Cell* code) {
	sc->callStack = cons(sc, code, sc->callStack);
	sc->callStack = cons(sc, sc->env, sc->callStack);
	sc->callStack = cons(sc, args, sc->callStack);
	sc->callStack = cons(sc, make_func(sc, op), sc->callStack);
}

Cell* s_return_helper(Scheme *sc, Cell* ret_value) {
	sc->returnValue = ret_value;
	if (sc->callStack == &g_nil)
		return &g_nil;
	sc->op = first(sc->callStack)->_op;
	sc->args = second(sc->callStack);
	sc->env = third(sc->callStack);
	sc->code = fourth(sc->callStack);
	sc->callStack = cddddr(sc->callStack);
	return &g_true;
}

static OperatorFunc syntax2op(Cell* p) {
	const char *s = car(p)->_string;

	if (strcmp(s, "if") == 0) {
		return op_if0;
	} else if (strcmp(s, "or") == 0) {
		return op_or0;
	} else if (strcmp(s, "and") == 0) {
		return op_and0;
	} else if (strcmp(s, "let") == 0) {
		return op_let0;
	} else if (strcmp(s, "case") == 0) {
		return op_case0;
	} else if (strcmp(s, "cond") == 0) {
		return op_cond0;
	} else if (strcmp(s, "let*") == 0) {
		return op_letstar0;
	} else if (strcmp(s, "set!") == 0) {
		return op_set0;
	} else if (strcmp(s, "begin") == 0) {
		return op_begin;
	} else if (strcmp(s, "delay") == 0) {
		return op_delay;
	} else if (strcmp(s, "macro") == 0) {
		return op_macro0;
	} else if (strcmp(s, "quote") == 0) {
		return op_quote;
	} else if (strcmp(s, "lambda") == 0) {
		return op_lambda;
	} else if (strcmp(s, "define") == 0) {
		return op_def0;
	} else if (strcmp(s, "letrec") == 0) {
		return op_let0rec;
	} else if (strcmp(s, "cons-stream") == 0) {
		return op_c0stream;
	}

	return NULL;
}

Cell* op_lambda(Scheme *sc) {
	Cell* hook = find_slot_in_env(sc, sc->env, sc->sym_compile_hook, TRUE);
	if (hook == &g_nil) {
		//这里sc->code为 ((var_name ...) body ...) 或者(var_name expr ...)
		return s_return_helper(sc, make_closure(sc, sc->code, sc->env));
	} else {
		s_save(sc, op_lambda1, sc->args, sc->code);
		sc->args = cons(sc, sc->code, &g_nil);			// 构造参数列表
		sc->code = slot_value_in_env(hook);
		sc->op = op_apply;
		return &g_true;
	}
}

Cell* op_lambda1(Scheme *sc) {
	return s_return_helper(sc, make_closure(sc, sc->returnValue, sc->env));	//定义闭包
}

// (get-closure-code closure)
Cell* op_get_closure(Scheme *sc) {
	sc->args = first(sc->args);
	if (sc->args == &g_nil) {
		return s_return_helper(sc, &g_false);
	} else if (is_closure(sc->args)) {
		return s_return_helper(sc,
				cons(sc, sc->sym_lambda, closure_code(sc->args)));
	} else if (is_macro(sc->args)) {
		return s_return_helper(sc,
				cons(sc, sc->sym_lambda, closure_code(sc->args)));
	} else {
		return s_return_helper(sc, &g_false);
	}
}

Cell* op_def0(Scheme *sc) {
	//(define var_name expr)
	Cell *x;

	if (is_immutable(car(sc->code)))
		return error_helper(sc, "define: unable to alter immutable",
				car(sc->code));
	if (is_pair(car(sc->code))) {
		//形式1 (define (var_name arg ...) body)
		//例子 (define (fname arg1 arg2) (+arg1 arg2))可以通过这个分支 ，最后结果是闭包 ,这个被变换为 (define fname (lambda (arg1 arg2) (+ arg1 arg2))
		//形式2 (define (var_name . arg) body) //x为(var_name arg ...)  被变换为 (define fname (lambda arg body)
		x = caar(sc->code);
		sc->code = cons(sc, sc->sym_lambda,
				cons(sc, cdar(sc->code), cdr(sc->code)));
	} else {
		x = car(sc->code);				//x为var_name
		sc->code = cadr(sc->code);				//sc->code为expr
	}
	if (!is_symbol(x))
		return error_helper(sc, "variable is not a symbol", NULL);
	s_save(sc, op_def1, &g_nil, x);
	sc->op = op_eval;
	return &g_true;
}

Cell* op_def1(Scheme *sc) {
	Cell *x;

	//sc->code为var_name
	x = find_slot_in_env(sc, sc->env, sc->code, FALSE);	//在当前环境中查找这个符号，并不是在全局环境中查找，而且不递归查找
	if (x != &g_nil) {
		set_slot_in_env(sc, x, sc->returnValue);
	} else {
		new_slot_in_env(sc, sc->code, sc->returnValue);
	}
	return s_return_helper(sc, sc->code);				//返回变量名
}

Cell *op_toplevel(Scheme *sc) {
	s_save(sc, op_eval_internal, &g_nil, &g_nil);
	sc->op = op_read_internal;
	return &g_true;
}

Cell* op_eval_internal(Scheme *sc) {
	sc->code = sc->returnValue;
	sc->op = op_eval;
	return &g_true;
}

Cell* op_eval(Scheme *sc) {
	Cell* x;

	if (is_symbol(sc->code)) { /* symbol */ //符号求值为他绑定的值
		x = find_slot_in_env(sc, sc->env, sc->code, TRUE);
		if (x != &g_nil) {
			return s_return_helper(sc, slot_value_in_env(x));
		} else {
			return error_helper(sc, "eval: unbound variable:", sc->code);
		}
	} else if (is_pair(sc->code)) { //链表求值
		x = car(sc->code);
		if (is_syntax(x)) { /* SYNTAX */ //lambda macro if等开头的链表会通过这个分支处理
			sc->code = cdr(sc->code);
			sc->op = syntax2op(x);
			return &g_true;
		} else {
			/* first, eval top element and eval arguments */
			s_save(sc, op_e0args, &g_nil, sc->code);
			/* If no macros => s_save(sc,OP_E1ARGS, sc->NIL, cdr(sc->code));*/
			sc->code = car(sc->code);
			//计算第一个字段，结果是函数 或者宏
			sc->op = op_eval;
			return &g_true;
		}
	} else { //其他类型求值结果就是自身
		return s_return_helper(sc, sc->code);
	}
}


Cell* op_e0args(Scheme *sc) {
	//sc->code的形式 (first_expr arg ...) 这里first_expr的求值结果可能是宏也可能是闭包
	if (is_macro(sc->returnValue)) { /* macro expansion */
		//如果结果是一个宏，那么参数不要进一步的处理，而是交给这个宏去处理
		s_save(sc, op_domacro, &g_nil, &g_nil); //由这个去执行宏生成的代码
		sc->args = cons(sc, sc->code, &g_nil); //sc->args 的形式是 (first_expr arg ...)  注意first_expr也保留了
		sc->code = sc->returnValue;
		//这里把参数交给宏去处理
		sc->op = op_apply;
		return &g_true;
	} else {
		sc->code = cdr(sc->code); //(arg ...)
		sc->op = op_e1args;
		return &g_true;
	}
}

Cell* op_e1args(Scheme *sc) {
	sc->args = cons(sc, sc->returnValue, sc->args);
	if (is_pair(sc->code)) { /* continue */ //对每个实参求值
		s_save(sc, op_e1args, sc->args, cdr(sc->code));
		sc->code = car(sc->code);
		sc->args = &g_nil;
		sc->op = op_eval;
		return &g_true;
	} else { /* end */ // 实参的值求完了 ，用这些实参调用函数
		sc->args = reverse(sc, sc->args);
		sc->code = car(sc->args);
		sc->args = cdr(sc->args);
		sc->op = op_apply;
		return &g_true;
	}
}

Cell* op_apply(Scheme *sc) {
	Cell *x, *y;

	if (is_proc(sc->code)) {/* PROCEDURE */
		sc->op = (sc->code)->_op;
		return &g_true;
	} else if (is_foreign(sc->code)) {
		/* Keep nested calls from GC'ing the arglist */
		//sc->code的形式foreign_proc
		x = sc->code->_func(sc, sc->args);
		return s_return_helper(sc, x);
	} else if (is_closure(sc->code) || is_macro(sc->code)
			|| is_promise(sc->code)) { /* CLOSURE */
		/* Should not accept promise */
		//sc->code的形式1 (var_name body ...)
		//sc->code的形式2 ( (var_name ...) body ...)
		//sc->code的形式3 ( (var_name ...   . var_name) body ...)
		new_frame_in_env(sc, closure_env(sc->code));				//局部变量环境
		x = car(closure_code(sc->code));
		for (y = sc->args; is_pair(x); x = cdr(x), y = cdr(y)) {//sc->code的形式2 3
			if (y == &g_nil)
				return error_helper(sc, "not enough arguments", NULL);
			else
				new_slot_in_env(sc, car(x), car(y));				//绑定函数形参
		}
		if (is_symbol(x))
			new_slot_in_env(sc, x, y);				//sc->code的形式1 3
		else if (x != &g_nil)
			return error_helper(sc, "syntax error in closure: not a symbol:", x);
		sc->code = cdr(closure_code(sc->code));
		sc->args = &g_nil;
		sc->op = op_begin;
		return &g_true;
	} else if (is_continuation(sc->code)) { /* CONTINUATION */
		sc->callStack = cdr(sc->code);
		return s_return_helper(sc, sc->args != &g_nil ? car(sc->args) : &g_nil);
	} else {
		return error_helper(sc, "illegal function", NULL);
	}
}

// 形式  (delay (expr ...))
Cell* op_delay(Scheme *sc) {
	Cell *x;
	//delay就是把表达式转化为一个无参数的闭包
	//例子 (define f (delay (+ 1 1))) (f)
	//例子 (define f (delay 1)) (f)
	x = make_closure(sc, cons(sc, &g_nil, sc->code), sc->env);
	(x)->_flag = T_PROMISE;
	return s_return_helper(sc, x);
}

// (apply + '(1 2 3))
Cell* op_papply(Scheme *sc) {
	sc->code = first(sc->args);
	sc->args = second(sc->args);
	sc->op = op_apply;
	return &g_true;
}

// (eval code env)
Cell* op_peval(Scheme *sc) {
	if (rest(sc->args) != &g_nil)
		sc->env = second(sc->args);   //设置环境
	sc->code = first(sc->args);       //要执行的代码
	sc->op = op_eval;
	return &g_true;
}

/* call-with-current-continuation */
//将当前堆栈作为参数传给 作为过程的call/cc的参数
Cell* op_continuation(Scheme *sc) {
	sc->code = car(sc->args);
	sc->args = cons(sc, make_continuation(sc, sc->callStack), &g_nil);
	sc->op = op_apply;
	return &g_true;
}

// (quit)
Cell* op_quit(Scheme *sc) {
	return &g_nil;
}

// (gc)
Cell* op_gc(Scheme *sc) {
	gc(sc, &g_nil, &g_nil);
	return s_return_helper(sc, &g_true);
}

// (gc-verbose #t)
// (gc-verbose)
Cell* op_gcverb(Scheme *sc) {
	//int was = sc->gc_verbose;
	//sc->gc_verbose = (car(sc->args) != &g_false);
	//return s_return_helper(sc, (was) ? &g_true : &g_false);
	return &g_true;
}
