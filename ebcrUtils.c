#include "ebcrUtils.h"

/**
 * This function checks the arguments for ebcR programs
 *
 * @param argc The number of arguments
 * @param scriptName The name of the script
 * @return 0 on success and exits on failure
 */
int ebrCheckArgs(int argc, char *scriptName)
{
    // Check the arguments
    if (argc == 1)
    {
        printf("Usage: %s <input file> <output file> <seed>\n", scriptName);
        exit(0);
    }
    else if (argc != 4)
    {
        printf("ERROR: Bad Arguments\n");
        exit(BAD_ARGS);
    }
    return 0;
}

// /**
//  * DEPRECATED - This function generates a random series of numbers
//  *
//  * @param randomSeries The array to store the random series
//  * @param seed The seed for the random number generator
//  * @param n The number of random numbers to generate
//  * @param min The minimum value for the random numbers
//  * @param max The maximum value for the random numbers
//  * @return 0 on success, -1 on failure
//  */
// int randSeries(int * randomSeries, int seed, int n, int min, int max){
//     // Safety checks
//     if(randomSeries == NULL){
//         printf("Error: Random series output array is NULL\n");
//         return -1;
//     }
//     if(n < 1){
//         printf("Error: Number of random numbers to generate is less than 1\n");
//         return -1;
//     }
//     if(min > max){
//         printf("Error: Minimum value is greater than maximum value\n");
//         return -1;
//     }
//     if(n > max - min + 1){
//         printf("Error: Number of random numbers to generate is greater than the range of random numbers\n");
//         return -1;
//     }
//     // Generate random numbers
//     srand(seed);
//     int randBuffer = 0;
//     int randGenerated = 0;
//     for(int i = 0; i < n; i++){
//         randBuffer = rand() % (max - min + 1) + min;
//         for(int j = 0; j < randGenerated; j++){
//             if(randomSeries[j] == randBuffer){
//                 i--;
//                 randBuffer = min - 1;
//                 break;
//             }
//         }
//         if(randBuffer != min - 1){
//             randomSeries[randGenerated] = randBuffer;
//             randGenerated++;
//         }
//     }
//     return 0;
// }

    /**
     * This function generates a series of non repeating paradigm blocks
     *
     * @param imageBlock The array of blocks of image data
     * @param imageBlockAmount The number of image blocks
     * @param paradigmBlockAmount The number of paradigm blocks needed to be generated
     * @param seed The seed for the random number generator
     * @return The array of paradigm blocks on success, NULL on failure
     */
    Block *
    generateParadigmBlocks(Block *imageBlock, int imageBlockAmount, int paradigmBlockAmount, int seed)
{
    // Safety checks
    if (imageBlock == NULL)
    { // check if the image block array is NULL
        return NULL;
    }
    if (imageBlockAmount < 1)
    { // check if the image block amount is less than 1
        return NULL;
    }

    Block *paradigmBlock = (Block *)malloc(sizeof(Block) * paradigmBlockAmount); // allocate memory for the paradigm block array
    if (paradigmBlock == NULL)
    {                // check if the memory allocation failed
        return NULL; // return NULL if the memory allocation failed
    }

    if (paradigmBlockAmount < 1)
    {                // check if the paradigm block amount is less than 1
        return NULL; // return NULL if the paradigm block amount is less than 1
    }

    srand(seed);                                                          // set the seed for the random number generator
    int *randomSeries = (int *)malloc(sizeof(int) * paradigmBlockAmount); // allocate memory for the random series
    if (randomSeries == NULL)
    {                // check if the memory allocation failed
        return NULL; // return NULL if the memory allocation failed
    }
    int randBuffer = 0;    // buffer for the random number
    int randGenerated = 0; // number of random numbers generated and added to the random series
    int blockDiff = 0;     // difference between two blocks (used to check if the paradigm block is the same as a paradigm block chosen before)
    for (int i = 0; i < paradigmBlockAmount; i++)
    {                                                 // loop through the paradigm block array
        randBuffer = rand() % (imageBlockAmount + 1); // generate a random number with max value of the number of image blocks
        for (int j = 0; j < randGenerated; j++)
        { // loop through the random series
            if (randomSeries[j] == randBuffer)
            {                    // check if the random number is the same as a random number chosen before
                i--;             // decrement i to generate another random number
                randBuffer = -1; // set the random number to -1 to indicate that the random number is invalid
                break;           // break out of the loop
            }
            // check if the paradigm block is the same as a paradigm block chosen before
            blockDiff = blockDifference(imageBlock[randBuffer], paradigmBlock[j]); // get the difference between the two blocks
            if (blockDiff == 0)
            {                    // If the difference is 0, the blocks are the same
                i--;             // decrement i to generate another random number
                randBuffer = -1; // set the random number to -1 to indicate that the random number is invalid
                break;           // break out of the loop
            }
        }
        if (randBuffer != -1)
        {                                                                                   // check if the random number is valid
            randomSeries[randGenerated] = randBuffer;                                       // add the random number to the random series if the random number is valid
            paradigmBlock[randGenerated].height = BLOCK_HEIGHT;                             // set the height of the paradigm block
            paradigmBlock[randGenerated].width = BLOCK_WIDTH;                               // set the width of the paradigm block
            paradigmBlock[randGenerated].data = ebCreate2DArray(BLOCK_HEIGHT, BLOCK_WIDTH); // allocate memory for the paradigm block data
            if (paradigmBlock[randGenerated].data == NULL)
            {                // check if the memory allocation failed
                return NULL; // return NULL if the memory allocation failed
            }
            for (int blockY = 0; blockY < BLOCK_HEIGHT; blockY++)
            { // loop through the rows of the paradigm block
                for (int blockX = 0; blockX < BLOCK_WIDTH; blockX++)
                {                                                                                                    // loop through the columns of the paradigm block
                    paradigmBlock[randGenerated].data[blockY][blockX] = imageBlock[randBuffer].data[blockY][blockX]; // copy the data from the image block to the paradigm block
                }
            }
            randGenerated++; // increment the number of random numbers generated and added to the random series
        }
    }
    free(randomSeries); // free the random series array as it is no longer needed
    return paradigmBlock;
}

