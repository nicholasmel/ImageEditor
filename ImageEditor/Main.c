/***************************************************************************************************************
 * Nicholas Mel
 *
 * DESCRIPTION
 * bimpie - BMP Image Editor
 **************************************************************************************************************/
//#include "K1as.hpp"


#include <stdbool.h>  // For bool data type
#include <stdio.h>    // For printf()
#include <stdlib.h>   // For exit(), strtod()
#include "Arg.h"
#include "Bmp.h"
#include "Image.h"
#include "Error.h"
#include "String.h"

// Enumerated type for the operations to be performed in the operation queue.
typedef enum {
	OperationFlipH = 1,
	OperationFlipV = 2,
	OperationRotR  = 3
} tOperation;

// The operation queue. Stores values indicating the operations to be performed in the order they were en-
// countered on the command line.
typedef struct {
	int 		index;
	tOperation	queue[3];
} tOpQueue;

// Stores command line argument info.
typedef struct {
	int			argc;		// argc from main()
	char		**argv;		// argv from main()
	bool		fliph;		// --fliph was specified
	bool		flipv;		// --flipv
	bool		h;			// -h, --help
	char		*inFile;	// The file name of the input BMP image
	bool		o;			// -o file, --output file
	tOpQueue	opQueue;	// Order in which the operations are to be performed.
	char		*outFile;	// The output file name following -o or --output
	int			rotArg;		// The argument n following --rotr
	bool		rotr;		// --rotr n
} tCmdLine;

const char *cAuthor  = "Nicholas Mel";
const char *cBinary  = "bimpie";

static bool	CheckDupOpt(bool pOptFlag, char *pOptStr);
static void	Help();
static void	Run(tCmdLine *);
static void	ScanCmdLine(tCmdLine *);
static int	ScanRotArg(char *pOpt, char *pArg);

/*--------------------------------------------------------------------------------------------------------------
 * FUNCTION: CheckDupOpt()
 *
 * DESCRIPTION
 * Checks to see if pOptFlag is already true, indicating that the option was duplicated on the command line.
 * If so, error out. If not, return true which will set the flag in the tCmdLine structure to true.
 *------------------------------------------------------------------------------------------------------------*/
static bool CheckDupOpt(bool pOptFlag, char *pOptStr)
{
	if (pOptFlag) ErrorExit(ErrorArgDup, "duplicate option %s", pOptStr);
	return true;
}

/*--------------------------------------------------------------------------------------------------------------
 * FUNCTION: Help()
 *
 * DESCRIPTION
 * Display help and exit.
 *------------------------------------------------------------------------------------------------------------*/
static void Help()
{
	printf("Usage: %s [options] bmpfile\n", cBinary);
	printf("Perform image processing operations on a BMP image.\n\n");
	printf("Options:\n\n");
	printf("    --fliph                  Flips the image horizontally.\n");
	printf("    --flipv                  Flips the image vertically.\n");
	printf("    -h, --help               Display a help message and exit.\n");
	printf("    -o file, --output file   Write the modified image to 'file' in .bmp format.\n");
	printf("    --rotr n                 Rotate the image 90 degs right (clockwise) n mod 4 times.\n");
	printf("By default, the modified image is written to 'bmpfile'.\n");
	exit(0);
}

/*--------------------------------------------------------------------------------------------------------------
 * FUNCTION: main()
 *
 * DESCRIPTION
 * Scan the command line for options and arguments and then call Run().
 *------------------------------------------------------------------------------------------------------------*/
int main(int pArgc, char *pArgv[])
{
	tCmdLine cmdLine;
	memset(&cmdLine, 0, sizeof(tCmdLine));
	cmdLine.argc = pArgc;
	cmdLine.argv = pArgv;
	ScanCmdLine(&cmdLine);
	Run(&cmdLine);
	return 0;
}

/*--------------------------------------------------------------------------------------------------------------
 * FUNCTION: Run()
 *
 * DESCRIPTION
 * Reads the BMP image, performs the operations, and writes the modified image.
 *------------------------------------------------------------------------------------------------------------*/
static void Run(tCmdLine *pCmdLine)
{
	tBmp bmp;
	tError result = BmpRead(pCmdLine->inFile, &bmp);

	// BmpRead() returns ErrorNone if the image was read correctly.
	switch (result) {
		case ErrorBmpInv:
			ErrorExit(result, "%s is not a BMP file", pCmdLine->inFile);
			break;
		case ErrorBmpCorrupt:
			ErrorExit(result, "%s is corrupted", pCmdLine->inFile);
			break;
		case ErrorFileOpen:
			ErrorExit(result, "could not open %s", pCmdLine->inFile);
			break;
		case ErrorFileRead:
			ErrorExit(result, "reading from %s failed", pCmdLine->inFile);
			break;
		default:
			break;
	}

	// Perform the operations in the order in which they appeared on the command line.
	for (int i = 0; i < pCmdLine->opQueue.index; ++i) {
		switch (pCmdLine->opQueue.queue[i]) {
			case OperationFlipH:
				ImageFlipHoriz(&bmp);
				break;
			case OperationFlipV:
				ImageFlipVert(&bmp);
				break;
			case OperationRotR:
				ImageRotRightMult(&bmp, pCmdLine->rotArg % 4);
				break;
		}
	}

	// Write the modified image to either the file name following the -o or --output option, or to the input
	// file name.
	BmpWrite(pCmdLine->o ? pCmdLine->outFile : pCmdLine->inFile, &bmp);

	// Even though the program is going to exit when we return, I'm going to free the BMP pixel array anyway
	// because I don't like memory leaks.
	BmpPixelFree(bmp.pixel, bmp.infoHeader.height);
}

