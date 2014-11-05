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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "init.h"
#include "read.h"
#include "eval.h"
#include "write.h"

/****************** init ******************/

Object *cons(Object *car, Object *cdr) 
{
    Object *obj = init_object();
    obj->type = PAIR;
    obj->data.pair.car = car;
    obj->data.pair.cdr = cdr;
    return obj;
}

Object *car(Object *pair) 
{
    return pair->data.pair.car;
}

void set_car(Object *obj, Object* value) 
{
    obj->data.pair.car = value;
}

Object *cdr(Object *pair) 
{
    return pair->data.pair.cdr;
}

void set_cdr(Object *obj, Object* value) 
{
    obj->data.pair.cdr = value;
}

void init(void) 
{
    the_empty_list = init_object();
    the_empty_list->type = THE_EMPTY_LIST;

    false = init_object();
    false->type = BOOLEAN;
    false->data.boolean.value = FALSE;

    true = init_object();
    true->type = BOOLEAN;
    true->data.boolean.value = TRUE;
    
    symbol_table = the_empty_list;
    quote_symbol = make_symbol("quote");
    define_symbol = make_symbol("define");
    set_symbol = make_symbol("set!");
    ok_symbol = make_symbol("ok");
    if_symbol = make_symbol("if");
    lambda_symbol = make_symbol("lambda");
    begin_symbol = make_symbol("begin");
    cond_symbol = make_symbol("cond");
    else_symbol = make_symbol("else");
    let_symbol = make_symbol("let");
    and_symbol = make_symbol("and");
    or_symbol = make_symbol("or");
    
    eof_object = init_object();
    eof_object->type = EOF_OBJECT;
    
    the_empty_environment = the_empty_list;

    the_global_environment = make_environment();
	
	load();
}

/****************** predicate ******************/

int is_the_empty_list(Object *obj) 
{
    return obj == the_empty_list;
}

int is_boolean(Object *obj) 
{
    return obj->type == BOOLEAN;
}

int is_false(Object *obj) 
{
    return obj == false;
}

int is_true(Object *obj) 
{
    return !is_false(obj);
}

int is_pair(Object *obj) 
{
    return obj->type == PAIR;
}

int is_symbol(Object *obj) 
{
    return obj->type == SYMBOL;
}

int is_fixnum(Object *obj) 
{
    return obj->type == FIXNUM;
}

int is_character(Object *obj) 
{
    return obj->type == CHARACTER;
}

int is_string(Object *obj) 
{
    return obj->type == STRING;
}

int is_primitive_proc(Object *obj) 
{
    return obj->type == PRIMITIVE_PROC;
}

int is_compound_proc(Object *obj) 
{
    return obj->type == COMPOUND_PROC;
}

int is_input_port(Object *obj) 
{
    return obj->type == INPUT_PORT;
}

int is_output_port(Object *obj) 
{
    return obj->type == OUTPUT_PORT;
}

int is_eof_object(Object *obj) 
{
    return obj == eof_object;
}

/****************** is_..._proc ******************/

Object *is_null_proc(Object *arguments) 
{
    return is_the_empty_list(car(arguments)) ? true : false;
}

Object *is_boolean_proc(Object *arguments) 
{
    return is_boolean(car(arguments)) ? true : false;
}

Object *is_symbol_proc(Object *arguments) 
{
    return is_symbol(car(arguments)) ? true : false;
}

Object *is_integer_proc(Object *arguments) 
{
    return is_fixnum(car(arguments)) ? true : false;
}

Object *is_char_proc(Object *arguments) 
{
    return is_character(car(arguments)) ? true : false;
}

Object *is_string_proc(Object *arguments) 
{
    return is_string(car(arguments)) ? true : false;
}

Object *is_pair_proc(Object *arguments) 
{
    return is_pair(car(arguments)) ? true : false;
}

Object *is_procedure_proc(Object *arguments) 
{
    Object *obj = car(arguments);
    return (is_primitive_proc(obj) || is_compound_proc(obj)) ? true : false;
}

Object *is_input_port_proc(Object *arguments) 
{
    return is_input_port(car(arguments)) ? true : false;
}

