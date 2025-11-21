/**
    @file input.c
    @author Jayani Sivakumar ( jsivaku )

    This file contains functions for reading and processing input data.
    The input.c file provides functionality to read strings from input
    and creates a map of column types.
  */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "input.h"

/** Define format options for string type */
#define STRING_FORMAT "string"

/** Define format options for int type */
#define INTEGER_FORMAT "integer"

/** Define format options for float type, output to 2 fractional digits */
#define FLOAT2_FORMAT  "float2"

/** Define format options for float type, output to 5 fractional digits */
#define FLOAT5_FORMAT  "float5"

/** Define format options for date type */
#define DATE_FORMAT  "date"

/** Maximum length for string */
#define FIELD_MAX 30

/** Maximum columns for dataset*/
#define COLUMN_MAX 25

/** File not containing at least formats and headings */
#define FILE_ERROR 100

/** Error in format type */
#define FORMAT_ERROR 101

/** Error in heading ( too long, too few, too many ) */
#define HEADING_ERROR 102

/** FLOAT2 format */
#define FLOAT_FORMAT2 2

/** FLOAT5 format */
#define FLOAT_FORMAT5 3

/** Date format */
#define DATE_NUMBER 4

/** String contains more than 30 characters in length */
#define RESULT_NUMBER -2

/** 
    Reads a string from input until we reach a delimiter.

    @param str array to store the string read
    @return last character that ends string,
        -2 if the string exceeds FIELD_MAX.
*/
int readString( char str[ FIELD_MAX + 1 ] ) 
{
    int ch;
    int i = 0;

    // reading until comma, newline or EOF
    while ( ( ch = getchar( ) ) != EOF && ch != ',' && ch != '\n' ) {
        if ( i < FIELD_MAX ) {
            str[ i++ ] = ch;
        } else {
            return RESULT_NUMBER;
        }
    }

    // null-terminate the string
    str[ i ] = '\0';

    // returns an integer value that corresponds to the last character read from the file
    return ch;

}

/** 
    Creates a map of column types based on input format specification.

    @param map array to store column type map
    @return number of columns found
*/
int createMap( int map[ COLUMN_MAX ] ) 
{
    char fmt[ FIELD_MAX + 1 ];
    int col = 0;
    int dataset;

    // empty file
    dataset = readString( fmt );
    if ( dataset == EOF ) {
        exit( FILE_ERROR );
    }

    do {
        // if the dataset contains more than 25 columns
        if ( col >= COLUMN_MAX ) {
            exit( FORMAT_ERROR );
        }

        // map to the format type of each data item in a row
        if ( strcmp( fmt, STRING_FORMAT ) == 0 ) {
            map[ col ] = 0;
        } else if ( strcmp( fmt, INTEGER_FORMAT ) == 0 ) {
            map[ col ] = 1;
        } else if ( strcmp( fmt, FLOAT2_FORMAT ) == 0 ) {
            map[ col ] = FLOAT_FORMAT2;
        } else if ( strcmp( fmt, FLOAT5_FORMAT ) == 0 ) {
            map[ col ] = FLOAT_FORMAT5;
        } else if ( strcmp( fmt, DATE_FORMAT ) == 0 ) {
            map[ col ] = DATE_NUMBER;
        } else {
            exit( FORMAT_ERROR );
        }

        col++;
        dataset = readString( fmt );
    } while ( dataset != EOF && dataset != '\n' );

    if ( col == 0 ) {
        exit( FILE_ERROR );
    }

    return col;
}