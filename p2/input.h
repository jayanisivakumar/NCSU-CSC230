/**
    @file input.h
    @author Jayani Sivakumar ( jsivaku )

    This file contains function declarations for reading and processing input data.
    The input.c file provides functionality to read strings from input
    and creates a map of column types.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

/** Maximum length of a format or heading name */
#define FIELD_MAX 30

/** Maximum number of columns permitted */
#define COLUMN_MAX 25

/** 
    Reads a string from input until we reach a delimiter.

    @param str array to store the string read
    @return last character that ends string,
        -2 if the string exceeds FIELD_MAX.
*/
int readString( char str[ FIELD_MAX + 1 ] );

/** 
    Creates a map of column types based on input format specification.

    @param map array to store column type map
    @return number of columns found
*/
int createMap( int map[ COLUMN_MAX ] );