Object *is_output_port_proc(Object *arguments) 
{
    return is_output_port(car(arguments)) ? true : false;
}

Object *is_eof_object_proc(Object *arguments) 
{
    return is_eof_object(car(arguments)) ? true : false;
}

/****************** make_..._proc ******************/

Object *make_symbol(String value) 
{
    Object *obj = NULL;
    Object *element = NULL;
    
    /* search for they symbol in the symbol table */
    element = symbol_table;
    while (!is_the_empty_list(element)) 
	{
        if (strcmp(car(element)->data.symbol.value, value) == 0) 
		{
            return car(element);
        }
        element = cdr(element);
    }
    
    /* create the symbol and add it to the symbol table */
    obj = init_object();
    obj->type = SYMBOL;
    obj->data.symbol.value = (String)malloc(strlen(value) + 1);
    exit_when_out_of_memory(obj->data.symbol.value);
	strcpy(obj->data.symbol.value, value);
    symbol_table = cons(obj, symbol_table);
    return obj;
}

Object *make_fixnum(long value) 
{
    Object *obj = init_object();
    obj->type = FIXNUM;
    obj->data.fixnum.value = value;
    return obj;
}

Object *make_character(char value) 
{
    Object *obj = init_object();
    obj->type = CHARACTER;
    obj->data.character.value = value;
    return obj;
}

Object *make_string(String value) 
{
    Object *obj = init_object();
    obj->type = STRING;
    obj->data.string.value = (String)malloc(strlen(value) + 1);
    exit_when_out_of_memory(obj->data.string.value);
    strcpy(obj->data.string.value, value);
    return obj;
}

Object *make_primitive_proc(Object *(*fn)(Object *arguments)) 
{
    Object *obj = init_object();
    obj->type = PRIMITIVE_PROC;
    obj->data.primitive_proc.fn = fn;
    return obj;
}

Object *make_compound_proc(Object *parameters, Object *body, Object* env) 
{
    Object *obj = init_object();
    obj->type = COMPOUND_PROC;
    obj->data.compound_proc.parameters = parameters;
    obj->data.compound_proc.body = body;
    obj->data.compound_proc.env = env;
    return obj;
}

Object *make_input_port(FILE *stream) 
{
    Object *obj = init_object();
    obj->type = INPUT_PORT;
    obj->data.input_port.stream = stream;
    return obj;
}

Object *make_output_port(FILE *stream) 
{
    Object *obj = init_object();
    obj->type = OUTPUT_PORT;
    obj->data.output_port.stream = stream;
    return obj;
}

/****************** ..._to_..._proc ******************/

Object *char_to_integer_proc(Object *arguments) 
{
    return make_fixnum((car(arguments))->data.character.value);
}

Object *integer_to_char_proc(Object *arguments) 
{
    return make_character((car(arguments))->data.fixnum.value);
}

Object *number_to_string_proc(Object *arguments) 
{
    char buffer[100];
    sprintf(buffer, "%ld", (car(arguments))->data.fixnum.value);
    return make_string(buffer);
}

Object *string_to_number_proc(Object *arguments) 
{
    return make_fixnum(atoi((car(arguments))->data.string.value));
}

Object *symbol_to_string_proc(Object *arguments) 
{
    return make_string((car(arguments))->data.symbol.value);
}

Object *string_to_symbol_proc(Object *arguments) 
{
    return make_symbol((car(arguments))->data.string.value);
}

/****************** add/sub/mul/div_proc ******************/

Object *add_proc(Object *arguments) 
{
    long result = 0;
    
    while (!is_the_empty_list(arguments)) 
	{
        result += (car(arguments))->data.fixnum.value;
        arguments = cdr(arguments);
    }
    return make_fixnum(result);
}

Object *sub_proc(Object *arguments) 
{
    long result = (car(arguments))->data.fixnum.value;
	
    while (!is_the_empty_list(arguments = cdr(arguments))) 
	{
        result -= (car(arguments))->data.fixnum.value;
    }
    return make_fixnum(result);
}

Object *mul_proc(Object *arguments) 
{
    long result = 1;
    
    while (!is_the_empty_list(arguments)) 
	{
        result *= (car(arguments))->data.fixnum.value;
        arguments = cdr(arguments);
    }
    return make_fixnum(result);
}

