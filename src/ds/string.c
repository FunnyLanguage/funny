/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2015 by fanguangping (fanguangping@163.com)
 * string.c
 */

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "datastructure.h"

String alloc_string(int size) {
	String result = (String) malloc(sizeof(char) * size);
	exit_when_out_of_memory(result);
	return result;
}

String *alloc_string_array(int size) {
	String *result = (String*) malloc(sizeof(String) * size);
	exit_when_out_of_memory(result);
	return result;
}

String realloc_string(String str, int size) {
	str = (String) realloc(str, size * sizeof(char));
	exit_when_out_of_memory(str);
	return str;
}

String init_string() {
	String result = alloc_string(1);
	result[0] = '\0';
	return result;
}

String copy_string(String str) {
	String result = alloc_string(strlen(str) + 1);
	strcpy(result, str);
	return result;
}

String trim_string(String str) {
	char *end;

	while (isspace(*str))
		str++;

	if (*str == '\0')
		return str;

	end = str + strlen(str) - 1;
	while (end > str && isspace(*end))
		end--;

	*(end + 1) = '\0';

	return str;
}

String substr_before(String str, char c) {
	char *p = strchr(str, c);
	if (p) {
		*p = '\0';
		return str;
	} else {
		return "";
	}
}

String substr_between(String str, char a, char b) {
	char *s = strchr(str, a);
	char *e = strchr(str, b);

	if (s && e && s < e) {
		*e = '\0';
		return s + 1;
	} else {
		return "";
	}
}

String substr_after(String str, char c) {
	char *p = strchr(str, c);
	if (p)
		return p + 1;
	else
		return "";
}

String dyna_strcat(String str, String append) {
	int length = strlen(str) + strlen(append) + 1;
	str = realloc_string(str, length);
	str = strcat(str, append);
	return str;
}

String append_char(String str, char c) {
	char *p = NULL;
	int length = strlen(str) + 2;
	str = realloc_string(str, length);
	p = str;
	while (*p)
		p++;
	*p++ = c;
	*p++ = '\0';
	return str;
}

String* split_string(String string, const char delimiter) {
	char** result = 0;
	int count = 0;
	char* tmp = string;
	char* last_comma = 0;
	char delim[2];
	delim[0] = delimiter;
	delim[1] = 0;

	/* Count how many elements will be extracted. */
	while (*tmp) {
		if (delimiter == *tmp) {
			count++;
			last_comma = tmp;
		}
		tmp++;
	}

	/* Add space for trailing token. */
	count += last_comma < (string + strlen(string) - 1);

	/* Add space for terminating null string so caller
	 knows where the list of returned strings ends. */
	count++;

	result = malloc(sizeof(char*) * count);

	if (result) {
		int index = 0;
		char* token = strtok(string, delim);

		while (token) {
			//assert(index < count);
			*(result + index++) = strdup(token);
			token = strtok(0, delim);
		}
		//assert(index == count - 1);
		*(result + index) = 0;
	}

	return result;
}

String* split_string_once(String string, const char delimiter) {
	char** result = NULL;
	char *p = strchr(string, delimiter);
	if (p) {
		result = alloc_string_array(3);
		*p = '\0';
		*result = copy_string(string);
		*(result + 1) = copy_string(p + 1);
		*(result + 2) = NULL;
	}

	return result;
}

int equals_string(void *a, void *b) {
	if (a == NULL && b == NULL)
		return TRUE;
	if (a == NULL || b == NULL)
		return FALSE;

	if (strcmp(a, b) == 0) {
		return TRUE;
	} else {
		return FALSE;
	}
}

int hash_code_string(void *key) {
	int hash = 0;
	char *p = key;
	while (*p++) {
		hash += *p;
	}
	hash = hash % HASHTABLE_SIZE;
	return hash;
}

char *strlwr(char *str) {
  size_t i;
  size_t len = strlen(str);

  for(i=0; i<len; i++)
	  str[i] = tolower((unsigned char)str[i]);

  return str;
}

