/*
 * Funny Language - a free style programming language.
 * Copyright (C) 2015 by fanguangping (fanguangping@163.com)
 * math.c
 */

#include <stdlib.h>
#include <math.h>
#include <float.h>
#include "operator.h"

static Num g_zero = { .isFix = 1, {.longValue = 0L} };
static Num g_one = { .isFix = 1, {.longValue = 1L} };

static long num2int(Num n) {
	return (n.isFix ? (n).longValue : (long) (n).doubleValue);
}

static double num2float(Num n) {
	return (!n.isFix ? (n).doubleValue : (double) (n).longValue);
}

// 两数相加
static Num num_add(Num a, Num b) {
	Num result;
	result.isFix = a.isFix && b.isFix;
	if (result.isFix) {
		result.longValue = a.longValue + b.longValue;
	} else {
		result.doubleValue = num2float(a) + num2float(b);
	}
	return result;
}

// 两数相减
static Num num_sub(Num a, Num b) {
	Num result;
	result.isFix = a.isFix && b.isFix;
	if (result.isFix) {
		result.longValue = a.longValue - b.longValue;
	} else {
		result.doubleValue = num2float(a) - num2float(b);
	}
	return result;
}

// 两数相乘
static Num num_mul(Num a, Num b) {
	Num result;
	result.isFix = a.isFix && b.isFix;
	if (result.isFix) {
		result.longValue = a.longValue * b.longValue;
	} else {
		result.doubleValue = num2float(a) * num2float(b);
	}
	return result;
}

// 两数相除
static Num num_div(Num a, Num b) {
	Num result;
	result.isFix = a.isFix && b.isFix && (a.longValue % b.longValue == 0);
	if (result.isFix) {
		result.longValue = a.longValue / b.longValue;
	} else {
		result.doubleValue = num2float(a) / num2float(b);
	}
	return result;
}

// 两数整除
static Num num_intdiv(Num a, Num b) {
	Num result;
	result.isFix = a.isFix && b.isFix;
	if (result.isFix) {
		result.longValue = a.longValue / b.longValue;
	} else {
		result.doubleValue = num2float(a) / num2float(b);
	}
	return result;
}

// 两数求余
static Num num_rem(Num a, Num b) {
	long e1 = num2int(a);
	long e2 = num2int(b);
	long result = e1 % e2;
	//余数必须和第一个操作数符号相同
	if ((result > 0) && (e1 < 0))
		result -= labs(e2);
	else if ((result < 0) && (e1 > 0))
		result += labs(e2);
	Num r;
	r.isFix = a.isFix && b.isFix;
	r.longValue = result;
	return r;
}

// 两数求模
static Num num_mod(Num a, Num b) {
	long e1 = num2int(a);
	long e2 = num2int(b);
	long result = e1 % e2;
	//模数必须和第二个操作数符号相同
	if (result * e2 < 0) {
		result += e2;
	}
	Num r;
	r.isFix = a.isFix && b.isFix;
	r.longValue = result;
	return r;
}

/* Round to nearest. Round to even if midway */
// 将浮点数四舍五入到整数
static double round_per_r5rs(double x) {
	double fl = floor(x);
	double ce = ceil(x);
	double dfl = x - fl;
	double dce = ce - x;
	if (dfl > dce) {
		return ce;
	} else if (dfl < dce) {
		return fl;
	} else {
		if (fmod(fl, 2.0) == 0.0) { /* I imagine this holds */
			return fl;
		} else {
			return ce;
		}
	}
}

// 判断一个浮点数是否近似为0
static int is_zero_double(double x) {
	return x < DBL_MIN && x > -DBL_MIN;
}

//将一个数字（整型或浮点型）无损的转化为整型
// (inexact->exact x)
Cell* op_inexact2exact(Scheme *sc) {
	Cell* num;
	double dd;
	num = first(sc->args);
	if (num->_num.isFix) {
		return s_return_helper(sc, num);
	} else if (modf(num->_num.doubleValue, &dd) == 0.0) {
		return s_return_helper(sc, make_integer(sc, long_value(num)));
	} else {
		return error_helper(sc, "inexact->exact: not integral:", num);
	}
}

// (exp x)
Cell* op_exp(Scheme *sc) {
	Cell* num = first(sc->args);
	return s_return_helper(sc, make_real(sc, exp(double_value(num))));
}

// (log x)
Cell* op_log(Scheme *sc) {
	Cell* num = first(sc->args);
	return s_return_helper(sc, make_real(sc, log(double_value(num))));
}

// (sin x)
Cell* op_sin(Scheme *sc) {
	Cell* num = first(sc->args);
	return s_return_helper(sc, make_real(sc, sin(double_value(num))));
}

// (cos x)
Cell* op_cos(Scheme *sc) {
	Cell* num = first(sc->args);
	return s_return_helper(sc, make_real(sc, cos(double_value(num))));
}

// (tan x)
Cell* op_tan(Scheme *sc) {
	Cell* num = first(sc->args);
	return s_return_helper(sc, make_real(sc, tan(double_value(num))));
}

// (asin x)
Cell* op_asin(Scheme *sc) {
	Cell* num = first(sc->args);
	return s_return_helper(sc, make_real(sc, asin(double_value(num))));
}

// (acos x)
Cell* op_acos(Scheme *sc) {
	Cell* num = first(sc->args);
	return s_return_helper(sc, make_real(sc, acos(double_value(num))));
}

// (atan x)
// (atan x y)
Cell* op_atan(Scheme *sc) {
	Cell* x = first(sc->args);
	if (rest(sc->args) == &g_nil) {
		return s_return_helper(sc, make_real(sc, atan(double_value(x))));
	} else {
		Cell* y = second(sc->args);
		return s_return_helper(sc,
				make_real(sc, atan2(double_value(x), double_value(y))));
	}
}

