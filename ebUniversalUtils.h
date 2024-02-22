// This header file should be added to the header file of a subtype of an eb utilities library. 
// These universal utilities will then be extended by additional functions for the specific type of eb file.
// For example, ebfUtils is a an eb utilities library for the subtype of eb files called ebf.
// Hence, ebfUtils.h will extend ebUniversalUtils.h with functions specific to ebf files.

#include <stdio.h>
#include <stdlib.h>

#ifndef EBUNIVERSALUTILS_H
#define EBUNIVERSALUTILS_H

#include "ebConstants.h"

typedef struct ebImage{
    unsigned char magicNumber[2]; // Char array to store the magic number
    int width; // Width of the image
    int height; // Height of the image
    unsigned int **data; // 2D array to store the image data
    unsigned int **paradigm; // 2D array to store the paradigm data
    int paradigmBlockAmount; // Amount of paradigm blocks
} Image;

// function prototypes
unsigned int ** ebCreate2DArray(int height, int width);
void ebFree2DArray(unsigned int **array);
int ebReadHeader(FILE * fp, Image * image, int expectedMagicNumber);
int ebWriteHeader(FILE * fp, Image * image, int expectedMagicNumber);
void ebCheckArgs(int argc, char * scriptName);
int ebCompare(Image * image1, Image * image2);
int ebErrorHandle(int errorCode, char * filename);

#endif