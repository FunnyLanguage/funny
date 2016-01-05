/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2015 by fanguangping (fanguangping@163.com)
 * read.c
 */

#include <string.h>
#include <ctype.h>
#include "../ds/datastructure.h"
#include "operator.h"

/*
 * 读取函数
 *
 * */

static int get_char(Scheme *sc) {
	int ret = 0;
	ret = sc->input[(sc->inputPointer)++];
	if (ret == '\0')
		ret = EOF;
	return ret;
}

static void unget_char(Scheme *sc, int c) {
	if (sc->inputPointer == 0) {
		fprintf(stderr, "Error while get char!\n");
		return;
	}

	(sc->inputPointer)--;
}

/* check c is in chars */
static int find_char_in_string(char *s, int c) {
	if (c == EOF)
		return TRUE;
	while (*s) {
		if (*s++ == c)
			return TRUE;
	}
	return FALSE;
}

/* skip white characters */
static int skipspace(Scheme *sc) {
	int c = 0;
	do {
		c = get_char(sc);
	} while (isspace(c));
	if (c != EOF) {
		unget_char(sc, c);
		return 1;
	} else
		return EOF;
}

/* read characters up to delimiter, but cater to character constants */
//读取一个字符串直到遇到分隔符
static char *readstr_upto(Scheme *sc, char *delim) {
	String str = init_string();
	while(TRUE) {
		char c = get_char(sc);
		str = append_char(str, c);
		if (*str && !find_char_in_string(delim, c)) ;
		else
			break;
	}
	unget_char(sc, *(str - 1));
	*(str + strlen(str) - 1) = '\0';
	return str;
}

static Cell* readstrexp(Scheme *sc) {
	String str = init_string();

	int c1 = 0;
	enum {
		st_ok, st_bsl, st_x1, st_x2, st_oct1, st_oct2
	} state = st_ok;

	while (TRUE) {
		char c = get_char(sc);
		if (c == EOF) {
			return &g_false;
		}
		switch (state) {
		case st_ok:
			switch (c) {
			case '\\':
				state = st_bsl;
				break;
			case '"':
				return make_string_n(sc, str, strlen(str)); //free(str); ?
			default:
				str = append_char(str, c);
				break;
			}
			break;
		case st_bsl:
			switch (c) {
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
				state = st_oct1;
				c1 = c - '0';
				break;
			case 'x':
			case 'X':
				state = st_x1;
				c1 = 0;
				break;
			case 'n':
				str = append_char(str, '\n');
				state = st_ok;
				break;
			case 't':
				str = append_char(str, '\t');
				state = st_ok;
				break;
			case 'r':
				str = append_char(str, '\r');
				state = st_ok;
				break;
			case '"':
				str = append_char(str, '"');
				state = st_ok;
				break;
			default:
				str = append_char(str, c);
				state = st_ok;
				break;
			}
			break;
		case st_x1:
		case st_x2:
			c = toupper(c);
			if (c >= '0' && c <= 'F') {
				if (c <= '9') {
					c1 = (c1 << 4) + c - '0';
				} else {
					c1 = (c1 << 4) + c - 'A' + 10;
				}
				if (state == st_x1) {
					state = st_x2;
				} else {
					str = append_char(str, c1);
					state = st_ok;
				}
			} else {
				return &g_false;
			}
			break;
		case st_oct1:
		case st_oct2:
			if (c < '0' || c > '7') {
				str = append_char(str, c1);
				unget_char(sc, c);
				state = st_ok;
			} else {
				if (state == st_oct2 && c1 >= 32)
					return &g_false;

				c1 = (c1 << 3) + (c - '0');

				if (state == st_oct1)
					state = st_oct2;
				else {
					str = append_char(str, c1);
					state = st_ok;
				}
			}
			break;
		}
	}
}

/* get token */
static int token(Scheme *sc) {
	int c = skipspace(sc);
	if (c == EOF)
		return (TOK_EOF);
	switch (c = get_char(sc)) {
	case EOF:
		return (TOK_EOF);
	case '(':
		return (TOK_LPAREN);
	case ')':
		return (TOK_RPAREN);
	case '.':
		c = get_char(sc);
		if (find_char_in_string(" \n\t", c)) {
			return (TOK_DOT);
		} else {
			unget_char(sc, c);
			unget_char(sc, '.');
			return TOK_ATOM;
		}
	case '\'': //引用
		return (TOK_QUOTE);
	case ';': //注释
		while ((c = get_char(sc)) != '\n' && c != EOF)
			; //跳过注释
		if (c == EOF)
			return TOK_EOF;
		else
			return token(sc);
	case '"': //双引号
		return (TOK_DQUOTE);
	case '#':
		c = get_char(sc);
		if (c == '(')
			return TOK_VECTOR;
		else if (c == '!') { //#!开头的注释
			while ((c = get_char(sc)) != '\n' && c != EOF)
				;
			if (c == EOF)
				return TOK_EOF;
			return token(sc);
		} else {
			unget_char(sc, c);
			if (find_char_in_string(" tfodxb\\", c))
				return TOK_SHARP_CONST;
			else
				return (TOK_SHARP);
		}
	default:
		unget_char(sc, c);
		return (TOK_ATOM);
	}
}

