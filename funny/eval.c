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
#include "eval.h"
#include "init.h"
#include "read.h"

int is_self_evaluating(Object *exp) 
{
    return is_boolean(exp) || is_fixnum(exp) || is_character(exp) || is_string(exp);
}

int is_variable(Object *expression) 
{
    return is_symbol(expression);
}

int is_tagged_list(Object *expression, Object *tag) 
{
    Object *the_car = NULL;

    if (is_pair(expression)) 
	{
        the_car = car(expression);
        return is_symbol(the_car) && (the_car == tag);
    }
    return 0;
}

int is_quoted(Object *exp) 
{
    return is_tagged_list(exp, quote_symbol);
}

int is_assignment(Object *exp) 
{
    return is_tagged_list(exp, set_symbol);
}

int is_definition(Object *exp) 
{
    return is_tagged_list(exp, define_symbol);
}

int is_if(Object *exp) 
{
    return is_tagged_list(exp, if_symbol);
}

int is_lambda(Object *exp) 
{
    return is_tagged_list(exp, lambda_symbol);
}

int is_begin(Object *exp) 
{
    return is_tagged_list(exp, begin_symbol);
}

int is_cond(Object *exp) 
{
    return is_tagged_list(exp, cond_symbol);
}

int is_let(Object *exp) 
{
    return is_tagged_list(exp, let_symbol);
}

int is_and(Object *exp) 
{
    return is_tagged_list(exp, and_symbol);
}

int is_or(Object *exp) 
{
    return is_tagged_list(exp, or_symbol);
}


Object *text_of_quotation(Object *exp) {
    return cadr(exp);
}

Object *assignment_variable(Object *exp) {
    return car(cdr(exp));
}

Object *assignment_value(Object *exp) {
    return car(cdr(cdr(exp)));
}

Object *definition_variable(Object *exp) {
    if (is_symbol(cadr(exp))) {
        return cadr(exp);
    }
    else {
        return caadr(exp);
    }
}

Object *make_lambda(Object *parameters, Object *body);

Object *definition_value(Object *exp) {
    if (is_symbol(cadr(exp))) {
        return caddr(exp);
    }
    else {
        return make_lambda(cdadr(exp), cddr(exp));
    }
}

Object *make_if(Object *predicate, Object *consequent, Object *alternative) {
    return cons(if_symbol,
                cons(predicate,
                     cons(consequent,
                          cons(alternative, the_empty_list))));
}


Object *if_predicate(Object *exp) {
    return cadr(exp);
}

Object *if_consequent(Object *exp) {
    return caddr(exp);
}

Object *if_alternative(Object *exp) {
    if (is_the_empty_list(cdddr(exp))) {
        return false;
    }
    else {
        return cadddr(exp);
    }
}

Object *make_lambda(Object *parameters, Object *body) {
    return cons(lambda_symbol, cons(parameters, body));
}

Object *lambda_parameters(Object *exp) {
    return cadr(exp);
}

Object *lambda_body(Object *exp) {
    return cddr(exp);
}

Object *make_begin(Object *seq) {
    return cons(begin_symbol, seq);
}


Object *begin_actions(Object *exp) {
    return cdr(exp);
}

char is_last_exp(Object *seq) {
    return is_the_empty_list(cdr(seq));
}

Object *first_exp(Object *seq) {
    return car(seq);
}

Object *rest_exps(Object *seq) {
    return cdr(seq);
}

Object *cond_clauses(Object *exp) {
    return cdr(exp);
}

Object *cond_predicate(Object *clause) {
    return car(clause);
}

Object *cond_actions(Object *clause) {
    return cdr(clause);
}

int is_cond_else_clause(Object *clause) {
    return cond_predicate(clause) == else_symbol;
}

Object *sequence_to_exp(Object *seq) {
    if (is_the_empty_list(seq)) {
        return seq;
    }
    else if (is_last_exp(seq)) {
        return first_exp(seq);
    }
    else {
        return make_begin(seq);
    }
}

