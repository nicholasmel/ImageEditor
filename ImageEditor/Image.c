/***************************************************************************************************************
 * Nicholas Mel
 *
 * DESCRIPTION
 * Functions for performing the image processing operations: flip horizontally, flip vertically, and rotate
 * right.
 **************************************************************************************************************/
#include "Bmp.h"

void ImageFlipHoriz(tBmp *pBmp)
{
	for (int row = 0; row < pBmp->infoHeader.height; ++row) {
		for (int col = 0; col < pBmp->infoHeader.width / 2; ++col) {
			tPixel temp = pBmp->pixel[row][col];
			pBmp->pixel[row][col] = pBmp->pixel[row][pBmp->infoHeader.width-1 - col];
			pBmp->pixel[row][pBmp->infoHeader.width-1 - col] = temp;
		}
	}
}

void ImageRotRight(tBmp *pBmp)
{
	int newHeight = pBmp->infoHeader.width, newWidth = pBmp->infoHeader.height;
	tPixel **newPixel = BmpPixelAlloc(newWidth, newHeight);
	for (int row = 0; row < newHeight; ++row) {
		for (int col = 0; col < newWidth; ++col) {
			newPixel[row][col] = pBmp->pixel[pBmp->infoHeader.height-1 - col][row];
		}
	}
	BmpPixelFree(pBmp->pixel, pBmp->infoHeader.height);
	pBmp->infoHeader.width = newWidth;
	pBmp->infoHeader.height = newHeight;
	pBmp->pixel = newPixel;
}

void ImageRotRightMult(tBmp *pBmp, int nTimes)
{
	while (nTimes--) {
		ImageRotRight(pBmp);
	}
}

void ImageFlipVert(tBmp *pBmp)
{
	for (int col = 0; col < pBmp->infoHeader.width; ++col) {
		for (int row = 0; row < pBmp->infoHeader.height / 2; ++row) {
			tPixel temp = pBmp->pixel[row][col];
			pBmp->pixel[row][col] = pBmp->pixel[pBmp->infoHeader.height-1 - row][col];
			pBmp->pixel[pBmp->infoHeader.height-1 - row][col] = temp;
		}
	}
}
