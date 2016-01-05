/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2015 by fanguangping (fanguangping@163.com)
 * init.h
 */

#ifndef INIT_H_
#define INIT_H_

#include <stdio.h>
#include "../ds/datastructure.h"

Trie *mappingTrie;
Queue *mappings;
Queue *functions;

void load_core_template();
void load();
void add_mapping_to_trie(Mapping *mapping);
Queue *get_mappings(Statement *s);
//Queue *get_mappings();
Queue *get_functions();
String get_sys_macro_string(String s, FILE *fp);
void parse_sys_macro(String fileName);
void match_template(Statement *source);
void load_template(String fileName);

#endif /* INIT_H_ */
