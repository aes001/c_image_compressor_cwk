#ifndef EBCR_UTILS_H
#define EBCR_UTILS_H

#include "ebUniversalUtils.h"
#include "bitTwiddlingUtils.h"
#include <math.h>
#include "blockUtils.h"

// function prototypes
int randSeries(int * randomSeries, int seed, int n, int min, int max);
int ebrCheckArgs(int argc, char * scriptName);
Block * generateParadigmBlocks(Block * imageBlock, int imageBlockAmount, int paradigmBlockAmount, int seed);
int ebrMatch(Image * ebrImage, Block * paradigmBlocks, Image * targetImage, int paradigmBlockAmount);
int ebrFindBestParadigmBlock(Block * imageBlock, int imageBlockAmount, Block * paradigmBlock, Image * compressedImage);

#endif