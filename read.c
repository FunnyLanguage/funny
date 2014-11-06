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
#include "read.h"
#include "init.h"

#define MAX_WORD_LENGTH 128
#define MAX_STR_LENGTH  1024
#define PAREN_L         '{'
#define PAREN_R         '}'
#define PAREN_L_STR     "{"
#define PAREN_R_STR     "}"
#define LOOP_L			'['
#define LOOP_R			']'
#define LOOP_L_STR		"["
#define LOOP_R_STR		"]"
#define DELIMITER       ','

int is_delimiter(int c) 
{
    return isspace(c) || c == EOF || c == '('   || c == ')' || c == '"'   || c == ';';
}

int is_initial(int c) 
{
    return isalpha(c) || c == '*' || c == '/' || c == '>' || c == '<' || c == '=' || c == '?' || c == '!';
}

int peek(FILE *in) 
{
    int c = getc(in);
    ungetc(c, in);
    return c;
}

void eat_whitespace(FILE *in) 
{
    int c;
    
    while ((c = getc(in)) != EOF) 
	{
        if (isspace(c)) 
		{
            continue;
        }
        else if (c == ';')  /* comments are whitespace also */
		{
            while (((c = getc(in)) != EOF) && (c != '\n'));
            continue;
        }
        ungetc(c, in);
        break;
    }
}

void eat_expected_string(FILE *in, char *str) 
{
    int c;

    while (*str != '\0') 
	{
        c = getc(in);
        if (c != *str) 
		{
            fprintf(stderr, "unexpected character '%c'\n", c);
            return;
        }
        str++;
    }
}

void peek_expected_delimiter(FILE *in) 
{
    if (!is_delimiter(peek(in))) 
	{
        fprintf(stderr, "character not followed by delimiter\n");
    }
}

Object *read_character(FILE *in) 
{
    int c;

    c = getc(in);
    switch (c) 
	{
        case EOF:
            fprintf(stderr, "incomplete character literal\n");
            return NULL;
        case 's':
            if (peek(in) == 'p') 
			{
                eat_expected_string(in, "pace");
                peek_expected_delimiter(in);
                return make_character(' ');
            }
            break;
        case 'n':
            if (peek(in) == 'e') 
			{
                eat_expected_string(in, "ewline");
                peek_expected_delimiter(in);
                return make_character('\n');
            }
            break;
    }
    peek_expected_delimiter(in);
    return make_character(c);
}

Object *read_pair(FILE *in) 
{
    int c;
    Object *car_obj;
    Object *cdr_obj;
    
    eat_whitespace(in);
    
    c = getc(in);
    if (c == ')') /* read the empty list */
	{
        return the_empty_list;
    }
    ungetc(c, in);

    car_obj = read(in);

    eat_whitespace(in);
    
    c = getc(in);    
    if (c == '.') /* read improper list */
	{
        c = peek(in);
        if (!is_delimiter(c)) 
		{
            fprintf(stderr, "dot not followed by delimiter\n");
            return NULL;
        }
        cdr_obj = read(in);
        eat_whitespace(in);
        c = getc(in);
        if (c != ')') 
		{
            fprintf(stderr, "where was the trailing right paren?\n");
            return NULL;
        }
        return cons(car_obj, cdr_obj);
    }
    else /* read list */
	{
        ungetc(c, in);
        cdr_obj = read_pair(in);        
        return cons(car_obj, cdr_obj);
    }
}

