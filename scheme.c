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
#include "scheme.h"
#include "memory.h"
#include "basic.h"
#include "operator.h"

#define OPERATOR_COUNT  178

// call/cc
// http://pathtovoid.net/post/2011/12/18/callcc-in-scheme/

OperatorInfo *init_operators() {
	OperatorInfo operators[OPERATOR_COUNT] = {
	// start
			{ op_load, "load", 1, 1, TYPE_STRING }, //
			{ op_toplevel, NULL, 0, 0, TYPE_NONE }, //
			{ op_eval_internal, NULL, 0, 0, TYPE_NONE }, //
			{ op_read_internal, NULL, 0, 0, TYPE_NONE }, //
			{ op_gensym, "gensym", 0, 0, TYPE_NONE }, //
			{ op_value_print, NULL, 0, 0, TYPE_NONE }, //
			{ op_eval, NULL, 0, 0, TYPE_NONE }, //
			{ op_e0args, NULL, 0, 0, TYPE_NONE }, //
			{ op_e1args, NULL, 0, 0, TYPE_NONE }, //
			{ op_apply, NULL, 0, 0, TYPE_NONE }, //
			{ op_domacro, NULL, 0, 0, TYPE_NONE }, //
			{ op_lambda, NULL, 0, 0, TYPE_NONE }, //
			{ op_lambda1, NULL, 0, 0, TYPE_NONE }, //
			{ op_make_closure, "make-closure", 0, 0, TYPE_PAIR }, //TST_PAIR TST_ENVIRONMENT //??
			{ op_quote, NULL, 0, 0, TYPE_NONE }, //
			{ op_def0, NULL, 0, 0, TYPE_NONE }, //
			{ op_def1, NULL, 0, 0, TYPE_NONE }, //
			{ op_defp, "defined?", 1, 2, TYPE_NONE }, //TST_SYMBOL TST_ENVIRONMENT
			{ op_begin, NULL, 0, 0, TYPE_NONE }, //
			{ op_if0, NULL, 0, 0, TYPE_NONE }, //
			{ op_if1, NULL, 0, 0, TYPE_NONE }, //
			{ op_set0, NULL, 0, 0, TYPE_NONE }, //
			{ op_set1, NULL, 0, 0, TYPE_NONE }, //
			{ op_let0, NULL, 0, 0, TYPE_NONE }, //
			{ op_let1, NULL, 0, 0, TYPE_NONE }, //
			{ op_let2, NULL, 0, 0, TYPE_NONE }, //
			{ op_letstar0, NULL, 0, 0, TYPE_NONE }, //
			{ op_letstar1, NULL, 0, 0, TYPE_NONE }, //
			{ op_letstar2, NULL, 0, 0, TYPE_NONE }, //
			{ op_let0rec, NULL, 0, 0, TYPE_NONE }, //
			{ op_let1rec, NULL, 0, 0, TYPE_NONE }, //
			{ op_let2rec, NULL, 0, 0, TYPE_NONE }, //
			{ op_cond0, NULL, 0, 0, TYPE_NONE }, //
			{ op_cond1, NULL, 0, 0, TYPE_NONE }, //
			{ op_delay, NULL, 0, 0, TYPE_NONE }, //
			{ op_and0, NULL, 0, 0, TYPE_NONE }, //
			{ op_and1, NULL, 0, 0, TYPE_NONE }, //
			{ op_or0, NULL, 0, 0, TYPE_NONE }, //
			{ op_or1, NULL, 0, 0, TYPE_NONE }, //
			{ op_c0stream, NULL, 0, 0, TYPE_NONE }, //
			{ op_c1stream, NULL, 0, 0, TYPE_NONE }, //
			{ op_macro0, NULL, 0, 0, TYPE_NONE }, //
			{ op_macro1, NULL, 0, 0, TYPE_NONE }, //
			{ op_case0, NULL, 0, 0, TYPE_NONE }, //
			{ op_case1, NULL, 0, 0, TYPE_NONE }, //
			{ op_peval, "eval", 1, 2, TYPE_ANY }, //TST_ANY TST_ENVIRONMENT
			{ op_papply, "apply", 1, 0xFFFF, TYPE_NONE }, //
			{ op_continuation, "call-with-current-continuation", 1, 1, TYPE_NONE }, //
			{ op_inexact2exact, "inexact->exact", 1, 1, TYPE_NUMBER }, //
			{ op_exp, "exp", 1, 1, TYPE_NUMBER }, //
			{ op_log, "log", 1, 1, TYPE_NUMBER }, //
			{ op_sin, "sin", 1, 1, TYPE_NUMBER }, //
			{ op_cos, "cos", 1, 1, TYPE_NUMBER }, //
			{ op_tan, "tan", 1, 1, TYPE_NUMBER }, //
			{ op_asin, "asin", 1, 1, TYPE_NUMBER }, //
			{ op_acos, "acos", 1, 1, TYPE_NUMBER }, //
			{ op_atan, "atan", 1, 2, TYPE_NUMBER }, //
			{ op_sqrt, "sqrt", 1, 1, TYPE_NUMBER }, //
			{ op_expt, "expt", 2, 2, TYPE_NUMBER }, //
			{ op_floor, "floor", 1, 1, TYPE_NUMBER }, //
			{ op_ceiling, "ceiling", 1, 1, TYPE_NUMBER }, //
			{ op_truncate, "truncate", 1, 1, TYPE_NUMBER }, //
			{ op_round, "round", 1, 1, TYPE_NUMBER }, //
			{ op_add, "+", 0, 0xFFFF, TYPE_NUMBER }, //
			{ op_sub, "-", 1, 0xFFFF, TYPE_NUMBER }, //
			{ op_mul, "*", 0, 0xFFFF, TYPE_NUMBER }, //
			{ op_div, "/", 1, 0xFFFF, TYPE_NUMBER }, //
			{ op_intdiv, "quotient", 1, 0xFFFF, TYPE_INTEGER }, //
			{ op_rem, "remainder", 2, 2, TYPE_INTEGER }, //
			{ op_mod, "modulo", 2, 2, TYPE_INTEGER }, //
			{ op_car, "car", 1, 1, TYPE_PAIR }, //
			{ op_cdr, "cdr", 1, 1, TYPE_PAIR }, //
			{ op_cons, "cons", 1, 1, TYPE_NONE }, //
			{ op_setcar, "set-car!", 2, 2, TYPE_PAIR }, //TST_PAIR TST_ANY
			{ op_setcdr, "set-cdr!", 2, 2, TYPE_PAIR }, //TST_PAIR TST_ANY
			{ op_char2int, "char->integer", 1, 1, TYPE_CHAR }, //
			{ op_int2char, "integer->char", 1, 1, TYPE_NATURAL }, //
			{ op_char_upcase, "char-upcase", 1, 1, TYPE_CHAR }, //
			{ op_char_downcase, "char-downcase", 1, 1, TYPE_CHAR }, //
			{ op_sym2str, "symbol->string", 1, 1, TYPE_SYMBOL }, //
			{ op_atom2str, "atom->string", 1, 2, TYPE_ANY }, //TST_ANY TST_NATURAL
			{ op_str2sym, "string->symbol", 1, 1, TYPE_STRING }, //
			{ op_str2atom, "string->atom", 1, 2, TYPE_STRING }, //TST_STRING TST_NATURAL
			{ op_makestring, "make-string", 1, 2, TYPE_NATURAL }, //TST_NATURAL TST_CHAR
			{ op_strlen, "string-length", 1, 1, TYPE_STRING }, //
			{ op_strref, "string-ref", 2, 2, TYPE_STRING }, //TST_STRING TST_NATURAL
			{ op_strset, "string-set!", 3, 3, TYPE_STRING }, //TST_STRING TST_NATURAL TST_CHAR
			{ op_strappend, "string-append", 0, 0xFFFF, TYPE_STRING }, //
			{ op_substr, "substring", 2, 3, TYPE_STRING }, //TST_STRING TST_NATURAL
			{ op_vector, "vector", 0, 0xFFFF, TYPE_NONE }, //
			{ op_makevector, "make-vector", 1, 2, TYPE_NATURAL }, //TST_NATURAL TST_ANY
			{ op_veclen, "vector-length", 1, 1, TYPE_VECTOR }, //
			{ op_vecref, "vector-ref", 2, 2, TYPE_VECTOR }, //TST_VECTOR TST_NATURAL
			{ op_vecset, "vector-set!", 3, 3, TYPE_VECTOR }, //TST_VECTOR TST_NATURAL TST_ANY
			{ op_not, "not", 1, 1, TYPE_NONE }, //
			{ op_boolp, "boolean?", 1, 1, TYPE_NONE }, //
			{ op_eofobjp, "eof-object?", 1, 1, TYPE_NONE }, //
			{ op_nullp, "null?", 1, 1, TYPE_NONE }, //
			{ op_numeq, "=", 2, 0xFFFF, TYPE_NUMBER }, //
			{ op_less, "<", 2, 0xFFFF, TYPE_NUMBER }, //
			{ op_gre, ">", 2, 0xFFFF, TYPE_NUMBER }, //
			{ op_leq, "<=", 2, 0xFFFF, TYPE_NUMBER }, //
			{ op_geq, ">=", 2, 0xFFFF, TYPE_NUMBER }, //
			{ op_symbolp, "symbol?", 1, 1, TYPE_ANY }, //
			{ op_numberp, "number?", 1, 1, TYPE_ANY }, //
			{ op_stringp, "string?", 1, 1, TYPE_ANY }, //
			{ op_integerp, "integer?", 1, 1, TYPE_ANY }, //
			{ op_realp, "real?", 1, 1, TYPE_ANY }, //
			{ op_charp, "char?", 1, 1, TYPE_ANY }, //
			{ op_charap, "char-alphabetic?", 1, 1, TYPE_CHAR }, //
			{ op_charnp, "char-numeric?", 1, 1, TYPE_CHAR }, //
			{ op_charwp, "char-whitespace?", 1, 1, TYPE_CHAR }, //
			{ op_charup, "char-upper-case?", 1, 1, TYPE_CHAR }, //
			{ op_charlp, "char-lower-case?", 1, 1, TYPE_CHAR }, //
			{ op_portp, "port?", 1, 1, TYPE_ANY }, //
			{ op_inportp, "input-port?", 1, 1, TYPE_ANY }, //
			{ op_outportp, "output-port?", 1, 1, TYPE_ANY }, //
			{ op_procp, "procedure?", 1, 1, TYPE_ANY }, //
			{ op_pairp, "pair?", 1, 1, TYPE_ANY }, //
			{ op_listp, "list?", 1, 1, TYPE_ANY }, //
			{ op_envp, "environment?", 1, 1, TYPE_ANY }, //
			{ op_vectorp, "vector?", 1, 1, TYPE_ANY }, //
			{ op_eq, "eq?", 2, 2, TYPE_ANY }, //
			{ op_eqv, "eqv?", 2, 2, TYPE_ANY }, //
			{ op_force, "force", 1, 1, TYPE_ANY }, //
			{ op_save_forced, NULL, 0, 0, TYPE_NONE }, //
			{ op_write, "write", 1, 2, TYPE_ANY }, //TST_ANY TST_OUTPORT
			{ op_write_char, "write-char", 1, 2, TYPE_CHAR }, //TST_CHAR TST_OUTPORT
			{ op_display, "display", 1, 2, TYPE_ANY }, //TST_ANY TST_OUTPORT
			{ op_newline, "newline", 0, 1, TYPE_OUTPORT }, //
			{ op_err0, "error", 1, 0xFFFF, TYPE_NONE }, //
			{ op_err1, NULL, 0, 0, TYPE_NONE }, //
			{ op_reverse, "reverse", 1, 1, TYPE_LIST }, //
			{ op_list_star, "list*", 1, 0xFFFF, TYPE_NONE }, //
			{ op_append, "append", 0, 0xFFFF, TYPE_NONE }, //
			{ op_quit, "quit", 0, 1, TYPE_NUMBER }, //
			{ op_gc, "gc", 0, 0, TYPE_NONE }, //
			{ op_gcverb, "gc-verbose", 0, 1, TYPE_NONE }, //
			{ op_newsegment, "new-segment", 0, 1, TYPE_NUMBER }, //
			{ op_oblist, "oblist", 0, 0, TYPE_NONE }, //
			{ op_curr_inport, "current-input-port", 0, 0, TYPE_NONE }, //
			{ op_curr_outport, "current-output-port", 0, 0, TYPE_NONE }, //
			{ op_open_infile, "open-input-file", 1, 1, TYPE_STRING }, //
			{ op_open_outfile, "open-output-file", 1, 1, TYPE_STRING }, //
			{ op_open_inoutfile, "open-input-output-file", 1, 1, TYPE_STRING }, //
			{ op_open_instring, "open-input-string", 1, 1, TYPE_STRING }, //
			{ op_open_inoutstring, "open-input-output-string", 1, 1, TYPE_STRING }, //
			{ op_open_outstring, "open-output-string", 0, 1, TYPE_STRING }, //
			{ op_get_outstring, "get-output-string", 1, 1, TYPE_OUTPORT }, //
			{ op_close_inport, "close-input-port", 1, 1, TYPE_INPORT }, //
			{ op_close_outport, "close-output-port", 1, 1, TYPE_OUTPORT }, //
			{ op_int_env, "interaction-environment", 0, 0, TYPE_NONE }, //
			{ op_curr_env, "current-environment", 0, 0, TYPE_NONE }, //
			{ op_read, "read", 0, 1, TYPE_INPORT }, //
			{ op_read_char, "read-char", 0, 1, TYPE_INPORT }, //
			{ op_peek_char, "peek-char", 0, 1, TYPE_INPORT }, //
			{ op_char_ready, "char-ready?", 0, 1, TYPE_INPORT }, //
			{ op_set_inport, "set-input-port", 1, 1, TYPE_INPORT }, //
			{ op_set_outport, "set-output-port", 1, 1, TYPE_OUTPORT }, //
			{ op_read_sexpr, NULL, 0, 0, TYPE_NONE }, //
			{ op_read_list, NULL, 0, 0, TYPE_NONE }, //
			{ op_read_dot, NULL, 0, 0, TYPE_NONE }, //
			{ op_read_quote, NULL, 0, 0, TYPE_NONE }, //
			{ op_read_qquote, NULL, 0, 0, TYPE_NONE }, //
			{ op_read_qquote_vec, NULL, 0, 0, TYPE_NONE }, //
			{ op_read_unquote, NULL, 0, 0, TYPE_NONE }, //
			{ op_read_uqtsp, NULL, 0, 0, TYPE_NONE }, //
			{ op_read_vec, NULL, 0, 0, TYPE_NONE }, //
			{ op_p0list, NULL, 0, 0, TYPE_NONE }, //
			{ op_p1list, NULL, 0, 0, TYPE_NONE }, //
			{ op_pvecfrom, NULL, 0, 0, TYPE_NONE }, //
			{ op_list_length, "length", 1, 1, TYPE_LIST }, //
			{ op_assq, "assq", 2, 2, TYPE_NONE }, //
			{ op_get_closure, "get-closure-code", 1, 1, TYPE_NONE }, //
			{ op_closurep, "closure?", 1, 1, TYPE_NONE }, //
			{ op_macrop, "macro?", 1, 1, TYPE_NONE }, //
			{ op_system, "system", 0, 0xFFFF, TYPE_STRING }, //
			{ op_pipe, "pipe", 0, 0xFFFF, TYPE_STRING }, //
			//end
			};

	OperatorInfo *result = (OperatorInfo *) malloc(sizeof(OperatorInfo) * OPERATOR_COUNT);
	memcpy(result, operators, sizeof(OperatorInfo) * OPERATOR_COUNT);
	return result;
}