Object *quotient_proc(Object *arguments) 
{
    return make_fixnum(
        ((car(arguments) )->data.fixnum.value)/
        ((cadr(arguments))->data.fixnum.value));
}

Object *remainder_proc(Object *arguments) 
{
    return make_fixnum(
        ((car(arguments) )->data.fixnum.value)%
        ((cadr(arguments))->data.fixnum.value));
}

Object *is_number_equal_proc(Object *arguments) 
{
    long value;
    
    value = (car(arguments))->data.fixnum.value;
    while (!is_the_empty_list(arguments = cdr(arguments))) 
	{
        if (value != ((car(arguments))->data.fixnum.value)) 
		{
            return false;
        }
    }
    return true;
}

Object *is_less_than_proc(Object *arguments) 
{
    long previous;
    long next;
    
    previous = (car(arguments))->data.fixnum.value;
    while (!is_the_empty_list(arguments = cdr(arguments))) 
	{
        next = (car(arguments))->data.fixnum.value;
        if (previous < next) 
		{
            previous = next;
        }
        else 
		{
            return false;
        }
    }
    return true;
}

Object *is_greater_than_proc(Object *arguments) 
{
    long previous;
    long next;
    
    previous = (car(arguments))->data.fixnum.value;
    while (!is_the_empty_list(arguments = cdr(arguments))) 
	{
        next = (car(arguments))->data.fixnum.value;
        if (previous > next) 
		{
            previous = next;
        }
        else 
		{
            return false;
        }
    }
    return true;
}

/****************** cons/car/cdr_proc ******************/

Object *cons_proc(Object *arguments) 
{
    return cons(car(arguments), cadr(arguments));
}

Object *car_proc(Object *arguments) 
{
    return caar(arguments);
}

Object *cdr_proc(Object *arguments) 
{
    return cdar(arguments);
}

Object *set_car_proc(Object *arguments) 
{
    set_car(car(arguments), cadr(arguments));
    return ok_symbol;
}

Object *set_cdr_proc(Object *arguments) 
{
    set_cdr(car(arguments), cadr(arguments));
    return ok_symbol;
}

Object *list_proc(Object *arguments) 
{
    return arguments;
}

Object *is_eq_proc(Object *arguments) 
{
    Object *obj1 = car(arguments);
    Object *obj2 = cadr(arguments);
    
    if (obj1->type != obj2->type) 
	{
        return false;
    }
    switch (obj1->type) 
	{
        case FIXNUM:
            return (obj1->data.fixnum.value == obj2->data.fixnum.value) ? true : false;
            break;
			
        case CHARACTER:
            return (obj1->data.character.value == obj2->data.character.value) ? true : false;
            break;
			
        case STRING:
            return (strcmp(obj1->data.string.value, obj2->data.string.value) == 0) ? true : false;
            break;
			
        default:
            return (obj1 == obj2) ? true : false;
    }
}

/****************** eval/apply_proc ******************/

Object *eval_proc(Object *arguments) 
{
    fprintf(stderr, "illegal state: The body of the eval primitive procedure should not execute.\n");
    return NULL;
}

Object *apply_proc(Object *arguments) 
{
    fprintf(stderr, "illegal state: The body of the apply primitive procedure should not execute.\n");
    return NULL;
}

/****************** io_proc ******************/

Object *load_proc(Object *arguments) 
{
    char *filename = NULL;
    FILE *in = NULL;
    Object *exp = NULL;
    Object *result = NULL;
    
    filename = car(arguments)->data.string.value;
    in = fopen(filename, "r");
    if (in == NULL) 
	{
        fprintf(stderr, "could not load file \"%s\"", filename);
        return NULL;
    }
    while ((exp = read(in)) != NULL) 
	{
        result = eval(exp, the_global_environment);
    }
    fclose(in);
    return result;
}

Object *open_input_port_proc(Object *arguments) 
{
    char *filename = NULL;
    FILE *in = NULL;

    filename = car(arguments)->data.string.value;
    in = fopen(filename, "r");
    if (in == NULL) 
	{
        fprintf(stderr, "could not open file \"%s\"\n", filename);
        return NULL;
    }
    return make_input_port(in);
}