Object *expand_clauses(Object *clauses) {
    Object *first;
    Object *rest;
    
    if (is_the_empty_list(clauses)) {
        return false;
    }
    else {
        first = car(clauses);
        rest  = cdr(clauses);
        if (is_cond_else_clause(first)) {
            if (is_the_empty_list(rest)) {
                return sequence_to_exp(cond_actions(first));
            }
            else {
                fprintf(stderr, "else clause isn't last cond->if");
                return NULL;
            }
        }
        else {
            return make_if(cond_predicate(first),
                           sequence_to_exp(cond_actions(first)),
                           expand_clauses(rest));
        }
    }
}

Object *cond_to_if(Object *exp) {
    return expand_clauses(cond_clauses(exp));
}

Object *make_application(Object *operator, Object *operands) {
    return cons(operator, operands);
}

int is_application(Object *exp) {
    return is_pair(exp);
}

Object *operator(Object *exp) {
    return car(exp);
}

Object *operands(Object *exp) {
    return cdr(exp);
}

int is_no_operands(Object *ops) {
    return is_the_empty_list(ops);
}

Object *first_operand(Object *ops) {
    return car(ops);
}

Object *rest_operands(Object *ops) {
    return cdr(ops);
}

Object *let_bindings(Object *exp) {
    return cadr(exp);
}

Object *let_body(Object *exp) {
    return cddr(exp);
}

Object *binding_parameter(Object *binding) {
    return car(binding);
}

Object *binding_argument(Object *binding) {
    return cadr(binding);
}

Object *bindings_parameters(Object *bindings) {
    return is_the_empty_list(bindings) ?
               the_empty_list :
               cons(binding_parameter(car(bindings)),
                    bindings_parameters(cdr(bindings)));
}

Object *bindings_arguments(Object *bindings) {
    return is_the_empty_list(bindings) ?
               the_empty_list :
               cons(binding_argument(car(bindings)),
                    bindings_arguments(cdr(bindings)));
}

Object *let_parameters(Object *exp) {
    return bindings_parameters(let_bindings(exp));
}

Object *let_arguments(Object *exp) {
    return bindings_arguments(let_bindings(exp));
}

Object *let_to_application(Object *exp) {
    return make_application(
               make_lambda(let_parameters(exp),
                           let_body(exp)),
               let_arguments(exp));
}

Object *and_tests(Object *exp) {
    return cdr(exp);
}

Object *or_tests(Object *exp) {
    return cdr(exp);
}

Object *apply_operator(Object *arguments) {
    return car(arguments);
}

Object *prepare_apply_operands(Object *arguments) {
    if (is_the_empty_list(cdr(arguments))) {
        return car(arguments);
    }
    else {
        return cons(car(arguments),
                    prepare_apply_operands(cdr(arguments)));
    }
}

Object *apply_operands(Object *arguments) {
    return prepare_apply_operands(cdr(arguments));
}

Object *eval_expression(Object *arguments) {
    return car(arguments);
}

Object *eval_environment(Object *arguments) {
    return cadr(arguments);
}

Object *list_of_values(Object *exps, Object *env) {
    if (is_no_operands(exps)) {
        return the_empty_list;
    }
    else {
        return cons(eval(first_operand(exps), env),
                    list_of_values(rest_operands(exps), env));
    }
}

Object *eval_assignment(Object *exp, Object *env) {
    set_variable_value(assignment_variable(exp), 
                       eval(assignment_value(exp), env),
                       env);
    return ok_symbol;
}

Object *eval_definition(Object *exp, Object *env) {
    define_variable(definition_variable(exp), 
                    eval(definition_value(exp), env),
                    env);
    return ok_symbol;
}

