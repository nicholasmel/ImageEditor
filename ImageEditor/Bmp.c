/***************************************************************************************************************
 * Nicholas Mel
 *
 * DESCRIPTION
 * Functions for reading and writing BMP images.
 **************************************************************************************************************/
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "Bmp.h"
#include "Error.h"
#include "File.h"

// Asserts that 'cond' is true. If it is not, then we close the file stream 'stream' and return from the
// BmpRead() function with the return value 'error'.
#define BmpAssert(cond, stream, error) if (!(cond)) { if ((stream)) FileClose((stream)); return error; }

const size_t cSizeofBmpHeader     = 14;  // Size of the BMPHEADER struct.
const size_t cSizeofBmpInfoHeader = 40;  // Size of the BMPINFOHEADER struct.

// A valid BMP file has to be at least 58 bytes in size.
const size_t cBmpMinFileSize  = 58;

static int BmpCalcFileSize(int pWidth, int pHeight);
static int BmpCalcPad(int pWidth);

static int BmpCalcFileSize(int pWidth, int pHeight)
{
	return pHeight * (3 * pWidth + BmpCalcPad(pWidth)) + cSizeofBmpHeader + cSizeofBmpInfoHeader;
}

static int BmpCalcPad(int pWidth)
{
	return (4 - (3 * pWidth) % 4) % 4;
}

tPixel **BmpPixelAlloc(int pWidth, int pHeight)
{
	tPixel **pixel = (tPixel **)malloc(pHeight * sizeof(tPixel *));
	for (int row = 0; row < pHeight; ++row) {
		pixel[row] = (tPixel *)malloc(pWidth * sizeof(tPixel));
	}
	return pixel;
}

void BmpPixelFree(tPixel **pPixel, int pHeight)
{
	for (int row = 0; row < pHeight; ++row) {
		free(pPixel[row]);
	}
	free(pPixel);
}

Error BmpRead(char *pFilename, tBmp *pBmp)
{
	byte buffer[cSizeofBmpInfoHeader];

	// Validity Test 1: Verify the size of the file is greater than or equal to cBmpMinFileSize bytes. If not,
	// it cannot be a valid BMP file.
	long fileSize = FileSize(pFilename);
	BmpAssert(fileSize >= cBmpMinFileSize, NULL, ErrorBmpInv);

	// Open the file for reading.
	FILE *bmpIn = FileOpen(pFilename, "rb");
	BmpAssert(bmpIn, NULL, ErrorFileOpen);

	// Read the BMPHEADER structure and initialize the tBmpHeader structure.
	BmpAssert(FileRead(bmpIn, buffer, cSizeofBmpHeader, 1) == 0, bmpIn, ErrorFileRead);
	pBmp->header.sigB = buffer[0];
	pBmp->header.sigM = buffer[1];
	memcpy(&pBmp->header.fileSize, &buffer[2], sizeof(pBmp->header.fileSize));
	memcpy(&pBmp->header.resv1, &buffer[6], sizeof(pBmp->header.resv1));
	memcpy(&pBmp->header.resv1, &buffer[8], sizeof(pBmp->header.resv2));
	memcpy(&pBmp->header.pixelOffset, &buffer[10], sizeof(pBmp->header.pixelOffset));

	// Validity Test 1: Validate the contents of the BMPHEADER.
	BmpAssert(pBmp->header.sigB == 'B' && pBmp->header.sigM == 'M', bmpIn, ErrorBmpInv);
	BmpAssert(pBmp->header.fileSize == fileSize, bmpIn, ErrorBmpInv);
	BmpAssert(pBmp->header.resv1 == 0 && pBmp->header.resv2 == 0, bmpIn, ErrorBmpInv);
	BmpAssert(pBmp->header.pixelOffset == 0x36, bmpIn, ErrorBmpInv);

	// Read the BMPINFOHEADER structure and initialize the tBmpInfoHeader structure.
	BmpAssert(FileRead(bmpIn, buffer, cSizeofBmpInfoHeader, 1) == 0, bmpIn, ErrorFileRead);
	memcpy(&pBmp->infoHeader.size, &buffer[0], sizeof(pBmp->infoHeader.size));
	memcpy(&pBmp->infoHeader.width, &buffer[4], sizeof(pBmp->infoHeader.width));
	memcpy(&pBmp->infoHeader.height, &buffer[8], sizeof(pBmp->infoHeader.height));
	memcpy(&pBmp->infoHeader.colorPlanes, &buffer[12], sizeof(pBmp->infoHeader.colorPlanes));
	memcpy(&pBmp->infoHeader.bitsPerPixel, &buffer[14], sizeof(pBmp->infoHeader.bitsPerPixel));
	memcpy(&pBmp->infoHeader.zeros, &buffer[16], sizeof(pBmp->infoHeader.zeros));

	// Validity Test 2: Validate the contents of the BMPINFOHEADER.
	BmpAssert(pBmp->infoHeader.size == 0x28, bmpIn, ErrorBmpInv);
	BmpAssert(pBmp->infoHeader.colorPlanes == 1, bmpIn, ErrorBmpInv);
	BmpAssert(pBmp->infoHeader.bitsPerPixel == 24, bmpIn, ErrorBmpInv);

	// Corrupted Test 1: Given width and height, we can calculate pad and then determine the size of the file.
	// If the size we calculate does not match the actual file size as stored on disk, then we assume the file
	// is corrupted.
	if (fileSize != BmpCalcFileSize(pBmp->infoHeader.width, pBmp->infoHeader.height)) {
		ErrorExit(ErrorBmpCorrupt, "%s is corrupted", pFilename);
	}

	// The headers check out, so this is most likely a valid BMP file. Let's read the pixel array. First, we
	// dynamically allocate a 2D array which is height x width with each element being a tPixel.
	pBmp->pixel = BmpPixelAlloc(pBmp->infoHeader.width, pBmp->infoHeader.height);

	int pad = BmpCalcPad(pBmp->infoHeader.width);

	for (int row = pBmp->infoHeader.height-1; row >= 0; --row) {
		for (int col = 0; col < pBmp->infoHeader.width; ++col) {
			BmpAssert(FileRead(bmpIn, &pBmp->pixel[row][col], sizeof(tPixel), 1) == 0, bmpIn, ErrorFileRead);
		}
		// Read the padding bytes and check they are zero.
		byte pb[4] = { 0 };
		BmpAssert(FileRead(bmpIn, pb, sizeof(byte), pad) == 0, bmpIn, ErrorFileRead);
		BmpAssert(pb[0] == 0 && pb[1] == 0 && pb[2] == 0 && pb[3] == 0, bmpIn, ErrorBmpCorrupt);
	}

	FileClose(bmpIn);
	return ErrorNone;
}

