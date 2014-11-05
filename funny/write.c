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
#include "write.h"
#include "init.h"

void write_pair(FILE *out, Object *pair) 
{
    Object *car_obj = NULL;
    Object *cdr_obj = NULL;
    
    car_obj = car(pair);
    cdr_obj = cdr(pair);
    write(out, car_obj);
    if (cdr_obj->type == PAIR) 
	{
        fprintf(out, " ");
        write_pair(out, cdr_obj);
    }
    else if (cdr_obj->type == THE_EMPTY_LIST) 
	{
        return;
    }
    else 
	{
        fprintf(out, " . ");
        write(out, cdr_obj);
    }
}

void write(FILE *out, Object *obj) 
{
    char c;
    char *str = NULL;
    
    switch (obj->type) 
	{
        case THE_EMPTY_LIST:
            fprintf(out, "()");
            break;
			
        case BOOLEAN:
            fprintf(out, "#%c", is_false(obj) ? 'f' : 't');
            break;
			
        case SYMBOL:
            fprintf(out, "%s", obj->data.symbol.value);
            break;
			
        case FIXNUM:
            fprintf(out, "%ld", obj->data.fixnum.value);
            break;
			
        case CHARACTER:
            c = obj->data.character.value;
            fprintf(out, "#\\");
            switch (c) 
			{
                case '\n':
                    fprintf(out, "newline");
                    break;
					
                case ' ':
                    fprintf(out, "space");
                    break;
					
                default:
                    putc(c, out);
            }
            break;
			
        case STRING:
            str = obj->data.string.value;
            putchar('"');
            while (*str != '\0') 
			{
                switch (*str) 
				{
                    case '\n':
                        fprintf(out, "\\n");
                        break;
						
                    case '\\':
                        fprintf(out, "\\\\");
                        break;
						
                    case '"':
                        fprintf(out, "\\\"");
                        break;
						
                    default:
                        putc(*str, out);
                }
                str++;
            }
            putchar('"');
            break;
			
        case PAIR:
            fprintf(out, "(");
            write_pair(out, obj);
            fprintf(out, ")");
            break;
			
        case PRIMITIVE_PROC:
            fprintf(out, "#<primitive-procedure>");
            break;
			
        case COMPOUND_PROC:
            fprintf(out, "#<compound-procedure>");
            break;
			
        case INPUT_PORT:
            fprintf(out, "#<input-port>");
            break;
			
        case OUTPUT_PORT:
            fprintf(out, "#<output-port>");
            break;
			
        case EOF_OBJECT:
            fprintf(out, "#<eof>");
            break;
			
        default:
            fprintf(stderr, "cannot write unknown type\n");
            return;
    }
}