Object *read(FILE *in) 
{
    int c;
    short sign = 1;
    int i;
    long num = 0;

    char buffer[MAX_STR_LENGTH];

    eat_whitespace(in);

    c = getc(in);    

    if (c == '#') /* read a boolean or character */
	{
        c = getc(in);
        switch (c) 
		{
            case 't':
                return true;
            case 'f':
                return false;
            case '\\':
                return read_character(in);
            default:
                fprintf(stderr, "unknown boolean or character literal\n");
                return NULL;
        }
    }
    else if (isdigit(c) || (c == '-' && (isdigit(peek(in))))) 
	{
		//TODO use strtod atof to make a NUMBER
		
        /* read a fixnum */
        if (c == '-') 
		{
            sign = -1;
        }
        else 
		{
            ungetc(c, in);
        }
        while (isdigit(c = getc(in))) 
		{
            num = (num * 10) + (c - '0');
        }
        num *= sign;
        if (is_delimiter(c)) 
		{
            ungetc(c, in);
            return make_fixnum(num);
        }
        else 
		{
            fprintf(stderr, "number not followed by delimiter\n");
            return NULL;
        }
    }
    else if (is_initial(c) || ((c == '+' || c == '-') && is_delimiter(peek(in)))) /* read a symbol */
	{
        i = 0;
        while (is_initial(c) || isdigit(c) || c == '+' || c == '-') 
		{
            /* subtract 1 to save space for '\0' terminator */
            if (i < MAX_STR_LENGTH - 1) 
			{
                buffer[i++] = c;
            }
            else 
			{
                fprintf(stderr, "symbol too long. Maximum length is %d\n", MAX_STR_LENGTH);
                return NULL;
            }
            c = getc(in);
        }
        if (is_delimiter(c)) 
		{
            buffer[i] = '\0';
            ungetc(c, in);
            return make_symbol(buffer);
        }
        else 
		{
            fprintf(stderr, "symbol not followed by delimiter. Found '%c'\n", c);
            return NULL;
        }
    }
    else if (c == '"') /* read a string */
	{
        i = 0;
        while ((c = getc(in)) != '"') 
		{
            if (c == '\\') 
			{
                c = getc(in);
                if (c == 'n') 
				{
                    c = '\n';
                }
            }
            if (c == EOF) 
			{
                fprintf(stderr, "non-terminated string literal\n");
                return NULL;
            }
            /* subtract 1 to save space for '\0' terminator */
            if (i < MAX_STR_LENGTH - 1) 
			{
                buffer[i++] = c;
            }
            else 
			{
                fprintf(stderr, "string too long. Maximum length is %d\n", MAX_STR_LENGTH);
                return NULL;
            }
        }
        buffer[i] = '\0';
        return make_string(buffer);
    }
    else if (c == '(') /* read the empty list or pair */
	{
        return read_pair(in);
    }
    else if (c == '\'') /* read quoted expression */
	{
        return cons(quote_symbol, cons(read(in), the_empty_list));
    }
    else if (c == EOF) 
	{
        return NULL;
    }
    else 
	{
        fprintf(stderr, "bad input. Unexpected '%c'\n", c);
        return NULL;
    }
    fprintf(stderr, "read illegal state\n");
    return NULL;
}

//  read from string  //

static int bufp = 0;

int str_getc(String buf)
{
	return buf[bufp++];
}

int str_ungetc(int c, String buf)
{
	if (bufp == 0)	return -1;
	else return buf[--bufp];
}

void str_rewind()
{
	bufp = 0;
}

int str_peek(String buf)
{
	return buf[bufp];
}

void str_eat_whitespace(String buf) 
{
    int c;
    
    while ((c = str_getc(buf)) != '\0') 
	{
        if (isspace(c)) 
		{
            continue;
        }
        else if (c == ';')  /* comments are whitespace also */
		{
            while (((c = str_getc(buf)) != '\0') && (c != '\n'));
            continue;
        }
        str_ungetc(c, buf);
        break;
    }
}

void str_eat_expected_string(String buf, char *str) 
{
    int c;

    while (*str != '\0') 
	{
        c = str_getc(buf);
        if (c != *str) {
            fprintf(stderr, "unexpected character '%c'\n", c);
            return;
        }
        str++;
    }
}

void str_peek_expected_delimiter(String buf) 
{
    if (!is_delimiter(str_peek(buf))) 
	{
        fprintf(stderr, "character not followed by delimiter\n");
    }
}