Object *eval(Object *exp, Object *env) {
    Object *procedure;
    Object *arguments;
    Object *result;

tailcall:
    if (is_self_evaluating(exp)) 
	{
        return exp;
    }
    else if (is_variable(exp)) 
	{
        return lookup_variable_value(exp, env);
    }
    else if (is_tagged_list(exp, quote_symbol)) 
	{
        return text_of_quotation(exp);
    }
    else if (is_tagged_list(exp, set_symbol)) 
	{
        return eval_assignment(exp, env);
    }
    else if (is_tagged_list(exp, define_symbol)) 
	{
        return eval_definition(exp, env);
    }
    else if (is_tagged_list(exp, if_symbol)) 
	{
        exp = is_true(eval(if_predicate(exp), env)) ? if_consequent(exp) : if_alternative(exp);
        goto tailcall;
    }
    else if (is_tagged_list(exp, lambda_symbol)) 
	{
        return make_compound_proc(lambda_parameters(exp), lambda_body(exp), env);
    }
    else if (is_tagged_list(exp, begin_symbol)) 
	{
        exp = begin_actions(exp);
        while (!is_last_exp(exp)) 
		{
            eval(first_exp(exp), env);
            exp = rest_exps(exp);
        }
        exp = first_exp(exp);
        goto tailcall;
    }
    else if (is_tagged_list(exp, cond_symbol)) 
	{
        exp = cond_to_if(exp);
        goto tailcall;
    }
    else if (is_tagged_list(exp, let_symbol)) 
	{
        exp = let_to_application(exp);
        goto tailcall;
    }
    else if (is_tagged_list(exp, and_symbol)) 
	{
        exp = and_tests(exp);
        if (is_the_empty_list(exp)) 
		{
            return true;
        }
        while (!is_last_exp(exp)) 
		{
            result = eval(first_exp(exp), env);
            if (is_false(result)) 
			{
                return result;
            }
            exp = rest_exps(exp);
        }
        exp = first_exp(exp);
        goto tailcall;
    }
    else if (is_tagged_list(exp, or_symbol)) 
	{
        exp = or_tests(exp);
        if (is_the_empty_list(exp)) 
		{
            return false;
        }
        while (!is_last_exp(exp)) 
		{
            result = eval(first_exp(exp), env);
            if (is_true(result)) 
			{
                return result;
            }
            exp = rest_exps(exp);
        }
        exp = first_exp(exp);
        goto tailcall;
    }
    else if (is_application(exp)) 
	{
        procedure = eval(operator(exp), env);
        arguments = list_of_values(operands(exp), env);

        /* handle eval specially for tail call requirement */
        if (is_primitive_proc(procedure) && procedure->data.primitive_proc.fn == eval_proc) 
		{
            exp = eval_expression(arguments);
            env = eval_environment(arguments);
            goto tailcall;
        }

        /* handle apply specially for tail call requirement */
        if (is_primitive_proc(procedure) && procedure->data.primitive_proc.fn == apply_proc) 
		{
            procedure = apply_operator(arguments);
            arguments = apply_operands(arguments);
        }

        if (is_primitive_proc(procedure)) 
		{
            return (procedure->data.primitive_proc.fn)(arguments);
        }
        else if (is_compound_proc(procedure)) 
		{
            env = extend_environment( 
                       procedure->data.compound_proc.parameters,
                       arguments,
                       procedure->data.compound_proc.env);
            exp = make_begin(procedure->data.compound_proc.body);
            goto tailcall;
        }
        else 
		{
            fprintf(stderr, "unknown procedure type\n");
            return NULL;
        }
    }
    else 
	{
        fprintf(stderr, "cannot eval unknown expression type\n");
        return NULL;
    }
    fprintf(stderr, "eval illegal state\n");
    return NULL;
}

//  dfa  //

