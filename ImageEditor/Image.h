/***************************************************************************************************************
 * Nicholas Mel
 *
 * DESCRIPTION
 * Functions for performing the image processing operations: flip horizontally, flip vertically, and rotate
 * right.
 **************************************************************************************************************/
#ifndef IMAGE_H
#define IMAGE_H

#include "Bmp.h"

/*--------------------------------------------------------------------------------------------------------------
 * FUNCTION: ImageFlipHoriz()
 *
 * DESCRIPTION
 * Flips the image pBmp horizontally, i.e., left-to-right.
 *------------------------------------------------------------------------------------------------------------*/
void ImageFlipHoriz(tBmp *pBmp);

/*--------------------------------------------------------------------------------------------------------------
 * FUNCTION: ImageRotRight()
 *
 * DESCRIPTION
 * Rotates the image pBmp right one time.
 *------------------------------------------------------------------------------------------------------------*/
void ImageRotRight(tBmp *pBmp);

/*--------------------------------------------------------------------------------------------------------------
 * FUNCTION: ImageRotRightMult()
 *
 * DESCRIPTION
 * Rotates the image pBmp right multiple (pTimes) times.
 *------------------------------------------------------------------------------------------------------------*/
void ImageRotRightMult(tBmp *pBmp, int pTimes);

/*--------------------------------------------------------------------------------------------------------------
 * FUNCTION: ImageFlipVert()
 *
 * DESCRIPTION
 * Flips the image pBmp vertically, i.e., top-to-bottom.
 *------------------------------------------------------------------------------------------------------------*/
void ImageFlipVert(tBmp *pBmp);


#endif
