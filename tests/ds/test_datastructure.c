/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2015 by fanguangping (fanguangping@163.com)
 * test_datastructure.c
 */

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include "test_datastructure.h"

Statement *add_statement(Statement *root, String symbol, int type) {
	Statement *s = (Statement *)malloc(sizeof(Statement));
	s->symbol = symbol;
	s->type = type;
	if (s->type == SYM) {
		s->children = NULL;
	} else {
		s->children = init_queue();
	}
	enqueue(root->children, s);
	return s;
}

// {define function {add}: {add {a} {b}} as {{a} + {b}}}
Statement *create_statement() {
	Statement *s = (Statement *)malloc(sizeof(Statement));
	Statement *arg1 = NULL;
	Statement *arg2 = NULL;
	Statement *arg3 = NULL;
	Statement *arg4 = NULL;
	Statement *arg5 = NULL;
	Statement *arg6 = NULL;
	Statement *arg7 = NULL;
	s->symbol = NULL;
	s->type = ARG;
	s->children = init_queue();
	add_statement(s, "define", SYM);
	add_statement(s, "function", SYM);
	arg1 = add_statement(s, NULL, ARG);
	add_statement(arg1, "add", SYM);
	add_statement(s, ":", SYM);
	arg2 = add_statement(s, NULL, ARG);
	add_statement(arg2, "add", SYM);
	arg3 = add_statement(arg2, NULL, ARG);
	add_statement(arg3, "a", SYM);
	arg4 = add_statement(arg2, NULL, ARG);
	add_statement(arg4, "b", SYM);
	add_statement(s, "as", SYM);
	arg5 = add_statement(s, NULL, ARG);
	arg6 = add_statement(arg5, NULL, ARG);
	add_statement(arg6, "a", SYM);
	add_statement(arg5, "+", SYM);
	arg7 = add_statement(arg5, NULL, ARG);
	add_statement(arg7, "b", SYM);
	return s;
}

// {for the conditions: [when {a} then {b} ,]}
Statement *create_loop_statement() {
	Statement *s = (Statement *)malloc(sizeof(Statement));
	Statement *arg1 = NULL;
	Statement *arg2 = NULL;
	Statement *arg3 = NULL;
	s->symbol = NULL;
	s->type = ARG;
	s->children = init_queue();
	add_statement(s, "for", SYM);
	add_statement(s, "the", SYM);
	add_statement(s, "conditions:", SYM);
	arg1 = add_statement(s, NULL, LOOP);
	add_statement(arg1, "when", SYM);
	arg2 = add_statement(arg1, NULL, ARG);
	add_statement(arg2, "a", SYM);
	add_statement(arg1, "then", SYM);
	arg3 = add_statement(arg1, NULL, ARG);
	add_statement(arg3, "b", SYM);
	add_statement(arg1, ",", SYM);
	return s;
}

// {let [{a}={b},] eval: [{c}]}
Statement *create_loop_statement1() {
	Statement *s = (Statement *)malloc(sizeof(Statement));
	Statement *arg1 = NULL;
	Statement *arg2 = NULL;
	Statement *arg3 = NULL;
	Statement *arg4 = NULL;
	Statement *arg5 = NULL;
	s->symbol = NULL;
	s->type = ARG;
	s->children = init_queue();
	add_statement(s, "let", SYM);
	arg1 = add_statement(s, NULL, LOOP);
	arg2 = add_statement(arg1, NULL, ARG);
	add_statement(arg2, "a", SYM);
	add_statement(arg1, "=", SYM);
	arg3 = add_statement(arg1, NULL, ARG);
	add_statement(arg3, "b", SYM);
	add_statement(arg1, ",", SYM);
	add_statement(s, "eval:", SYM);
	arg4 = add_statement(s, NULL, LOOP);
	arg5 = add_statement(arg4, NULL, ARG);
	add_statement(arg5, "c", SYM);
	return s;
}

// {define template {name}: {template} as {statement}}
Statement *create_sys_statement() {
	Statement *s = (Statement *)malloc(sizeof(Statement));
	Statement *arg1 = NULL;
	Statement *arg2 = NULL;
	Statement *arg3 = NULL;
	s->symbol = NULL;
	s->type = ARG;
	s->children = init_queue();
	add_statement(s, "define", SYM);
	add_statement(s, "template", SYM);
	arg1 = add_statement(s, NULL, ARG);
	add_statement(arg1, "name", SYM);
	add_statement(s, ":", SYM);
	arg2 = add_statement(s, NULL, ARG);
	add_statement(arg2, "template", SYM);
	add_statement(s, "as", SYM);
	arg3 = add_statement(s, NULL, ARG);
	add_statement(arg3, "statement", SYM);
	return s;
}

Mapping *create_sys_mapping() {
	Mapping *m = (Mapping *) malloc(sizeof(Mapping));
	m->sys = (SysBlock *) malloc(sizeof(SysBlock));
	m->sys->name = copy_string(SYS_TEMPLATE_NAME);
	m->sys->prepare = copy_string(SYS_TEMPLATE_PREPARE);
	m->sys->eval = copy_string(SYS_TEMPLATE_EVAL);

	m->source = copy_string(SYS_TEMPLATE_STATEMENT);
	m->target = NULL;
	m->sourceFunc = NULL;
	m->targetFunc = NULL;
	m->sourceStatement = create_sys_statement();
	m->targetStatement = NULL;
	m->starter = to_dfa(m->sourceStatement);

	return m;
}

// {{x} + {y}} -> {(+ {x} {y})}
Mapping *create_mapping() {
	Mapping *m = (Mapping *) malloc(sizeof(Mapping));
	m->sys = NULL;

	m->source = copy_string("{{x} + {y}}");
	m->target = copy_string("{(+ {x} {y})}");
	m->sourceFunc = "+";
	m->targetFunc = "+";
	m->sourceStatement = create_source_statement3();
	m->targetStatement = create_target_statement3();
	m->starter = to_dfa(m->sourceStatement);

	return m;
}