String match_one_arg(String symbol)
{
	Queue *functions = get_functions();
	Queue *mappings = get_mappings();
	Queue *temp = NULL;
	Statement *s = NULL;
	Mapping *mapping = NULL;
	
	// FUNCTIONS.contains(symbol)
	QueueNode *node = functions->head;
	while (node != NULL)
	{
		if (strcmp(symbol, node->element) == 0)
		{
			return symbol;
		}
		node = node->next;
	}
	
	//MAPPINGS
	node = mappings->head;
	while (node != NULL)
	{
		mapping = (Mapping *)node->element;
		if (mapping->sys != NULL)
		{
			node = node->next;
			continue;
		}
		temp = mapping->sourceStatement->children;
		if (temp != NULL && temp->head == temp->tail)
		{
			s = (Statement *)temp->head->element;
			if (s->type == SYM && strcmp(symbol, s->symbol) == 0)
			{
				return mapping->target;
			}
		}
		
		if (mapping->sourceFunc != NULL && strcmp(symbol, mapping->sourceFunc) == 0)
		{
			return mapping->targetFunc;
		}
		
		node = node->next;
	}
	
	return NULL;
}

String match_orignal(Statement *source)
{
	int orignal = TRUE;
	Queue *queue = source->children;
	QueueNode *node = queue->head;
	Statement *s = NULL;
	String result = init_string();
	
	while (node != NULL)
	{
		s = (Statement *)node->element;
		if (s->type == SYM)
		{
			result = dyna_strcat(result, s->symbol);
			result = dyna_strcat(result, " ");
		}
		else
		{
			orignal = FALSE;
			break;
		}
		
		node = node->next;
	}
	
	if (orignal)
	{
		return result;
	}
	else
	{
		free(result);
		return NULL;
	}
}

State *test_match(Mapping *mapping, Statement *source, Queue *repQueue)
{
	State *start = mapping->starter;
	State *next = NULL;
	QueueNode *node = NULL;
	Statement *s = NULL;
	
	if (source->children != NULL)
	{
		node = source->children->head;
		while (node != NULL)
		{
			s = (Statement *)node->element;
			next = transit(mapping->starter, start, s, repQueue);
			start = next;
			if (start == NULL) 
			{
				break;
			}
			node = node->next;
		}
	}

	return start;
}

