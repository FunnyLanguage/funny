/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2015 by fanguangping (fanguangping@163.com)
 * predicate.c
 */

#include <ctype.h>
#include "operator.h"

int is_pair(Cell* p) {
	return (type(p) == T_PAIR);
}

//判断是不是数字
int is_number(Cell* p) {
	return (type(p) == T_NUMBER);
}

//判断是不是整数,或者是一个可以无损转换为整数的浮点数
int is_integer(Cell* p) {
	if (!is_number(p))
		return FALSE;
	if (p->_num.isFix || (double) long_value(p) == double_value(p))
		return TRUE;
	return FALSE;
}

//判断是不是浮点数
int is_real(Cell* p) {
	return is_number(p) && !(p->_num.isFix);
}
//判断是不是字符
int is_character(Cell* p) {
	return (type(p) == T_CHAR);
}

//是否是字符串
int is_string(Cell* p) {
	return (type(p) == T_STRING);
}

//是否是符号
int is_symbol(Cell* sym) {
	return (type(sym) == T_SYMBOL);
}

// 是否SYNTAX
int is_syntax(Cell* p) {
	return (typeflag(p) & T_SYNTAX);
}

// 是否是数组
int is_vector(Cell* p) {
	return (type(p) == T_VECTOR);
}

// 是否过程
int is_proc(Cell* p) {
	return (type(p) == T_PROC);
}

// 是否扩展过程
int is_foreign(Cell* p) {
	return (type(p) == T_FOREIGN);
}

// 是否闭包
int is_closure(Cell* p) {
	return (type(p) == T_CLOSURE);
}

// 是否promise
int is_promise(Cell* p) {
	return (type(p) == T_PROMISE);
}

// 是否宏
int is_macro(Cell* p) {
	return (type(p) == T_MACRO);
}

// 是否延续
int is_continuation(Cell* p) {
	return (type(p) == T_CONTINUATION);
}

// 是否环境
int is_environment(Cell* p) {
	return (type(p) == T_ENVIRONMENT);
}

// 是否不可改变
int is_immutable(Cell* p) {
	return (typeflag(p) & T_IMMUTABLE);
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

// (boolean? x)
Cell* op_boolp(Scheme *sc) {
	int istrue = (first(sc->args) == &g_false || first(sc->args) == &g_true);
	return s_return_helper(sc, (istrue) ? &g_true : &g_false);
}

// (null? x)
Cell* op_nullp(Scheme *sc) {
	int istrue = (first(sc->args) == &g_nil);
	return s_return_helper(sc, (istrue) ? &g_true : &g_false);
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
	int istrue = (ls_length(sc, first(sc->args)) >= 0);
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
	int istrue = eqv(first(sc->args), second(sc->args));
	return s_return_helper(sc, (istrue) ? &g_true : &g_false);
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