/*--------------------------------------------------------------------------------------------------------------
 * FUNCTION: ScanCmdLine()
 *
 * DESCRIPTION
 * Scan the command line extracting the options and arguments.
 *------------------------------------------------------------------------------------------------------------*/
static void ScanCmdLine(tCmdLine *pCmdLine)
{
	tArgScan argScan;
	memset(&argScan, 0, sizeof(tArgScan));
	argScan.argc = pCmdLine->argc;
	argScan.argv = pCmdLine->argv;
	argScan.longOpts = "fliph;flipv;help;output:;rotr:;";
	argScan.shortOpts = "ho:v";

	// Start scanning the command line at argv[1]. Note: argv[0] is always the name of the binary.
	argScan.index = 1;
	int result = ArgScan(&argScan);

	// Keep calling ArgargScan() to scan the next option until it returns cArgEnd indicating that the entire
	// command line has been scanned.
	while (result != cArgEnd) {

		// Was a required argument following a short or long option omitted?
		if (result == cArgMissingArg) {
			ErrorExit(ErrorArg, "%s", argScan.error);

		// Was an invalid short or long option specified?
		} else if (result == cArgInvOpt) {
			ErrorExit(ErrorArgInvOpt, "%s", argScan.error);

		// Was an unexpected string encountered?
		} else if (result == cArgUnexpStr) {
			ErrorExit(ErrorArgUnexpStr, "%s", argScan.error);

		// Was an argument encountered? The only argument on the command line should be the BMP image file name.
		} else if (result == cArg) {
			// Check for two arguments.
			if (pCmdLine->inFile) ErrorExit(ErrorArgUnexpStr, "unexpected string %s", argScan.arg);
			pCmdLine->inFile = argScan.arg;

		// We encountered a valid option. Was it --fliph?
		} else if (streq(argScan.opt, "--fliph")) {
			pCmdLine->fliph = CheckDupOpt(pCmdLine->fliph, argScan.opt);
			pCmdLine->opQueue.queue[pCmdLine->opQueue.index++] = OperationFlipH;

		// Was it --flipv?
		} else if (streq(argScan.opt, "--flipv")) {
			pCmdLine->flipv = CheckDupOpt(pCmdLine->flipv, argScan.opt);
			pCmdLine->opQueue.queue[pCmdLine->opQueue.index++] = OperationFlipV;

		// Was it -h or --help?
		} else if (streq(argScan.opt, "-h") || streq(argScan.opt, "--help")) {
			pCmdLine->h= CheckDupOpt(pCmdLine->h, argScan.opt);

		// Was it -o or --output?
		} else if (streq(argScan.opt, "-o") || streq(argScan.opt, "--output")) {
			pCmdLine->o = CheckDupOpt(pCmdLine->o, argScan.opt);
			pCmdLine->outFile = argScan.arg;

		// Was it --rotr? If so, attempt to convert the argument following --rotr to an integer. ScanRotArg()
		// does not return if the conversion fails.
		} else if (streq(argScan.opt, "--rotr")) {
			pCmdLine->rotr = CheckDupOpt(pCmdLine->rotr, argScan.opt);
			pCmdLine->rotArg = ScanRotArg(argScan.opt, argScan.arg);
			pCmdLine->opQueue.queue[pCmdLine->opQueue.index++] = OperationRotR;
		}

		// Scan next option.
		result = ArgScan(&argScan);
	}

	if (pCmdLine->h) Help();     // Help() does not return.

	// Check that an input file name was specified.
	if (!pCmdLine->inFile) {
		ErrorExit(ErrorArgRot, "expecting input file");
	}
}

/*--------------------------------------------------------------------------------------------------------------
 * FUNCTION: ScanRotArg()
 *
 * DESCRIPTION
 * The --rotr option is supposed to be followed by an integer, n, which is the number of times to rotate the
 * image. This function attempts to convert n (pArg is a pointer to the string representation of n on the
 * command line) to an integer. There is a function in the C Std Lib named strtol() that can be used to do this.
 * If the conversion fails, strtol() will return 0, the 'end' parameter will be pointing at the char in the
 * string where the failure was detected (this character will not be a - or a digit char). If the conversion
 * fails, we error out.
 *------------------------------------------------------------------------------------------------------------*/
static int ScanRotArg(char *pOpt, char *pArg)
{
	char *end;
	int n = (int)strtol(pArg, &end, 10);
	if (n == 0 && *end != '\0') {
		ErrorExit(ErrorArgRot, "%s: invalid argument %s", pOpt, pArg);
	}
	return n;
}
