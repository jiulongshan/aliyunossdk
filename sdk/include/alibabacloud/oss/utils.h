/*
 * @Author: your name
 * @Date: 2022-02-24 10:00:08
 * @LastEditTime: 2022-02-24 11:00:34
 * @LastEditors: Please set LastEditors
 * @Description: 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 * @FilePath: /aliyun-oss-cpp-sdk-1.8.2/sdk/include/alibabacloud/oss/utils.h
 */
#include <iostream>
#include <sstream>
#include <string>
#include <ctype.h>
#include <climits>
#include <stdio.h>

#include <stdarg.h>
#include <stddef.h>


namespace mystd{

// int vsnprintf (char *, size_t, const char *, va_list);

// int snprintf (char *s, size_t n, const char *format, ...)
// {
//   int result;
//   va_list ap;
//   va_start (ap, format);
//   result = vsnprintf (s, n, format, ap);
//   va_end (ap);
//   return result;
// }

std::string toString(int data)
{
    std::stringstream ss;
    ss.str("");
	ss << data;
    return ss.str();
}

unsigned long strToull(const char *nptr, char **endptr, register int base)
{
	register const char *s = nptr;
	register unsigned long long acc;
	register int c;
	register unsigned long cutoff;
	register int neg = 0, any, cutlim;

	/*
	 * See strtol for comments as to the logic used.
	 */
	do {
		c = *s++;
	} while (isspace(c));
	if (c == '-') {
		neg = 1;
		c = *s++;
	} else if (c == '+')
		c = *s++;
	if ((base == 0 || base == 16) &&
	    c == '0' && (*s == 'x' || *s == 'X')) {
		c = s[1];
		s += 2;
		base = 16;
	}
	if (base == 0)
		base = c == '0' ? 8 : 10;
	cutoff = (unsigned long)ULONG_MAX / (unsigned long)base;
	cutlim = (unsigned long)ULONG_MAX % (unsigned long)base;
	for (acc = 0, any = 0;; c = *s++) {
		if (isdigit(c))
			c -= '0';
		else if (isalpha(c))
			c -= isupper(c) ? 'A' - 10 : 'a' - 10;
		else
			break;
		if (c >= base)
			break;
		if (any < 0 || acc > cutoff || (acc == cutoff && c > cutlim))
			any = -1;
		else {
			any = 1;
			acc *= base;
			acc += c;
		}
	}
	if (any < 0) {
		acc = ULONG_MAX;
		// errno = ERANGE;
	} else if (neg)
		acc = -acc;
	if (endptr != 0)
		*endptr = (char *) (any ? s - 1 : nptr);
	return (acc);
}

}