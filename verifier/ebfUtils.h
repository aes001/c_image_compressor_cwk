// Only c files that deal with ebf files should include this header file

#ifndef EBFUTILS_H
#define EBFUTILS_H

#define MAGIC_NUMBER_EBF 0x6265

#include "ebUniversalUtils.h"

// function prototypes
int ebfRead(Image *image, char *filename);
int ebfWrite(Image *image, char *filename);

#endif