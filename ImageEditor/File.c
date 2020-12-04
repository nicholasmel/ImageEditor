/***************************************************************************************************************
 * Nicholas Mel
 *
 * DESCRIPTION
 * Functions for performing file I/O.
 **************************************************************************************************************/
#include <sys/stat.h>
#include "File.h"
#include "String.h"

void FileClose(FILE *pStream)
{
	if (pStream != stdin && pStream != stdout) fclose(pStream);
}

FILE *FileOpen(char *pFilename, char *pMode)
{
	if (!pFilename) return NULL;
	if (*pFilename) return fopen(pFilename, pMode);
	if (streq(pMode, "rt")) return stdin;
	else if (streq(pMode, "wt")) return stdout;
	else return NULL;
}

int FileRead(FILE *pStream, void *pBlock, size_t pSize, size_t pCount)
{
	if (fread(pBlock, pSize, pCount, pStream) == pCount) return 0;
	else return -1;
}

long FileSize(char *pFilename)
{
	struct stat fileStat;
	if (stat(pFilename, &fileStat)) return -1;
	else return (long)fileStat.st_size;
}

int FileWrite(FILE *pStream, void *pBlock, size_t pSize, size_t pCount)
{
	if (fwrite(pBlock, pSize, pCount, pStream) == pCount) return 0;
	else return -1;
}