tError BmpWrite(char *pFilename, tBmp *pBmp)
{
	byte buffer[cSizeofBmpInfoHeader];

	// Open the file for writing.
	FILE *bmpOut = FileOpen(pFilename, "wb");
	BmpAssert(bmpOut, NULL, ErrorFileOpen);

	// Calculate the file size which is written in the BMPHEADER structure.
	pBmp->header.fileSize = BmpCalcFileSize(pBmp->infoHeader.width, pBmp->infoHeader.height);

	// Write the BMPHEADER structure to the file.
	buffer[0] = pBmp->header.sigB;
	buffer[1] = pBmp->header.sigM;
	memcpy(&buffer[2], &pBmp->header.fileSize, sizeof(pBmp->header.fileSize));
	memcpy(&buffer[6], &pBmp->header.resv1, sizeof(pBmp->header.resv1));
	memcpy(&buffer[8], &pBmp->header.resv1, sizeof(pBmp->header.resv2));
	memcpy(&buffer[10], &pBmp->header.pixelOffset, sizeof(pBmp->header.pixelOffset));
	BmpAssert(FileWrite(bmpOut, buffer, cSizeofBmpHeader, 1) == 0, bmpOut, ErrorFileRead);

	// Write the BMPINFOHEADER structure to the file.
	memcpy(&buffer[0], &pBmp->infoHeader.size, sizeof(pBmp->infoHeader.size));
	memcpy(&buffer[4], &pBmp->infoHeader.width, sizeof(pBmp->infoHeader.width));
	memcpy(&buffer[8], &pBmp->infoHeader.height, sizeof(pBmp->infoHeader.height));
	memcpy(&buffer[12], &pBmp->infoHeader.colorPlanes, sizeof(pBmp->infoHeader.colorPlanes));
	memcpy(&buffer[14], &pBmp->infoHeader.bitsPerPixel, sizeof(pBmp->infoHeader.bitsPerPixel));
	memcpy(&buffer[16], &pBmp->infoHeader.zeros, sizeof(pBmp->infoHeader.zeros));
	BmpAssert(FileWrite(bmpOut, buffer, cSizeofBmpInfoHeader, 1) == 0, bmpOut, ErrorFileWrite);

	int pad = BmpCalcPad(pBmp->infoHeader.width);

	for (int row = pBmp->infoHeader.height-1; row >= 0; --row) {
		for (int col = 0; col < pBmp->infoHeader.width; ++col) {
			BmpAssert(FileWrite(bmpOut, &pBmp->pixel[row][col], sizeof(tPixel), 1) == 0, bmpOut, ErrorFileWrite);
		}
		// Write the padding bytes.
		byte pb[4] = { 0 };
		BmpAssert(FileWrite(bmpOut, pb, sizeof(byte), pad) == 0, bmpOut, ErrorFileWrite);
	}

	FileClose(bmpOut);
	return ErrorNone;
}
