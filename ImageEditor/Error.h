/***************************************************************************************************************
 * Nicholas Mel
 *
 * DESCRIPTION
 * Functions for error handling.
 **************************************************************************************************************/
#ifndef ERROR_H
#define ERROR_H

// Program exit codes when errors occur.
typedef enum {
	ErrorNone			=   0,
	ErrorArg          	=  -1,
	ErrorArgDup 		=  -2,
	ErrorArgInputFile	=  -3,
	ErrorArgInvOpt		=  -4,
	ErrorArgRot			=  -5,
	ErrorArgUnexpStr	=  -6,
	ErrorBmpInv			=  -7,
	ErrorBmpCorrupt		=  -8,
	ErrorFileOpen		=  -9,
	ErrorFileRead		= -10,
	ErrorFileWrite		= -11
} tError;


/*--------------------------------------------------------------------------------------------------------------
 * FUNCTION: ErrorExit
 *
 * DESCRIPTION
 * Called when an error occurs and the program must be terminated. This function works similarly to printf().
 *------------------------------------------------------------------------------------------------------------*/
void ErrorExit(tError pError, char *pFmt, ...);

#endif
