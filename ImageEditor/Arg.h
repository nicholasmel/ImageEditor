/***************************************************************************************************************
 * Nicholas Mel
 *
 * DESCRIPTION
 * Functions for scanning the command line for options and arguments.
 **************************************************************************************************************/
#ifndef ARG_H
#define ARG_H

typedef struct {
	char   *arg;			// A pointer to an argument found on the command line.
	int 	argc;			// The argc param to main().
	char  **argv;			// The argv param to main().
	char 	error[1024];	// An error message indicating what was wrong on the command line.
	int 	index;			// The index of the argv[] element being examined.
	char   *longOpts;		// A string containing the list of long options.
	char   *opt;			// For a valid option, points to the option string in argv.
	char   *shortOpts;		// A string containing the list of short options.
} tArgScan;

extern const int cArg;
extern const int cArgEnd;
extern const int cArgInvOpt;
extern const int cArgLongOpt;
extern const int cArgMissingArg;
extern const int cArgShortOpt;
extern const int cArgUnexpStr;

/*--------------------------------------------------------------------------------------------------------------
 * FUNCTION: ArgScan()
 *
 * DESCRIPTION
 * The input parameter is a pointer to a tArgScan struct object containing these data members.
 *------------------------------------------------------------------------------------------------------------*/
int ArgScan(tArgScan *pScan);

#endif
