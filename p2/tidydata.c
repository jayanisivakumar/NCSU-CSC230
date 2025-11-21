/**
    @file input.c
    @author Jayani Sivakumar ( jsivaku )

    This program reads input data, creates a map of column types,
    processes column headings, and processes each row of data.
    It handles error conditions and formats output according
    to requirements.	
*/
#include "input.h"
#include "data.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/** Maximum columns for dataset*/
#define COLUMN_MAX 25

/** Maximum length for string */
#define FIELD_MAX 30

/** Error in format type */
#define FORMAT_ERROR 101

/** Error in heading (too long, too few, too many) */
#define HEADING_ERROR 102

/** String contains more than 30 characters in length */
#define RESULT_NUMBER -2

/**
    The main function of the program creates a map of column types,
    validates number of columns, processes and prints column headings 
    and each data row.

    @return 0 program executes successfully
*/
int main() 
{
    int map[ COLUMN_MAX ];
    char field[ FIELD_MAX + 1 ];
    int numColumns = createMap( map );

    if ( numColumns <= 0 || numColumns >= COLUMN_MAX ) {
        exit( FORMAT_ERROR );
    }

    // column headings
    for ( int i = 0; i <= numColumns; i++ ) {
        int result = readString( field );
        if ( result == RESULT_NUMBER || result == EOF ) {
            exit( HEADING_ERROR ); // exceeds 30 characters
        }
        printf( "%s", field );
        if ( i < numColumns ) {
            if ( result == '\n' ) {
                exit( HEADING_ERROR ); // too few headings
            }
            printf( "," );
        }
    }

    printf( "\n" );

    // Process data 
    while ( checkObservation( numColumns, map ) ) {
        // Continue processing
    }

    return 0;
}