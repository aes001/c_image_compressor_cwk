#include "ebcUtils.h"

/**
 * This function reads the header of the file and stores the information in the image struct.
 *
 * @param fp The file pointer to the file that is being read
 * @param image The image struct that the header information is stored in
 * @param expectedMagicNumber The magic number that the file should have
 * @return 0 if the header was read correctly
 * @return one of the error codes in ebConstants.h if we encountered their respective error
 */
int ebcRead(Image *image, char *filename, int expectedMagicNumber)
{
    // open the file
    FILE *fp = fopen(filename, "rb");
    if (fp == NULL)
    { // Check if the file opened
        return BAD_FILE;
    }

    int check = ebReadHeader(fp, image, expectedMagicNumber);
    if (check != SUCCESS)
    { // Check if the header was read correctly
        fclose(fp);
        return check;
    }

    int mode = 5;
    int paradigmBlockAmount = 0;
    if (expectedMagicNumber == MAGIC_NUMBER_EBCR32 || expectedMagicNumber == MAGIC_NUMBER_EBCR128)
    { // Check if the file is compressed
        if (expectedMagicNumber == MAGIC_NUMBER_EBCR32)
        {                             // Check if the file is compressed with 32 paradigms
            mode = 5;                 // Set the mode to 5 so that the data is read correctly
            paradigmBlockAmount = 32; // Set the paradigm block amount to 32 so that the data is read correctly
        }
        else if (expectedMagicNumber == MAGIC_NUMBER_EBCR128)
        {                              // Check if the file is compressed with 128 paradigms
            mode = 7;                  // Set the mode to 7 so that the data is read correctly
            paradigmBlockAmount = 128; // Set the paradigm block amount to 128 so that the data is read correctly
        }
        // Read the paradigm block
        image->paradigm = ebCreate2DArray(BLOCK_HEIGHT, paradigmBlockAmount * BLOCK_WIDTH); // Allocate memory for the paradigm block
        if (image->paradigm == NULL)
        { // If the memory allocation failed return an error
            fclose(fp);
            return BAD_MALLOC;
        }
        fgetc(fp);                                                                                              // Skip the newline character at the end of the header
        check = ebcUniversalReader(image->paradigm, fp, mode, BLOCK_HEIGHT, paradigmBlockAmount * BLOCK_WIDTH); // Read the paradigm block
        if (check != SUCCESS)
        { // Check if the data was read correctly
            fclose(fp);
            return check;
        }
    }

    // Get the data
    image->data = ebCreate2DArray(image->height, image->width); // Allocate memory for the data
    if (image->data == NULL)
    { // If the memory allocation failed return an error
        fclose(fp);
        return BAD_MALLOC;
    }

    fgetc(fp); // skip the newline character at the end of the paradigm block

    // Read the data
    check = ebcUniversalReader(image->data, fp, mode, image->height, image->width);
    if (check != SUCCESS)
    { // Check if the data was read correctly
        fclose(fp);
        return check;
    }

    if (fgetc(fp) != EOF)
    { // Try to read a byte from the file. If we are able to read a byte from the file then there is too much data in the file and we return an error
        fclose(fp);
        return BAD_DATA;
    }

    fclose(fp);     // Close the file
    return SUCCESS; // Return success
} // ebcRead()

/**
 * This function writes the header of the file and stores the information in the image struct.
 *
 * @param fp The file pointer to the file that is being written
 * @param image The image struct that the header information is stored in
 * @param magicNumber The magic number that the file should have
 * @return 0 if the header was written correctly
 * @return one of the error codes in ebConstants.h if we encountered their respective error
 */
int ebcWrite(Image *image, char *filename, int magicNumber)
{
    // open the file
    FILE *fp = fopen(filename, "wb");
    if (fp == NULL)
    { // Check if the file opened
        return BAD_FILE;
    }

    // write the header
    int check = ebWriteHeader(fp, image, magicNumber);
    if (check != SUCCESS)
    { // Check if the header was written correctly
        fclose(fp);
        return check;
    }

    int mode = 5;
    if (magicNumber == MAGIC_NUMBER_EBCR32 || magicNumber == MAGIC_NUMBER_EBCR128) // Check if the file is compressed
    {
        if (magicNumber == MAGIC_NUMBER_EBCR32) // Check if the file is compressed with 32 paradigms
        {
            mode = 5; // Set the mode to 5 so the data can be read 5 bits at a time
        }
        else if (magicNumber == MAGIC_NUMBER_EBCR128) // Check if the file is compressed with 128 paradigms
        {
            mode = 7; // Set the mode to 7 so the data can be read 7 bits at a time
        }
        check = ebcUniversalWriter(image->paradigm, fp, mode, BLOCK_HEIGHT, image->paradigmBlockAmount * BLOCK_WIDTH); // Write the paradigm block
        if (check != SUCCESS)                                                                                          // Check if the paradigm blocks were written correctly
        {
            fclose(fp);
            return check;
        }

        check = fprintf(fp, "\n"); // Write a newline character at the end of the paradigm block
    }

    check = ebcUniversalWriter(image->data, fp, mode, image->height, image->width); // Write the data
    if (check != SUCCESS)
    { // Check if the data was written correctly
        fclose(fp);
        return check;
    }

    fclose(fp);     // Close the file
    return SUCCESS; // Return success
} // ebcWrite()