Object *close_input_port_proc(Object *arguments) 
{
    int result;
    
    result = fclose(car(arguments)->data.input_port.stream);
    if (result == EOF) 
	{
        fprintf(stderr, "could not close input port\n");
        return NULL;
    }
    return ok_symbol;
}

Object *read_proc(Object *arguments) 
{
    FILE *in = NULL;
    Object *result = NULL;
    
    in = is_the_empty_list(arguments) ? stdin : car(arguments)->data.input_port.stream;
    result = read(in);
    return (result == NULL) ? eof_object : result;
}

Object *read_char_proc(Object *arguments) 
{
    FILE *in = NULL;
    int result;
    
    in = is_the_empty_list(arguments) ? stdin : car(arguments)->data.input_port.stream;
    result = getc(in);
    return (result == EOF) ? eof_object : make_character(result);
}

Object *peek_char_proc(Object *arguments) 
{
    FILE *in = NULL;
    int result;
    
    in = is_the_empty_list(arguments) ? stdin : car(arguments)->data.input_port.stream;
    result = peek(in);
    return (result == EOF) ? eof_object : make_character(result);
}

Object *open_output_port_proc(Object *arguments) 
{
    char *filename = NULL;
    FILE *out = NULL;

    filename = car(arguments)->data.string.value;
    out = fopen(filename, "w");
    if (out == NULL) 
	{
        fprintf(stderr, "could not open file \"%s\"\n", filename);
        return NULL;
    }
    return make_output_port(out);
}

Object *close_output_port_proc(Object *arguments) 
{
    int result;
    
    result = fclose(car(arguments)->data.output_port.stream);
    if (result == EOF) 
	{
        fprintf(stderr, "could not close output port\n");
        return NULL;
    }
    return ok_symbol;
}

Object *write_char_proc(Object *arguments) 
{
    Object *character = NULL;
    FILE *out = NULL;
    
    character = car(arguments);
    arguments = cdr(arguments);
    out = is_the_empty_list(arguments) ? stdout : car(arguments)->data.output_port.stream;
    putc(character->data.character.value, out);    
    fflush(out);
    return ok_symbol;
}

Object *write_proc(Object *arguments) 
{
    Object *exp = NULL;
    FILE *out = NULL;
    
    exp = car(arguments);
    arguments = cdr(arguments);
    out = is_the_empty_list(arguments) ? stdout : car(arguments)->data.output_port.stream;
    write(out, exp);
    fflush(out);
    return ok_symbol;
}

Object *error_proc(Object *arguments) 
{
    while (!is_the_empty_list(arguments)) 
	{
        write(stderr, car(arguments));
        fprintf(stderr, " ");
        arguments = cdr(arguments);
    };
    //printf("\nexiting\n");
    //exit(1);
	printf("\n");
	return NULL;
}

/****************** env ******************/

Object *interaction_environment_proc(Object *arguments) 
{
    return the_global_environment;
}

Object *null_environment_proc(Object *arguments) 
{
    return setup_environment();
}

Object *environment_proc(Object *arguments) 
{
    return make_environment();
}

Object *enclosing_environment(Object *env) 
{
    return cdr(env);
}

Object *first_frame(Object *env) 
{
    return car(env);
}

Object *make_frame(Object *variables, Object *values) 
{
    return cons(variables, values);
}

Object *frame_variables(Object *frame) 
{
    return car(frame);
}

Object *frame_values(Object *frame) 
{
    return cdr(frame);
}

void add_binding_to_frame(Object *var, Object *val, Object *frame) 
{
    set_car(frame, cons(var, car(frame)));
    set_cdr(frame, cons(val, cdr(frame)));
}

Object *extend_environment(Object *vars, Object *vals, Object *base_env) 
{
    return cons(make_frame(vars, vals), base_env);
}

Object *lookup_variable_value(Object *var, Object *env) 
{
    Object *frame = NULL;
    Object *vars = NULL;
    Object *vals = NULL;
	
    while (!is_the_empty_list(env)) 
	{
        frame = first_frame(env);
        vars = frame_variables(frame);
        vals = frame_values(frame);
        while (!is_the_empty_list(vars)) 
		{
            if (var == car(vars)) 
			{
                return car(vals);
            }
            vars = cdr(vars);
            vals = cdr(vals);
        }
        env = enclosing_environment(env);
    }
    fprintf(stderr, "unbound variable, %s\n", var->data.symbol.value);
    return NULL;
}

