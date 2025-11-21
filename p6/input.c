/** 
    @file input.c
    @author Jayani Sivakumar ( jsivaku )

    This file contains the implementation of the input component.
    It provides functionality to read a single line of input from the
    given input stream (stdin or a file) using a dynamically resizable buffer.
 */
#include "input.h"
#include <stdlib.h>
#include <string.h>

/** Initial size for reading the line. */
#define INITIAL_CAPACITY 10

/** Factor by which buffer grows. */
#define BUFFER_GROWTH_FACTOR 2

char *readLine( FILE *fp ) 
{
    // initial size allocation
    int capacity = INITIAL_CAPACITY;
    // number of characters read
    int length = 0;
    // memory allocation for input line
    char *line = malloc( capacity );
    int ch;

    // doubling capacity
    while ( ( ch = fgetc( fp ) ) != EOF && ch != '\n' ) {
        if ( length + 1 >= capacity ) {
            capacity *= BUFFER_GROWTH_FACTOR;
            char *temp = realloc( line, capacity );
            line = temp;
        }
        line[ length++ ] = ch;
    }
    
    if ( length == 0 && ch == EOF ) {
        free( line );
        return NULL;
    }
    
    while (length > 0 && (line[length - 1] == '\n' || line[length - 1] == '\r')) {
        length--;
    }
  
    // check null termination
    line[ length ] = '\0';
    return line;
}
