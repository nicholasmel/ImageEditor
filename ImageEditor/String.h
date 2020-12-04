/***************************************************************************************************************
 * Nicholas Mel
 *
 * DESCRIPTION
 * String handling functions.
 **************************************************************************************************************/
#ifndef STRING_H  // Preprocessor guard to prevent String.h from being included more than once.
#define STRING_H

#include <string.h>

/*
 * streq is a preprocessor macro function definition.
 */
#define streq(s1,s2) (!strcmp((s1),(s2)))

/*--------------------------------------------------------------------------------------------------------------
 * FUNCTION: StrCatCh
 *
 * DESCRIPTION
 * Concatenates pCh onto the end of pString and returns pString.
 *------------------------------------------------------------------------------------------------------------*/
char *StrCatCh(char *pString, char pChar);

/*--------------------------------------------------------------------------------------------------------------
 * FUNCTION: StrCatInt
 *
 * DESCRIPTION
 * Concatenates pInt (turned into a string) onto the end of pString and returns pString.
 *------------------------------------------------------------------------------------------------------------*/
char *StrCatInt(char *pString, int pInt);

#endif