OperatorInfo *g_dispatch_table = NULL;

OperatorInfo get_dispatch_info(OperatorFunc op) {
	if (g_dispatch_table == NULL)
		g_dispatch_table = init_operators();

	int i = 0;
	for (i = 0; i < OPERATOR_COUNT; i++) {
		if (g_dispatch_table[i].func == op) {
			return g_dispatch_table[i];
		}
	}
	exit(1);
}

void eval(Scheme *sc, String statement) {
	sc->op = op_toplevel;
	Cell* port = port_from_string(sc, statement, statement + strlen(statement), PORT_INPUT);
	sc->args = port;
	sc->inPort = port;
	while (TRUE) {
		OperatorInfo dispatch = get_dispatch_info(sc->op);
		if (dispatch.func(sc) == &g_nil)
			break;
	}
}

int scheme_init(Scheme *sc) {
	int i = 0;
	init_constants();
	if (g_dispatch_table == NULL)
		g_dispatch_table = init_operators();

	sc->lastCellSeg = -1;
	sc->freeCell = &g_nil;
	sc->freeCellCount = 0;
	alloc_cellseg(sc, 3);

	sc->outPort = port_from_file(sc, stdout, PORT_OUTPUT);
	sc->inPort = port_from_file(sc, stdin, PORT_INPUT);

	sc->code = &g_nil;
	sc->callStack = &g_nil;

	sc->objectList = oblist_initial_value(sc);
	/* init global_env */
	new_frame_in_env(sc, &g_nil);
	sc->globalEnv = sc->env;

	add_syntax_symbol(sc, "lambda");
	add_syntax_symbol(sc, "quote");
	add_syntax_symbol(sc, "define");
	add_syntax_symbol(sc, "if");
	add_syntax_symbol(sc, "begin");
	add_syntax_symbol(sc, "set!");
	add_syntax_symbol(sc, "let");
	add_syntax_symbol(sc, "let*");
	add_syntax_symbol(sc, "letrec");
	add_syntax_symbol(sc, "cond");
	add_syntax_symbol(sc, "delay");
	add_syntax_symbol(sc, "and");
	add_syntax_symbol(sc, "or");
	add_syntax_symbol(sc, "cons-stream");
	add_syntax_symbol(sc, "macro");
	add_syntax_symbol(sc, "case");

	/* initialization of global cell*s to special symbols */
	sc->sym_lambda = make_symbol(sc, "lambda");
	sc->sym_quote = make_symbol(sc, "quote");
	sc->sym_qquote = make_symbol(sc, "quasiquote");
	sc->sym_unquote = make_symbol(sc, "unquote");
	sc->sym_unquote_sp = make_symbol(sc, "unquote-splicing");
	sc->sym_feed_to = make_symbol(sc, "=>");
	sc->sym_colon_hook = make_symbol(sc, "*colon-hook*");
	sc->sym_error_hook = make_symbol(sc, "*error-hook*");
	sc->sym_sharp_hook = make_symbol(sc, "*sharp-hook*");
	sc->sym_compile_hook = make_symbol(sc, "*compile-hook*");

	for (i = 0; i < OPERATOR_COUNT; i++) {
		if (g_dispatch_table[i].name != NULL) {
			Cell *sym = make_symbol(sc, g_dispatch_table[i].name);
			Cell *func = make_func(sc, g_dispatch_table[i].func);
			new_slot_in_env(sc, sym, func);
		}
	}

	return TRUE;
}

