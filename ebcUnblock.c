#include "ebcUnblock.h"

int main(int argc, char **argv)
{
    // Check the arguments
    ebCheckArgs(argc, "ebcUnblock");

    // Read the image
    Image image;
    int check = ebcRead(&image, argv[1], MAGIC_NUMBER_EBCBLOCK);
    if (check != SUCCESS)
    {
        return ebErrorHandle(check, argv[1]); // return if read failed
    }

    /**
     * Custom blockerize function
     *
     * This section will read every pixel in the image and put it into a BLOCK_WIDTH x BLOCK_HEIGHT block
     * Every value in the block will be the same value as the pixel
     * Essentially, we are duplicating every pixel 9 times.
     */
    int blockAmount = image.height * image.width;         // Calculate how many blocks are needed
    Block *block;                                         // The blocks
    block = (Block *)malloc(sizeof(Block) * blockAmount); // Allocate the memory for the blocks
    if (block == NULL)
    {
        ebFree2DArray(image.data);                 // Free the memory for the image
        return ebErrorHandle(BAD_MALLOC, argv[1]); // return if memory was not allocated
    }
    int currentBlockIndex = 0; // The current block index
    for (int imgY = 0; imgY < image.height; imgY++)
    { // Loop through the image
        for (int imgX = 0; imgX < image.width; imgX++)
        {
            block[currentBlockIndex].width = BLOCK_WIDTH;                               // Assign the block width
            block[currentBlockIndex].height = BLOCK_HEIGHT;                             // Assign the block height
            block[currentBlockIndex].data = ebCreate2DArray(BLOCK_HEIGHT, BLOCK_WIDTH); // Allocate the memory for the block
            if (block[currentBlockIndex].data == NULL)
            {
                ebFree2DArray(image.data);                 // Free the memory for the image
                free(block);                               // Free the memory for the block
                return ebErrorHandle(BAD_MALLOC, argv[1]); // return if memory was not allocated
            }

            for (int blockY = 0; blockY < BLOCK_HEIGHT; blockY++)
            { // Loop through the block
                for (int blockX = 0; blockX < BLOCK_WIDTH; blockX++)
                {
                    block[currentBlockIndex].data[blockY][blockX] = image.data[imgY][imgX]; // Assign the pixel value to each element in the block
                }
            }
            currentBlockIndex++; // Increment the current block index
        }
    }

    Image imageDecompressed;                                                                     // Create the decompressed image struct
    imageDecompressed.height = image.height * BLOCK_HEIGHT;                                      // Assign the height
    imageDecompressed.width = image.width * BLOCK_WIDTH;                                         // Assign the width
    imageDecompressed.data = ebCreate2DArray(imageDecompressed.height, imageDecompressed.width); // Allocate the memory for the image
    if (imageDecompressed.data == NULL)
    {
        ebFree2DArray(image.data); // Free the memory for the image
        for (int i = 0; i < blockAmount; i++)
        {
            ebFree2DArray(block[i].data); // Free the memory for the individual blocks
        }
        free(block);                               // Free the memory for the block
        return ebErrorHandle(BAD_MALLOC, argv[1]); // return if memory was not allocated
    }

    check = unblockerize(imageDecompressed.data, block, imageDecompressed.height, imageDecompressed.width); // Unblockerize the image (Assign blocks to an image data array)
    if (check != SUCCESS)
    {
        ebFree2DArray(imageDecompressed.data); // Free the memory for the decompressed image
        ebFree2DArray(image.data);             // Free the memory for the image
        for (int i = 0; i < blockAmount; i++)
        {
            ebFree2DArray(block[i].data); // Free the memory for the individual blocks
        }
        free(block);                          // Free the memory for the block
        return ebErrorHandle(check, argv[1]); // return if unblockerize failed
    }

    check = ebcWrite(&imageDecompressed, argv[2], MAGIC_NUMBER_EBC); // Write the image to a file
    if (check != SUCCESS)
    {
        ebFree2DArray(imageDecompressed.data); // Free the memory for the decompressed image
        ebFree2DArray(image.data);             // Free the memory for the image
        for (int i = 0; i < blockAmount; i++)
        {
            ebFree2DArray(block[i].data); // Free the memory for the individual blocks
        }
        free(block);                          // Free the memory for the block
        return ebErrorHandle(check, argv[2]); // return if write failed
    }

    // If the program reaches this point, the image was successfully decompressed
    printf("DECOMPRESSED\n"); // Print that the image was decompressed

    // Free the memory
    ebFree2DArray(image.data);             // Free the memory for the image
    ebFree2DArray(imageDecompressed.data); // Free the memory for the decompressed image
    for (int i = 0; i < blockAmount; i++)
    {
        ebFree2DArray(block[i].data); // Free the memory for the individual blocks
    }
    free(block); // Free the memory for the block

    return SUCCESS;
}