/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2014 by fanguangping (fanguangping@163.com)
 *
 * Funny is based on TinyScheme which is now maintained by Dimitrios Souflis (dsouflis@acm.org)
 * This is a revised and modified version of TinyScheme
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "basic.h"
#include "memory.h"
#include "command.h"
#include "operator.h"

#define STR_BUFF_SIZE 1024

static Num g_zero = { .isFix = 1, {.longValue = 0L} };
static Num g_one = { .isFix = 1, {.longValue = 1L} };

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
		sc->op = op_err0;
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

OperatorFunc syntax2op(Cell* p) {
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

/*
 * operators
 *
 * */

Cell *op_load(Scheme *sc) {
	return NULL;
}
Cell *op_toplevel(Scheme *sc) {
	s_save(sc, op_value_print, &g_nil, &g_nil);
	s_save(sc, op_eval_internal, &g_nil, &g_nil);
	sc->op = op_read_internal;
	return &g_true;
}

Cell* op_read_internal(Scheme *sc) {
	sc->token = token(sc);
	if (sc->token == TOK_EOF)
		return s_return_helper(sc, sc->returnValue); //到这里，表示端口中的代码被读完了，应当返回nil
	sc->op = op_read_sexpr;
	return &g_true;
}

Cell* op_eval_internal(Scheme *sc) {
	sc->code = sc->returnValue;
	//sc->inPort = sc->save_inport;
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

Cell* op_domacro(Scheme *sc) {
	sc->code = sc->returnValue;
	sc->op = op_eval;
	return &g_true;
}

Cell* op_quote(Scheme *sc) {
	return s_return_helper(sc, car(sc->code));
}

Cell* op_lambda(Scheme *sc) {
	Cell* hook = find_slot_in_env(sc, sc->env, sc->sym_compile_hook, 1);
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

// (make-closure closure)
Cell* op_make_closure(Scheme *sc) {
	Cell *x, *y;

	//形式1 (make-clourse '(lambda? var-name body ...) envir?)      lambda 和 envir是可有可无的
	//形式2 (make-clourse '(lambda? (var-name ...) body ...) envir?)      lambda 和 envir是可有可无的
	//例子 (define f (make-closure '(plus ((car plus) 3 4))))  结果为7
	x = car(sc->args);
	if (car(x) == sc->sym_lambda)
		x = cdr(x);
	if (cdr(sc->args) == &g_nil)
		y = sc->env;
	else
		y = cadr(sc->args);
	return s_return_helper(sc, make_closure(sc, x, y));
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

// (defined? var_name)
// (defined? var_name env)
Cell* op_defp(Scheme *sc) {
	Cell *env;
	if (rest(sc->args) != &g_nil)
		env = second(sc->args);
	else
		env = sc->env;
	int istrue = (find_slot_in_env(sc, env, first(sc->args), TRUE) != &g_nil);
	return s_return_helper(sc, (istrue) ? &g_true : &g_false);
}

Cell* op_set0(Scheme *sc) {
	//形式 (set! var_name expr)
	if (is_immutable(car(sc->code)))
		return error_helper(sc, "set!: unable to alter immutable variable",
				car(sc->code));
	s_save(sc, op_set1, &g_nil, car(sc->code));
	sc->code = cadr(sc->code);				//sc->code结果为expr
	//对expr进行求值
	sc->op = op_eval;
	return &g_true;
}

Cell* op_set1(Scheme *sc) {
	Cell *y;

	//sc->code为 var_name
	y = find_slot_in_env(sc, sc->env, sc->code, 1);
	if (y != &g_nil) {
		set_slot_in_env(sc, y, sc->returnValue);
		return s_return_helper(sc, sc->returnValue);
	} else {
		return error_helper(sc, "set!: unbound variable:", sc->code);
	}
}

Cell* op_begin(Scheme *sc) {
	//形式 (begin body ...)
	if (!is_pair(sc->code))
		return s_return_helper(sc, sc->code);		//?? 这里这样的代码可以通过(begin . 1)
	if (cdr(sc->code) != &g_nil) {
		s_save(sc, op_begin, &g_nil, cdr(sc->code));		//保存剩余的未求值的部分
	}
	sc->code = car(sc->code);
	sc->op = op_eval;
	return &g_true;
}

//if的形式 (if condition true_body false_body?) ， false_body可有可无
Cell* op_if0(Scheme *sc) {
	s_save(sc, op_if1, &g_nil, cdr(sc->code));//这里cdr(sc->code) 结果是(true_body false_body?)
	sc->code = car(sc->code);				//最后 sc->code 是条件
	//对条件求值
	sc->op = op_eval;
	return &g_true;
}

Cell* op_if1(Scheme *sc) {
	if (is_true(sc->returnValue))
		sc->code = first(sc->code);    //最后sc->code为true_body
	else
		sc->code = second(sc->code);   //最后sc->code为false_body
	//对true_body或者false_body求值
	sc->op = op_eval;
	return &g_true;
}

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

Cell* op_value_print(Scheme *sc) {
	//if (file_interactive(sc)) { //如果在最高层求值环境中，则打印结果
		//sc->print_flag = 1;
		sc->args = sc->returnValue;
		sc->op = op_p0list;
		return &g_true;
	//} else {
	//	return s_return_helper(sc, sc->returnValue); //否则作为load的结果返回
	//}
}

// (gensym)
Cell* op_gensym(Scheme *sc) {
	return s_return_helper(sc, gensym(sc));
}

//default print error
Cell* op_default(Scheme *sc) {
	//_snprintf(sc->strbuff, STR_BUFF_SIZE, "%d: illegal operator", sc->op);
	//Error0(sc, sc->strbuff);
	return error_helper(sc, "illegal operator", NULL);
	//return &g_true;
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

Cell* op_cond0(Scheme *sc) {
	//形式1 (cond (cond_expr ) ...)
	//形式2 (cond (cond_expr  body) ...)
	//形式3 (cond (cond_expr => body) ...)
	if (!is_pair(sc->code))
		return error_helper(sc, "syntax error in cond", NULL);
	s_save(sc, op_cond1, &g_nil, sc->code);
	sc->code = caar(sc->code);			//sc->code 变为 cond_expr
	sc->op = op_eval;
	return &g_true;
}

Cell* op_cond1(Scheme *sc) {
	Cell *x;

	if (is_true(sc->returnValue)) {
		if ((sc->code = cdar(sc->code)) == &g_nil)
			return s_return_helper(sc, sc->returnValue);//形式1   这里 sc->code 被改变了
		if (car(sc->code) == sc->sym_feed_to) {	//形式3 这里如果通过 sc->code 应当为  (=> body)
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

Cell* op_and0(Scheme *sc) {
	if (sc->code == &g_nil)
		return s_return_helper(sc,&g_true);
	s_save(sc, op_and1, &g_nil, cdr(sc->code));
	sc->code = car(sc->code);
	sc->op = op_eval;
	return &g_true;
}

Cell* op_and1(Scheme *sc) {
	if (is_false(sc->returnValue))
		return s_return_helper(sc, sc->returnValue);
	else if (sc->code == &g_nil)
		return s_return_helper(sc, sc->returnValue);
	else {
		s_save(sc, op_and1, &g_nil, cdr(sc->code));
		sc->code = car(sc->code);
		sc->op = op_eval;
		return &g_true;
	}
}

Cell* op_or0(Scheme *sc) {
	if (sc->code == &g_nil)
		return s_return_helper(sc,&g_false);
	s_save(sc, op_or1, &g_nil, cdr(sc->code));
	sc->code = car(sc->code);
	sc->op = op_eval;
	return &g_true;
}

Cell* op_or1(Scheme *sc) {
	if (is_true(sc->returnValue))
		return s_return_helper(sc, sc->returnValue);
	else if (sc->code == &g_nil)
		return s_return_helper(sc, sc->returnValue);
	else {
		s_save(sc, op_or1, &g_nil, cdr(sc->code));
		sc->code = car(sc->code);
		sc->op = op_eval;
		return &g_true;
	}
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

Cell* op_macro0(Scheme *sc) {
	Cell *x;
	//形式1 (macro (macro-name var-name ...) body ...)
	//形式2 (macro macro-name (lambda ....))
	if (is_pair(car(sc->code))) {			//处理形式1
		x = caar(sc->code);			//macro-name
		sc->code = cons(sc, sc->sym_lambda,
				cons(sc, cdar(sc->code), cdr(sc->code)));	//lambda表达式 （宏代码）
	} else {			//处理形式2
		x = car(sc->code);			//macro-name
		sc->code = cadr(sc->code);			//lambda表达式 （宏代码）
	}
	if (!is_symbol(x))
		return error_helper(sc, "variable is not a symbol", NULL);
	s_save(sc, op_macro1, &g_nil, x);
	//为这个宏生成一个闭包
	sc->op = op_eval;
	return &g_true;
}

Cell* op_macro1(Scheme *sc) {
	Cell *x;

	(sc->returnValue)->_flag = T_MACRO;		//sc->returnValue是一个闭包，这里吧类型改为宏
	x = find_slot_in_env(sc, sc->env, sc->code, 0);
	if (x != &g_nil)
		set_slot_in_env(sc, x, sc->returnValue);			//将宏和他的名字关联起来
	else
		new_slot_in_env(sc, sc->code, sc->returnValue);
	return s_return_helper(sc, sc->code);
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

//将一个数字（整型或浮点型）无损的转化为整型
// (inexact->exact x)
Cell* op_inexact2exact(Scheme *sc) {
	Cell* num;
	double dd;
	num = first(sc->args);
	if (num->_num.isFix) {
		return s_return_helper(sc, num);
	} else if (modf(num->_num.doubleValue, &dd) == 0.0) {
		return s_return_helper(sc, make_integer(sc, long_value(num)));
	} else {
		return error_helper(sc, "inexact->exact: not integral:", num);
	}
}

// (exp x)
Cell* op_exp(Scheme *sc) {
	Cell* num = first(sc->args);
	return s_return_helper(sc, make_real(sc, exp(double_value(num))));
}

// (log x)
Cell* op_log(Scheme *sc) {
	Cell* num = first(sc->args);
	return s_return_helper(sc, make_real(sc, log(double_value(num))));
}

// (sin x)
Cell* op_sin(Scheme *sc) {
	Cell* num = first(sc->args);
	return s_return_helper(sc, make_real(sc, sin(double_value(num))));
}

// (cos x)
Cell* op_cos(Scheme *sc) {
	Cell* num = first(sc->args);
	return s_return_helper(sc, make_real(sc, cos(double_value(num))));
}

// (tan x)
Cell* op_tan(Scheme *sc) {
	Cell* num = first(sc->args);
	return s_return_helper(sc, make_real(sc, tan(double_value(num))));
}

// (asin x)
Cell* op_asin(Scheme *sc) {
	Cell* num = first(sc->args);
	return s_return_helper(sc, make_real(sc, asin(double_value(num))));
}

// (acos x)
Cell* op_acos(Scheme *sc) {
	Cell* num = first(sc->args);
	return s_return_helper(sc, make_real(sc, acos(double_value(num))));
}

// (atan x)
// (atan x y)
Cell* op_atan(Scheme *sc) {
	Cell* x = first(sc->args);
	if (rest(sc->args) == &g_nil) {
		return s_return_helper(sc, make_real(sc, atan(double_value(x))));
	} else {
		Cell* y = second(sc->args);
		return s_return_helper(sc,
				make_real(sc, atan2(double_value(x), double_value(y))));
	}
}

// (sqrt x)
Cell* op_sqrt(Scheme *sc) {
	Cell* num = first(sc->args);
	return s_return_helper(sc, make_real(sc, sqrt(double_value(num))));
}

// (expt x y)
Cell* op_expt(Scheme *sc) {
	Cell* x = first(sc->args);
	Cell* y = second(sc->args);

	double result;
	int real_result = TRUE;

	if (x->_num.isFix && y->_num.isFix)
		real_result = FALSE;
	/* This 'if' is an R5RS compatibility fix. */
	/* NOTE: Remove this 'if' fix for R6RS.    */
	if (double_value(x) == 0 && double_value(y) < 0) {
		result = 0.0;
	} else {
		result = pow(double_value(x), double_value(y));
	}
	/* Before returning integer result make sure we can. */
	/* If the test fails, result is too big for integer. */
	if (!real_result) {
		long result_as_long = (long) result; //如果result有小数位，必然导致result_as_long和result不相等
		if (result != (double) result_as_long)
			real_result = TRUE;
	}
	if (real_result) {
		return s_return_helper(sc, make_real(sc, result));
	} else {
		return s_return_helper(sc, make_integer(sc, (long) result));
	}
}

// (floor x)
Cell* op_floor(Scheme *sc) {
	Cell* num = first(sc->args);
	return s_return_helper(sc, make_real(sc, floor(double_value(num))));
}

// (ceiling x)
Cell* op_ceiling(Scheme *sc) {
	Cell* num = first(sc->args);
	return s_return_helper(sc, make_real(sc, ceil(double_value(num))));
}

// (truncate x)
Cell* op_truncate(Scheme *sc) {
	Cell* num = first(sc->args);

	double doubleValue = double_value(num);
	if (doubleValue > 0) {
		return s_return_helper(sc, make_real(sc, floor(doubleValue)));
	} else {
		return s_return_helper(sc, make_real(sc, ceil(doubleValue)));
	}
}

// (round x)
Cell* op_round(Scheme *sc) {
	Cell* num = first(sc->args);

	if (num->_num.isFix)
		return s_return_helper(sc, num);
	return s_return_helper(sc, make_real(sc, round_per_r5rs(double_value(num))));
}

// (+ x y ...)
Cell* op_add(Scheme *sc) {
	Num value = g_zero;

	Cell* x;
	for (x = sc->args; x != &g_nil; x = cdr(x)) {
		value = num_add(value, num_value(car(x)));
	}
	return s_return_helper(sc, make_number(sc, value));
}

// (* x y ...)
Cell* op_mul(Scheme *sc) {
	Num value = g_one;

	Cell* x;
	for (x = sc->args; x != &g_nil; x = cdr(x)) {
		value = num_mul(value, num_value(car(x)));
	}
	return s_return_helper(sc, make_number(sc, value));
}

// (- x y ...)
Cell* op_sub(Scheme *sc) {
	Num value;
	Cell* x;
	if (cdr(sc->args) == &g_nil) {
		x = sc->args;
		value = g_zero;
	} else {
		x = cdr(sc->args);
		value = num_value(car(sc->args));
	}
	for (; x != &g_nil; x = cdr(x)) {
		value = num_sub(value, num_value(car(x)));
	}
	return s_return_helper(sc, make_number(sc, value));
}

// (/ x y ...)
Cell* op_div(Scheme *sc) {
	Num value;
	Cell* x;
	if (cdr(sc->args) == &g_nil) {
		x = sc->args;
		value = g_one;
	} else {
		x = cdr(sc->args);
		value = num_value(car(sc->args));
	}
	for (; x != &g_nil; x = cdr(x)) {
		if (!is_zero_double(double_value(car(x))))
			value = num_div(value, num_value(car(x)));
		else {
			return error_helper(sc, "/: division by zero", NULL);
		}
	}
	return s_return_helper(sc, make_number(sc, value));
}

// (quotient x y ...)
Cell* op_intdiv(Scheme *sc) {
	Num value;
	Cell* x;
	if (cdr(sc->args) == &g_nil) {
		x = sc->args;
		value = g_one;
	} else {
		x = cdr(sc->args);
		value = num_value(car(sc->args));
	}
	for (; x != &g_nil; x = cdr(x)) {
		if (long_value(car(x)) != 0)
			value = num_intdiv(value, num_value(car(x)));
		else {
			return error_helper(sc, "quotient: division by zero", NULL);
		}
	}
	return s_return_helper(sc, make_number(sc, value));
}

// (remainder x y)
Cell* op_rem(Scheme *sc) {
	Cell* x = first(sc->args);
	Cell* y = second(sc->args);

	Num value = num_value(x);
	if (long_value(y) != 0) {
		value = num_rem(value, num_value(y));
		return s_return_helper(sc, make_number(sc, value));
	} else {
		return error_helper(sc, "remainder: division by zero", NULL);
	}
}

// (modulo x y)
Cell* op_mod(Scheme *sc) {
	Cell* x = first(sc->args);
	Cell* y = second(sc->args);

	Num value = num_value(x);
	if (long_value(y) != 0) {
		value = num_mod(value, num_value(y));
		return s_return_helper(sc, make_number(sc, value));
	} else {
		return error_helper(sc, "modulo: division by zero", NULL);
	}
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

// (char->integer x)
Cell* op_char2int(Scheme *sc) {
	char c = (char) long_value(first(sc->args));
	return s_return_helper(sc, make_integer(sc, (unsigned char) c));
}

// (integer->char x)
Cell* op_int2char(Scheme *sc) {
	unsigned char c = (unsigned char) long_value(first(sc->args));
	return s_return_helper(sc, make_character(sc, (char) c));
}

// (char-upcase x)
Cell* op_char_upcase(Scheme *sc) {
	unsigned char c = (unsigned char) long_value(first(sc->args));
	c = toupper(c);
	return s_return_helper(sc, make_character(sc, (char) c));
}

// (char-downcase x)
Cell* op_char_downcase(Scheme *sc) {
	unsigned char c = (unsigned char) long_value(first(sc->args));
	c = tolower(c);
	return s_return_helper(sc, make_character(sc, (char) c));
}

// (string->symbol x)
Cell* op_str2sym(Scheme *sc) {
	return s_return_helper(sc, make_symbol(sc, first(sc->args)->_string));
}

// (string->atom str)
// (string->atom numstr base)
Cell* op_str2atom(Scheme *sc) {
	char *s = first(sc->args)->_string;
	long print_flag = 0;
	if (rest(sc->args) != &g_nil) {
		/* we know cadr(sc->args) is a natural number */
		/* see if it is 2, 8, 10, or 16, or error */
		print_flag = long_value_unchecked(second(sc->args));
		if (print_flag == 16 || print_flag == 10 || print_flag == 8
				|| print_flag == 2) {
			/* base is OK */
		} else {
			print_flag = -1;
		}
	}
	if (print_flag < 0) {
		return error_helper(sc, "string->atom: bad base:", second(sc->args));
	} else if (*s == '#') /* no use of base! */{
		return s_return_helper(sc, make_sharp_const(sc, s + 1));
	} else {
		if (print_flag == 0 || print_flag == 10) {
			return s_return_helper(sc, make_atom_from_string(sc, s));
		} else {
			char *ep;
			long iv = strtol(s, &ep, (int) print_flag);
			if (*ep == 0) {
				return s_return_helper(sc, make_integer(sc, iv));
			} else {
				return s_return_helper(sc, &g_false);
			}
		}
	}
}

// (symbol->string x)
Cell* op_sym2str(Scheme *sc) {
	Cell* x = make_string(sc, symbol_name(first(sc->args)));
	set_immutable(x);
	return s_return_helper(sc, x);
}

// (atom->string x)
// (atom->string num base)
Cell* op_atom2str(Scheme *sc) {
	Cell* x = first(sc->args);

	long print_flag = 0;
	if (rest(sc->args) != &g_nil) {
		/* we know cadr(sc->args) is a natural number */
		/* see if it is 2, 8, 10, or 16, or error */
		print_flag = long_value_unchecked(second(sc->args));
		if (is_number(x)
				&& (print_flag == 16 || print_flag == 10 || print_flag == 8
						|| print_flag == 2)) {
			/* base is OK */
		} else
			print_flag = -1;
	}
	if (print_flag < 0) {
		return error_helper(sc, "atom->string: bad base:", second(sc->args));
	} else if (is_number(x) || is_character(x) || is_string(x)
			|| is_symbol(x)) {
		char *p;
		int len;
		atom2str(sc, x, (int) print_flag, &p, &len);
		return s_return_helper(sc, make_string_n(sc, p, len));
	} else {
		return error_helper(sc, "atom->string: not an atom:", x);
	}
}

// (make-string n)
// (make-string n c)
Cell* op_makestring(Scheme *sc) {
	int len = long_value(first(sc->args));
	int fill = ' ';
	if (rest(sc->args) != &g_nil) {
		fill = char_value(second(sc->args));
	}
	return s_return_helper(sc, make_empty_string(sc, len, (char) fill));
}

// (string-length x)
Cell* op_strlen(Scheme *sc) {
	return s_return_helper(sc,
			make_integer(sc, strlen(first(sc->args)->_string)));
}

// (string-ref str index)
Cell* op_strref(Scheme *sc) {
	char *str = first(sc->args)->_string;
	int index = long_value(second(sc->args));
	if (index >= strlen(first(sc->args)->_string)) {
		return error_helper(sc, "string-ref: out of bounds:", second(sc->args));
	}
	return s_return_helper(sc,
			make_character(sc, ((unsigned char*) str)[index]));
}

// (string-set! str index c)
Cell* op_strset(Scheme *sc) {
	if (is_immutable(first(sc->args))) {
		return error_helper(sc,
				"string-set!: unable to alter immutable string:",
				first(sc->args));
	}
	char *str = (first(sc->args))->_string;
	int index = long_value(second(sc->args));
	if (index >= strlen(first(sc->args)->_string)) {
		return error_helper(sc, "string-set!: out of bounds:", second(sc->args));
	}
	int c = char_value(third(sc->args));
	str[index] = (char) c;
	return s_return_helper(sc, first(sc->args));
}

// (string-append x y ...)
Cell* op_strappend(Scheme *sc) {
	Cell* x;
	/* in 1.29 string-append was in Scheme in init.scm but was too slow */
	int len = 0;
	/* compute needed length for new string */
	for (x = sc->args; x != &g_nil; x = cdr(x)) {
		len += strlen(car(x)->_string);
	}
	Cell* newstr = make_empty_string(sc, len, ' ');
	/* store the contents of the argument strings into the new string */
	char *pos = (newstr)->_string;
	for (x = sc->args; x != &g_nil;
			pos += strlen(car(x)->_string), x = cdr(x)) {
		memcpy(pos, car(x)->_string, strlen(car(x)->_string));
	}
	return s_return_helper(sc, newstr);
}

// (substring str begin)
// (substring str begin end)
Cell* op_substr(Scheme *sc) {
	Cell* x;
	char *str = first(sc->args)->_string;
	int length = strlen(str);
	int begin = long_value(second(sc->args));
	if (begin > length) {
		return error_helper(sc, "substring: start out of bounds:",
				second(sc->args));
	}
	int end;
	if (cddr(sc->args) != &g_nil) {
		end = long_value(third(sc->args));
		if (end > length || end < begin) {
			return error_helper(sc, "substring: end out of bounds:",
					third(sc->args));
		}
	} else {
		end = length;
	}
	int len = end - begin;
	x = make_empty_string(sc, len, ' ');
	memcpy(x->_string, str + begin, len);
	(x->_string)[len] = 0;
	return s_return_helper(sc, x);
}

// (vector x y ...)
Cell* op_vector(Scheme *sc) {
	Cell* x;
	int len = list_length(sc, sc->args);
	if (len < 0)
		return error_helper(sc, "vector: not a proper list:", sc->args);
	Cell* vec = make_vector(sc, len);
	int index = 0;
	for (x = sc->args; is_pair(x); x = cdr(x), index++) {
		set_vector_item(vec, index, car(x));
	}
	return s_return_helper(sc, vec);
}

// (make-vector n e)
Cell* op_makevector(Scheme *sc) {
	int len = long_value(first(sc->args));
	Cell* vec = make_vector(sc, len);
	if (rest(sc->args) != &g_nil) {
		fill_vector(vec, second(sc->args));
	}
	return s_return_helper(sc, vec);
}

// (vector-length x)
Cell* op_veclen(Scheme *sc) {
	return s_return_helper(sc, make_integer(sc, long_value(first(sc->args))));
}

// (vector-ref x index)
Cell* op_vecref(Scheme *sc) {
	int index = long_value(second(sc->args));
	if (index >= long_value(first(sc->args))) {
		return error_helper(sc, "vector-ref: out of bounds:", second(sc->args));
	}
	return s_return_helper(sc, get_vector_item(first(sc->args), index));
}

// (vector-set! x index e)
Cell* op_vecset(Scheme *sc) {
	if (is_immutable(first(sc->args))) {
		return error_helper(sc,
				"vector-set!: unable to alter immutable vector:",
				first(sc->args));
	}
	int index = long_value(second(sc->args));
	if (index >= long_value(first(sc->args))) {
		return error_helper(sc, "vector-set!: out of bounds:", second(sc->args));
	}
	set_vector_item(first(sc->args), index, third(sc->args));
	return s_return_helper(sc, first(sc->args));
}

// (not x)
Cell* op_not(Scheme *sc) {
	int istrue = (is_false(first(sc->args)));
	return s_return_helper(sc, (istrue) ? &g_true : &g_false);
}

// (boolean? x)
Cell* op_boolp(Scheme *sc) {
	int istrue = (first(sc->args) == &g_false || first(sc->args) == &g_true);
	return s_return_helper(sc, (istrue) ? &g_true : &g_false);
}

// (eof-object? x)
Cell* op_eofobjp(Scheme *sc) {
	int istrue = (first(sc->args) == &g_eof);
	return s_return_helper(sc, (istrue) ? &g_true : &g_false);
}

// (null? x)
Cell* op_nullp(Scheme *sc) {
	int istrue = (first(sc->args) == &g_nil);
	return s_return_helper(sc, (istrue) ? &g_true : &g_false);
}

// (= x y ...)
Cell* op_numeq(Scheme *sc) {
	Cell* x = sc->args;
	Num value;
	value = num_value(car(x));
	for (x = cdr(x); x != &g_nil; x = cdr(x)) {
		if (!num_eq(value, num_value(car(x)))) {
			return s_return_helper(sc, &g_false);
		}
		value = num_value(car(x));
	}
	return s_return_helper(sc, &g_true);
}

// (< x y ...)
Cell* op_less(Scheme *sc) {
	Cell* x = sc->args;
	Num value;
	value = num_value(car(x));
	for (x = cdr(x); x != &g_nil; x = cdr(x)) {
		if (!num_lt(value, num_value(car(x)))) {
			return s_return_helper(sc, &g_false);
		}
		value = num_value(car(x));
	}
	return s_return_helper(sc, &g_true);
}

// (> x y ...)
Cell* op_gre(Scheme *sc) {
	Cell* x = sc->args;
	Num value;
	value = num_value(car(x));
	for (x = cdr(x); x != &g_nil; x = cdr(x)) {
		if (!num_gt(value, num_value(car(x)))) {
			return s_return_helper(sc, &g_false);
		}
		value = num_value(car(x));
	}
	return s_return_helper(sc, &g_true);
}

// (<= x y ...)
Cell* op_leq(Scheme *sc) {
	Cell* x = sc->args;
	Num value;
	value = num_value(car(x));
	for (x = cdr(x); x != &g_nil; x = cdr(x)) {
		if (!num_le(value, num_value(car(x)))) {
			return s_return_helper(sc, &g_false);
		}
		value = num_value(car(x));
	}
	return s_return_helper(sc, &g_true);
}

// (>= x y ...)
Cell* op_geq(Scheme *sc) {
	Cell* x = sc->args;
	Num value;
	value = num_value(car(x));
	for (x = cdr(x); x != &g_nil; x = cdr(x)) {
		if (!num_ge(value, num_value(car(x)))) {
			return s_return_helper(sc, &g_false);
		}
		value = num_value(car(x));
	}
	return s_return_helper(sc, &g_true);
}

// (symbol? x)
Cell* op_symbolp(Scheme *sc) {
	int istrue = (is_symbol(first(sc->args)));
	return s_return_helper(sc, (istrue) ? &g_true : &g_false);
}

// (number? x)
Cell* op_numberp(Scheme *sc) {
	int istrue = (is_number(first(sc->args)));
	return s_return_helper(sc, (istrue) ? &g_true : &g_false);
}

// (string? x)
Cell* op_stringp(Scheme *sc) {
	int istrue = (is_string(first(sc->args)));
	return s_return_helper(sc, (istrue) ? &g_true : &g_false);
}

// (integer? x)
Cell* op_integerp(Scheme *sc) {
	int istrue = (is_integer(first(sc->args)));
	return s_return_helper(sc, (istrue) ? &g_true : &g_false);
}

// (real? x)
Cell* op_realp(Scheme *sc) {
	int istrue = (is_number(first(sc->args))); /* All numbers are real */
	return s_return_helper(sc, (istrue) ? &g_true : &g_false);
}

// (char? x)
Cell* op_charp(Scheme *sc) {
	int istrue = (is_character(first(sc->args)));
	return s_return_helper(sc, (istrue) ? &g_true : &g_false);
}

//是否是字母
static int isalphax(int c) {
	return isascii(c) && isalpha(c);
}
//是否是数字
static int isdigitx(int c) {
	return isascii(c) && isdigit(c);
}
//是否是空格
static int isspacex(int c) {
	return isascii(c) && isspace(c);
}
//是否是大写字母
static int isupperx(int c) {
	return isascii(c) && isupper(c);
}
//是否是小写字母
static int islowerx(int c) {
	return isascii(c) && islower(c);
}

// (char-alphabetic? x)
Cell* op_charap(Scheme *sc) {
	int istrue = (isalphax(long_value(first(sc->args))));
	return s_return_helper(sc, (istrue) ? &g_true : &g_false);
}

// (char-numeric? x)
Cell* op_charnp(Scheme *sc) {
	int istrue = (isdigitx(long_value(first(sc->args))));
	return s_return_helper(sc, (istrue) ? &g_true : &g_false);
}

// (char-whitespace? x)
Cell* op_charwp(Scheme *sc) {
	int istrue = (isspacex(long_value(first(sc->args))));
	return s_return_helper(sc, (istrue) ? &g_true : &g_false);
}

// (char-upper-case? x)
Cell* op_charup(Scheme *sc) {
	int istrue = (isupperx(long_value(first(sc->args))));
	return s_return_helper(sc, (istrue) ? &g_true : &g_false);
}

// (char-lower-case? x)
Cell* op_charlp(Scheme *sc) {
	int istrue = (islowerx(long_value(first(sc->args))));
	return s_return_helper(sc, (istrue) ? &g_true : &g_false);
}

// (port? x)
Cell* op_portp(Scheme *sc) {
	int istrue = (is_port(first(sc->args)));
	return s_return_helper(sc, (istrue) ? &g_true : &g_false);
}

// (input-port? x)
Cell* op_inportp(Scheme *sc) {
	int istrue = (is_inport(first(sc->args)));
	return s_return_helper(sc, (istrue) ? &g_true : &g_false);
}

// (output-port? x)
Cell* op_outportp(Scheme *sc) {
	int istrue = (is_outport(first(sc->args)));
	return s_return_helper(sc, (istrue) ? &g_true : &g_false);
}

// (procedure? x)
Cell* op_procp(Scheme *sc) {
	/*continuation should be procedure by the example
	 * (call-with-current-continuation procedure?) ==> #t
	 * in R^3 report sec. 6.9*/
	int istrue = (is_proc(first(sc->args)) || is_closure(first(sc->args))
			|| is_continuation(first(sc->args)) || is_foreign(first(sc->args)));
	return s_return_helper(sc, (istrue) ? &g_true : &g_false);
}

// (pair? x)
Cell* op_pairp(Scheme *sc) {
	int istrue = (is_pair(first(sc->args)));
	return s_return_helper(sc, (istrue) ? &g_true : &g_false);
}

// (list? x)
Cell* op_listp(Scheme *sc) {
	int istrue = (list_length(sc, first(sc->args)) >= 0);
	return s_return_helper(sc, (istrue) ? &g_true : &g_false);
}

// (environment? x)
Cell* op_envp(Scheme *sc) {
	int istrue = (is_environment(first(sc->args)));
	return s_return_helper(sc, (istrue) ? &g_true : &g_false);
}

// (vector? x)
Cell* op_vectorp(Scheme *sc) {
	int istrue = (is_vector(first(sc->args)));
	return s_return_helper(sc, (istrue) ? &g_true : &g_false);
}

// (eq? x y)
Cell* op_eq(Scheme *sc) {
	int istrue = (first(sc->args) == second(sc->args));
	return s_return_helper(sc, (istrue) ? &g_true : &g_false);
}

// (eqv? x y)
Cell* op_eqv(Scheme *sc) {
	int istrue = (eqv(first(sc->args), second(sc->args)));
	return s_return_helper(sc, (istrue) ? &g_true : &g_false);
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

// (write expr outport)
// (write expr)
Cell* op_write(Scheme *sc) {
	Cell* x;
	if (is_pair(rest(sc->args))) {
		if (second(sc->args) != sc->outPort) {
			x = cons(sc, sc->outPort, &g_nil);
			s_save(sc, op_set_outport, x, &g_nil);
			sc->outPort = second(sc->args);
		}
	}
	sc->args = first(sc->args);
	//sc->print_flag = 1;
	sc->op = op_p0list;
	return &g_true;
}

// (display expr outport)
// (display expr)
Cell* op_display(Scheme *sc) {
	Cell* x;
	if (is_pair(rest(sc->args))) {
		if (second(sc->args) != sc->outPort) {
			x = cons(sc, sc->outPort, &g_nil);
			s_save(sc, op_set_outport, x, &g_nil);
			sc->outPort = second(sc->args);
		}
	}
	sc->args = first(sc->args);
	//sc->print_flag = 0;
	sc->op = op_p0list;
	return &g_true;
}

// (write-char char outport)
// (write-char char)
Cell* op_write_char(Scheme *sc) {
	Cell* x;
	if (is_pair(rest(sc->args))) {
		if (second(sc->args) != sc->outPort) {
			x = cons(sc, sc->outPort, &g_nil);
			s_save(sc, op_set_outport, x, &g_nil);
			sc->outPort = second(sc->args);
		}
	}
	sc->args = first(sc->args);
	//sc->print_flag = 0;
	sc->op = op_p0list;
	return &g_true;
}

// (newline outport)
// (newline)
Cell* op_newline(Scheme *sc) {
	Cell* x;
	if (is_pair(sc->args)) {
		if (first(sc->args) != sc->outPort) {
			x = cons(sc, sc->outPort, &g_nil);
			s_save(sc, op_set_outport, x, &g_nil);
			sc->outPort = first(sc->args);
		}
	}
	write_string(sc, "\n");
	return s_return_helper(sc, &g_true);
}

// (error x y z ...)
Cell* op_err0(Scheme *sc) {
	//sc->eval_result = -1;
	if (!is_string(car(sc->args))) {
		sc->args = cons(sc, make_string(sc, " -- "), sc->args);
		set_immutable(sc->args);
	}
	write_string(sc, "Error: ");
	write_string(sc, car(sc->args)->_string);
	sc->args = cdr(sc->args);
	sc->op = op_err1;
	return &g_true;
}

Cell* op_err1(Scheme *sc) {
	write_string(sc, " ");
	if (sc->args != &g_nil) {
		s_save(sc, op_err1, cdr(sc->args), &g_nil);
		sc->args = car(sc->args);
		//sc->print_flag = 1;
		sc->op = op_p0list;
		return &g_true;
	} else {
		write_string(sc, "\n");

		/*
		 s_goto(sc, op_toplevel);
		 if (file_interactive(sc)) {
		 sc->op = op_toplevel;
		 return &g_true;
		 } else
		 return &g_nil;
		 */

		sc->op = op_toplevel;
		return &g_true;
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

// (new-segment num)
Cell* op_newsegment(Scheme *sc) {
	if (!is_pair(sc->args) || !is_number(first(sc->args))) {
		return error_helper(sc, "new-segment: argument must be a number", NULL);
	}
	alloc_cellseg(sc, (int) long_value(first(sc->args)));
	return s_return_helper(sc, &g_true);
}

// (oblist)
Cell* op_oblist(Scheme *sc) {
	return s_return_helper(sc, oblist_all_symbols(sc));
}

// (current-input-port)
Cell* op_curr_inport(Scheme *sc) {
	return s_return_helper(sc, sc->inPort);
}

// (current-output-port)
Cell* op_curr_outport(Scheme *sc) {
	return s_return_helper(sc, sc->outPort);
}

// (open-input-file infile)
Cell* op_open_infile(Scheme *sc) {
	Cell* port = port_from_filename(sc, first(sc->args)->_string, PORT_INPUT);
	if (port == &g_nil)
		return s_return_helper(sc, &g_false);
	else
		return s_return_helper(sc, port);
}

// (open-output-file outfile)
Cell* op_open_outfile(Scheme *sc) {
	Cell* port = port_from_filename(sc, first(sc->args)->_string, PORT_OUTPUT);
	if (port == &g_nil)
		return s_return_helper(sc, &g_false);
	else
		return s_return_helper(sc, port);
}

// (open-input-output-file inoutfile)
Cell* op_open_inoutfile(Scheme *sc) {
	Cell* port = port_from_filename(sc, first(sc->args)->_string,
			PORT_INPUT | PORT_OUTPUT);
	if (port == &g_nil)
		return s_return_helper(sc, &g_false);
	else
		return s_return_helper(sc, port);
}

// (open-input-string str)
Cell* op_open_instring(Scheme *sc) {
	char *start = first(sc->args)->_string;
	int length = strlen(start);
	Cell* port = port_from_string(sc, start, start + length, PORT_INPUT);
	if (port == &g_nil)
		return s_return_helper(sc, &g_false);
	else
		return s_return_helper(sc, port);
}

// (open-input-output-string str)
Cell* op_open_inoutstring(Scheme *sc) {
	char *start = first(sc->args)->_string;
	int length = strlen(start);
	Cell* port = port_from_string(sc, start, start + length, PORT_INPUT | PORT_OUTPUT);
	if (port == &g_nil)
		return s_return_helper(sc, &g_false);
	else
		return s_return_helper(sc, port);
}

// (open-output-string str)
// (open-output-string)
Cell* op_open_outstring(Scheme *sc) {
	char *start;
	int length;
	Cell* port;
	if (first(sc->args) == &g_nil) {
		//分配一片空的内存做为输出端口
		port = port_from_string(sc, NULL, NULL, PORT_OUTPUT | PORT_CAN_REALLOC);
	} else {
		start = first(sc->args)->_string;
		length = strlen(start);
		port = port_from_string(sc, start, start + length, PORT_OUTPUT);
	}
	if (port == &g_nil)
		return s_return_helper(sc, &g_false);
	else
		return s_return_helper(sc, port);
}

// (get-output-string outport)
Cell* op_get_outstring(Scheme *sc) {
	Port *port = ((sc->args)->_pair._car)->_port;
	if (port->kind & PORT_STRING) {
		return s_return_helper(sc,
				make_string_n(sc, port->stringPort.start,
						port->stringPort.cur - port->stringPort.start));
	}
	return s_return_helper(sc, &g_false);
}

// (close-input-port inport)
Cell* op_close_inport(Scheme *sc) {
	port_close(first(sc->args), PORT_INPUT);
	return s_return_helper(sc, &g_true);
}

// (close-output-port outport)
Cell* op_close_outport(Scheme *sc) {
	port_close(first(sc->args), PORT_OUTPUT);
	return s_return_helper(sc, &g_true);
}

// (interaction-environment)
Cell* op_int_env(Scheme *sc) {
	return s_return_helper(sc, sc->globalEnv);
}

// (current-environment)
Cell* op_curr_env(Scheme *sc) {
	return s_return_helper(sc, sc->env);
}

// (read)
// (read inport)
Cell* op_read(Scheme *sc) {
	Cell* inport;
	if (is_pair(sc->args)) {
		if (!is_inport(first(sc->args))) {
			return error_helper(sc, "read: not an input port:", first(sc->args));
		}
		if (first(sc->args) != sc->inPort) {
			inport = cons(sc, sc->inPort, &g_nil);
			s_save(sc, op_set_inport, inport, &g_nil);
			sc->inPort = first(sc->args);
		}
	}
	sc->op = op_read_internal;
	return &g_true;
}

// (read-char)
// (read-char inport)
Cell* op_read_char(Scheme *sc) {
	Cell* inport;
	if (is_pair(sc->args)) {
		if (first(sc->args) != sc->inPort) {
			inport = cons(sc, sc->inPort, &g_nil);
			s_save(sc, op_set_inport, inport, &g_nil);
			sc->inPort = first(sc->args);
		}
	}
	int c = get_char(sc);
	if (c == EOF)
		return s_return_helper(sc, &g_eof);
	else
		return s_return_helper(sc, make_character(sc, c));
}

// (peek-char)
// (peek-char inport)
Cell* op_peek_char(Scheme *sc) {
	Cell* inport;
	if (is_pair(sc->args)) {
		if (first(sc->args) != sc->inPort) {
			inport = cons(sc, sc->inPort, &g_nil);
			s_save(sc, op_set_inport, inport, &g_nil);
			sc->inPort = first(sc->args);
		}
	}
	int c = get_char(sc);
	unget_char(sc, c);
	if (c == EOF)
		return s_return_helper(sc, &g_eof);
	else
		return s_return_helper(sc, make_character(sc, c));
}

// (char-ready?)
// (char-ready? inport)
Cell* op_char_ready(Scheme *sc) {
	Cell* inport;
	inport = sc->inPort;
	if (is_pair(sc->args))
		inport = first(sc->args);
	int istrue = inport->_port->kind & PORT_STRING;
	return s_return_helper(sc, (istrue) ? &g_true : &g_false);
}

// (set-input-port inport)
Cell* op_set_inport(Scheme *sc) {
	sc->inPort = first(sc->args);
	return s_return_helper(sc, sc->returnValue);
}

// (set-output-port outport)
Cell* op_set_outport(Scheme *sc) {
	sc->outPort = first(sc->args);
	return s_return_helper(sc, sc->returnValue);
}

Cell* op_read_sexpr(Scheme *sc) {
	Cell* x;

	switch (sc->token) {
	case TOK_EOF:
		return s_return_helper(sc, &g_eof);
	case TOK_VECTOR:
		s_save(sc, op_read_vec, &g_nil, &g_nil); //压入 数组 处理过程
		break;
		/* no break */
		/* fall through */
	case TOK_LPAREN:
		sc->token = token(sc);
		if (sc->token == TOK_RPAREN) {
			return s_return_helper(sc, &g_nil);
		} else if (sc->token == TOK_DOT) {
			return error_helper(sc, "syntax error: illegal dot expression",
			NULL);
		} else {
			//sc->nesting_stack[sc->top_file_index]++;
			s_save(sc, op_read_list, &g_nil, &g_nil); //压入读取列表的函数
			//读取S表达式
			sc->op = op_read_sexpr;
			return &g_true;
		}
	case TOK_QUOTE: //引用
		s_save(sc, op_read_quote, &g_nil, &g_nil); //压入 引用 处理函数
		sc->token = token(sc);
		//读取S表达式
		sc->op = op_read_sexpr;
		return &g_true;
	case TOK_BQUOTE: //反引用
		sc->token = token(sc);
		if (sc->token == TOK_VECTOR) {
			s_save(sc, op_read_qquote_vec, &g_nil, &g_nil); //压入 解引用数组 处理过程
			sc->token = TOK_LPAREN; //这里替换成了TOK_LPAREN ，就不会压入TOK_VECTOR的处理函数
			//读取S表达式 这里会转向读取一个列表的处理过程
			sc->op = op_read_sexpr;
			return &g_true;
		} else {
			s_save(sc, op_read_qquote, &g_nil, &g_nil); //压入 准引用 处理过程
		}
		//读取S表达式
		sc->op = op_read_sexpr;
		return &g_true;
	case TOK_COMMA: //,解引用
		s_save(sc, op_read_unquote, &g_nil, &g_nil); //压入执行,操作的代码
		sc->token = token(sc);
		//读取S表达式
		sc->op = op_read_sexpr;
		return &g_true;
	case TOK_ATMARK: //,@解引用
		s_save(sc, op_read_uqtsp, &g_nil, &g_nil); //压入执行,@操作的代码
		sc->token = token(sc);
		sc->op = op_read_sexpr;
		return &g_true;
	case TOK_ATOM: //原子
		return s_return_helper(sc,
				make_atom_from_string(sc, readstr_upto(sc, DELIMITERS)));
	case TOK_DQUOTE: //双引号 (字符串类型的原子)
		x = readstrexp(sc);
		if (x == &g_false)
			return error_helper(sc, "Error reading string", NULL);
		set_immutable(x);
		return s_return_helper(sc, x);
	case TOK_SHARP: { //#是eval的简写吗？
		Cell* f = find_slot_in_env(sc, sc->env, sc->sym_sharp_hook, 1); //f的类型symbol_kv ,f是一段处理#的代码
		if (f == &g_nil)
			return error_helper(sc, "undefined sharp expression", NULL);
		sc->code = cons(sc, slot_value_in_env(f), &g_nil);
		sc->op = op_eval;
		return &g_true;
	}
		/* no break */
	case TOK_SHARP_CONST: //常量
		x = make_sharp_const(sc, readstr_upto(sc, DELIMITERS));
		if (x == &g_nil)
			return error_helper(sc, "undefined const sharp expression", NULL);
		return s_return_helper(sc, x);
	default:
		return error_helper(sc, "syntax error: illegal token", NULL);
	}

	return NULL;
}

Cell* op_read_list(Scheme *sc) {
	sc->args = cons(sc, sc->returnValue, sc->args);
	sc->token = token(sc);
	if (sc->token == TOK_EOF)
		return s_return_helper(sc, &g_eof);
	else if (sc->token == TOK_RPAREN) {
		//sc->nesting_stack[sc->top_file_index]--;
		return s_return_helper(sc, reverse(sc, sc->args));
	} else if (sc->token == TOK_DOT) {
		s_save(sc, op_read_dot, sc->args, &g_nil);
		sc->token = token(sc);
		sc->op = op_read_sexpr;
		return &g_true;
	} else {
		s_save(sc, op_read_list, sc->args, &g_nil);
		sc->op = op_read_sexpr;
		return &g_true;
	}
}

Cell* op_read_vec(Scheme *sc) {
	sc->args = sc->returnValue;
	sc->op = op_vector;
	return &g_true;
}

Cell* op_read_dot(Scheme *sc) {
	if (token(sc) != TOK_RPAREN) {
		return error_helper(sc, "syntax error: illegal dot expression", NULL);
	} else {
		//sc->nesting_stack[sc->top_file_index]--;
		return s_return_helper(sc, revappend(sc, sc->returnValue, sc->args));
	}
}

Cell* op_read_quote(Scheme *sc) {
	return s_return_helper(sc,
			cons(sc, sc->sym_quote, cons(sc, sc->returnValue, &g_nil)));
}

Cell* op_read_qquote(Scheme *sc) {
	return s_return_helper(sc,
			cons(sc, sc->sym_qquote, cons(sc, sc->returnValue, &g_nil)));
}

Cell* op_read_qquote_vec(Scheme *sc) {
	//下面的代码会进行类似这样的一个操作 (apply vector `(,(+ 1 2) 4)) =>#(3 4)
	return s_return_helper(sc,
			cons(sc, make_symbol(sc, "apply"),
					cons(sc, make_symbol(sc, "vector"),
							cons(sc,
									cons(sc, sc->sym_qquote,
											cons(sc, sc->returnValue, &g_nil)),
									&g_nil))));
}

Cell* op_read_unquote(Scheme *sc) {
	return s_return_helper(sc,
			cons(sc, sc->sym_unquote, cons(sc, sc->returnValue, &g_nil)));
}

Cell* op_read_uqtsp(Scheme *sc) {
	return s_return_helper(sc,
			cons(sc, sc->sym_unquote_sp, cons(sc, sc->returnValue, &g_nil)));
}

#define   ok_abbrev(p)   (is_pair(p) && cdr(p) == &g_nil)
Cell* op_p0list(Scheme *sc) {
	/* ========== printing part ========== */
	if (is_vector(sc->args)) {
		write_string(sc, "#(");
		sc->args = cons(sc, sc->args, make_integer(sc, 0));
		sc->op = op_pvecfrom;
		return &g_true;
	} else if (is_environment(sc->args)) { //环境
		write_string(sc, "#<ENVIRONMENT>");
		return s_return_helper(sc, &g_true);
	} else if (!is_pair(sc->args)) { //原子
		printatom(sc, sc->args, 1);
		return s_return_helper(sc, &g_true);
	} else if (car(sc->args) == sc->sym_quote && ok_abbrev(cdr(sc->args))) {
		write_string(sc, "'");
		sc->args = cadr(sc->args);
		sc->op = op_p0list;
		return &g_true;
	} else if (car(sc->args) == sc->sym_qquote && ok_abbrev(cdr(sc->args))) {
		write_string(sc, "`");
		sc->args = cadr(sc->args);
		sc->op = op_p0list;
		return &g_true;
	} else if (car(sc->args) == sc->sym_unquote && ok_abbrev(cdr(sc->args))) {
		write_string(sc, ",");
		sc->args = cadr(sc->args);
		sc->op = op_p0list;
		return &g_true;
	} else if (car(sc->args) == sc->sym_unquote_sp
			&& ok_abbrev(cdr(sc->args))) {
		write_string(sc, ",@");
		sc->args = cadr(sc->args);
		sc->op = op_p0list;
		return &g_true;
	} else {
		write_string(sc, "(");
		s_save(sc, op_p1list, cdr(sc->args), &g_nil);
		sc->args = car(sc->args);
		sc->op = op_p0list;
		return &g_true;
	}
}

Cell* op_p1list(Scheme *sc) {
	if (is_pair(sc->args)) {
		s_save(sc, op_p1list, cdr(sc->args), &g_nil);
		write_string(sc, " ");
		sc->args = car(sc->args);
		sc->op = op_p0list;
		return &g_true;
	} else if (sc->args != &g_nil) { //这里处理数据位原子（包括数组）的情况
		s_save(sc, op_p1list, &g_nil, &g_nil);
		write_string(sc, " . ");
		sc->op = op_p0list;
		return &g_true;
	} else {
		write_string(sc, ")");
		return s_return_helper(sc, &g_true);
	}
}

Cell* op_pvecfrom(Scheme *sc) {
	int index = long_value_unchecked(cdr(sc->args));
	Cell* vec = car(sc->args);
	int len = long_value_unchecked(vec);
	if (index == len) {
		write_string(sc, ")");
		return s_return_helper(sc, &g_true);
	} else {
		Cell* elem = get_vector_item(vec, index);
		long_value_unchecked(cdr(sc->args)) = index + 1;
		s_save(sc, op_pvecfrom, sc->args, &g_nil);
		sc->args = elem;
		if (index > 0)
			write_string(sc, " ");
		sc->op = op_p0list;
		return &g_true;
	}
}

// (length LIST)
Cell* op_list_length(Scheme *sc) {
	long len = list_length(sc, first(sc->args));
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
// (closure? obj)
Cell* op_closurep(Scheme *sc) {
	/* Note, macro object is also a closure.Therefore, (closure? <#MACRO>) ==> #t*/
	int istrue = is_closure(first(sc->args));
	return s_return_helper(sc, (istrue) ? &g_true : &g_false);
}

// (macro? obj)
Cell* op_macrop(Scheme *sc) {
	int istrue = is_macro(first(sc->args));
	return s_return_helper(sc, (istrue) ? &g_true : &g_false);
}

Cell *op_system(Scheme *sc) {
	String cmd = NULL;
	Cell* x = NULL;
	for (x = sc->args; x != &g_nil; x = cdr(x)) {
		cmd = car(x)->_string;
		execute_command(cmd);
	}
	return s_return_helper(sc, &g_true);
}

Cell *op_pipe(Scheme *sc) {
	String cmd = copy_string(car(sc->args)->_string);
	Cell* x = cdr(sc->args);
	for (; x != &g_nil; x = cdr(x)) {
		cmd = dyna_strcat(cmd, " | ");
		cmd = dyna_strcat(cmd, car(x)->_string);
	}
	execute_command(cmd);
	free(cmd);
	return s_return_helper(sc, &g_true);
}