Object *str_read_character(String buf) 
{
    int c;

    c = str_getc(buf);
    switch (c) 
	{
        case '\0':
            fprintf(stderr, "incomplete character literal\n");
            return NULL;
			
        case 's':
            if (str_peek(buf) == 'p') 
			{
                str_eat_expected_string(buf, "pace");
                str_peek_expected_delimiter(buf);
                return make_character(' ');
            }
            break;
			
        case 'n':
            if (str_peek(buf) == 'e') 
			{
                str_eat_expected_string(buf, "ewline");
                str_peek_expected_delimiter(buf);
                return make_character('\n');
            }
            break;
    }
    str_peek_expected_delimiter(buf);
    return make_character(c);
}

Object *str_read_pair(String buf) 
{
    int c;
    Object *car_obj;
    Object *cdr_obj;
    
    str_eat_whitespace(buf);
    
    c = str_getc(buf);
    if (c == ')') /* read the empty list */
	{
        return the_empty_list;
    }
    str_ungetc(c, buf);

    car_obj = str_read(buf);

    str_eat_whitespace(buf);
    
    c = str_getc(buf);    
    if (c == '.') /* read improper list */
	{
        c = str_peek(buf);
        if (!is_delimiter(c)) 
		{
            fprintf(stderr, "dot not followed by delimiter\n");
            return NULL;
        }
        cdr_obj = str_read(buf);
        str_eat_whitespace(buf);
        c = str_getc(buf);
        if (c != ')') 
		{
            fprintf(stderr, "where was the trailing right paren?\n");
            return NULL;
        }
        return cons(car_obj, cdr_obj);
    }
    else /* read list */
	{
        str_ungetc(c, buf);
        cdr_obj = str_read_pair(buf);        
        return cons(car_obj, cdr_obj);
    }
}

Object *str_read(String buf) 
{
    int c;
    short sign = 1;
    int i;
    long num = 0;

    char buffer[MAX_STR_LENGTH];

    str_eat_whitespace(buf);

    c = str_getc(buf);    

    if (c == '#') /* read a boolean or character */
	{
        c = str_getc(buf);
        switch (c) 
		{
            case 't':
                return true;
            case 'f':
                return false;
            case '\\':
                return str_read_character(buf);
            default:
                fprintf(stderr, "unknown boolean or character literal\n");
                return NULL;
        }
    }
    else if (isdigit(c) || (c == '-' && (isdigit(str_peek(buf))))) 
	{
        /* read a fixnum */
        if (c == '-') 
		{
            sign = -1;
        }
        else 
		{
            str_ungetc(c, buf);
        }
        while (isdigit(c = str_getc(buf))) 
		{
            num = (num * 10) + (c - '0');
        }
        num *= sign;
        if (is_delimiter(c)) 
		{
            str_ungetc(c, buf);
            return make_fixnum(num);
        }
        else 
		{
            fprintf(stderr, "number not followed by delimiter\n");
            return NULL;
        }
    }
    else if (is_initial(c) || ((c == '+' || c == '-') && is_delimiter(str_peek(buf)))) /* read a symbol */
	{
        i = 0;
        while (is_initial(c) || isdigit(c) || c == '+' || c == '-') 
		{
            /* subtract 1 to save space for '\0' terminator */
            if (i < MAX_STR_LENGTH - 1) 
			{
                buffer[i++] = c;
            }
            else 
			{
                fprintf(stderr, "symbol too long. Maximum length is %d\n", MAX_STR_LENGTH);
                return NULL;
            }
            c = str_getc(buf);
        }
        if (is_delimiter(c)) 
		{
            buffer[i] = '\0';
            str_ungetc(c, buf);
            return make_symbol(buffer);
        }
        else 
		{
            fprintf(stderr, "symbol not followed by delimiter. Found '%c'\n", c);
            return NULL;
        }
    }
    else if (c == '"') /* read a string */
	{
        i = 0;
        while ((c = str_getc(buf)) != '"') 
		{
            if (c == '\\') 
			{
                c = str_getc(buf);
                if (c == 'n') 
				{
                    c = '\n';
                }
            }
            if (c == '\0') 
			{
                fprintf(stderr, "non-terminated string literal\n");
                return NULL;
            }
            /* subtract 1 to save space for '\0' terminator */
            if (i < MAX_STR_LENGTH - 1) 
			{
                buffer[i++] = c;
            }
            else 
			{
                fprintf(stderr, "string too long. Maximum length is %d\n", MAX_STR_LENGTH);
                return NULL;
            }
        }
        buffer[i] = '\0';
        return make_string(buffer);
    }
    else if (c == '(') /* read the empty list or pair */
	{
        return str_read_pair(buf);
    }
    else if (c == '\'') /* read quoted expression */
	{
        return cons(quote_symbol, cons(str_read(buf), the_empty_list));
    }
    else if (c == '\0') 
	{
        return NULL;
    }
    else 
	{
        fprintf(stderr, "bad input. Unexpected '%c'\n", c);
        return NULL;
    }
    fprintf(stderr, "read illegal state\n");
    return NULL;
}

