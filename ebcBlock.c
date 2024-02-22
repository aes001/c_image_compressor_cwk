#include "ebcBlock.h"

int main(int argc, char **argv)
{
    // Check the arguments
    ebCheckArgs(argc, "ebcBlock");

    // Read the image
    Image image;
    int check = ebcRead(&image, argv[1], MAGIC_NUMBER_EBC);
    if (check != SUCCESS)
    {
        return ebErrorHandle(check, argv[1]);
    }

    // Create the blocks
    int heightBlockLength = ceil((((double)image.height) / BLOCK_HEIGHT)); // The number of blocks in the height
    int widthBlockLength = ceil((((double)image.width) / BLOCK_WIDTH));    // The number of blocks in the width
    int blockAmount = heightBlockLength * widthBlockLength;                // How many blocks are needed

    Block *block;                                         // The blocks
    block = (Block *)malloc(sizeof(Block) * blockAmount); // Allocate the memory for the blocks
    if (block == NULL)
    {                                                    // Check if the memory was allocated
        ebFree2DArray(image.data);                       // Free the memory for the image
        return ebErrorHandle(BAD_BLOCK_MALLOC, argv[1]); // return if memory was not allocated
    }
    check = blockerize(image.data, block, image.height, image.width); // Assign the blocks
    if (check != SUCCESS)
    {                                         // Check if the blocks were assigned
        ebFree2DArray(image.data);            // Free the memory for the image
        free(block);                          // Free the memory for the blocks
        return ebErrorHandle(check, argv[1]); // return if the blocks were not assigned
    }

    Image imageCompressed;                                                                 // Create the compressed image struct
    imageCompressed.height = heightBlockLength;                                            // Assign the height
    imageCompressed.width = widthBlockLength;                                              // Assign the width
    imageCompressed.data = ebCreate2DArray(imageCompressed.height, imageCompressed.width); // Allocate the memory for the compressed image
    if (imageCompressed.data == NULL)
    {
        ebFree2DArray(image.data); // Free the memory for the image
        for (int i = 0; i < blockAmount; i++)
        {
            ebFree2DArray(block[i].data); // Free the memory for each individual blocks
        }
        free(block);                                     // Free the memory for the blocks
        return ebErrorHandle(BAD_BLOCK_MALLOC, argv[1]); // return if the memory was not allocated
    }
    for (int currentBlockIndex = 0; currentBlockIndex < blockAmount; currentBlockIndex++)
    {                                                                                 // Loop through the blocks
        int blockAverageValue = blockAverage(block[currentBlockIndex]);               // Get the average value of the block
        int compressedImageY = currentBlockIndex / widthBlockLength;                  // Get the y position of the block in the compressed image
        int compressedImageX = currentBlockIndex % widthBlockLength;                  // Get the x position of the block in the compressed image
        imageCompressed.data[compressedImageY][compressedImageX] = blockAverageValue; // Assign the average value to the compressed image
    }

    check = ebcWrite(&imageCompressed, argv[2], MAGIC_NUMBER_EBCBLOCK); // Write the compressed image
    if (check != SUCCESS)
    {
        ebFree2DArray(image.data);           // Free the original image data
        ebFree2DArray(imageCompressed.data); // Free the compressed image data
        for (int i = 0; i < blockAmount; i++)
        {
            ebFree2DArray(block[i].data); // Free the memory for each individual blocks
        }
        free(block);                          // Free the memory for the blocks
        return ebErrorHandle(check, argv[2]); // return if the image was not written
    }

    for (int i = 0; i < blockAmount; i++)
    {
        ebFree2DArray(block[i].data); // Free the memory for each individual blocks
    }
    ebFree2DArray(image.data);           // Free the original image memory
    ebFree2DArray(imageCompressed.data); // Free the compressed image memory
    free(block);                         // Free the memory for the blocks

    printf("COMPRESSED\n"); // Print that the image was compressed

    return SUCCESS;
}