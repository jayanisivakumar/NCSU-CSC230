/**
    @file calc.c
    @author Jayani Sivakumar (jsivaku)
    
    This file contains the main() function and supporting code and
    functions to parse the command-line arguments and output the contents
    of the table at the end.
 */
#include "table.h"
#include <stdio.h>
#include <stdlib.h>

/** Number of filenames that must be given on the command line. */
#define REQUIRED_FILENAMES 1

/** Number of filenames that may be given on the command line. */
#define OPTIONAL_FILENAMES 2

/** Command line argument lower limit. */
#define MIN_ARGS 2

/** Command line argument upper limit. */
#define MAX_ARGS 3

/**
    Prints the table to the terminal in a formatted manner.

    @param rows number of rows in the table
    @param cols number of columns in the table
    @param table 2D array containing computed values
 */
void printTable(  int rows, int cols, long table[ rows ][ cols ] )
{
    int colW[ cols ];

    // output field width
    for ( int j = 0; j < cols; j++ ) {
        colW[ j ] = 0;
        for ( int i = 0; i < rows; i++ ) {
            int numW = snprintf( NULL, 0, "%ld", table[ i ][ j ] );
            if ( numW > colW[ j ] ) {
                colW[ j ] = numW;
            }
        }
    }

    // table top border
    for ( int j = 0; j < cols; j++ ) {
        printf( "+" );
        for ( int k = 0; k < colW[ j ]; k++ ) {
            printf( "-" );
        }
    }
    printf( "+\n" );

    // table contents
    for ( int i = 0; i < rows; i++ ) {
        for ( int j = 0; j < cols; j++ ) {
            printf( "|" );
            int numW = snprintf( NULL, 0, "%ld", table[ i ][ j ] );
            int space = colW[ j ] - numW;
            for ( int k = 0; k < space; k++ ) {
                printf( " " );
            }

            // print the number
            printf( "%ld", table[ i ][ j ] );
        }
        printf( "|\n" );

        // row border
        for ( int j = 0; j < cols; j++ ) {
            printf( "+" );
            for ( int k = 0; k < colW[ j ]; k++ ) {
                printf( "-" );
            }
        }
        printf( "+\n" );
    }
}

/**
    Writes the table to an output file.

    @param fp file pointer for writing output
    @param rows number of rows in the table.
    @param cols number of columns in the table
    @param table array containing computed values
 */
void writeTable( FILE *fp, int rows, int cols, long table[ rows ][ cols ] ) 
{
    fprintf( fp, "%d %d\n", rows, cols ); // dimensions

    for ( int i = 0; i < rows; i++ ) {
        for ( int j = 0; j < cols; j++ ) {
            fprintf( fp, "%ld", table[ i ][ j ] );
            if ( j < cols - 1 ) {
                fprintf( fp, "," );
            }
        }
        fprintf( fp, "\n" );
    }
}

/**
    Main function for processing input file and generating output.
 
    @param numArgs number of command-line arguments
    @param argValues[] array of command-line arguments
    @return EXIT_SUCCESS on successful exit, else EXIT_FAILURE
 */
int main( int numArgs, char *argValues[] ) 
{
    if ( numArgs < MIN_ARGS || numArgs > MAX_ARGS ) {
        fprintf( stderr, "usage: calc INPUT-FILE [OUTPUT-FILE]\n" );
        return EXIT_FAILURE;
    }

    // opening input file
    FILE *inputFile = fopen( argValues[ 1 ], "r" );
    if ( !inputFile ) {
        fprintf( stderr, "Can't open file: %s\n", argValues[ 1 ] );
        return EXIT_FAILURE;
    }

    // table dimensions
    int rows, cols;
    parseHeader( inputFile, &rows, &cols );

    long table[ rows ][ cols ];

    // read table contents
    parseTable( inputFile, rows, cols, table );
    fclose( inputFile );

    // output method
    if ( numArgs == MAX_ARGS ) {
        FILE *outputFile = fopen( argValues[ MIN_ARGS ], "w" );
        if ( !outputFile ) {
            fprintf( stderr, "Can't open file: %s\n", argValues[ MIN_ARGS ] );
            return EXIT_FAILURE;
        }
        writeTable( outputFile, rows, cols, table );
        fclose( outputFile );
    } else {
        printTable( rows, cols, table );
    }

    return EXIT_SUCCESS;
}
    