Cell* op_read_internal(Scheme *sc) {
	sc->token = token(sc);
	if (sc->token == TOK_EOF)
		return s_return_helper(sc, sc->returnValue); //到这里，表示端口中的代码被读完了，应当返回nil
	sc->op = op_read_sexpr;
	return &g_true;
}

Cell* op_read_sexpr(Scheme *sc) {
	Cell* x;
	char *temp;

	switch (sc->token) {
	case TOK_EOF:
		return s_return_helper(sc, &g_eof);
	case TOK_VECTOR:
		s_save(sc, op_read_vec, &g_nil, &g_nil); //压入 数组 处理过程
		break;
		/* no break */
		/* fall through */
	case TOK_LPAREN:
		sc->token = token(sc);
		if (sc->token == TOK_RPAREN) {
			return s_return_helper(sc, &g_nil);
		} else if (sc->token == TOK_DOT) {
			return error_helper(sc, "syntax error: illegal dot expression",
			NULL);
		} else {
			//sc->nesting_stack[sc->top_file_index]++;
			s_save(sc, op_read_list, &g_nil, &g_nil); //压入读取列表的函数
			//读取S表达式
			sc->op = op_read_sexpr;
			return &g_true;
		}
	case TOK_QUOTE: //引用
		s_save(sc, op_read_quote, &g_nil, &g_nil); //压入 引用 处理函数
		sc->token = token(sc);
		//读取S表达式
		sc->op = op_read_sexpr;
		return &g_true;
	case TOK_ATOM: //原子
		temp = readstr_upto(sc, DELIMITERS);
		// printf("%s\n", temp);

		return s_return_helper(sc,
				make_atom_from_string(sc, temp));
	case TOK_DQUOTE: //双引号 (字符串类型的原子)
		x = readstrexp(sc);
		if (x == &g_false)
			return error_helper(sc, "Error reading string", NULL);
		set_immutable(x);
		return s_return_helper(sc, x);
	case TOK_SHARP: { //#是eval的简写吗？
		Cell* f = find_slot_in_env(sc, sc->env, sc->sym_sharp_hook, TRUE); //f的类型symbol_kv ,f是一段处理#的代码
		if (f == &g_nil)
			return error_helper(sc, "undefined sharp expression", NULL);
		sc->code = cons(sc, slot_value_in_env(f), &g_nil);
		sc->op = op_eval;
		return &g_true;
	}
		/* no break */
	case TOK_SHARP_CONST: //常量
		x = make_sharp_const(sc, readstr_upto(sc, DELIMITERS));
		if (x == &g_nil)
			return error_helper(sc, "undefined const sharp expression", NULL);
		return s_return_helper(sc, x);
	default:
		return error_helper(sc, "syntax error: illegal token", NULL);
	}

	return NULL;
}

Cell* op_read_list(Scheme *sc) {
	sc->args = cons(sc, sc->returnValue, sc->args);
	sc->token = token(sc);
	if (sc->token == TOK_EOF)
		return s_return_helper(sc, &g_eof);
	else if (sc->token == TOK_RPAREN) {
		return s_return_helper(sc, reverse(sc, sc->args));
	} else if (sc->token == TOK_DOT) {
		s_save(sc, op_read_dot, sc->args, &g_nil);
		sc->token = token(sc);
		sc->op = op_read_sexpr;
		return &g_true;
	} else {
		s_save(sc, op_read_list, sc->args, &g_nil);
		sc->op = op_read_sexpr;
		return &g_true;
	}
}

Cell* op_read_vec(Scheme *sc) {
	sc->args = sc->returnValue;
	sc->op = op_vector;
	return &g_true;
}

Cell* op_read_dot(Scheme *sc) {
	if (token(sc) != TOK_RPAREN) {
		return error_helper(sc, "syntax error: illegal dot expression", NULL);
	} else {
		return s_return_helper(sc, revappend(sc, sc->returnValue, sc->args));
	}
}

Cell* op_read_quote(Scheme *sc) {
	return s_return_helper(sc,
			cons(sc, sc->sym_quote, cons(sc, sc->returnValue, &g_nil)));
}
