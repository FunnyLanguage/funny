/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2014 by fanguangping (fanguangping@163.com)
 *
 * Funny is based on TinyScheme which is now maintained by Dimitrios Souflis (dsouflis@acm.org)
 * This is a revised and modified version of TinyScheme
 */

#ifndef SCHEME_H_
#define SCHEME_H_

#include "ds.h"

enum CheckType {
	TYPE_NONE = 1,
	TYPE_ANY = 2,
	TYPE_STRING = 3,
	TYPE_SYMBOL = 4,
	TYPE_PORT = 5,
	TYPE_INPORT = 6,
	TYPE_OUTPORT = 7,
	TYPE_ENVIRONMENT = 8,
	TYPE_PAIR = 9,
	TYPE_LIST = 10,
	TYPE_CHAR = 11,
	TYPE_VECTOR = 12,
	TYPE_NUMBER = 13,
	TYPE_INTEGER = 14,
	TYPE_NATURAL = 15
};

typedef struct _OperatorInfo{
	OperatorFunc func;   //函数过程
	String name;         //函数名
	int minArity;        //最少参数个数
	int maxArity;        //最大参数个数
	enum CheckType type;
} OperatorInfo;

OperatorInfo *init_operators();
OperatorInfo get_dispatch_info(OperatorFunc op);
void eval(Scheme *sc, String statement);
int scheme_init(Scheme *sc);

#endif /* SCHEME_H_ */