String match(Statement *source)
{
	QueueNode *node = NULL;
	QueueNode *stnode = NULL;
	Mapping *mapping = NULL;
	String temp = NULL;
	String loopBuffer = NULL;
	String result = NULL;
	State *start = NULL;
	Statement *s = NULL;
	Statement *st = NULL;
	Queue *repQueue = NULL;
	int emptyLoop = FALSE;
	
	int matchTimes = 0;
	Queue *loop = NULL;
	QueueNode *loopNode = NULL;
	Statement *loopSt = NULL;
	int i = 0;
	int index = 0;
	
	// match one arg
	if (source->children != NULL && 
		source->children->head != NULL &&
		source->children->head == source->children->tail)
	{
		temp = to_string_statement((Statement *)source->children->head->element);
		result = match_one_arg(temp);
		if (result != NULL)
		{
			return result;
		}
	}
	
	// match orignal
	if (source->children != NULL && 
		source->children->head != NULL)
	{
		result = match_orignal(source);
		if (result != NULL)
		{
			return result;
		}
	}

	node = get_mappings()->head;
	while (node != NULL)
	{
		mapping = (Mapping *)node->element;
		
		if (repQueue == NULL)
		{
			repQueue = init_queue();
		}
		
		start = test_match(mapping, source, repQueue);
		if (start == NULL)
		{
			node = node->next;
			continue;
		}
		if (start->accept) 
		{
			if (mapping->sys != NULL)
			{
				return eval_sys(mapping, source, repQueue);
			}
			
			result = init_string();
			stnode = mapping->targetStatement->children->head;
			while (stnode != NULL)
			{
				s = (Statement *)stnode->element;
				temp = to_string_statement(s);
				
				switch (s->type)
				{
				case SYM:
					result = dyna_strcat(result, temp);
					result = dyna_strcat(result, " ");
					break;
					
				case ARG:
					result = dyna_strcat(result, match(find_replace_statement(repQueue, s)));
					break;
					
				case LOOP:
					matchTimes = 0;
					loop = s->children;
					loopNode = NULL;
					loopSt = NULL;
					i = 0;
					index = 0;
					
					if (loop != NULL)
					{
						loopNode = loop->head;
						while (loopNode != NULL)
						{
							loopSt = (Statement *)loopNode->element;
							if (loopSt->type == ARG)
							{
								Queue *loopQueue = find_loop_replace_statement(repQueue, loopSt);
								QueueNode *loopQueueNode = loopQueue->head;
								while (loopQueueNode != NULL)
								{
									matchTimes++;
									loopQueueNode = loopQueueNode->next;
								}
								
								break;
							}
							loopNode = loopNode->next;
						}
					}
					
					loopBuffer = init_string();
					
					for (i = 0; i < matchTimes; i++)
					{
						if (loop != NULL)
						{
							loopNode = loop->head;
							while (loopNode != NULL)
							{
								loopBuffer = dyna_strcat(loopBuffer, " ");
								
								loopSt = (Statement *)loopNode->element;
								if (loopSt->type == ARG)
								{
									index = 0;
									
									Queue *loopQueue = find_loop_replace_statement(repQueue, loopSt);
									QueueNode *loopQueueNode = loopQueue->head;
									while (loopQueueNode != NULL)
									{
										if (index == i)
										{
											st = (Statement *)get_queue_element(loopQueue, index);
											if (strcmp("{}", to_string_statement(st)) == 0)
											{
												emptyLoop = TRUE;
												break;
											}
											loopBuffer = dyna_strcat(loopBuffer, match(st));
											break;
										}
										index++;
										loopQueueNode = loopQueueNode->next;
									}
								}
								else
								{
									loopBuffer = dyna_strcat(loopBuffer, to_string_statement(loopSt));
								}
								if (emptyLoop)	break;
								loopNode = loopNode->next;
							}
						}
					}
					
					if (!emptyLoop) 
					{
						result = dyna_strcat(result, loopBuffer);
					}
					else
					{
						free(loopBuffer);
					}
					
					break;
					
				default:
					break;
				}
				
				stnode = stnode->next;
			}
		}

		node = node->next;
	}
	
	if (result == NULL)
	{
		printf("Cannot find mapping for statement: %s\n", to_string_statement(source));
	}

	return result;
}

//  eval_sys  //

String get_arg_list(Statement *s)
{
	QueueNode *node = NULL;
	Statement *st = NULL;
	String temp = NULL;
	String result = init_string();

	node = s->children->head;
	while (node != NULL) 
	{
		st = (Statement *)node->element;
		if (st->type == ARG)
		{
			temp = to_string_statement(st);
			temp = substr_between(temp, '{', '}');
			result = dyna_strcat(result, temp);
			result = dyna_strcat(result, " ");
		}
		node = node->next;
	}
	return trim_string(result);
}

String get_function_name(String func)
{
	String temp = copy_string(func);
	char *p = strchr(temp, '(');
	if (p)
	{
		*p = ' ';
		p = trim_string(p);
		temp = p;
		p = strchr(temp, ' ');
		if(p) *p = '\0';
		return temp;
	}
	else
	{
		return NULL;
	}
}

String get_mapping_target(Statement *s)
{
	QueueNode *node = NULL;
	Statement *st = NULL;
	String temp = NULL;
	String result = init_string();
	
	node = s->children->head;
	while (node != NULL) 
	{
		st = (Statement *)node->element;
		if (st->type == ARG)
		{
			temp = to_string_statement(st);
			result = dyna_strcat(result, temp);
			result = dyna_strcat(result, " ");
		}
		node = node->next;
	}
	result = trim_string(result);
	
	return result;
}

String next_macro_statement(String macro);
String parse_macro(Queue *repQueue, String macro);