/**
 * This function initializes an ebcMask array with the values for the read mask.
 *
 * Variable 'm' is an array of 12 unsigned chars that hold the masks for the 12 masks.
 * The masks are as follows:
 * 248 = 11111000,  7 = 00000111, 192 = 11000000,  62 = 00111110, 1 = 00000001,
 * 240 = 11110000, 15 = 00001111, 128 = 10000000, 124 = 01111100, 3 = 00000011,
 * 224 = 11100000, 31 = 00011111
 *
 * Variable 'sA' is an array of 12 ints that hold the shift values for each mask in the
 * same index.
 * A positive value means that the bits are shifted to the right and a negative value
 * means that the bits are shifted to the left.
 *
 * Variable 'nBG' is an array of 12 ints that hold the number of bits that are gathered.
 * This is used to calculate the number of bits left in the current byte.
 * This value is calculated by counting the number of 1's in the mask using the btuPopCount function.
 *
 * @param mask The ebcMask array that is being initialized
 */
void ebcReadMask5Init(ebcMask *mask)
{
    unsigned char m[] = {248, 7, 192, 62, 1, 240, 15, 128, 124, 3, 224, 31}; // The masks
    int sA[] = {3, -2, 6, 1, -4, 4, -1, 7, 2, -3, 5, 0};                     // The shift values

    for (int i = 0; i < 12; i++)
    {                                               // Loop through the arrays and initialize the ebcMask array
        mask[i].mask = m[i];                        // Set the mask value
        mask[i].shift = sA[i];                      // Set the shift value
        mask[i].noBitsGathered = btuPopCount(m[i]); // Set the number of bits gathered
    }
} // ebcReadMaskInit()

/**
 * This function initializes an ebcMask array with the values for the read mask for 7 bit reading.
 *
 * @param mask The ebcMask array that is being initialized
 */
void ebcReadMask7Init(ebcMask *mask)
{
    unsigned char m[] = {254, 1, 252, 3, 248, 7, 240, 15, 224, 31, 192, 63, 128, 127};
    int sA[] = {1, -6, 2, -5, 3, -4, 4, -3, 5, -2, 6, -1, 7, 0};

    for (int i = 0; i < 14; i++)
    {
        mask[i].mask = m[i];
        mask[i].shift = sA[i];
        mask[i].noBitsGathered = btuPopCount(m[i]);
    }
}

/**
 * This function initializes an ebcMask array with the values for the write mask.
 * The write mask is used to construct a byte from the 5 bits data in the data array.
 *
 * Variable 'm' is an array of 12 unsigned chars that hold the masks.
 * The masks are as follows:
 * 31 = 0b11111, 28 = 0b11100, 3 = 0b00011, 31 = 0b11111, 16 = 0b10000, 15 = 0b01111,
 * 30 = 0b11110, 1 = 0b00001, 31 = 0b11111, 24 = 0b11000, 7 = 0b00111, 31 = 0b11111
 *
 * Variable 'sA' is an array of 12 ints that hold the shift values for each mask in the same index.
 * A positive shift amount means that the bits are shifted to the right and a negative
 * shift amount means that the bits are shifted to the left.
 *
 * Variable 'nBG' is an array of 12 ints that hold the number of bits a mask gathers.
 * This is used to calculate the number of bits left in the current byte.
 * This value is calculated by counting the number of 1's in the mask using the btuPopCount function.
 *
 * @param mask The ebcMask array that is being initialized
 */
void ebcWriteMask5Init(ebcMask *mask)
{
    unsigned char m[] = {31, 28, 3, 31, 16, 15, 30, 1, 31, 24, 7, 31}; // Array of mask values
    int sA[] = {-3, 2, -6, -1, 4, -4, 1, -7, -2, 3, -5, 0};            // Array of shift values

    for (int i = 0; i < 12; i++)
    {                                               // Loop through the array of ebcMask structs and initialize each struct with their respective values
        mask[i].mask = m[i];                        // Put the mask value in each mask struct
        mask[i].shift = sA[i];                      // Put the shift amount in each mask struct
        mask[i].noBitsGathered = btuPopCount(m[i]); // Put the number of bits gathered in each mask struct
    }
} // ebcWriteMaskInit()

