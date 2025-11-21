/** 
    @file tcrypt.c
    @author Jayani Sivakumar
    Contains the main function and parses the command-line arguments. 
    It uses the other components to read the input files, perform encryption
    or decryption and to write the output.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "IO.h"
#include "TDES.h" 

/** Number of command-line arguments expected in decryption mode. */
#define ARGS_DECRYPT 5

/** Number of DES key parts used in Triple DES.*/
#define NUM_KEY_PARTS 3

/** Argument index for the input file in encryption mode. */
#define ENCRYPT_INPUT_ARG_INDEX 2

/** Argument index for the output file in encryption mode. */
#define ENCRYPT_OUTPUT_ARG_INDEX 3

/** Argument index for the key file in decryption mode. */
#define DECRYPT_KEY_ARG_INDEX 2

/** Argument index for the input file in decryption mode. */
#define DECRYPT_INPUT_ARG_INDEX 3

/** Argument index for the output file in decryption mode. */
#define DECRYPT_OUTPUT_ARG_INDEX 4

/**
    This function reads command-line arguments and performs Triple DES encryption or decryption.
    The program reads the key file and input file. It then calls encryptTDES() or decryptTDES()
    accordingly and writes the output to the specified file.
    @param numArgs number of command-line arguments
    @param argValues array of command-line argument strings
    @return int returns EXIT_SUCCESS if processed successfully, else EXIT_FAILURE.
*/
int main( int numArgs, char *argValues[] ) 
{
    int decryptMode = 0;
    char *keyFileName = NULL;
    char *inputFileName = NULL;
    char *outputFileName = NULL;

    // command-line arguments
    if (numArgs == ARGS_DECRYPT && strcmp( argValues[ 1 ], "-d" ) == 0 ) {
        decryptMode = 1;
        keyFileName = argValues[ DECRYPT_KEY_ARG_INDEX ];
        inputFileName = argValues[ DECRYPT_INPUT_ARG_INDEX ];
        outputFileName = argValues[ DECRYPT_OUTPUT_ARG_INDEX ];
    } else if ( numArgs == ( ARGS_DECRYPT - 1 ) ) {
        decryptMode = 0;
        keyFileName = argValues[ 1 ];
        inputFileName = argValues[ ENCRYPT_INPUT_ARG_INDEX ];
        outputFileName = argValues[ ENCRYPT_OUTPUT_ARG_INDEX ];
    } else {
        fprintf( stderr, "usage: tcrypt [-d] KEY_FILE INPUT_FILE OUTPUT_FILE\n" );
        exit( EXIT_FAILURE );
    }

    // reading the key file
    int keyLength = 0;
    byte *keyData = readFile( keyFileName, &keyLength );
    if ( keyData == NULL ) {
        perror( keyFileName );
        exit( EXIT_FAILURE );
    }

    if ( keyLength != ( NUM_KEY_PARTS * BLOCK_BYTES ) ) {
        fprintf( stderr, "Invalid key length\n" );
        free( keyData );
        exit( EXIT_FAILURE );
    }

    // reading the input file
    int fileLength = 0;
    byte *inputData = readFile( inputFileName, &fileLength );
    if ( inputData == NULL ) {
        perror( inputFileName );
        free( keyData );
        exit( EXIT_FAILURE );
    }
    
    // decryption
    if ( decryptMode && ( fileLength == 0 || ( fileLength % BLOCK_BYTES ) != 0 ) ) {
        fprintf( stderr, "Invalid encrypted data length\n" );
        free( keyData );
        free( inputData );
        exit( EXIT_FAILURE );
    }

    // process encryption/decryption
    int outputLength = 0;
    byte *resultData = NULL;
    if ( decryptMode ) {
        resultData = decryptTDES( inputData, fileLength, keyData, keyLength, &outputLength );
    } else {
        resultData = encryptTDES( inputData, fileLength, keyData, keyLength, &outputLength );
    }

    // writing result to output file
    if ( !writeFile( outputFileName, resultData, outputLength ) ) {
        free( keyData );
        free( inputData );
        free( resultData );
        exit( EXIT_FAILURE );
    }

    // freeing memory
    free( keyData );
    free( inputData );
    free( resultData );
    return EXIT_SUCCESS;
    
}

