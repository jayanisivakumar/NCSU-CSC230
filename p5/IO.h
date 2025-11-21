/** 
    @file IO.h
    @author Jayani Sivakumar
    Functions to make it easy to read and write binary files.
*/

#ifndef _IO_H_
#define _IO_H_

#include <stdbool.h>

/** Type used to represent a byte. */
typedef unsigned char byte;

/**
    This function reads the contents of the file with the given filename.
 
    @param filename the name of file to be read.
    @param n pass-by-reference parameter that is used to return the length of the file.
    @return pointer to a dynamically allocated array of bytes with the contents of the file, else null.
 */
byte *readFile( const char *filename, int *n );

/**
    This function is for writing bytes in binary. It writes the sequence
    of n bytes in the data array to the output file with a name given by filename.
 
    @param filename the name of of the output file.
    @param data pointer to the data array.
    @param n number of bytes to write.
    @return true if successful, else false.
 */
bool writeFile( const char *filename, byte *data, int n );

#endif