// read_statement //

char *read_symbol(int c, FILE *reader);
char *read_symbol_escaped(int c, FILE *reader);
char *read_string(int c, FILE *reader);

String char_to_string(int c)
{
	String result = (String)malloc(2 * sizeof(char));
	exit_when_out_of_memory(result);
	result[0] = c;
	result[1] = '\0';
	return result;
}

int has_next(FILE *reader)
{
	int c = 0;
	
	while (TRUE) 
	{
		c = getc(reader);
		
		switch (c)
		{
		case -1:
			return FALSE;
			
		case ';':
			while ('\n' != c && -1 != c)
				c = getc(reader);
			break;
		
		case DELIMITER:
			c = getc(reader);
			
			if (!isspace(c) && DELIMITER != c) 
			{
				ungetc(c, reader);
				return TRUE;
			}
			break;
			
		default:
			if (!isspace(c)) 
			{
				ungetc(c, reader);
				return TRUE;
			}
		}
	}
}

String next(FILE *reader)
{
	int c = 0;
	
	while (TRUE) 
	{
		c = getc(reader);
		
		switch (c)
		{
		case -1:
			printf("Unexpected EOS\n");
			return NULL;
			
		case '\\':
			return read_symbol_escaped(c, reader);
			
		case PAREN_L:
			return char_to_string(PAREN_L);
			
		case PAREN_R:
			return char_to_string(PAREN_R);
			
		case DELIMITER:
			printf("Unexpected DELIMITER\n");
			return NULL;
			//return char_to_string(DELIMITER);
			
		case '"':
			return read_string(c, reader);
		
		default:
			return read_symbol(c, reader);
		}
	}
	
	return NULL;
}

String read_symbol(int c, FILE *reader)
{
	String buffer = (String)malloc(MAX_WORD_LENGTH * sizeof(char));
	exit_when_out_of_memory(buffer);
	int i=0;
	while (!isspace(c) && ';' != c && PAREN_L != c && PAREN_R != c && DELIMITER != c && -1 != c)
	{
		if ('\\' == c) 
		{
			c = getc(reader);
		}
		buffer[i++] = (char)c;
		c = getc(reader);
	}
	buffer[i] = '\0';
	
	ungetc(c, reader);
	return buffer;
}

String read_symbol_escaped(int c, FILE *reader)
{
	String buffer = (String)malloc(MAX_WORD_LENGTH * sizeof(char));
	exit_when_out_of_memory(buffer);
	int i=0;
	
	buffer[i++] = c;
	c = getc(reader);
	while (!isspace(c) && ';' != c && PAREN_L != c && PAREN_R != c && DELIMITER != c && -1 != c)
	{
		if ('\\' == c) 
		{
			c = getc(reader);
		}
		buffer[i++] = (char)c;
		c = getc(reader);
	}
	buffer[i] = '\0';
	
	ungetc(c, reader);
	return buffer;
}