/**
 * This function initializes an ebcMask array with the values for the write mask for 7 bit writing.
 *
 * @param mask The ebcMask array that is being initialized
 */
void ebcWriteMask7Init(ebcMask *mask)
{
    unsigned char m[] = {127, 64, 63, 96, 31, 112, 15, 120, 7, 124, 3, 126, 1, 127};
    int sA[] = {-1, 6, -2, 5, -3, 4, -4, 3, -5, 2, -6, 1, -7, 0};

    for (int i = 0; i < 14; i++)
    {
        mask[i].mask = m[i];
        mask[i].shift = sA[i];
        mask[i].noBitsGathered = btuPopCount(m[i]);
    }
}

/**
 * This function writes a given set of pixel values to a file depending on the mode.
 *
 * @param values The array of pixel values that are being written to the file
 * @param fp The file pointer to the file that is being written to
 * @param mode The mode that is being used to write the data to the file (5 or 7 bit)
 * @param height The height of the image
 * @param width The width of the image
 * @return Returns 0 if the function was successful; BAD_OUTPUT if the file cannot be written to
 */
int ebcUniversalWriter(unsigned int **values, FILE *fp, int mode, int height, int width)
{
    /**
     * The following code is used to write the data to the file.
     * One pixel is read from the data array.
     * The pixel is then masked with ebcWriteMask and the bits are shifted according to the shift value in the mask.
     * The masked and shifted bits are then added to the cleanBuffer and the number of bits added to the cleanBuffer is
     * counted in bitsGathered.
     * The number of bits that have been read from the pixel is subtracted from bitsLeft.
     * If the cleanBuffer has 8 bits it is written to the file and the cleanBuffer is cleared.
     * If the bitsLeft is 0 then a new pixel is read from the data array.
     * Then we advance to the next mask and repeat the process until the pixelCount is equal to the number of pixels in the image
     * and the cleanBuffer is empty.
     */
    int check = 0;                 // Used to check if the write was successful
    int bitsRead = mode;           // Number of bits read from the current byte
    int bitsGathered = 0;          // Number of bits that have been added to the cleanBuffer
    unsigned char dirtyBuffer = 0; // The working buffer that holds the bits that have not been positioned to fit the 8 bit byte
    unsigned char cleanBuffer = 0; // The buffer that holds the bits that have been positioned to fit the 8 bit byte
    unsigned char currentByte = 0; // The current byte that is read from the image data array
    int pixelCount = 0;            // How times we have read a pixel from the data array
    int run = GO;                  // Controls the loop
    ebcMask mask[14];              // The array of masks
    int maskAmount = 12;           // The number of masks
    if (mode == 5)
    {
        ebcWriteMask5Init(mask); // Initialise the masks in the array as an ebcWriteMask
        maskAmount = 12;         // Set the number of masks
    }
    else
    {
        ebcWriteMask7Init(mask); // Initialise the masks in the array as an ebcWriteMask
        maskAmount = 14;         // Set the number of masks
    }

    while (run)
    {
        for (int i = 0; i < maskAmount; i++)
        {
            if (bitsRead == mode)
            {                                                                 // Check if we need to read a new byte (if we have read all the bits from the current pixel)
                currentByte = values[pixelCount / width][pixelCount % width]; // If we need to, read a pixel from the data array and store it in the currentByte
                bitsRead = 0;                                                 // Reset the bitsRead counter
                pixelCount++;                                                 // Increment the pixelCount
            }
            dirtyBuffer = currentByte & mask[i].mask;               // Mask the pixel with the mask
            btuMultiDirectionBitShift(&dirtyBuffer, mask[i].shift); // Shift the bits according to the shift value in the mask
            cleanBuffer = cleanBuffer | dirtyBuffer;                // Append the masked and shifted bits to the cleanBuffer
            bitsRead = bitsRead + mask[i].noBitsGathered;           // Add the number of bits read from the pixel to the bitsRead counter
            bitsGathered = bitsGathered + mask[i].noBitsGathered;   // Add the number of bits read from the pixel to the bitsGathered counter
            if (bitsGathered == 8)
            {                                           // Check if we have read 8 bits to the cleanBuffer
                check = fwrite(&cleanBuffer, 1, 1, fp); // Write the cleanBuffer to the file if we have read 8 bits
                if (check == 0)
                { // Check for write errors
                    return BAD_OUTPUT;
                }
                bitsGathered = 0; // Reset the bitsGathered counter
                cleanBuffer = 0;  // Reset the cleanBuffer
            }
            dirtyBuffer = 0; // Reset the dirtyBuffer
            if (pixelCount == height * width)
            { // Check if we have read the expected number of pixels
                if (bitsGathered != 0)
                {                                           // Check if the cleanBuffer is empty
                    check = fwrite(&cleanBuffer, 1, 1, fp); // Write the cleanBuffer to the file if it is not empty
                    if (check == 0)
                    { // Check for write errors
                        return BAD_OUTPUT;
                    }
                }
                run = STOP; // If we have read the expected number of pixel and there is no more data in the cleanBuffer then we exit the loop
                break;      // Exit the loop
            }
        }
    }

    return SUCCESS;
}

