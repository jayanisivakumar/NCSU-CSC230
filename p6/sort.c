/**
    @file sort.c
    @author Jayani Sivakumar ( jsivaku )

    This file implements a command-line program that reads a sequence of
    double values from an input file, inserts them into a generic segment
    tree, and then outputs the values in ascending sorted order. 
*/
#include <stdio.h>
#include <stdlib.h>
#include "segTree.h"
#include <math.h>

/** Number of command-line arguments. */
#define NO_ARGS 2

/**
    Comparison function for doubles used in the segment tree.

    @param a pointer to the first double
    @param b pointer to the second double
    @return 1 if a < b, -1 if a > b, 0 if equal
*/
int compare( void const *a, void const *b )
{
    double sa = *( double const * ) a;
    double sb = *( double const * ) b;
    
    // if the first value is less than the second, return positive value
    if ( sa < sb ) {
        return 1;
    }
    // if the first value is greater than the second, return negative value
    if ( sa > sb ) {
        return -1;
    }
    // if both values are equal, return zero
    return 0;
}

/**
    Main function for the sort program.

    @param argc number of command-line arguments
    @param argv array of command-line argument strings
    @return 0 on successful completion, non-zero on error
*/
int main( int argc, char *argv[] )
{
    if ( argc != NO_ARGS ) {
        fprintf( stderr, "Usage: sort INPUT_FILE\n" );
        exit( EXIT_FAILURE );
    }
    
    FILE *fp = fopen( argv[ 1 ], "r" );
    if ( fp == NULL ) {
        perror( argv[ 1 ] );
        exit( EXIT_FAILURE );
    }
    
    // counting how many double values are in the file
    double value;
    int count = 0;
    while ( fscanf( fp, " %lf", &value ) == 1 )
        count++;
    if ( !feof( fp ) ) {
        fprintf( stderr, "Invalid input file\n" );
        fclose( fp );
        exit( EXIT_FAILURE );
    }
    rewind( fp );
    
    // creating a segment tree for doubles
    SegTree *st = makeST( sizeof( double ), compare );
    if ( !st ) {
        fclose( fp );
        exit( EXIT_FAILURE );
    }
    
    // reading double values from the file and add them to the segment tree
    for ( int i = 0; i < count; i++ ) {
        if ( fscanf( fp, " %lf", &value ) != 1 ) {
            fprintf( stderr, "Invalid input file\n" );
            fclose( fp );
            freeST( st );
            exit( EXIT_FAILURE );
        }
        addST( st, &value );
    }
    fclose( fp );
    
    // allocating an array to store the sorted values
    double *sorted = malloc( count * sizeof( double ) );
    if ( !sorted ) {
        freeST( st );
        exit( EXIT_FAILURE );
    }
    
    for ( int i = 0; i < count; i++ ) {
        int sz = sizeST( st );
        if ( sz <= 0 )
            break;
        int idx = queryST( st, 0, sz - 1, NULL );
        double *pVal = ( double * ) getST( st, idx, NULL );
        sorted[ i ] = *pVal;
        
        int last = sz - 1;
        if ( idx != last ) {
            double lastVal = *( ( double * ) getST( st, last, NULL ) );
            setST( st, idx, &lastVal, NULL );
        }
        removeST( st, NULL );
    }
    
    // printing the sorted values
    for ( int i = 0; i < count; i++ ) {
        printf( "%8.3f\n", sorted[ i ] );
    }
    
    free( sorted );
    freeST( st );
    return 0;
    
}
