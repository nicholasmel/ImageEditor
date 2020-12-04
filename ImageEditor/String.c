/***************************************************************************************************************
 * Nicholas Mel
 *
 * DESCRIPTION
 * String handling functions.
 **************************************************************************************************************/
#include <stdio.h>   // For sprintf()
#include "String.h"

char *StrCatCh(char *pString, char pCh)
{
    char buf[2] = { pCh, '\0' };
    strcat(pString, buf);
    return pString;
}

char *StrCatInt(char *pString, int pInt)
{
    char buf[32];
    sprintf(buf, "%d", pInt);
    strcat(pString, buf);
    return pString;
}
