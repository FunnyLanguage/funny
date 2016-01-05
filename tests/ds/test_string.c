/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2015 by fanguangping (fanguangping@163.com)
 * test_string.c
 */

#include <assert.h>
#include <string.h>
#include "test_string.h"
#include "../../src/ds/datastructure.h"

void test_alloc_string() {
	String str = alloc_string(1);
	assert(str != NULL);
}

void test_alloc_string_array() {
	String *str_arr = alloc_string_array(1);
	assert(str_arr != NULL);
}

void test_realloc_string() {
	String str = alloc_string(1);
	str = realloc_string(str, 2);
	assert(str != NULL);
}

void test_init_string() {
	String str = init_string();
	assert(str != NULL);
	assert(strlen(str) == 0);
}

void test_copy_string() {
	String src = "abc";
	String dest = copy_string(src);
	assert(dest != NULL);
	assert(strcmp(src, dest) == 0);
}

void test_trim_string() {
	String str = copy_string(" abc ");
	String lstr = copy_string("abc ");
	String rstr = copy_string(" abc");
	String trimed = "abc";
	assert(strcmp(trim_string(str), trimed) == 0);
	assert(strcmp(trim_string(lstr), trimed) == 0);
	assert(strcmp(trim_string(rstr), trimed) == 0);
}

void test_substr_before() {
	String str = copy_string("aaaaaa[bbbbbb]cccccc");
	String substr = substr_before(str, '[');
	assert(strcmp(substr, "aaaaaa") == 0);
}

void test_substr_between() {
	String str = copy_string("aaaaaa[bbbbbb]cccccc");
	String substr = substr_between(str, '[', ']');
	assert(strcmp(substr, "bbbbbb") == 0);
}

void test_substr_after() {
	String str = copy_string("aaaaaa[bbbbbb]cccccc");
	String substr = substr_after(str, ']');
	assert(strcmp(substr, "cccccc") == 0);
}

void test_dyna_strcat() {
	String str = copy_string("aaa");
	str = dyna_strcat(str, "bbb");
	assert(strcmp(str, "aaabbb") == 0);
}

void test_append_char() {
	String str = copy_string("aaa");
	str = append_char(str, '1');
	assert(strcmp(str, "aaa1") == 0);
}

void test_split_string() {
	String str = copy_string("aaaaaa|bbbbbb|cccccc");
	String *arr = split_string(str, '|');
	assert(strcmp(arr[0], "aaaaaa") == 0);
	assert(strcmp(arr[1], "bbbbbb") == 0);
	assert(strcmp(arr[2], "cccccc") == 0);
	assert(arr[3] == NULL);
}

void test_split_string_once() {
	String str = copy_string("aaaaaa|bbbbbb|cccccc");
	String *arr = split_string_once(str, '|');
	assert(strcmp(arr[0], "aaaaaa") == 0);
	assert(strcmp(arr[1], "bbbbbb|cccccc") == 0);
	assert(arr[2] == NULL);
}

void test_equals_string() {
	String str = copy_string("abc");
	assert(equals_string(str, "abc"));
	assert(equals_string(NULL, NULL));
	assert(!equals_string(str, NULL));
	assert(!equals_string(NULL, str));
}

void test_hash_code_string() {
	String str = copy_string("abc");
	int hashCode = hash_code_string("abc");
	assert(hashCode >= 0);
	assert(hashCode < HASHTABLE_SIZE);
	assert(hash_code_string(str) == hashCode);
}

void test_strlwr() {
	String str = copy_string("AbC");
	assert(strcmp(strlwr(str), "abc") == 0);
}

void test_string() {
	test_alloc_string();
	test_alloc_string_array();
	test_realloc_string();
	test_init_string();
	test_copy_string();
	test_trim_string();
	test_substr_before();
	test_substr_between();
	test_substr_after();
	test_dyna_strcat();
	test_append_char();
	test_split_string();
	test_split_string_once();
	test_equals_string();
	test_hash_code_string();
	test_strlwr();
}