/**
 * This function finds the best paradigm block for a given image block
 *
 * @param imageBlock The image block to find the best paradigm block for
 * @param paradigmBlock The array of paradigm blocks
 * @param paradigmBlockAmount The number of paradigm blocks
 */
int ebrFindBestParadigmBlock(Block *imageBlock, int imageBlockAmount, Block *paradigmBlock, Image *compressedImage)
{
    // Loop through the image and find the best match with the paradigm blocks by calculating the difference each pixel in the block and the paradigm block
    // and averaging the difference. The paradigm block with the lowest average difference is the best match.

    DiffBlock currentDifference;                                                 // the current difference between the image block and the paradigm block (DiffBlock is a struct like Block but with int data instead of unsigned int data so that negative values can be stored)
    currentDifference.height = BLOCK_HEIGHT;                                     // set the height of the current difference block
    currentDifference.width = BLOCK_WIDTH;                                       // set the width of the current difference block
    currentDifference.data = (int **)ebCreate2DArray(BLOCK_HEIGHT, BLOCK_WIDTH); // allocate memory for the current difference block
    if (currentDifference.data == NULL)
    {                      // check if the memory allocation failed
        return BAD_MALLOC; // return BAD_MALLOC if the memory allocation failed
    }
    int bestMatchPBIndex = 0;      // The index of the current lowest average difference paradigm block
    double avgDifferenceBuff = 0;  // Buffer for the average difference between the image block and the paradigm block
    double bestAvgDifference = 32; // The current lowest average difference (32 is 1 + maximum average between two blocks)

    for (int currentBlockIndex = 0; currentBlockIndex < imageBlockAmount; currentBlockIndex++)
    { // loop through the image blocks
        for (int pbIndex = 0; pbIndex < compressedImage->paradigmBlockAmount; pbIndex++)
        { // loop through the paradigm blocks
            for (int blockY = 0; blockY < BLOCK_HEIGHT; blockY++)
            { // loop through the elements of the current difference block
                for (int blockX = 0; blockX < BLOCK_WIDTH; blockX++)
                {
                    currentDifference.data[blockY][blockX] = abs(((int)imageBlock[currentBlockIndex].data[blockY][blockX]) - ((int)paradigmBlock[pbIndex].data[blockY][blockX])); // calculate the difference between the image block and the paradigm block
                }
            }
            avgDifferenceBuff = diffBlockAverage(currentDifference); // calculate the average difference between the image block and the paradigm block
            if (avgDifferenceBuff < bestAvgDifference)
            {                                          // check if the average difference is lower than the current lowest average difference
                bestAvgDifference = avgDifferenceBuff; // set the current lowest average difference to the average difference
                bestMatchPBIndex = pbIndex;            // set the index of the current lowest average difference to the new lowest average difference paradigm block
            }
        }
        int compressedImageY = floor(currentBlockIndex / compressedImage->width);     // Calculate the y coordinate of the compressed image to store the found paradigm block index
        int compressedImageX = currentBlockIndex % compressedImage->width;            // Calculate the x coordinate of the compressed image to store the found paradigm block index
        compressedImage->data[compressedImageY][compressedImageX] = bestMatchPBIndex; // Set the value of the compressed image data to the index of the best match paradigm block
        bestAvgDifference = 32;                                                       // Reset the best average difference
    }

    ebFree2DArray((unsigned int **)currentDifference.data); // free the current difference block data
    return SUCCESS;
}

