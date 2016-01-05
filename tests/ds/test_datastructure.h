/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2015 by fanguangping (fanguangping@163.com)
 * test_datastructure.h
 */

#ifndef TEST_DATASTRUCTURE_H_
#define TEST_DATASTRUCTURE_H_

#include "../../src/ds/datastructure.h"

#define STATEMENT_STRING        "{define function {add} : {add {a} {b}} as {{a} + {b}}}"
#define LOOP_STATEMENT_STRING   "{for the conditions: [when {a} then {b} ,]}"

#define SYS_TEMPLATE_STATEMENT  "{define template {name} : {template} as {statement}}"
#define SYS_TEMPLATE_NAME       "deftpl"
#define SYS_TEMPLATE_PREPARE    "EXTRACT[{name}], {template} > FUNCNAME[{statement}], {statement}"
#define SYS_TEMPLATE_EVAL       "NIL"

#define SOURCE_STATEMENT1       "{{x} or {y}}"
#define TARGET_STATEMENT1       "{(or {x} {y})}"
#define SOURCE_STATEMENT2       "{for the conditions: [when {a} then {b} ,]}"
#define TARGET_STATEMENT2       "{(cond [({a} {b})])}"

Statement *add_statement(Statement *root, String symbol, int type);
Statement *create_statement();
Statement *create_loop_statement();
Statement *create_loop_statement1();
Statement *create_sys_statement();
Mapping *create_sys_mapping();
Mapping *create_mapping();
Statement *create_source_statement1();
Statement *create_target_statement1();
Statement *create_source_statement2();
Statement *create_target_statement2();
Statement *create_source_statement3();
Statement *create_target_statement3();
Statement *create_replacement1();
Statement *create_replacement2();
Statement *create_replacement3();
Statement *create_replacement4();

#endif /* TEST_DATASTRUCTURE_H_ */
