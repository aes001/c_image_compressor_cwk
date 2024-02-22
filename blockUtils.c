#include "blockUtils.h"

/**
 * This function takes in an ebimage and a block array and fills the block array with the data from the image.
 * The normal block size is 3x3
 *
 * @param pixels The image to be blocked
 * @param block The block array to be filled
 * @param height The height of the image
 * @param width The width of the image
 * @return 0 on success, BAD_MALLOC if memory allocation fails
 *
 * Note: The Block struct must be initialized with the correct amount of blocks before calling this function or else the behavior is undefined
 */
int blockerize(unsigned int **pixels, Block *block, int height, int width)
{
    int currentBlockIndex = 0; // The current block index
    for (int imageY = 0; imageY < height; imageY = imageY + BLOCK_HEIGHT)
    { // Loop through the image
        for (int imageX = 0; imageX < width; imageX = imageX + BLOCK_WIDTH)
        {
            int widthRemaining = width - imageX;   // The width remaining in the image
            int heightRemaining = height - imageY; // The height remaining in the image

            int blockWidth = widthRemaining < BLOCK_WIDTH ? widthRemaining : BLOCK_WIDTH;      // If the width remaining is less than the block width, set the block width to the width remaining else set it to the block width constant
            int blockHeight = heightRemaining < BLOCK_HEIGHT ? heightRemaining : BLOCK_HEIGHT; // If the height remaining is less than the block height, set the block height to the height remaining else set it to the block height constant

            block[currentBlockIndex].width = blockWidth;                              // Set the block width
            block[currentBlockIndex].height = blockHeight;                            // Set the block height
            block[currentBlockIndex].data = ebCreate2DArray(blockHeight, blockWidth); // Allocate the memory for the block data
            if (block[currentBlockIndex].data == NULL)
            {                      // Check if the memory allocation failed
                return BAD_MALLOC; // Return if memory allocation failed
            }

            for (int blockY = 0; blockY < blockHeight; blockY++)
            { // Loop through the block and fill every element with the data from the image
                for (int blockX = 0; blockX < blockWidth; blockX++)
                {
                    block[currentBlockIndex].data[blockY][blockX] = pixels[imageY + blockY][imageX + blockX]; // Fill the block with the data from the image
                }
            }
            currentBlockIndex++;
        }
    }
    return SUCCESS;
}

/**
 * This function takes in a block array and an ebImage and fills the ebImage with the data from the block array.
 *
 * @param target The image to be filled
 * @param block The block array to be unblocked
 * @param height The height of the image
 * @param width The width of the image
 * @return 0 on success, BAD_MALLOC if memory allocation fails
 *
 * Note: The image struct must have the correct height and width values set before calling this function of else the behavior is undefined
 */
int unblockerize(unsigned int **target, Block *block, int height, int width)
{
    int currentBlockIndex = 0;
    for (int imgY = 0; imgY < height; imgY = imgY + BLOCK_HEIGHT)
    { // Loop through the image block by block
        for (int imgX = 0; imgX < width; imgX = imgX + BLOCK_HEIGHT)
        {
            for (int blockY = 0; blockY < block[currentBlockIndex].height; blockY++)
            { // Loop through the block and fill every element with the data from the image
                for (int blockX = 0; blockX < block[currentBlockIndex].width; blockX++)
                {
                    target[imgY + blockY][imgX + blockX] = block[currentBlockIndex].data[blockY][blockX]; // Fill the image with the data from the block
                }
            }
            currentBlockIndex++; // Increment the block index
        }
    }
    return SUCCESS;
}

/**
 * This function calculates the average value of a block
 *
 * @param block The block to be averaged
 * @return The average value of the block
 */
int blockAverage(Block block)
{
    int sum = 0; // The sum of the block
    for (int blockY = 0; blockY < block.height; blockY++)
    { // Loop through the block
        for (int blockX = 0; blockX < block.width; blockX++)
        {
            sum += block.data[blockY][blockX]; // Add the value of the current element to the sum
        }
    }
    return round((double)sum / BLOCK_SIZE); // Return the average of the block
}