/**
 * This function reads a given set of pixel values from a file depending on the mode.
 *
 * @param store The array of pixel values to store the data that is read from the file
 * @param fp The file pointer to the file that is being read from
 * @param bitMode The mode that is being used to read the data from the file (5 or 7 bit)
 * @param height The height of the image
 * @param width The width of the image
 * @return Returns 0 if the function was successful; BAD_DATA if the file has an incorrect number of pixels
 */
int ebcUniversalReader(unsigned int **store, FILE *fp, int bitMode, int height, int width)
{
    /**
     * The following code is used to read the data from the file.
     * One byte is read from the file.
     * The byte is then masked with ebcReadMask and the bits are shifted according to the shift value in the mask.
     * The masked and shifted byte is then added to the cleanBuffer and the number of bits added to the cleanBuffer is
     * counted in bitsGathered.
     * The number of bits that have been read from the byte is subtracted from bitsLeft.
     * If the cleanBuffer has 5 bits it is written to the data array and the cleanBuffer is cleared and we add 1 to the pixelCount.
     * If the bitsLeft is 0 then a new byte is read from the file.
     * Then we advance to the next mask and repeat the process until the pixelCount is equal to the number of pixels in the image.
     */
    int check = 0;                 // Used to check if the read was successful
    int bitsLeft = 0;              // The number of bits left in the current byte
    int bitsGathered = 0;          // The number of bits that have been added to the cleanBuffer
    unsigned char cleanBuffer = 0; // The buffer that holds the bits from the file that are in their correct position
    unsigned char dirtyBuffer = 0; // The buffer that holds the bits from the file that are not in their correct position
    unsigned char currentByte = 0; // The current byte that is read from the file
    int pixelCount = 0;            // The number of pixels that have been read from the file
    int run = GO;                  // Used to control the loop
    int maskAmount = 12;           // The number of masks
    ebcMask mask[14];              // The array of masks
    if (bitMode == 5)
    {
        maskAmount = 12;        // Set the number of masks
        ebcReadMask5Init(mask); // Initialise the masks in the array as an ebcReadMask
    }
    else
    {
        maskAmount = 14;        // Set the number of masks
        ebcReadMask7Init(mask); // Initialise the masks in the array as an ebcReadMask
    }

    while (run)
    {
        for (int i = 0; i < maskAmount; i++)
        { // Loop through the masks
            // Check if we need to read a new byte (if we have read all the bits from the current byte)
            if (bitsLeft == 0)
            {
                check = fread(&currentByte, 1, 1, fp);
                bitsLeft = 8; // Reset the bitsLeft counter
                if (check == 0)
                { // If we fail to read a byte then there is something wrong with the file and we return an error
                    fclose(fp);
                    return BAD_DATA;
                }
            }
            dirtyBuffer = currentByte & mask[i].mask;               // Mask the byte with the mask
            btuMultiDirectionBitShift(&dirtyBuffer, mask[i].shift); // Shift the bits according to the shift value in the mask
            cleanBuffer = cleanBuffer | dirtyBuffer;                // Add the masked and shifted bits to the cleanBuffer
            bitsLeft = bitsLeft - mask[i].noBitsGathered;           // Subtract the number of bits read from the byte from the bitsLeft counter
            bitsGathered = bitsGathered + mask[i].noBitsGathered;   // Add the number of bits read from the byte to the bitsGathered counter
            if (bitsGathered == bitMode)
            {                                                                              // Check if we have read the amount bits depending on the mode to the cleanBuffer (5 or 7)
                store[pixelCount / width][pixelCount % width] = (unsigned int)cleanBuffer; // Write the cleanBuffer to the data array if we have read 5 bits
                bitsGathered = 0;                                                          // Reset the bitsGathered counter
                cleanBuffer = 0;                                                           // Reset the cleanBuffer
                pixelCount++;                                                              // Increment the pixelCount
            }
            dirtyBuffer = 0; // Reset the dirtyBuffer
            if (pixelCount == height * width)
            {               // Check if we have read the expected number of pixels
                run = STOP; // If we have read the expected number of pixels and there is no more data in the file then we exit the loop
                break;
            }
        }
    }

    return SUCCESS;
}
