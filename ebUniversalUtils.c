#include "ebUniversalUtils.h"

/**
 * Creates a 2D array of unsigned ints using only 2 mallocs
 *
 * @param height The height of the 2D array
 * @param width The width of the 2D array
 * @return A 2D array of unsigned ints
 * @return NULL if the malloc fails
 */
unsigned int **ebCreate2DArray(int height, int width)
{
    unsigned int **imageArray;                  // The 2D array to return
    long numBytes = (long)height * (long)width; // Calculates the number of bytes needed for the 2D array

    imageArray = (unsigned int **)malloc(height * sizeof(unsigned int *)); // Allocate memory for the rows of the 2D array
    if (imageArray == NULL)
    {
        return NULL; // return NULL if malloc failed
    }

    unsigned int *imageArrayData = (unsigned int *)malloc(numBytes * sizeof(unsigned int)); // Allocate memory the individual elements of the 2D array
    if (imageArrayData == NULL)
    {
        return NULL; // return NULL if malloc failed
    }

    for (int i = 0; i < height; i++)
    {                                               // Loop through the rows of the 2D array
        imageArray[i] = imageArrayData + i * width; // Assign the pointers in the 2D array to the correct locations in the 1D array
    }

    return imageArray; // If we get here, the 2D array was created successfully and we return it
}

/**
 * Frees a 2D array of unsigned ints that was created using ebCreate2DArray
 *
 * @param array The 2D array to free
 */
void ebFree2DArray(unsigned int **array)
{
    free(array[0]); // Free the 1D array
    free(array);    // Free the 2D array
}

/**
 * Reads the header of an eb file and checks that it is valid
 *
 * @param fp The file pointer to the file to read
 * @param image The image struct to store the header information in
 * @param expectedMagicNumber The magic number that the file should have
 * @return 0 if the header is valid else an error code
 */
int ebReadHeader(FILE *fp, Image *image, int expectedMagicNumber)
{
    // Read in the magic number
    if (fscanf(fp, "%c%c", &image->magicNumber[0], &image->magicNumber[1]) != 2)
    {                            // Read in the magic number and check that it was read correctly
        return BAD_MAGIC_NUMBER; // If the magic number was not read correctly, return an error code
    }
    // Validate the magic number
    unsigned short *magicNumberValue = (unsigned short *)image->magicNumber; // Cast the magic number to an unsigned short
    if (*magicNumberValue != expectedMagicNumber)
    {                            // Check against the casted magic number and the expected magic number
        return BAD_MAGIC_NUMBER; // If the magic number is different, return an error code
    }

    // Read and validate the width and height
    fscanf(fp, "%d %d", &image->height, &image->width); // Read in the width and height
    if (image->width < MIN_DIMENSION || image->width > MAX_DIMENSION || image->height < MIN_DIMENSION || image->height > MAX_DIMENSION)
    {                   // Check that the width and height are valid
        return BAD_DIM; // If width or height are not valid, return an error code
    }
    return SUCCESS; // If we get here, the header is valid and we return success
}

/**
 * Writes the header of an eb file
 *
 * @param fp The file pointer to the file to write to
 * @param image The image struct to get the header information from
 * @param expectedMagicNumber The magic number that the file should have
 * @return 0 if the header is written successfully else an error code
 */
int ebWriteHeader(FILE *fp, Image *image, int expectedMagicNumber)
{
    char * magicNumber = (char *) &expectedMagicNumber;

    // Write the magic number
    if (fprintf(fp, "%c%c", magicNumber[0], magicNumber[1]) != 2)
    { // Check write
        return BAD_OUTPUT;
    }

    // Calculate how many digits are in the width and height
    int widthDigits = 0;
    int heightDigits = 0;
    for (int i = image->width; i > 0; i /= 10)
    { // Continously divide by 10 until the width is 0, how many times we divide is how many digits there are
        widthDigits++;
    }
    for (int i = image->height; i > 0; i /= 10)
    { // Same as above but for the height
        heightDigits++;
    }
    int expectedWriteLength = widthDigits + heightDigits + 3; // Calculate how many characters we expect to write (+3 because 1 spaces and 2 newline)

    // Write the width and height
    if (fprintf(fp, "\n%d %d\n", image->height, image->width) != expectedWriteLength)
    {                      // Write the width and height and check that the correct number of characters were written
        return BAD_OUTPUT; // If the wrong number of characters were written, return an error code
    }

    return SUCCESS; // If we get here, the header has been written successfully and we return success
}

