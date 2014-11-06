/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2014 by fanguangping. Email: fguangping@gmail.com
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Affero General Public
 * License version 3 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License version 3 for more details.
 *
 * You should have received a copy of the GNU Affero General Public
 * License version 3 along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */

#ifndef _INIT_H_
#define _INIT_H_

#include "ds.h"

/****************** init ******************/

Object *the_empty_list;
Object *false;
Object *true;
Object *symbol_table;
Object *quote_symbol;
Object *define_symbol;
Object *set_symbol;
Object *ok_symbol;
Object *if_symbol;
Object *lambda_symbol;
Object *begin_symbol;
Object *cond_symbol;
Object *else_symbol;
Object *let_symbol;
Object *and_symbol;
Object *or_symbol;
Object *eof_object;
Object *the_empty_environment;
Object *the_global_environment;

Object *cons(Object *car, Object *cdr);
Object *car(Object *pair);
void set_car(Object *obj, Object* value);
Object *cdr(Object *pair);
void set_cdr(Object *obj, Object* value);

void init();

#define caar(obj)   car(car(obj))
#define cadr(obj)   car(cdr(obj))
#define cdar(obj)   cdr(car(obj))
#define cddr(obj)   cdr(cdr(obj))
#define caaar(obj)  car(car(car(obj)))
#define caadr(obj)  car(car(cdr(obj)))
#define cadar(obj)  car(cdr(car(obj)))
#define caddr(obj)  car(cdr(cdr(obj)))
#define cdaar(obj)  cdr(car(car(obj)))
#define cdadr(obj)  cdr(car(cdr(obj)))
#define cddar(obj)  cdr(cdr(car(obj)))
#define cdddr(obj)  cdr(cdr(cdr(obj)))
#define caaaar(obj) car(car(car(car(obj))))
#define caaadr(obj) car(car(car(cdr(obj))))
#define caadar(obj) car(car(cdr(car(obj))))
#define caaddr(obj) car(car(cdr(cdr(obj))))
#define cadaar(obj) car(cdr(car(car(obj))))
#define cadadr(obj) car(cdr(car(cdr(obj))))
#define caddar(obj) car(cdr(cdr(car(obj))))
#define cadddr(obj) car(cdr(cdr(cdr(obj))))
#define cdaaar(obj) cdr(car(car(car(obj))))
#define cdaadr(obj) cdr(car(car(cdr(obj))))
#define cdadar(obj) cdr(car(cdr(car(obj))))
#define cdaddr(obj) cdr(car(cdr(cdr(obj))))
#define cddaar(obj) cdr(cdr(car(car(obj))))
#define cddadr(obj) cdr(cdr(car(cdr(obj))))
#define cdddar(obj) cdr(cdr(cdr(car(obj))))
#define cddddr(obj) cdr(cdr(cdr(cdr(obj))))

/****************** predicate ******************/

int is_the_empty_list(Object *obj);
int is_boolean(Object *obj);
int is_false(Object *obj);
int is_true(Object *obj);
int is_pair(Object *obj);
int is_symbol(Object *obj);
int is_fixnum(Object *obj);
int is_character(Object *obj);
int is_string(Object *obj);
int is_primitive_proc(Object *obj);
int is_compound_proc(Object *obj);
int is_input_port(Object *obj);
int is_output_port(Object *obj);
int is_eof_object(Object *obj);

/****************** is_..._proc ******************/

Object *is_null_proc(Object *arguments);
Object *is_boolean_proc(Object *arguments);
Object *is_symbol_proc(Object *arguments);
Object *is_integer_proc(Object *arguments);
Object *is_char_proc(Object *arguments);
Object *is_string_proc(Object *arguments);
Object *is_pair_proc(Object *arguments);
Object *is_procedure_proc(Object *arguments);
Object *is_input_port_proc(Object *arguments);
Object *is_output_port_proc(Object *arguments);
Object *is_eof_object_proc(Object *arguments);

/****************** make_..._proc ******************/

Object *make_symbol(String value);
Object *make_fixnum(long value);
Object *make_character(char value);
Object *make_string(String value);
Object *make_primitive_proc(Object *(*fn)(Object *arguments));
Object *make_compound_proc(Object *parameters, Object *body, Object* env);
Object *make_input_port(FILE *in);
Object *make_output_port(FILE *in);

/****************** ..._to_..._proc ******************/

Object *char_to_integer_proc(Object *arguments);
Object *integer_to_char_proc(Object *arguments);
Object *number_to_string_proc(Object *arguments);
Object *string_to_number_proc(Object *arguments);
Object *symbol_to_string_proc(Object *arguments);
Object *string_to_symbol_proc(Object *arguments);

/****************** add/sub/mul/div_proc ******************/

Object *add_proc(Object *arguments);
Object *sub_proc(Object *arguments);
Object *mul_proc(Object *arguments);
Object *quotient_proc(Object *arguments);
Object *remainder_proc(Object *arguments);
Object *is_number_equal_proc(Object *arguments);
Object *is_less_than_proc(Object *arguments);
Object *is_greater_than_proc(Object *arguments);

/****************** cons/car/cdr_proc ******************/

Object *cons_proc(Object *arguments);
Object *car_proc(Object *arguments);
Object *cdr_proc(Object *arguments);
Object *set_car_proc(Object *arguments);
Object *set_cdr_proc(Object *arguments);
Object *list_proc(Object *arguments);
Object *is_eq_proc(Object *arguments);

/****************** eval/apply_proc ******************/

Object *eval_proc(Object *arguments);
Object *apply_proc(Object *arguments);

/****************** io_proc ******************/

Object *load_proc(Object *arguments);
Object *open_input_port_proc(Object *arguments);
Object *close_input_port_proc(Object *arguments);
Object *read_proc(Object *arguments);
Object *read_char_proc(Object *arguments);
Object *peek_char_proc(Object *arguments);
Object *open_output_port_proc(Object *arguments);
Object *close_output_port_proc(Object *arguments);
Object *write_char_proc(Object *arguments);
Object *write_proc(Object *arguments);
Object *error_proc(Object *arguments);

/****************** env ******************/

Object *interaction_environment_proc(Object *arguments);
Object *null_environment_proc(Object *arguments);
Object *environment_proc(Object *arguments);
Object *enclosing_environment(Object *env);
Object *first_frame(Object *env);
Object *make_frame(Object *variables, Object *values);
Object *frame_variables(Object *frame);
Object *frame_values(Object *frame);
void add_binding_to_frame(Object *var, Object *val, Object *frame);
Object *extend_environment(Object *vars, Object *vals, Object *base_env);
Object *lookup_variable_value(Object *var, Object *env);
void set_variable_value(Object *var, Object *val, Object *env);
void define_variable(Object *var, Object *val, Object *env);
Object *setup_environment();
void populate_environment(Object *env);
Object *make_environment();

/****************** init_sys ******************/
Queue *mappings;
Queue *functions;

void load();
Queue *get_mappings();
Queue *get_functions();
Queue *parse_sys_macro();

#endif // _INIT_H_