String eval_sys(Mapping *m, Statement *source, Queue *repQueue)
{
	String prepare = copy_string(m->sys->prepare);
	String eval = copy_string(m->sys->eval);
	Mapping *mapping = NULL;
	char *p = NULL;
	
	if (strcmp(prepare, "NIL") != 0)
	{
		mapping = (Mapping *)malloc(sizeof(Mapping));
		mapping->sys = NULL;
		p = strchr(prepare, '>');
		if (p)	*p = '\0';
		String prepare_source = prepare;
		String prepare_target = p + 1;
		
		p = strchr(prepare_source, ',');
		if (p)	*p = '\0';
		String sourceFuncMacro = trim_string(prepare_source);
		String sourceMacro = trim_string(p + 1);
		
		mapping->source = parse_macro(repQueue, sourceMacro);
		mapping->sourceFunc = parse_macro(repQueue, sourceFuncMacro);
		mapping->sourceStatement = parse_regex(mapping->source);
		mapping->starter = to_dfa(mapping->sourceStatement);
		
		p = strchr(prepare_target, ',');
		if (p)	*p = '\0';
		String targetFuncMacro = trim_string(prepare_target);
		String targetMacro = trim_string(p + 1);
		
		mapping->target = parse_macro(repQueue, targetMacro);
		mapping->targetFunc = parse_macro(repQueue, targetFuncMacro);
		mapping->targetStatement = parse_regex(mapping->target);
		
		enqueue(get_mappings(), mapping);
	}
	
	if (strcmp(eval, "NIL") != 0)
	{
		return parse_macro(repQueue, eval);
	}
	else
	{
		return "";
	}
}

int is_macro_char(char c)
{
    return ( c=='^' || c=='%' || c=='#' || c=='@' || c=='&' );
}

String parse_macro(Queue *repQueue, String macro)
{
	String buf = init_string();
	String st = NULL;
	Statement *pst = NULL;
	Statement *s = NULL;
	String temp = NULL;
	char c;
	
	while (*macro)
	{
		c = *macro;
		
	    if (c == '{')
	    {
	        st = next_macro_statement(macro);
			while (*macro != '}')	macro++;
			macro++;
			pst = parse(st);
			s = find_replace_statement(repQueue, pst);
			temp = to_string_statement(s);
			buf = dyna_strcat(buf, temp);
	    }
	    else if (is_macro_char(c))
	    {
	        macro++;
	        st = next_macro_statement(macro);
			while (*macro != '}')	macro++;
			macro++;
			pst = parse(st);
			s = find_replace_statement(repQueue, pst);
	        
	        switch (c)
	        {
	        case '^' :
	            temp = substr_between(to_string_statement(s), '{', '}');
	            break;
	            
	        case '%' :
	            temp = get_function_name(substr_between(to_string_statement(s), '{', '}'));
			    if (temp == NULL)	return NULL;
			    break;
			
			case '#' :
			    temp = get_mapping_target(s);
			    break;
			
			case '@' :
			    temp = match(s);
			    break;
			
			case '&' :
			    temp = get_arg_list(s);
			    break;
	        }
	        
	        buf = dyna_strcat(buf, temp);
	    }
	    else if (c == '\\')
	    {
	        macro++;
	        buf = append_char(buf, *macro);
	        if (*macro)	macro++;
	    }
	    else
	    {
	        buf = append_char(buf, *macro);
	        if (*macro)	macro++;
	    }
	}
	
	return buf;
}

String next_macro_statement(String macro)
{
	String statement = (String)malloc(20 * sizeof(char));
	
	if (*macro == '{')
	{
		int i = 0;
		while (*macro != '}')
		{
			statement[i++] = *macro;
			macro++;
		}
		statement[i++] = *macro;
		macro++;
		statement[i++] = '\0';
		
		return statement;
	}
	
	return NULL;
}

Object *eval_statement(Statement *s, Object *env)
{
	String str = match(s);
	
	str_rewind();
	Object *exp = str_read(str);
	if (exp == NULL)	return NULL;
	return eval(exp, env);
}
