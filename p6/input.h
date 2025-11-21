/**
    @file input.h
    @author Jayani Sivakumar ( jsivaku )

    This file contains the function prototype for reading input.
*/
#include <stdio.h>

/**
    Reads a single line from the given file stream, dynamically allocating memory as needed.
    This function implements a resizable buffer to accommodate long lines.

    @param fp pointer to the file stream from which input is read
    @return dynamically allocated string containing line, else NULL if EOF is reached.
 */
char *readLine( FILE *fp );
