#include "bitTwiddlingUtils.h"

/**
 * This function Shifts a byte left or right by an integer number of bits
 * 
 * Negative numbers shift left, positive numbers shift right
 * 
 * @param byte The byte to be shifted
 * @param shift The number of bits to shift by
*/
void btuMultiDirectionBitShift(unsigned char * byte, int shift){
    if(shift > 0){ // If the shift is positive, shift right
        *byte = *byte >> shift;
    } else if(shift < 0){ // If the shift is negative, shift left
        *byte = *byte << abs(shift);
    } else if (shift == 0){ // If the shift is 0, do nothing
        return;
    }
}

/**
 * This function counts the number of bits set to 1 in a byte
 * 
 * This function uses the bit hack to set the least significant bit to 0 
 * (also known as the Kernighan's algorithm or population count)
 * 
 * @param byte The byte to be counted
 * @return The number of bits set to 1 in the byte
*/
int btuPopCount(unsigned char byte){
    int count = 0;
    for (count = 0; byte != 0; count++){
        byte &= byte - 1; // Using the bit hack to set the least significant bit to 0
                          // We keep doing this until the byte is 0
                          // How many times we do this is the number of bits set to 1
    }
    return count;
}