/**
 * This function average of the values in a differece block
 *
 * @param block The block to be averaged
 * @return The average value of the block
 */
double diffBlockAverage(DiffBlock block)
{
    double sum = 0; // The sum of the block
    for (int blockY = 0; blockY < block.height; blockY++)
    { // Loop through the block
        for (int blockX = 0; blockX < block.width; blockX++)
        {
            sum += (double)block.data[blockY][blockX]; // Add the value of the current element to the sum
        }
    }
    return sum / BLOCK_SIZE; // Return the average of the block
}

/**
 * This function calculates the sum of a block
 *
 * @param block The block to be summed
 * @return The sum of the block
 */
double diffBlockSum(DiffBlock block)
{
    double sum = 0; // The sum of the block
    for (int blockY = 0; blockY < block.height; blockY++)
    { // Loop through the block
        for (int blockX = 0; blockX < block.width; blockX++)
        {                                              // Add the value of the current element to the sum
            sum += (double)block.data[blockY][blockX]; // Add the value of the current element to the sum
        }
    }
    return sum; // Return the sum of the block
}

/**
 * This function calculates the difference between two blocks
 *
 * @param block1 The first block
 * @param block2 The second block
 * @return The difference between the two blocks
 */
int blockDifference(Block block1, Block block2)
{
    if (block1.width != block2.width || block1.height != block2.height)
    {                    // Check if the blocks are the same size
        return BAD_DATA; // Return if the blocks are not the same size
    }

    int sum = 0; // The sum of the difference
    for (int blockY = 0; blockY < block1.height; blockY++)
    { // Loop through the blocks
        for (int blockX = 0; blockX < block1.width; blockX++)
        {
            sum += abs(((int)block1.data[blockY][blockX]) - ((int)block2.data[blockY][blockX])); // Add the difference between the two blocks to the sum
        }
    }
    return sum; // Return the sum of the difference
}

/**
 * This function takes in an ebimage and a block array and fills the block array with the data from the image.
 * However, unlike blockerize() this function forces all blocks to be the same size.
 *
 * @param image The image to be blocked
 * @param block The block array to be filled
 * @return 0 on success, BAD_MALLOC if memory allocation fails
 * Note: The image struct must have the correct height and width values set before calling this function of else the behavior is undefined
 * Note: The block array must be large enough to hold all the blocks in the image
 */
int uniformBlockerize(Image *image, Block *block)
{
    int currentBlockIndex = 0;
    for (int imageY = 0; imageY < image->height; imageY = imageY + BLOCK_HEIGHT)
    { // Loop through the image block by block
        for (int imageX = 0; imageX < image->width; imageX = imageX + BLOCK_WIDTH)
        {
            int widthRemaining = image->width - imageX;   // The amount of width left in the image
            int heightRemaining = image->height - imageY; // The amount of height left in the image

            if (widthRemaining < BLOCK_WIDTH || heightRemaining < BLOCK_HEIGHT)
            {          // Check if the block is smaller than the standard block size
                break; // Break out of the loop if the block is smaller than the standard block size
            }

            block[currentBlockIndex].height = BLOCK_HEIGHT;                             // Set the height of the block
            block[currentBlockIndex].width = BLOCK_WIDTH;                               // Set the width of the block
            block[currentBlockIndex].data = ebCreate2DArray(BLOCK_HEIGHT, BLOCK_WIDTH); // Allocate memory for the block
            if (block[currentBlockIndex].data == NULL)
            {
                return BAD_MALLOC; // Return if memory allocation fails
            }

            for (int blockY = 0; blockY < BLOCK_HEIGHT; blockY++)
            { // Loop through the block and fill every element with the data from the image
                for (int blockX = 0; blockX < BLOCK_WIDTH; blockX++)
                {
                    block[currentBlockIndex].data[blockY][blockX] = image->data[imageY + blockY][imageX + blockX]; // Fill the block with the data from the image
                }
            }
            currentBlockIndex++; // Increment the block index
        }
    }
    return SUCCESS;
}