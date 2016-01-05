/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2015 by fanguangping (fanguangping@163.com)
 * string.h
 */

#ifndef STRING_H_
#define STRING_H_

typedef char* String;

String alloc_string(int size);
String *alloc_string_array(int size);
String realloc_string(String str, int size);

String init_string();
String copy_string(String str);
String trim_string(String str);
String substr_before(String str, char c);
String substr_between(String str, char a, char b);
String substr_after(String str, char c);
String dyna_strcat(String str, String append);
String append_char(String str, char c);
String* split_string(String string, const char delimiter);
String* split_string_once(String string, const char delimiter);
int equals_string(void *a, void *b);
int hash_code_string(void *key);
String strlwr(String str);

#endif /* STRING_H_ */
