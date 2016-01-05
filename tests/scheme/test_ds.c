/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2015 by fanguangping (fanguangping@163.com)
 * test_ds.c
 */

#include <assert.h>
#include <string.h>
#include "../../src/scheme/operator.h"
#include "../../src/scheme/converter.h"
#include "test_ds.h"

void test_op(Scheme *sc, String input, String output) {
	Cell* result = eval(sc, input);
	assert(strcmp(cell2str(sc, result), output) == 0);
}

/*

	my_g_hash_table_insert(table, "sqrt", op_sqrt);
	my_g_hash_table_insert(table, "expt", op_expt);
	my_g_hash_table_insert(table, "floor", op_floor);
	my_g_hash_table_insert(table, "ceiling", op_ceiling);
	my_g_hash_table_insert(table, "truncate", op_truncate);
	my_g_hash_table_insert(table, "round", op_round);
	my_g_hash_table_insert(table, "+", op_add);
	my_g_hash_table_insert(table, "*", op_mul);
	my_g_hash_table_insert(table, "-", op_sub);
	my_g_hash_table_insert(table, "/", op_div);
	my_g_hash_table_insert(table, "quotient", op_intdiv);
	my_g_hash_table_insert(table, "remainder", op_rem);
	my_g_hash_table_insert(table, "modulo", op_mod);

	my_g_hash_table_insert(table, "$toplevel", op_toplevel);
	my_g_hash_table_insert(table, "quit", op_quit);
	my_g_hash_table_insert(table, "$eval-internal", op_eval_internal);
	my_g_hash_table_insert(table, "$read-internal", op_read_internal);
	my_g_hash_table_insert(table, "$read-sexpr", op_read_sexpr);
	my_g_hash_table_insert(table, "$eval", op_eval);
	my_g_hash_table_insert(table, "$e0args", op_e0args);
	my_g_hash_table_insert(table, "$domacro", op_domacro);
	my_g_hash_table_insert(table, "$apply", op_apply);
	my_g_hash_table_insert(table, "$e1args", op_e1args);
	my_g_hash_table_insert(table, "$begin", op_begin);
	my_g_hash_table_insert(table, "$quote", op_quote);
	my_g_hash_table_insert(table, "$lambda", op_lambda);
	my_g_hash_table_insert(table, "$lambda1", op_lambda1);
	my_g_hash_table_insert(table, "make-closure", op_make_closure);
	my_g_hash_table_insert(table, "$def0", op_def0);
	my_g_hash_table_insert(table, "$def1", op_def1);
	my_g_hash_table_insert(table, "defined?", op_defp);
	my_g_hash_table_insert(table, "$set0", op_set0);
	my_g_hash_table_insert(table, "$set1", op_set1);
	my_g_hash_table_insert(table, "$if0", op_if0);
	my_g_hash_table_insert(table, "$if1", op_if1);
	my_g_hash_table_insert(table, "$let0", op_let0);
	my_g_hash_table_insert(table, "$let1", op_let1);
	my_g_hash_table_insert(table, "$let2", op_let2);
	my_g_hash_table_insert(table, "$letstar0", op_letstar0);
	my_g_hash_table_insert(table, "$letstar1", op_letstar1);
	my_g_hash_table_insert(table, "$letstar2", op_letstar2);
	//my_g_hash_table_insert(table, "$p0list", op_p0list);
	my_g_hash_table_insert(table, "gensym", op_gensym);
	//my_g_hash_table_insert(table, "$default", op_default);
	my_g_hash_table_insert(table, "$let0rec", op_let0rec);
	my_g_hash_table_insert(table, "$let1rec", op_let1rec);
	my_g_hash_table_insert(table, "$let2rec", op_let2rec);
	my_g_hash_table_insert(table, "$cond0", op_cond0);
	my_g_hash_table_insert(table, "$cond1", op_cond1);
	my_g_hash_table_insert(table, "$case0", op_case0);
	my_g_hash_table_insert(table, "$case1", op_case1);
	my_g_hash_table_insert(table, "$and0", op_and0);
	my_g_hash_table_insert(table, "$and1", op_and1);
	my_g_hash_table_insert(table, "$or0", op_or0);
	my_g_hash_table_insert(table, "$or1", op_or1);
	my_g_hash_table_insert(table, "$c0stream", op_c0stream);
	my_g_hash_table_insert(table, "$c1stream", op_c1stream);
	my_g_hash_table_insert(table, "$macro0", op_macro0);
	my_g_hash_table_insert(table, "$macro1", op_macro1);
	my_g_hash_table_insert(table, "$delay", op_delay);
	my_g_hash_table_insert(table, "apply", op_papply);
	my_g_hash_table_insert(table, "eval", op_peval);
	my_g_hash_table_insert(table, "call-with-current-continuation", op_continuation);

	my_g_hash_table_insert(table, "car", op_car);
	my_g_hash_table_insert(table, "cdr", op_cdr);
	my_g_hash_table_insert(table, "cons", op_cons);
	my_g_hash_table_insert(table, "set-car!", op_setcar);
	my_g_hash_table_insert(table, "set-cdr!", op_setcdr);
	my_g_hash_table_insert(table, "char->integer", op_char2int);
	my_g_hash_table_insert(table, "integer->char", op_int2char);
	my_g_hash_table_insert(table, "char-upcase", op_char_upcase);
	my_g_hash_table_insert(table, "char-downcase", op_char_downcase);
	my_g_hash_table_insert(table, "string->symbol", op_str2sym);
	my_g_hash_table_insert(table, "string->atom", op_str2atom);
	my_g_hash_table_insert(table, "symbol->string", op_sym2str);
	my_g_hash_table_insert(table, "atom->string", op_atom2str);
	my_g_hash_table_insert(table, "make-string", op_makestring);
	my_g_hash_table_insert(table, "string-length", op_strlen);
	my_g_hash_table_insert(table, "string-ref", op_strref);
	my_g_hash_table_insert(table, "string-set!", op_strset);
	my_g_hash_table_insert(table, "string-append", op_strappend);
	my_g_hash_table_insert(table, "substring", op_substr);
	my_g_hash_table_insert(table, "vector", op_vector);
	my_g_hash_table_insert(table, "make-vector", op_makevector);
	my_g_hash_table_insert(table, "vector-length", op_veclen);
	my_g_hash_table_insert(table, "vector-ref", op_vecref);
	my_g_hash_table_insert(table, "vector-set!", op_vecset);
	my_g_hash_table_insert(table, "not", op_not);
	my_g_hash_table_insert(table, "boolean?", op_boolp);
	my_g_hash_table_insert(table, "eof-object?", op_eofobjp);
	my_g_hash_table_insert(table, "null?", op_nullp);
	my_g_hash_table_insert(table, "=", op_numeq);
	my_g_hash_table_insert(table, "<", op_less);
	my_g_hash_table_insert(table, ">", op_gre);
	my_g_hash_table_insert(table, "<=", op_leq);
	my_g_hash_table_insert(table, ">=", op_geq);
	my_g_hash_table_insert(table, "symbol?", op_symbolp);
	my_g_hash_table_insert(table, "number?", op_numberp);
	my_g_hash_table_insert(table, "string?", op_stringp);
	my_g_hash_table_insert(table, "integer?", op_integerp);
	my_g_hash_table_insert(table, "real?", op_realp);
	my_g_hash_table_insert(table, "char?", op_charp);
	my_g_hash_table_insert(table, "char-alphabetic?", op_charap);
	my_g_hash_table_insert(table, "char-numeric?", op_charnp);
	my_g_hash_table_insert(table, "char-whitespace?", op_charwp);
	my_g_hash_table_insert(table, "char-upper-case?", op_charup);
	my_g_hash_table_insert(table, "char-lower-case?", op_charlp);
	my_g_hash_table_insert(table, "procedure?", op_procp);
	my_g_hash_table_insert(table, "pair?", op_pairp);
	my_g_hash_table_insert(table, "list?", op_listp);
	my_g_hash_table_insert(table, "environment?", op_envp);
	my_g_hash_table_insert(table, "vector?", op_vectorp);
	my_g_hash_table_insert(table, "eq?", op_eq);
	my_g_hash_table_insert(table, "eqv?", op_eqv);
	my_g_hash_table_insert(table, "force", op_force);
	my_g_hash_table_insert(table, "$save-forced", op_save_forced);
	my_g_hash_table_insert(table, "print", op_print);
	my_g_hash_table_insert(table, "print-line", op_print_line);
	my_g_hash_table_insert(table, "error", op_err);
	my_g_hash_table_insert(table, "reverse", op_reverse);
	my_g_hash_table_insert(table, "list*", op_list_star);
	my_g_hash_table_insert(table, "append", op_append);
	my_g_hash_table_insert(table, "gc", op_gc);
	my_g_hash_table_insert(table, "gc-verbose", op_gcverb);
	my_g_hash_table_insert(table, "new-segment", op_newsegment);
	my_g_hash_table_insert(table, "oblist", op_oblist);
	my_g_hash_table_insert(table, "interaction-environment", op_int_env);
	my_g_hash_table_insert(table, "current-environment", op_curr_env);
	my_g_hash_table_insert(table, "$read-vector", op_read_vec);
	my_g_hash_table_insert(table, "$read-list", op_read_list);
	my_g_hash_table_insert(table, "$read-quote", op_read_quote);
	my_g_hash_table_insert(table, "$read-dot", op_read_dot);
	my_g_hash_table_insert(table, "length", op_list_length);
	my_g_hash_table_insert(table, "assq", op_assq);
	my_g_hash_table_insert(table, "get-closure-code", op_get_closure);
	my_g_hash_table_insert(table, "closure?", op_closurep);
	my_g_hash_table_insert(table, "macro?", op_macrop);
 * */
