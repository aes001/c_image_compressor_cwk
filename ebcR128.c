#include "ebcR128.h"

int main(int argc, char **argv)
{
    // Check the arguments
    ebrCheckArgs(argc, "ebrR128");

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
    compressedImage.height = heightBlockLength; // Set the height of the compressed image
    compressedImage.width = widthBlockLength;   // Set the width of the compressed image

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
        return ebErrorHandle(BAD_MALLOC, argv[1]); // Return the error code
    }

    compressedImage.paradigm = ebCreate2DArray(BLOCK_HEIGHT, PARADIGM_COUNT * BLOCK_WIDTH); // Allocate the memory for the paradigm blocks
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
        return ebErrorHandle(BAD_MALLOC, argv[1]); // Return the error code
    }

    compressedImage.paradigmBlockAmount = PARADIGM_COUNT; // Set the number of paradigm blocks in the compressed image

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
        return ebErrorHandle(check, argv[1]); // Return the error code
    }

    check = ebrFindBestParadigmBlock(imageBlock, blockAmount, paradigmBlock, &compressedImage); // Find the best paradigm block for each block in the image
    if (check != SUCCESS)
    { // Check if the paradigm block finding failed
        for (int i = 0; i < PARADIGM_COUNT; i++)
        { // Free the memory if the paradigm block finding failed
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
        return ebErrorHandle(check, argv[1]); // Return the error code
    }

    check = ebcWrite(&compressedImage, argv[2], MAGIC_NUMBER_EBCR128); // Write the compressed image to the output file
    if (check != SUCCESS)
    { // Check if the writing failed
        for (int i = 0; i < PARADIGM_COUNT; i++)
        { // Free the memory if the writing failed
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
        return ebErrorHandle(check, argv[2]); // Return the error code
    }

    printf("COMPRESSED\n"); // If we got here, the compression was successful

    // Free the memory
    for (int i = 0; i < PARADIGM_COUNT; i++)
    {
        ebFree2DArray(paradigmBlock[i].data); // Free the memory for the every paradigm blocks
    }
    for (int i = 0; i < blockAmount; i++)
    {
        ebFree2DArray(imageBlock[i].data); // Free the memory for the every image blocks
    }
    free(imageBlock);                        // Free the memory for the image blocks
    free(paradigmBlock);                     // Free the memory for the paradigm blocks
    ebFree2DArray(image.data);               // Free the memory for the image
    ebFree2DArray(compressedImage.data);     // Free the memory for the compressed image
    ebFree2DArray(compressedImage.paradigm); // Free the memory for the paradigm blocks in the compressed image

    return SUCCESS;
}