String read_string(int c, FILE *reader)
{
	String buffer = (String)malloc(MAX_STR_LENGTH * sizeof(char));
	exit_when_out_of_memory(buffer);
	int i=0;
	
	buffer[i++] = c;
	c = getc(reader);
	while ('"' != c && -1 != c)
	{
		if ('\\' == c) 
		{
			buffer[i++] = (char)c;
			c = getc(reader);
		}
		buffer[i++] = (char)c;
		c = getc(reader);
	}
	buffer[i++] = '"';
	buffer[i] = '\0';
	
	return buffer;
}

Statement *parse_statement(String token, FILE *reader);
Statement *parse_expression(String token, FILE *reader);

String next_token(FILE *reader)
{
	if (!has_next(reader))
	{
		perror("Unexpected end of Object stream.\n");
		return NULL;
	}
	return next(reader);
}

Statement *parse_statement(String token, FILE *reader)
{
	Statement *root = NULL, *s = NULL;
	String the_next_token = NULL;
	
	root = init_arg();
	if (strcmp(PAREN_L_STR, token) == 0)
	{
		the_next_token = next_token(reader);
		while (strcmp(PAREN_R_STR, the_next_token) != 0)
		{
			s = parse_expression(the_next_token, reader);
			enqueue(root->children, s);
			the_next_token = next_token(reader);
		}
	}
	return root;
}

Statement *parse_expression(String token, FILE *reader)
{
	if (is_sym(token))
	{
		return init_sym(token);
	}
	else
	{
		return parse_statement(token, reader);
	}
}

int has_next_statement(FILE *reader)
{
	return has_next(reader);
}

Statement *next_statement(FILE *reader)
{
	String the_next_token = next(reader);
	Statement *s = parse_statement(the_next_token, reader);
	return s;
}

Statement *read_statement(FILE *in)
{
	Statement *s = NULL;
	eat_whitespace(in);
	s = next_statement(in);
	
	while (empty_statement(s) && peek(in) != '\n')
	{
		s = next_statement(in);
	}
	return s;
}

// parse_statement //

int str_has_next(String buf)
{
	int c;
	
	while (TRUE) 
	{
		c = str_getc(buf);
		
		switch (c)
		{
		case '\0':
			return FALSE;
			
		case ';':
			while ('\n' != c && -1 != c)
				c = str_getc(buf);
			break;
		
		case DELIMITER:
			c = str_getc(buf);
			
			if (!isspace(c) && DELIMITER != c) 
			{
				str_ungetc(c, buf);
				return TRUE;
			}
			break;
			
		default:
			if (!isspace(c)) 
			{
				str_ungetc(c, buf);
				return TRUE;
			}
		}
	}
}

String str_read_symbol(int c, String buf)
{
	String buffer = (String)malloc(MAX_WORD_LENGTH * sizeof(char));
	exit_when_out_of_memory(buffer);
	int i=0;
	while (!isspace(c) && ';' != c && PAREN_L != c && PAREN_R != c && DELIMITER != c && -1 != c)
	{
		if ('\\' == c) 
		{
			c = str_getc(buf);
		}
		buffer[i++] = (char)c;
		c = str_getc(buf);
	}
	buffer[i] = '\0';
	
	str_ungetc(c, buf);
	return buffer;
}

String str_read_symbol_escaped(int c, String buf)
{
	String buffer = (String)malloc(MAX_WORD_LENGTH * sizeof(char));
	exit_when_out_of_memory(buffer);
	int i=0;
	
	buffer[i++] = c;
	c = str_getc(buf);
	while (!isspace(c) && ';' != c && PAREN_L != c && PAREN_R != c && DELIMITER != c && -1 != c)
	{
		if ('\\' == c) 
		{
			c = str_getc(buf);
		}
		buffer[i++] = (char)c;
		c = str_getc(buf);
	}
	buffer[i] = '\0';
	
	str_ungetc(c, buf);
	return buffer;
}

