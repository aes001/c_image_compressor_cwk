#include "ebcU128.h"

int main(int argc, char **argv)
{
    // Check the arguments
    ebCheckArgs(argc, "ebcU128");

    // Read the input file
    Image image;                                                // Create a new image to store the compressed data
    int check = ebcRead(&image, argv[1], MAGIC_NUMBER_EBCR128); // Read the compressed data into the image
    if (check != SUCCESS)
    {                                         // Check if ebcRead failed
        return ebErrorHandle(check, argv[1]); // Return the error if ebcRead failed
    }

    // Blockerize the paradigm blocks
    int paradigmBlockPixelHeight = BLOCK_HEIGHT;                // How many pixels tall is each paradigm block
    int paradigmBlockPixelWidth = BLOCK_WIDTH * PARADIGM_COUNT; // How many pixels wide is each paradigm block
    Block *paradigmBlock;
    paradigmBlock = (Block *)malloc(sizeof(Block) * PARADIGM_COUNT); // Allocate memory for the paradigm blocks
    if (paradigmBlock == NULL)
    {                                              // Check if malloc failed
        ebFree2DArray(image.data);                 // Free the memory for the image data if malloc failed
        ebFree2DArray(image.paradigm);             // Free the memory for the image paradigm if malloc failed
        return ebErrorHandle(BAD_MALLOC, argv[1]); // Return the error
    }
    check = blockerize(image.paradigm, paradigmBlock, paradigmBlockPixelHeight, paradigmBlockPixelWidth); // Put the paradigm blocks into actual blocks
    if (check != SUCCESS)
    {                                         // Check if blockerize failed
        free(paradigmBlock);                  // Free the memory for the paradigm blocks if blockerize failed
        ebFree2DArray(image.data);            // Free the memory for the image data if blockerize failed
        ebFree2DArray(image.paradigm);        // Free the memory for the image paradigm if blockerize failed
        return ebErrorHandle(check, argv[1]); // Return the error
    }

    // Match the paradigm blocks to the image data
    Image imageDecompressed;                                                     // Create a new image to store the decompressed data
    check = ebrMatch(&image, paradigmBlock, &imageDecompressed, PARADIGM_COUNT); // Decompress the image
    if (check != SUCCESS)
    { // Check if ebrMatch failed
        for (int i = 0; i < PARADIGM_COUNT; i++)
        {
            ebFree2DArray(paradigmBlock[i].data); // Free every single paradigm block
        }
        free(paradigmBlock);           // Free the memory for the paradigm blocks if ebrMatch failed
        ebFree2DArray(image.data);     // Free the memory for the image data if ebrMatch failed
        ebFree2DArray(image.paradigm); // Free the memory for the image paradigm if ebrMatch failed
        if (imageDecompressed.data != NULL)
        {                                          // Check if the decompressed image data was allocated
            ebFree2DArray(imageDecompressed.data); // Free the memory for the decompressed image data if it was allocated
        }
        return ebErrorHandle(check, argv[1]); // Return the error
    }

    check = ebcWrite(&imageDecompressed, argv[2], MAGIC_NUMBER_EBC);
    if (check != SUCCESS)
    { // Check if ebcWrite failed
        for (int i = 0; i < PARADIGM_COUNT; i++)
        {
            ebFree2DArray(paradigmBlock[i].data); // Free every single paradigm block
        }
        free(paradigmBlock);                   // Free the memory for the paradigm blocks if ebcWrite failed
        ebFree2DArray(image.data);             // Free the memory for the image data if ebcWrite failed
        ebFree2DArray(image.paradigm);         // Free the memory for the image paradigm if ebcWrite failed
        ebFree2DArray(imageDecompressed.data); // Free the memory for the decompressed image data if ebcWrite failed
        return ebErrorHandle(check, argv[1]);  // Return the error
    }

    // If we got here, then the image was decompressed successfully
    printf("DECOMPRESSED\n"); // Print that the image was decompressed successfully

    // Free the memory
    for (int i = 0; i < PARADIGM_COUNT; i++)
    {
        ebFree2DArray(paradigmBlock[i].data); // Free every single paradigm block
    }
    free(paradigmBlock);                   // Free the memory for the paradigm blocks
    ebFree2DArray(image.data);             // Free the memory for the image data
    ebFree2DArray(image.paradigm);         // Free the memory for the image paradigm
    ebFree2DArray(imageDecompressed.data); // Free the memory for the decompressed image data

    return SUCCESS;
}
