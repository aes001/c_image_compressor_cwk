#include "ebcR32.h"

int main(int argc, char **argv)
{
    // Check the arguments
    ebrCheckArgs(argc, "ebrR32");

    // Read the input file
    Image image;
    int check = ebcRead(&image, argv[1], MAGIC_NUMBER_EBC);
    if (check != SUCCESS)
    {
        return ebErrorHandle(check, argv[1]);
    }

    // Uniform Blockerize the image
    int heightBlockLength = floor((((double)image.height) / BLOCK_HEIGHT)); // The number of blocks in the height
    int widthBlockLength = floor((((double)image.width) / BLOCK_WIDTH));    // The number of blocks in the width
    int blockAmount = heightBlockLength * widthBlockLength;                 // How many blocks are needed
    Block *imageBlock;

    imageBlock = (Block *)malloc(sizeof(Block) * blockAmount); // Allocate the memory for the blocks
    if (imageBlock == NULL)
    { // Check if the memory allocation failed
        ebFree2DArray(image.data);
        return ebErrorHandle(BAD_MALLOC, argv[1]); // return if memory allocation failed
    }

    check = uniformBlockerize(&image, imageBlock); // Blockerize the image and store it in the imageBlock array
    if (check != SUCCESS)
    { // Check if the blockerization failed
        for (int i = 0; i < blockAmount; i++)
        {
            ebFree2DArray(imageBlock[i].data);
        }
        free(imageBlock);
        ebFree2DArray(image.data);
        return ebErrorHandle(check, argv[1]); // Return the error code
    }

    // Create the paradigm blocks
    Block *paradigmBlock = generateParadigmBlocks(imageBlock, blockAmount, PARADIGM_COUNT, atoi(argv[3]));
    if (paradigmBlock == NULL)
    { // Check if the paradigm block generation failed
        for (int i = 0; i < blockAmount; i++)
        {
            ebFree2DArray(imageBlock[i].data);
        }
        free(imageBlock);
        ebFree2DArray(image.data);
        printf("Error: Paradigm block generation failed\n"); // Print the error message
        return BAD_PARADIGM_GENERATION;                      // Return the error code
    }

    // Create the compressed image struct
    Image compressedImage;
    compressedImage.height = heightBlockLength; // Set the height of the compressed image to the number of blocks in the height
    compressedImage.width = widthBlockLength;   // Set the width of the compressed image to the number of blocks in the width

    compressedImage.data = ebCreate2DArray(heightBlockLength, widthBlockLength); // Allocate the memory for the compressed image
    if (compressedImage.data == NULL)
    { // Check if the memory allocation failed
        for (int i = 0; i < PARADIGM_COUNT; i++)
        {
            ebFree2DArray(paradigmBlock[i].data);
        }
        for (int i = 0; i < blockAmount; i++)
        {
            ebFree2DArray(imageBlock[i].data);
        }
        free(imageBlock);
        free(paradigmBlock);
        ebFree2DArray(image.data);
        return ebErrorHandle(BAD_MALLOC, argv[1]); // return if memory allocation failed
    }

    compressedImage.paradigm = ebCreate2DArray(BLOCK_HEIGHT, PARADIGM_COUNT * BLOCK_WIDTH);
    if (compressedImage.paradigm == NULL)
    { // Check if the memory allocation failed
        for (int i = 0; i < PARADIGM_COUNT; i++)
        {
            ebFree2DArray(paradigmBlock[i].data);
        }
        for (int i = 0; i < blockAmount; i++)
        {
            ebFree2DArray(imageBlock[i].data);
        }
        free(imageBlock);
        free(paradigmBlock);
        ebFree2DArray(image.data);
        ebFree2DArray(compressedImage.data);
        return ebErrorHandle(BAD_MALLOC, argv[1]); // return if memory allocation failed
    }

    compressedImage.paradigmBlockAmount = PARADIGM_COUNT; // Set the paradigm block amount to the number of paradigm blocks

    check = unblockerize(compressedImage.paradigm, paradigmBlock, BLOCK_HEIGHT, PARADIGM_COUNT * BLOCK_HEIGHT); // Put the paradigm blocks into the compressed image struct
    if (check != SUCCESS)
    { // Check if the unblockerization failed
        for (int i = 0; i < PARADIGM_COUNT; i++)
        { // Free the memory if the unblockerization failed
            ebFree2DArray(paradigmBlock[i].data);
        }
        for (int i = 0; i < blockAmount; i++)
        {
            ebFree2DArray(imageBlock[i].data);
        }
        free(imageBlock);
        free(paradigmBlock);
        ebFree2DArray(image.data);
        ebFree2DArray(compressedImage.data);
        ebFree2DArray(compressedImage.paradigm);
        return ebErrorHandle(check, argv[1]); // Return if the unblockerization failed
    }

    check = ebrFindBestParadigmBlock(imageBlock, blockAmount, paradigmBlock, &compressedImage); // Find the best paradigm block for each image block
    if (check != SUCCESS)
    { // Check if the find best paradigm block failed
        for (int i = 0; i < PARADIGM_COUNT; i++)
        { // Free the memory if we failed to find the best paradigm block
            ebFree2DArray(paradigmBlock[i].data);
        }
        for (int i = 0; i < blockAmount; i++)
        {
            ebFree2DArray(imageBlock[i].data);
        }
        free(imageBlock);
        free(paradigmBlock);
        ebFree2DArray(image.data);
        ebFree2DArray(compressedImage.data);
        ebFree2DArray(compressedImage.paradigm);
        return ebErrorHandle(check, argv[1]); // Return if the find best paradigm block failed
    }

    check = ebcWrite(&compressedImage, argv[2], MAGIC_NUMBER_EBCR32); // Write the compressed image to the output file
    if (check != SUCCESS)
    { // Check if the write failed
        for (int i = 0; i < PARADIGM_COUNT; i++)
        { // Free the memory if the write failed
            ebFree2DArray(paradigmBlock[i].data);
        }
        for (int i = 0; i < blockAmount; i++)
        {
            ebFree2DArray(imageBlock[i].data);
        }
        free(imageBlock);
        free(paradigmBlock);
        ebFree2DArray(image.data);
        ebFree2DArray(compressedImage.data);
        ebFree2DArray(compressedImage.paradigm);
        return ebErrorHandle(check, argv[2]); // Return if the write failed
    }

    printf("COMPRESSED\n"); // If we got here, the compression was successful and we can print "COMPRESSED"

    // Free the memory
    for (int i = 0; i < PARADIGM_COUNT; i++)
    {
        free(paradigmBlock[i].data); // Free every single paradigm block
    }
    for (int i = 0; i < blockAmount; i++)
    {
        free(imageBlock[i].data); // Free every single image block
    }
    free(imageBlock);                        // Free the image block array
    free(paradigmBlock);                     // Free the paradigm block array
    ebFree2DArray(image.data);               // Free the image data
    ebFree2DArray(compressedImage.data);     // Free the compressed image data
    ebFree2DArray(compressedImage.paradigm); // Free the compressed image paradigm

    return SUCCESS; // Return success
}