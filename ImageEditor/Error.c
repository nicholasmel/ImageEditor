/***************************************************************************************************************
 * Nicholas Mel
 *
 * DESCRIPTION
 * See comments in Error.h.
 **************************************************************************************************************/
#include <stdarg.h>  // For variadic function macros
#include <stdio.h>   // For printf()
#include <stdlib.h>  // For exit()
#include "Error.h"
#include "Main.h"
#include "String.h"

void ErrorExit(tError pError, char *pFmt, ...)
{
	char msg[1024];
	sprintf(msg, "%s: ", cBinary);
	va_list argp; va_start(argp, pFmt);
	for (char *fp = pFmt; fp && *fp; ++fp) {
		if (*fp != '%') {
			StrCatCh(msg, *fp);
		} else {
			switch (*++fp) {
				case 'c': StrCatCh(msg, (char)va_arg(argp, int)); break;
				case 'd': StrCatInt(msg, va_arg(argp, int)); break;
				case 's': strcat(msg, va_arg(argp, char *)); break;
			}
		}
	}
	va_end(argp);
	printf("%s\n", msg);
	exit(pError);
}
