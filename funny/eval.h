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

#ifndef _EVAL_H_
#define _EVAL_H_

#include "ds.h"

int is_self_evaluating(Object *exp);
int is_variable(Object *expression);
int is_tagged_list(Object *expression, Object *tag);

/****************** is_tagged ******************/

int is_quoted(Object *exp);
int is_assignment(Object *exp);
int is_definition(Object *exp);
int is_if(Object *exp);
int is_lambda(Object *exp);
int is_begin(Object *exp);
int is_cond(Object *exp);
int is_let(Object *exp);
int is_and(Object *exp);
int is_or(Object *exp);

Object *eval(Object *exp, Object *env);

String match(Statement *source);
String eval_sys(Mapping *mapping, Statement *source, Queue *repQueue);

Object *eval_statement(Statement *s, Object *env);

#endif // _EVAL_H_