/**
 * Checks the arguments passed to an eb image processing script
 *
 * This function will exit the program if the argument count is not correct else it will do nothing
 *
 * @param argc The number of arguments passed
 * @param scriptName The name of the script
 */
void ebCheckArgs(int argc, char *scriptName)
{
    if (argc == 1)
    { // If no arguments are passed, print usage and exit
        printf("Usage: %s file1 file2\n", scriptName);
        exit(0);
    }
    else if (argc != 3)
    { // If the wrong number of arguments are passed, print error and exit with error code
        printf("ERROR: Bad Arguments\n");
        exit(BAD_ARGS);
    }
    else
    { // If the correct number of arguments are passed, do nothing
        return;
    }
}

/**
 * Compares two eb images and reports if they are the same or not
 *
 * @param image1 The first image to compare
 * @param image2 The second image to compare
 * @return 0 if the images are the same else 1
 */
int ebCompare(Image *image1, Image *image2)
{
    // Compare their magic numbers
    if (image1->magicNumber[0] != image2->magicNumber[0] || image1->magicNumber[1] != image2->magicNumber[1])
    {
        return DIFFERENT; // If the magic numbers are different, the images are different so return 1
    }

    // Compare the dimensions
    if (image1->width != image2->width || image1->height != image2->height)
    {
        return DIFFERENT; // If the dimensions are different, the images are different so return 1
    }

    // Compare the data
    for (int i = 0; i < image1->height; i++)
    {
        for (int j = 0; j < image1->width; j++)
        {
            if (image1->data[i][j] != image2->data[i][j])
            {
                return DIFFERENT; // If we encounter a pixel that is different, the images are different so return 1
            }
        }
    }
    return IDENTICAL; // If we get here, the images are the same
}

/**
 * Prints the error message for the given error code
 *
 * @param errorCode The error code to print the message for
 * @param filename The name of the file that caused the error
 * @return The error code that was passed in
 */
int ebErrorHandle(int errorCode, char *filename)
{
    // Switch on the error code and print the appropriate error message
    switch (errorCode)
    {
    case SUCCESS: // Incase this function is called with a success code, we notify the developer that something is wrong
        printf("If you see this output, you coded something wrong\n");
        return SUCCESS;
        break;
    case BAD_ARGS:
        printf("ERROR: Bad Arguments\n");
        return BAD_ARGS;
        break;
    case BAD_FILE:
        printf("ERROR: Bad File Name (%s)\n", filename);
        return BAD_FILE;
        break;
    case BAD_MAGIC_NUMBER:
        printf("ERROR: Bad Magic Number (%s)\n", filename);
        return BAD_MAGIC_NUMBER;
        break;
    case BAD_DIM:
        printf("ERROR: Bad Dimensions (%s)\n", filename);
        return BAD_DIM;
        break;
    case BAD_MALLOC:
        printf("ERROR: Image Malloc Failed\n");
        return BAD_MALLOC;
        break;
    case BAD_DATA:
        printf("ERROR: Bad Data (%s)\n", filename);
        return BAD_DATA;
        break;
    case BAD_OUTPUT:
        printf("ERROR: Bad Output\n");
        return BAD_OUTPUT;
        break;
    case BAD_BLOCK_MALLOC:
        printf("ERROR: Block Malloc Failed\n");
        return BAD_BLOCK_MALLOC;
        break;
    default:
        printf("ERROR: Unknown Error\n");
        return -1;
        break;
    }
}