// (sqrt x)
Cell* op_sqrt(Scheme *sc) {
	Cell* num = first(sc->args);
	return s_return_helper(sc, make_real(sc, sqrt(double_value(num))));
}

// (expt x y)
Cell* op_expt(Scheme *sc) {
	Cell* x = first(sc->args);
	Cell* y = second(sc->args);

	double result;
	int real_result = TRUE;

	if (x->_num.isFix && y->_num.isFix)
		real_result = FALSE;
	/* This 'if' is an R5RS compatibility fix. */
	/* NOTE: Remove this 'if' fix for R6RS.    */
	if (double_value(x) == 0 && double_value(y) < 0) {
		result = 0.0;
	} else {
		result = pow(double_value(x), double_value(y));
	}
	/* Before returning integer result make sure we can. */
	/* If the test fails, result is too big for integer. */
	if (!real_result) {
		long result_as_long = (long) result; //如果result有小数位，必然导致result_as_long和result不相等
		if (result != (double) result_as_long)
			real_result = TRUE;
	}
	if (real_result) {
		return s_return_helper(sc, make_real(sc, result));
	} else {
		return s_return_helper(sc, make_integer(sc, (long) result));
	}
}

// (floor x)
Cell* op_floor(Scheme *sc) {
	Cell* num = first(sc->args);
	return s_return_helper(sc, make_real(sc, floor(double_value(num))));
}

// (ceiling x)
Cell* op_ceiling(Scheme *sc) {
	Cell* num = first(sc->args);
	return s_return_helper(sc, make_real(sc, ceil(double_value(num))));
}

// (truncate x)
Cell* op_truncate(Scheme *sc) {
	Cell* num = first(sc->args);

	double doubleValue = double_value(num);
	if (doubleValue > 0) {
		return s_return_helper(sc, make_real(sc, floor(doubleValue)));
	} else {
		return s_return_helper(sc, make_real(sc, ceil(doubleValue)));
	}
}

// (round x)
Cell* op_round(Scheme *sc) {
	Cell* num = first(sc->args);

	if (num->_num.isFix)
		return s_return_helper(sc, num);
	return s_return_helper(sc, make_real(sc, round_per_r5rs(double_value(num))));
}

// (+ x y ...)
Cell* op_add(Scheme *sc) {
	Num value = g_zero;

	Cell* x;
	for (x = sc->args; x != &g_nil; x = cdr(x)) {
		value = num_add(value, num_value(car(x)));
	}
	return s_return_helper(sc, make_number(sc, value));
}

// (* x y ...)
Cell* op_mul(Scheme *sc) {
	Num value = g_one;

	Cell* x;
	for (x = sc->args; x != &g_nil; x = cdr(x)) {
		value = num_mul(value, num_value(car(x)));
	}
	return s_return_helper(sc, make_number(sc, value));
}

// (- x y ...)
Cell* op_sub(Scheme *sc) {
	Num value;
	Cell* x;
	if (cdr(sc->args) == &g_nil) {
		x = sc->args;
		value = g_zero;
	} else {
		x = cdr(sc->args);
		value = num_value(car(sc->args));
	}
	for (; x != &g_nil; x = cdr(x)) {
		value = num_sub(value, num_value(car(x)));
	}
	return s_return_helper(sc, make_number(sc, value));
}

// (/ x y ...)
Cell* op_div(Scheme *sc) {
	Num value;
	Cell* x;
	if (cdr(sc->args) == &g_nil) {
		x = sc->args;
		value = g_one;
	} else {
		x = cdr(sc->args);
		value = num_value(car(sc->args));
	}
	for (; x != &g_nil; x = cdr(x)) {
		if (!is_zero_double(double_value(car(x))))
			value = num_div(value, num_value(car(x)));
		else {
			return error_helper(sc, "/: division by zero", NULL);
		}
	}
	return s_return_helper(sc, make_number(sc, value));
}

// (quotient x y ...)
Cell* op_intdiv(Scheme *sc) {
	Num value;
	Cell* x;
	if (cdr(sc->args) == &g_nil) {
		x = sc->args;
		value = g_one;
	} else {
		x = cdr(sc->args);
		value = num_value(car(sc->args));
	}
	for (; x != &g_nil; x = cdr(x)) {
		if (long_value(car(x)) != 0)
			value = num_intdiv(value, num_value(car(x)));
		else {
			return error_helper(sc, "quotient: division by zero", NULL);
		}
	}
	return s_return_helper(sc, make_number(sc, value));
}

// (remainder x y)
Cell* op_rem(Scheme *sc) {
	Cell* x = first(sc->args);
	Cell* y = second(sc->args);

	Num value = num_value(x);
	if (long_value(y) != 0) {
		value = num_rem(value, num_value(y));
		return s_return_helper(sc, make_number(sc, value));
	} else {
		return error_helper(sc, "remainder: division by zero", NULL);
	}
}

// (modulo x y)
Cell* op_mod(Scheme *sc) {
	Cell* x = first(sc->args);
	Cell* y = second(sc->args);

	Num value = num_value(x);
	if (long_value(y) != 0) {
		value = num_mod(value, num_value(y));
		return s_return_helper(sc, make_number(sc, value));
	} else {
		return error_helper(sc, "modulo: division by zero", NULL);
	}
}

void init_math_constants(Scheme *sc) {
	add_constant(sc, "*PI*", make_real(sc, M_PI));
	add_constant(sc, "*E*", make_real(sc, M_E));
}
