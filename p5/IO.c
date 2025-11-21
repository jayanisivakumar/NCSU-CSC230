/** 
    @file IO.c
    @author Jayani Sivakumar
    This file provides implementations for reading the contents of a file into a 
    dynamically allocated byte array and for writing a given byte array to a file.
    All file operations are performed in binary mode.
*/

#include <stdio.h>
#include <stdlib.h>
#include "IO.h"

byte *readFile( const char *filename, int *n ) 
{  
    // opening the file for reading in binary mode
    FILE *file = fopen( filename, "rb" );
    if ( !file ) {
        return NULL;
    }
    
    // determining the size of our file
    fseek( file, 0, SEEK_END );
    *n = ftell( file );
    rewind( file );
    
    // storing file contents using memory allocation
    byte *temp = ( byte * )malloc( *n );
    if ( !temp ) {
        fclose( file );
        return NULL;
    }
    
    // reading the file contents temporarily
    size_t read = fread( temp, 1, *n, file );
    if ( read != ( size_t )*n ) {
        free( temp );
        fclose( file );
        return NULL;
    }

    fclose( file );
    return temp;
}

bool writeFile( const char *filename, byte *data, int n ) 
{
    // opening the file for writing in binary mode
    FILE *file = fopen( filename, "wb" );
    if ( !file ) {
        return false;
    }
    
    // writing to the file
    if ( fwrite( data, 1, n, file ) != ( size_t )n) {
        fclose( file );
        return false;
    }

    fclose( file );
    return true;
    
}





    