String str_read_string(int c, String buf)
{
	int length = strlen(buf);
	String buffer;
	int i=0;

	if (length > MAX_STR_LENGTH)	length = MAX_STR_LENGTH;
	buffer = (String)malloc(length * sizeof(char));
	exit_when_out_of_memory(buffer);
	
	buffer[i++] = c;
	c = str_getc(buf);
	while ('"' != c && -1 != c)
	{
		if ('\\' == c) 
		{
			buffer[i++] = (char)c;
			c = str_getc(buf);
		}
		buffer[i++] = (char)c;
		c = str_getc(buf);
	}
	buffer[i++] = '"';
	buffer[i] = '\0';
	
	return buffer;
}

String str_next(String buf)
{
	int c;
	
	while (TRUE) 
	{
		c = str_getc(buf);
		
		switch (c)
		{
		case -1:
			printf("Unexpected EOS\n");
			return 0;
			
		case '\\':
			return str_read_symbol_escaped(c, buf);
			
		case PAREN_L:
			return char_to_string(PAREN_L);
			
		case PAREN_R:
			return char_to_string(PAREN_R);
			
		case DELIMITER:
			printf("Unexpected DELIMITER\n");
			return NULL;
			//return char_to_string(DELIMITER);
			
		case '"':
			return str_read_string(c, buf);
		
		default:
			return str_read_symbol(c, buf);
		}
	}
	
	return NULL;
}

String str_next_token(String buf)
{
	if (!str_has_next(buf))
	{
		perror("Unexpected end of Object stream.\n");
		return NULL;
	}
	return str_next(buf);
}

Statement *str_parse_expression(String token, String buf);
Statement *str_parse_statement(String token, String buf);

Statement *str_parse_statement(String token, String buf)
{
	Statement *root = NULL, *s = NULL;
	String next_token = NULL;
	
	root = init_arg();
	if (strcmp(PAREN_L_STR, token) == 0)
	{
		next_token = str_next_token(buf);
		while (strcmp(PAREN_R_STR, next_token) != 0)
		{
			s = str_parse_expression(next_token, buf);
			enqueue(root->children, s);
			next_token = str_next_token(buf);
		}
	}
	
	return root;
}

Statement *str_parse_expression(String token, String buf)
{
	if (is_sym(token))
	{
		return init_sym(token);
	}
	else
	{
		return str_parse_statement(token, buf);
	}
}

Statement *parse(String funny)
{
	String next_token = NULL;
	str_rewind();
	next_token = str_next(funny);
	return str_parse_statement(next_token, funny);
}

Statement *parse_regex(String funny)
{
	Statement *s = NULL;
	Statement *temp = NULL;
	String before = NULL;
	String between = NULL;
	String after = copy_string(funny);
	char *p = NULL;

	s = init_arg();
	while ((p = strchr(after, LOOP_L)))
	{
		before = (String)malloc((p - after + 1) * sizeof(char));
		*p = '\0';
		strcpy(before, after);
		trim_string(before);
		before = strcat(before, "}");

		if (strcmp("{}", before) != 0)
		{
			temp = parse(before);
			append_queue(s->children, temp->children);
			free(temp);
		}
		after = p + 1;
		p = strchr(after, LOOP_R);
		between = (String)malloc((p - after + 2) * sizeof(char));
		*p = '\0';
		strcpy(between, after);
		trim_string(between);
		
		after = between;
		while(*between++);
		*between++ = '}';
		*between++ = '\0';
		between--;
		while(between != after)
		{
			*(between + 1) = *between;
			between--;
		}
		*(between + 1) = *between;
		*between= '{';
		between = strcat(between, "}");

		if (strcmp("{}", between) != 0)
		{
			temp = parse(between);
			temp->type = LOOP;
			enqueue(s->children, temp);
		}

		after = p + 1;
		if (after && *after=='*')
		{
			*after = '{';
		}
	}
	if (strcmp("{}", after) != 0)
	{
		temp = parse(after);
		append_queue(s->children, temp->children);
		free(temp);
	}

	return s;
}
