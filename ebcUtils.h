#ifndef EBCUTILS_H
#define EBCUTILS_H

#include "ebUniversalUtils.h"
#include "bitTwiddlingUtils.h"
#include <math.h>
#include "blockUtils.h"
#define MAGIC_NUMBER_EBC 0x6365
#define GO 1
#define STOP 0

// Part 2 Constants
#define MAGIC_NUMBER_EBCBLOCK 0x4345
#define MAGIC_NUMBER_EBCR32 0x3545
#define MAGIC_NUMBER_EBCR128 0x3745

typedef struct ebcmask{
    unsigned char mask;
    int shift;
    unsigned int noBitsGathered;
} ebcMask;

int ebcRead(Image *image, char * filename, int magicNumberMode);
int ebcWrite(Image * image, char * filename, int magicNumberMode);
void ebcReadMask5Init(ebcMask *mask);
void ebcWriteMask5Init(ebcMask *mask);
int ebcUniversalWriter(unsigned int ** values, FILE * fp, int mode, int height, int width);
int ebcUniversalReader(unsigned int ** store, FILE * fp, int mode, int height, int width);

#endif
