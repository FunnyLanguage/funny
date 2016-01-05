/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2015 by fanguangping (fanguangping@163.com)
 * test_scanner.h
 */

#ifndef TEST_SCANNER_H_
#define TEST_SCANNER_H_

void test_alloc_input();
void test_file_input();
void test_string_input();
void test_char_to_string();
void test_next();
void test_scanner();

/*
int get_char_from_input(Input *input);
int unget_char_from_input(int c, Input *input);
String char_to_string(int c);
int has_next(Input *input);
String next(Input *input);
String read_symbol(int c, Input *input, int escaped);
String read_string(int c, Input *input);
String next_token(Input *input);
 * */

#endif /* TEST_SCANNER_H_ */
