/***************************************************************************************************************
 * Nicholas Mel
 *
 * DESCRIPTION
 * Functions for reading and writing BMP images.
 **************************************************************************************************************/
#ifndef BMP_H
#define BMP_H

#include <stdlib.h>
#include "Error.h"
#include "Type.h"

// The BMPHEADER structure.
typedef struct {
	byte		sigB;
	byte 		sigM;
	int32_t		fileSize;
	int16_t		resv1;
	int16_t		resv2;
	int32_t		pixelOffset;
} tBmpHeader;

// The BMPINFOHEADER structure.
typedef struct {
	int32_t		size;
	int32_t		width;
	int32_t		height;
	int16_t		colorPlanes;
	int16_t		bitsPerPixel;
	byte		zeros[24];
} tBmpInfoHeader;

// A BMP image consists of the BMPHEADER and BMPINFOHEADER structures, and the 2D pixel array.
typedef struct {
	tBmpHeader		header;
	tBmpInfoHeader	infoHeader;
	tPixel			**pixel;
} tBmp;

extern const size_t cSizeofBmpHeader;
extern const size_t cSizeofBmpInfoHeader;

/*--------------------------------------------------------------------------------------------------------------
 * FUNCTION: BmpPixelAlloc()
 *
 * DESCRIPTION
 * Allocates a 2D array of tPixel objects with pHeight rows and pWidth columns.
 *------------------------------------------------------------------------------------------------------------*/
tPixel **BmpPixelAlloc(int pWidth, int pHeight);

/*--------------------------------------------------------------------------------------------------------------
 * FUNCTION: BmpPixelFree()
 *
 * DESCRIPTION
 * Deallocates a 2D array allocated by BmpPixelAlloc().
 *------------------------------------------------------------------------------------------------------------*/
void BmpPixelFree(tPixel **pPixel, int pHeight);

/*--------------------------------------------------------------------------------------------------------------
 * FUNCTION: BmpRead()
 *
 * DESCRIPTION
 * Read a BMP image from the file pFilename and return the image info in the pBmp object.
 *------------------------------------------------------------------------------------------------------------*/
tError BmpRead(char *pFilename, tBmp *pBmp);

/*--------------------------------------------------------------------------------------------------------------
 * FUNCTION: BmpWrite()
 *
 * DESCRIPTION
 * Write the BMP image stored in the pBmp object to the file named pFilename.
 *------------------------------------------------------------------------------------------------------------*/
tError BmpWrite(char *pFilename, tBmp *pBmp);

#endif
