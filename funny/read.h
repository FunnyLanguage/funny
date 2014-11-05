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

#ifndef _READ_H_
#define _READ_H_

#include "ds.h"

int is_delimiter(int c);
int is_initial(int c);
int peek(FILE *in);
void eat_whitespace(FILE *in);
void eat_expected_string(FILE *in, char *str);
void peek_expected_delimiter(FILE *in);
Object *read_character(FILE *in);
Object *read_pair(FILE *in);
Object *read(FILE *in);

Statement *read_statement(FILE *in);

int has_next_statement(FILE *reader);
Statement *next_statement(FILE *reader);

Statement *parse(String funny);
Statement *parse_regex(String funny);

void str_rewind();
Object *str_read(String buf);

int str_has_next(String buf);
String str_next(String buf);

#endif // _READ_H_