/**
 * This function matches an ebrImage data to the correct paradigm block
 *
 * @param ebrImage The image to match
 * @param paradigmBlocks The array of paradigm blocks
 * @param targetImage The image to store the matched data in
 * @param paradigmBlockAmount The number of paradigm blocks
 * @return 0 on success; BAD_DATA if any of the arguments are invalid or invalid data is detected; BAD_MALLOC if a malloc fails
 */
int ebrMatch(Image *ebrImage, Block *paradigmBlocks, Image *targetImage, int paradigmBlockAmount)
{
    // Safety checks
    if (ebrImage == NULL)
    {                    // Check if the ebrImage is NULL
        return BAD_DATA; // Return BAD_DATA if the ebrImage is NULL
    }
    if (paradigmBlocks == NULL)
    {                    // Check if the paradigmBlocks is NULL
        return BAD_DATA; // Return BAD_DATA if the paradigmBlocks is NULL
    }

    if (paradigmBlockAmount < 1)
    {                    // Check if the paradigm block amount is less than 1
        return BAD_DATA; // Return BAD_DATA if the paradigm block amount is less than 1
    }

    // Check if all the paradigm blocks are the same size
    for (int i = 0; i < paradigmBlockAmount; i++)
    { // Loop through the paradigm blocks
        if (paradigmBlocks[i].height != paradigmBlocks[0].height || paradigmBlocks[i].width != paradigmBlocks[0].width)
        {                    // Check if the current paradigm block is the same size as the first paradigm block
            return BAD_DATA; // Return BAD_DATA if the current paradigm block is not the same size as the first paradigm block
        }
    }
    // Allocate memory for the target image
    targetImage->height = ebrImage->height * paradigmBlocks[0].height;            // Set the height of the target image
    targetImage->width = ebrImage->width * paradigmBlocks[0].width;               // Set the width of the target image
    targetImage->data = ebCreate2DArray(targetImage->height, targetImage->width); // Allocate memory for the target image data
    if (targetImage->data == NULL)
    {                      // Check if the memory allocation failed
        return BAD_MALLOC; // Return BAD_MALLOC if the memory allocation failed
    }

    // Match the image data to the paradigm blocks
    int buffer = 0; // Buffer for the current paradigm block index
    for (int imageY = 0; imageY < ebrImage->height; imageY++)
    { // Loop through the image
        for (int imageX = 0; imageX < ebrImage->width; imageX++)
        {
            for (int blockY = 0; blockY < paradigmBlocks[0].height; blockY++)
            { // Loop through the size of the paradigm blocks
                for (int blockX = 0; blockX < paradigmBlocks[0].width; blockX++)
                {
                    buffer = paradigmBlocks[ebrImage->data[imageY][imageX]].data[blockY][blockX]; // Get the current paradigm block data
                    if (buffer > paradigmBlockAmount || buffer < 0)
                    {                    // Check if the current paradigm block index is valid
                        return BAD_DATA; // Return BAD_DATA if the current paradigm block index is invalid
                    }
                    targetImage->data[imageY * paradigmBlocks[0].height + blockY][imageX * paradigmBlocks[0].width + blockX] = buffer; // Set the target image data to the current paradigm block data
                }
            }
        }
    }

    return SUCCESS;
}