// This header file should only be included in ebUniversalUtils.
// So it is automatically included whenever a file includes ebUniversalUtils.
// This is designed like this as the author thinks that it would be more intuitive to include 
// ebUniversalUtils.h in your code and expect to have all the constants that are related to eb files.

#ifndef EB_CONSTANTS_H
#define EB_CONSTANTS_H

#define SUCCESS 0
#define BAD_ARGS 1
#define BAD_FILE 2
#define BAD_MAGIC_NUMBER 3
#define BAD_DIM 4
#define BAD_MALLOC 5
#define BAD_DATA 6
#define BAD_OUTPUT 7
#define BAD_DIMENSIONS_PLACEMENT 9
#define MAX_DIMENSION 262144
#define MIN_DIMENSION 1
#define IDENTICAL 0
#define DIFFERENT 1
#define MAX_GREY_VALUE 31
#define MIN_GREY_VALUE 0

// Part 2 constants
#define BAD_BLOCK_MALLOC 8
#define BAD_PARADIGM_GENERATION 21

#endif