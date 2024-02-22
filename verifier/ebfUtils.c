#include "ebfUtils.h"

/**
 * Reads in an ebf file and stores the data in an Image struct
 *
 * @param image - pointer to an Image struct
 * @param filename - name of the file to be read
 * @return - SUCCESS if the file was read correctly, otherwise an error code
 */
int ebfRead(Image *image, char *filename)
{

    // Open the file in read mode
    FILE *inputFile = fopen(filename, "r");
    if (inputFile == NULL)
    {                    // Check file opened correctly
        return BAD_FILE; // return error code if file pointer is NULL
    }

    // Read the header data
    int check = ebReadHeader(inputFile, image, MAGIC_NUMBER_EBF);
    if (check != SUCCESS)
    {                 // Check read header was successful
        return check; // return error code if header read failed
    }

    // Allocate memory for array
    image->data = ebCreate2DArray(image->height, image->width);
    if (image->data == NULL)
    {                      // Check if malloc failed
        fclose(inputFile); // Close file if malloc fails
        return BAD_MALLOC; // return error code if malloc fails
    }

    // Read in each grey value from the file and store in the array
    unsigned int currentValBuffer = 0; // Buffer to store the current value read in from the file
    for (int row = 0; row < image->height; row++)
    {
        for (int current = 0; current < image->width; current++)
        {
            check = fscanf(inputFile, "%u", &currentValBuffer); // Read in the value from the file
            if (check != 1)
            {                               // Check if we have read in a pixel value
                ebFree2DArray(image->data); // Free allocated memory if we failed to read in a pixel value
                fclose(inputFile);          // Close file if we failed to read in a pixel value
                return BAD_DATA;            // return error code we failed to read in a pixel value
            }
            if (currentValBuffer > MAX_GREY_VALUE || currentValBuffer < MIN_GREY_VALUE)
            {                               // Check that the value we read is in range
                ebFree2DArray(image->data); // Free allocated memory if value is out of range
                fclose(inputFile);          // Close file if value is out of range
                return BAD_DATA;            // return error code if value is out of range
            }

            image->data[row][current] = currentValBuffer; // Assign the value to the array
        }
    } // reading in

    // Check that there is no more data in the file
    check = fscanf(inputFile, "%u", &currentValBuffer); // Try to read in another value from the file
    if (check != EOF)
    {                               // Check if we have read in a pixel value (if we read in anything other than EOF there is too much data in the file)
        ebFree2DArray(image->data); // Free allocated memory if we have too much data in the file
        fclose(inputFile);          // Close the file if we have too much data in the file
        return BAD_DATA;            // return error code if we have too much data in the file
    }

    // If we have reached this point the file has been read successfully
    fclose(inputFile); // Close the file as we have finished reading it
    return SUCCESS;    // return success code
} // ebfRead()

/**
 * Writes an ebf file from the data stored in an Image struct
 *
 * @param image - pointer to an Image struct
 * @param filename - name of the file to be written
 * @return - SUCCESS if the file was written correctly, otherwise an error code
 */
int ebfWrite(Image *image, char *filename)
{
    // Open the file in write mode
    FILE *fp = fopen(filename, "w");
    if (fp == NULL)
    {                    // Check file opened successfully
        return BAD_FILE; // return error code if file failed to open
    }

    // Write the header data
    int check = ebWriteHeader(fp, image, MAGIC_NUMBER_EBF);
    if (check != SUCCESS)
    {                 // Check header was written successfully
        fclose(fp);   // Close the file if header write failed
        return check; // return error code if header write failed
    }

    // Write the data
    for (int row = 0; row < image->height; row++)
    {
        for (int col = 0; col < image->width; col++)
        {
            check = fprintf(fp, "%u", image->data[row][col]); // Write the data to the file
            if (!(check == 2 || check == 1))
            {                    // Check if fprintf wrote something (2 or 1 because the number of digits can only be either 1 or 2)
                fclose(fp);      // Close the file if fprintf failed to write
                return BAD_FILE; // return error code if fprintf failed to write
            }

            if (col != image->width - 1)
            {                             // Check if we are not at the end of the row
                check = fprintf(fp, " "); // Write a space after each value except if we are at the end of the row
                if (check != 1)
                {                    // Check if fprintf wrote a space
                    fclose(fp);      // Close the file if fprintf failed to write
                    return BAD_FILE; // return error code if fprintf failed to write
                }
            }
        }

        if (row != image->height - 1)
        {                              // Check if we are not at the end of the file
            check = fprintf(fp, "\n"); // Write a new line after each row except if we are at the end of the file
            if (check != 1)
            {                    // Check if fprintf wrote a new line
                fclose(fp);      // Close the file if fprintf failed to write
                return BAD_FILE; // return error code if fprintf failed to write
            }
        }
    }

    // If we get here, we have successfully written the file
    fclose(fp);     // Close the file as we have finished writing it
    return SUCCESS; // return success code
} // ebfWrite()
