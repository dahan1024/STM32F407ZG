/*
 * atof.c
 *
 *  Created on: 2019Äê12ÔÂ22ÈÕ
 *      Author: cjl
 */


/* this program is an example of atof located in <stdlib.h>; converts
	a string into a double */
#include <stdio.h>
#include <ctype.h>

double atof(const char *s)
{
	double val, power;
	int i, sign;

	for (i = 0; isspace(s[i]); i++) {	/* skip white space */
		;
	}

	sign = (s[i] == '-') ? -1 : 1;
	if (s[i] == '+' || s[i] == '-') {
		i++;
	}
	for (val = 0.0; isdigit(s[i]); i++) {
		val = 10.0 * val + (s[i] - '0');
	}
	if (s[i] == '.') {
		i++;
	}
	for (power = 1.0; isdigit(s[i]); i++) {
		val = 10.0 * val + (s[i] - '0');
		power *= 10.0;
//		printf("doing stuff\n");
	}
	return sign * val / power;
}