void set_variable_value(Object *var, Object *val, Object *env) 
{
    Object *frame = NULL;
    Object *vars = NULL;
    Object *vals = NULL;

    while (!is_the_empty_list(env)) 
	{
        frame = first_frame(env);
        vars = frame_variables(frame);
        vals = frame_values(frame);
        while (!is_the_empty_list(vars)) 
		{
            if (var == car(vars)) 
			{
                set_car(vals, val);
                return;
            }
            vars = cdr(vars);
            vals = cdr(vals);
        }
        env = enclosing_environment(env);
    }
    fprintf(stderr, "unbound variable, %s\n", var->data.symbol.value);
}

void define_variable(Object *var, Object *val, Object *env) 
{
    Object *frame = NULL;
    Object *vars = NULL;
    Object *vals = NULL;
    
    frame = first_frame(env);    
    vars = frame_variables(frame);
    vals = frame_values(frame);

    while (!is_the_empty_list(vars)) 
	{
        if (var == car(vars)) 
		{
            set_car(vals, val);
            return;
        }
        vars = cdr(vars);
        vals = cdr(vals);
    }
    add_binding_to_frame(var, val, frame);
}

Object *setup_environment(void) 
{
    Object *initial_env;
    
    initial_env = extend_environment(the_empty_list, the_empty_list, the_empty_environment);
    return initial_env;
}

void populate_environment(Object *env) {

#define add_procedure(scheme_name, c_name)	define_variable(make_symbol(scheme_name), make_primitive_proc(c_name), env);

    add_procedure("null?"      , is_null_proc);
    add_procedure("boolean?"   , is_boolean_proc);
    add_procedure("symbol?"    , is_symbol_proc);
    add_procedure("integer?"   , is_integer_proc);
    add_procedure("char?"      , is_char_proc);
    add_procedure("string?"    , is_string_proc);
    add_procedure("pair?"      , is_pair_proc);
    add_procedure("procedure?" , is_procedure_proc);
    
    add_procedure("char->integer" , char_to_integer_proc);
    add_procedure("integer->char" , integer_to_char_proc);
    add_procedure("number->string", number_to_string_proc);
    add_procedure("string->number", string_to_number_proc);
    add_procedure("symbol->string", symbol_to_string_proc);
    add_procedure("string->symbol", string_to_symbol_proc);
      
    add_procedure("+"        , add_proc);
    add_procedure("-"        , sub_proc);
    add_procedure("*"        , mul_proc);
    add_procedure("quotient" , quotient_proc);
    add_procedure("remainder", remainder_proc);
    add_procedure("="        , is_number_equal_proc);
    add_procedure("<"        , is_less_than_proc);
    add_procedure(">"        , is_greater_than_proc);

    add_procedure("cons"    , cons_proc);
    add_procedure("car"     , car_proc);
    add_procedure("cdr"     , cdr_proc);
    add_procedure("set-car!", set_car_proc);
    add_procedure("set-cdr!", set_cdr_proc);
    add_procedure("list"    , list_proc);

    add_procedure("eq?", is_eq_proc);

    add_procedure("apply", apply_proc);
    
    add_procedure("interaction-environment", interaction_environment_proc);
    add_procedure("null-environment", null_environment_proc);
    add_procedure("environment"     , environment_proc);
    add_procedure("eval"            , eval_proc);

    add_procedure("load"             , load_proc);
    add_procedure("open-input-port"  , open_input_port_proc);
    add_procedure("close-input-port" , close_input_port_proc);
    add_procedure("input-port?"      , is_input_port_proc);
    add_procedure("read"             , read_proc);
    add_procedure("read-char"        , read_char_proc);
    add_procedure("peek-char"        , peek_char_proc);
    add_procedure("eof-object?"      , is_eof_object_proc);
    add_procedure("open-output-port" , open_output_port_proc);
    add_procedure("close-output-port", close_output_port_proc);
    add_procedure("output-port?"     , is_output_port_proc);
    add_procedure("write-char"       , write_char_proc);
    add_procedure("write"            , write_proc);

    add_procedure("error", error_proc);
}

