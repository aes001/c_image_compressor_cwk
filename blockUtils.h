#ifndef blockUtils_h
#define blockUtils_h

#include "ebUniversalUtils.h"
#include <math.h>

#define BLOCK_WIDTH  3
#define BLOCK_HEIGHT 3
#define BLOCK_SIZE (BLOCK_WIDTH * BLOCK_HEIGHT)

typedef struct block{
    int width;
    int height;
    unsigned int **data;
} Block;

typedef struct diffBlock{
    int width;
    int height;
    int **data;
} DiffBlock;

// function prototypes
int blockerize(unsigned int ** pixels, Block * block, int height, int width);
int blockAverage(Block block);
int unblockerize(unsigned int ** target, Block * block, int height, int width);
int uniformBlockerize(Image * image, Block * block);
double diffBlockAverage(DiffBlock block);
double diffBlockSum(DiffBlock block);
int blockDifference(Block block1, Block block2);

#endif