// {{x} or {y}}
Statement *create_source_statement1() {
	Statement *s = (Statement *)malloc(sizeof(Statement));
	Statement *arg1 = NULL;
	Statement *arg2 = NULL;
	s->symbol = NULL;
	s->type = ARG;
	s->children = init_queue();
	arg1 = add_statement(s, NULL, ARG);
	add_statement(arg1, "x", SYM);
	add_statement(s, "or", SYM);
	arg2 = add_statement(s, NULL, ARG);
	add_statement(arg2, "y", SYM);
	return s;
}

// {(or {x} {y})}
Statement *create_target_statement1() {
	Statement *s = (Statement *)malloc(sizeof(Statement));
	Statement *arg1 = NULL;
	Statement *arg2 = NULL;
	s->symbol = NULL;
	s->type = ARG;
	s->children = init_queue();
	add_statement(s, "(or", SYM);
	arg1 = add_statement(s, NULL, ARG);
	add_statement(arg1, "x", SYM);
	arg2 = add_statement(s, NULL, ARG);
	add_statement(arg2, "y", SYM);
	add_statement(s, ")", SYM);
	return s;
}

// {for the conditions: [when {a} then {b} ,]}
Statement *create_source_statement2() {
	return create_loop_statement();
}

// {(cond [({a} {b})])}
Statement *create_target_statement2() {
	Statement *s = (Statement *)malloc(sizeof(Statement));
	Statement *arg1 = NULL;
	Statement *arg2 = NULL;
	Statement *arg3 = NULL;
	s->symbol = NULL;
	s->type = ARG;
	s->children = init_queue();
	add_statement(s, "(cond", SYM);
	arg1 = add_statement(s, NULL, LOOP);
	add_statement(arg1, "(", SYM);
	arg2 = add_statement(arg1, NULL, ARG);
	add_statement(arg2, "a", SYM);
	arg3 = add_statement(arg1, NULL, ARG);
	add_statement(arg3, "b", SYM);
	add_statement(arg1, ")", SYM);
	add_statement(s, ")", SYM);
	return s;
}

// {{x} + {y}}
Statement *create_source_statement3() {
	Statement *s = (Statement *)malloc(sizeof(Statement));
	Statement *arg1 = NULL;
	Statement *arg2 = NULL;
	s->symbol = NULL;
	s->type = ARG;
	s->children = init_queue();
	arg1 = add_statement(s, NULL, ARG);
	add_statement(arg1, "x", SYM);
	add_statement(s, "+", SYM);
	arg2 = add_statement(s, NULL, ARG);
	add_statement(arg2, "y", SYM);
	return s;
}

// {(+ {x} {y})}
Statement *create_target_statement3() {
	Statement *s = (Statement *)malloc(sizeof(Statement));
	Statement *arg1 = NULL;
	Statement *arg2 = NULL;
	s->symbol = NULL;
	s->type = ARG;
	s->children = init_queue();
	add_statement(s, "(+", SYM);
	arg1 = add_statement(s, NULL, ARG);
	add_statement(arg1, "x", SYM);
	arg2 = add_statement(s, NULL, ARG);
	add_statement(arg2, "y", SYM);
	add_statement(s, ")", SYM);
	return s;
}

// {{x} or {y}}
Statement *create_replacement1() {
	Statement *s = (Statement *)malloc(sizeof(Statement));
	Statement *arg1 = NULL;
	Statement *arg2 = NULL;
	s->symbol = NULL;
	s->type = ARG;
	s->children = init_queue();
	arg1 = add_statement(s, NULL, ARG);
	add_statement(arg1, "x", SYM);
	add_statement(s, "or", SYM);
	arg2 = add_statement(s, NULL, ARG);
	add_statement(arg2, "y", SYM);
	return s;
}

// {(or {x} {y})}
Statement *create_replacement2() {
	Statement *s = (Statement *)malloc(sizeof(Statement));
	Statement *arg1 = NULL;
	Statement *arg2 = NULL;
	s->symbol = NULL;
	s->type = ARG;
	s->children = init_queue();
	add_statement(s, "(or", SYM);
	arg1 = add_statement(s, NULL, ARG);
	add_statement(arg1, "x", SYM);
	arg2 = add_statement(s, NULL, ARG);
	add_statement(arg2, "y", SYM);
	add_statement(s, ")", SYM);
	return s;
}


// {{x} and {y}}
Statement *create_replacement3() {
	Statement *s = (Statement *)malloc(sizeof(Statement));
	Statement *arg1 = NULL;
	Statement *arg2 = NULL;
	s->symbol = NULL;
	s->type = ARG;
	s->children = init_queue();
	arg1 = add_statement(s, NULL, ARG);
	add_statement(arg1, "x", SYM);
	add_statement(s, "and", SYM);
	arg2 = add_statement(s, NULL, ARG);
	add_statement(arg2, "y", SYM);
	return s;
}

// {(and {x} {y})}
Statement *create_replacement4() {
	Statement *s = (Statement *)malloc(sizeof(Statement));
	Statement *arg1 = NULL;
	Statement *arg2 = NULL;
	s->symbol = NULL;
	s->type = ARG;
	s->children = init_queue();
	add_statement(s, "(and", SYM);
	arg1 = add_statement(s, NULL, ARG);
	add_statement(arg1, "x", SYM);
	arg2 = add_statement(s, NULL, ARG);
	add_statement(arg2, "y", SYM);
	add_statement(s, ")", SYM);
	return s;
}