Object *make_environment(void) 
{
    Object *env;
    
    env = setup_environment();
    populate_environment(env);
    return env;
}

/****************** init_sys ******************/

#define MAX_SIZE       512

#define COMMENT_CHAR   '$'
#define STATEMENT_CHAR 's'
#define STATEMENT_STR  "statement:"
#define PREPARE_CHAR   '>'
#define PREPARE_STR    ">"
#define EVAL_CHAR      '='
#define EVAL_STR       "="

#define SYS_MACRO      "sys.macro"
#define CORE_TEMPLATE  "core.fn"

void load_core_template();

void load()
{
	mappings = parse_sys_macro();
	load_core_template();
	functions = init_queue();
}

Queue *get_mappings()
{
	if (mappings == NULL)
	{
		load();
	}
	return mappings;
}

Queue *get_functions()
{
	if (functions == NULL)
	{
		functions = init_queue();
	}
	return functions;
}


Queue *parse_sys_macro()
{
	String source = NULL, prepare_str = NULL, eval_str = NULL;
	Queue *queue = init_queue();
	Mapping *mapping = NULL;

	FILE *fp = NULL;
	int c = 0, i = 0;

	fp = fopen ( SYS_MACRO , "rb" );
	if( !fp ) perror(SYS_MACRO), exit(1);
	
	while (TRUE)
	{
		c = getc(fp);
		
		if (c == -1)
			break;

		if (c == COMMENT_CHAR)
		{
			while (c != '\n')	c = getc(fp);
			continue;
		}
		
		if (c == STATEMENT_CHAR)
		{
			i = strlen(STATEMENT_STR);
			while (i-->0)	c = getc(fp);

			i = 0;
			source = (String)malloc(MAX_SIZE * sizeof(char));
			while (c != '\n')
			{
				source[i++] = c = getc(fp);
			}
			source[--i] = '\0';

			mapping = (Mapping *)malloc(sizeof(Mapping));
			exit_when_out_of_memory(mapping);
			mapping->sys = (SysBlock *)malloc(sizeof(SysBlock));
			exit_when_out_of_memory(mapping->sys);
			mapping->source = source;
			mapping->sourceStatement = parse(source);
			mapping->sourceFunc = NULL;
			mapping->starter = to_dfa(mapping->sourceStatement);
			
			mapping->target = NULL;
			mapping->targetStatement = NULL;
			mapping->targetFunc = NULL;
			
			continue;
		}
		else if (c == PREPARE_CHAR)
		{
			i = strlen(PREPARE_STR);
			while (i-->0)	c = getc(fp);

			i = 0;
			prepare_str = (String)malloc(MAX_SIZE * sizeof(char));
			exit_when_out_of_memory(prepare_str);
			while (c != '\n')
			{
				prepare_str[i++] = c = getc(fp);
			}
			prepare_str[--i] = '\0';

			mapping->sys->prepare = trim_string(prepare_str);
			continue;
		}
		else if (c == EVAL_CHAR)
		{
			i = strlen(EVAL_STR);
			while (i-->0)	c = getc(fp);

			i = 0;
			eval_str = (String)malloc(MAX_SIZE * sizeof(char));
			exit_when_out_of_memory(eval_str);
			while (c != '\n')
			{
				eval_str[i++] = c = getc(fp);
			}
			eval_str[--i] = '\0';

			mapping->sys->eval = trim_string(eval_str);
			enqueue(queue, mapping);
			continue;
		}
		else
		{
			while (c != '\n')	c = getc(fp);
			continue;
		}
	}
	
	fclose(fp);

	return queue;
}

void load_core_template()
{
	Statement *s = NULL;
	FILE *fp = NULL;
	
	fp = fopen ( CORE_TEMPLATE , "rb" );
	if( !fp ) perror(CORE_TEMPLATE), exit(1);

	while (has_next_statement(fp))
	{
		s = next_statement(fp);
		match(s);
	}

	fclose(fp);
}

