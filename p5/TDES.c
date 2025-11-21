/** 
    @file TDES.c
    @author Jayani Sivakumar
    Implementation of the DES / Triple DES algorithm.
*/

#include "TDES.h"
#include "magic.h"
#include "TDESinternal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/** Number of halves used when combining subkeys.*/
#define NUM_HALVES 2

/** Number of DES key parts used in Triple DES.*/
#define NUM_KEY_PARTS 3

/** A full byte value with all bits set. */
#define FULL_BYTE 0xFF

/** Mask to get upper 4 bits of the byte. */
#define UPPER_MASK 0xF0

/** Mask to get the lower 4 bits of the byte. */
#define LOWER_MASK 0x0F


/** 
    Helper method that rotates a 28-bit value stored in an array of
    HALF_SUBKEY_BYTES bytes left by shift positions. 
    @param bits the 28 bits of the array
    @param shift the number of positions to shift left
 */
static void rotateLeft( byte bits[], int shift );

int getBit( byte const data[], int idx ) 
{
    // start from zero
    idx -= 1;  
    // finding the byte that has the bit
    int bytIndex = idx / BYTE_SIZE;
    // finding bit position
    int bitIndex = ( BYTE_SIZE - 1 ) - ( idx % BYTE_SIZE );
    return ( data[ bytIndex ] >> bitIndex ) & 1;
}

void putBit( byte data[], int idx, int val ) 
{
    // start from zero
    idx -= 1;
    // finding the byte that has the bit
    int bytIndex = idx / BYTE_SIZE;
    // finding bit position
    int bitIndex = ( BYTE_SIZE - 1 ) - ( idx % BYTE_SIZE );
    if ( val ) {
        // setting to 1
        data[ bytIndex ] |= ( 1 << bitIndex );
    } else {
        // setting to 0
        data[ bytIndex ] &= ~( 1 << bitIndex );
    }
}

void permute( byte output[], byte const input[], int const perm[], int n ) 
{
    // setting bits to 0
    for ( int i = 0; i < ( n + BYTE_SIZE - 1 ) / BYTE_SIZE; i++ ) {
        output[ i ] = 0;
    }
    // copying bits on permutation table
    for ( int i = 0; i < n; i++ ) {
        int value = getBit( input, perm[ i ] );
        putBit( output, i + 1, value );
    }
    // unused bits should be 0
    int bits = n % BYTE_SIZE;
    if ( bits != 0 ) {
        int index = ( n - 1 ) / BYTE_SIZE;
        int p = ( FULL_BYTE << ( BYTE_SIZE - bits ) ) & FULL_BYTE;
        output[ index ] &= p;
    }
}

void generateSubkeys( byte K[ ROUND_COUNT ][ SUBKEY_BYTES ], byte const key[ BLOCK_BYTES ] )
{
    // arrays to hold the 28-bits
    byte C[ HALF_SUBKEY_BYTES ] = { 0 };
    byte D[ HALF_SUBKEY_BYTES ] = { 0 };
    
    // getting left and right
    permute( C, key, leftSubkeyPerm, HALF_SUBKEY_BITS );
    permute( D, key, rightSubkeyPerm, HALF_SUBKEY_BITS );
    
    C[ HALF_SUBKEY_BYTES - 1 ] &= UPPER_MASK;
    D[ HALF_SUBKEY_BYTES - 1 ] &= UPPER_MASK;
    
    // subkey calculation
    for ( int round = 1; round < ROUND_COUNT; round++ ) {
        int shift = subkeyShiftSchedule[ round ];
        // rotations
        rotateLeft( C, shift );
        rotateLeft( D, shift );
        // combining C and D to a 56-bit value
        int comb = ROUND_TO_BYTES( HALF_SUBKEY_BITS * NUM_HALVES );
        byte CD[ comb ];
        for ( int i = 0; i < comb; i++ ) {
            CD[ i ] = 0;
        }
        // copying C into first 28 bits
        for ( int i = 1; i <= HALF_SUBKEY_BITS; i++ ) {
            int value = getBit( C, i );
            putBit( CD, i, value );
        }
        // copying D into last 28 bits
        for ( int i = 1; i <= HALF_SUBKEY_BITS; i++ ) {
            int value = getBit( D, i );
            putBit( CD, i + HALF_SUBKEY_BITS, value );
        }
        // compressing the bits into the subkey
        permute( K[ round ], CD, subkeyPerm, SUBKEY_BITS );
    }
}

static void rotateLeft( byte bits[], int shift )
{
    for ( int i = 0; i < shift; i++ ) {
        // first bit index 1
        int first = getBit( bits, 1 );
        // shifting the bits left 
        for ( int i = 1; i < HALF_SUBKEY_BITS; i++ ) {
            int bit = getBit( bits, i + 1 );
            putBit( bits, i, bit );
        }
        // putting bit to the last position
        putBit( bits, HALF_SUBKEY_BITS, first );
    }
}

void sBox( byte output[ 1 ], byte const input[ SUBKEY_BYTES ], int idx )
{
    // starting bit computation
    int start = idx * SBOX_INPUT_BITS + 1;
    int bits = 0;
    
    // getting the six consecutive bits from input
    for ( int i = 0; i < SBOX_INPUT_BITS; i++ ) {
        int bit = getBit( input, start + i );
        bits = ( bits << 1 ) | bit;
    }
    
    // row and column formations
    int row = ( ( ( bits >> ( SBOX_INPUT_BITS - 1 ) ) & 0x1 ) << 1 ) | ( bits & 0x1 );
    int col = ( bits >> 1 ) & LOWER_MASK;
    
    // S-Box value
    int sboxVal = sBoxTable[ idx ][ row ][ col ];
    
    // output
    output[ 0 ] = ( byte )( sboxVal << SBOX_OUTPUT_BITS );
}

void fFunction( byte result[ HALF_BLOCK_BYTES ], byte const R[ HALF_BLOCK_BYTES ], byte const K[ SUBKEY_BYTES ] )
{
    // making the 32-bit R larger to 48 bits.
    byte expandedR[ SUBKEY_BYTES ];
    for ( int i = 0; i < SUBKEY_BYTES; i++ ) {
        expandedR[ i ] = 0;
    }
    permute( expandedR, R, expandedRSelector, SUBKEY_BITS );
    
    // XOR adding the expanded R with the 48-bit subkey
    byte B[ SUBKEY_BYTES ];
    for ( int i = 0; i < SUBKEY_BYTES; i++ ) {
        B[ i ] = expandedR[ i ] ^ K[ i ];
    }
    
    // processing using the S-boxes
    byte sBoxOutput[ HALF_BLOCK_BYTES ];
    for ( int i = 0; i < HALF_BLOCK_BYTES; i++ ) {
        sBoxOutput[ i ] = 0;
    }
    
    for ( int i = 0; i < SBOX_COUNT; i++ ) {
        byte temp[ 1 ];
        sBox( temp, B, i );
        int n = temp[ 0 ] >> SBOX_OUTPUT_BITS;
        for ( int j = 0; j < SBOX_OUTPUT_BITS; j++ ) {
            int value = ( n >> ( SBOX_OUTPUT_BITS - 1 - j ) ) & 1;
            putBit( sBoxOutput, i * SBOX_OUTPUT_BITS + j + 1, value );
        }
    }
    
    permute( result, sBoxOutput, fFunctionPerm, HALF_BLOCK_BITS );
}

void encryptBlock( byte block[ BLOCK_BYTES ], byte const K[ ROUND_COUNT ][ SUBKEY_BYTES ] )
{
    // splitting block into half
    byte L[ HALF_BLOCK_BYTES ];
    byte R[ HALF_BLOCK_BYTES ];
    for ( int i = 0; i < HALF_BLOCK_BYTES; i++ ) {
        L[ i ] = 0;
        R[ i ] = 0;
    }
    permute( L, block, leftInitialPerm, HALF_BLOCK_BITS );
    permute( R, block, rightInitialPerm, HALF_BLOCK_BITS );
    
    // processing
    for ( int round = 1; round < ROUND_COUNT; round++ ) {
        byte fOut[ HALF_BLOCK_BYTES ];
        for ( int i = 0; i < HALF_BLOCK_BYTES; i++ ) {
            fOut [ i ] = 0;
        }
        fFunction( fOut, R, K[ round ] );
        // the new right half
        byte rNext[ HALF_BLOCK_BYTES ];
        for ( int i = 0; i < HALF_BLOCK_BYTES; i++ ) {
            rNext[ i ] = L[ i ] ^ fOut[ i ];
        }
        // left half
        for ( int i = 0; i < HALF_BLOCK_BYTES; i++ ) {
            L[ i ] = R[ i ];
        }
        // right half
        for ( int i = 0; i < HALF_BLOCK_BYTES; i++ ) {
            R[ i ] = rNext[ i ];
        }
    }
    
    // combined final halves in reverse order
    byte combinedBlock[ BLOCK_BYTES ] = { 0 };
    // copying R 
    for ( int i = 1; i <= HALF_BLOCK_BITS; i++ ) {
        int value = getBit( R, i );
        putBit( combinedBlock, i, value );
    }
    // copying L
    for ( int i = 1; i <= HALF_BLOCK_BITS; i++ ) {
        int value = getBit( L, i );
        putBit( combinedBlock, i + HALF_BLOCK_BITS, value );
    }
    
    // producing the final block
    byte finOutput[ BLOCK_BYTES ] = { 0 };
    permute( finOutput, combinedBlock, finalPerm, BLOCK_BITS );
    
    // copying to the original array
    for ( int i = 0; i < BLOCK_BYTES; i++ ) {
        block[ i ] = finOutput[ i ];
    }
}

void decryptBlock( byte block[ BLOCK_BYTES ], byte const K[ ROUND_COUNT ][ SUBKEY_BYTES ] )
{
    // splitting block into half
    byte L[ HALF_BLOCK_BYTES ];
    byte R[ HALF_BLOCK_BYTES ];
    for ( int i = 0; i < HALF_BLOCK_BYTES; i++ ) {
        L[ i ] = 0;
        R[ i ] = 0;
    }
    permute( L, block, leftInitialPerm, HALF_BLOCK_BITS );
    permute( R, block, rightInitialPerm, HALF_BLOCK_BITS );
    
    // processing
    for ( int round = 1; round < ROUND_COUNT; round++ ) {
        byte fOut[ HALF_BLOCK_BYTES ];
        for ( int i = 0; i < HALF_BLOCK_BYTES; i++ ) {
            fOut[ i ] = 0;
        }
        fFunction( fOut, R, K[ ROUND_COUNT - round ] );
        // the new right half
        byte rNext[ HALF_BLOCK_BYTES ];
        for ( int i = 0; i < HALF_BLOCK_BYTES; i++ ) {
            rNext[ i ] = L[ i ] ^ fOut[ i ];
        }
        // left half
        for ( int i = 0; i < HALF_BLOCK_BYTES; i++ ) {
            L[ i ] = R[ i ];
        }
        // right half
        for ( int i = 0; i < HALF_BLOCK_BYTES; i++ ) {
            R[ i ] = rNext[ i ];
        }
    }
    
    // combined final halves in reverse order
    byte combinedBlock[ BLOCK_BYTES ] = { 0 };
    // copying R 
    for ( int i = 1; i <= HALF_BLOCK_BITS; i++ ) {
        int value = getBit( R, i );
        putBit( combinedBlock, i, value );
    }
    // copying L
    for ( int i = 1; i <= HALF_BLOCK_BITS; i++ ) {
        int value = getBit( L, i );
        putBit( combinedBlock, i + HALF_BLOCK_BITS, value );
    }
    
    // producing the final block
    byte finOutput[ BLOCK_BYTES ] = { 0 };
    permute( finOutput, combinedBlock, finalPerm, BLOCK_BITS );
    
    // copying to the original array
    for ( int i = 0; i < BLOCK_BYTES; i++ ) {
        block[ i ] = finOutput[ i ];
    }  
}

byte *encryptTDES( byte input[], int inputLen, byte key[], int keyLen, int *n ) 
{
    if ( keyLen != ( NUM_KEY_PARTS * BLOCK_BYTES ) ) {
        fprintf( stderr, "Invalid key length\n" );
        exit( EXIT_FAILURE );
    }
    
    // computing the padding
    int desBlockSize = BLOCK_BYTES;
    int padCount = desBlockSize - ( inputLen % desBlockSize );
    if ( ( inputLen % desBlockSize ) == 0 )
        padCount = desBlockSize;
    int totalLength = inputLen + padCount;
    
    // new array for the padded input
    byte *paddedData = malloc( totalLength );
    if ( paddedData == NULL ) {
        exit( EXIT_FAILURE );
    }   
    memcpy( paddedData, input, inputLen );
    for ( int i = inputLen; i < totalLength; i++ ) {
        paddedData[ i ] = ( byte ) padCount;
    }
    
    // subkeys
    byte subkeys1[ ROUND_COUNT ][ SUBKEY_BYTES ];
    byte subkeys2[ ROUND_COUNT ][ SUBKEY_BYTES ];
    byte subkeys3[ ROUND_COUNT ][ SUBKEY_BYTES ];
    generateSubkeys( subkeys1, key );
    generateSubkeys( subkeys2, key + BLOCK_BYTES );
    generateSubkeys( subkeys3, key + ( NUM_HALVES * BLOCK_BYTES ) );
    
    // processing each block using Triple DES
    for ( int blkIdx = 0; blkIdx < totalLength; blkIdx += desBlockSize ) {
        byte *curBlock = paddedData + blkIdx;
        encryptBlock( curBlock, subkeys1 );
        decryptBlock( curBlock, subkeys2 );
        encryptBlock( curBlock, subkeys3 );
    }
    *n = totalLength;
    return paddedData;
}

byte *decryptTDES( byte input[], int inputLen, byte key[], int keyLen, int *n )
{
    if ( keyLen != ( NUM_KEY_PARTS * BLOCK_BYTES ) ) {
        fprintf( stderr, "Invalid key length\n" );
        exit( EXIT_FAILURE );
    }
    
    // computing the padding
    int desBlockSize = BLOCK_BYTES;
    if ( inputLen == 0 || ( inputLen % desBlockSize ) != 0 ) {
        fprintf( stderr, "Invalid encrypted data length\n" );
        exit( EXIT_FAILURE );
    }
    
    // ciphertext
    byte *cipherBuffer = malloc( inputLen );
    if ( cipherBuffer == NULL ) {
        exit( EXIT_FAILURE );
    }
    memcpy( cipherBuffer, input, inputLen );
    
    // subkeys
    byte subkeys1[ ROUND_COUNT ][ SUBKEY_BYTES ];
    byte subkeys2[ ROUND_COUNT ][ SUBKEY_BYTES ];
    byte subkeys3[ ROUND_COUNT ][ SUBKEY_BYTES ];
    generateSubkeys( subkeys1, key );
    generateSubkeys( subkeys2, key + BLOCK_BYTES );
    generateSubkeys( subkeys3, key + ( NUM_HALVES * BLOCK_BYTES ) );
    
    // processing each block using Triple DES
    for ( int blkIdx = 0; blkIdx < inputLen; blkIdx += desBlockSize ) {
        byte *curBlock = cipherBuffer + blkIdx;
        decryptBlock( curBlock, subkeys3 );
        encryptBlock( curBlock, subkeys2 );
        decryptBlock( curBlock, subkeys1 );
    }
    
    // rmeoving padding
    int padValue = cipherBuffer[ inputLen - 1 ];
    if ( padValue < 1 || padValue > desBlockSize ) {
        fprintf(stderr, "Invalid padding\n");
        free( cipherBuffer );
        exit( EXIT_FAILURE );
    }
    int plainLen = inputLen - padValue;
    
    byte *plainBuffer = malloc( plainLen );
    if ( plainBuffer == NULL ) {
        free( cipherBuffer );
        exit( EXIT_FAILURE );
    }
    memcpy( plainBuffer, cipherBuffer, plainLen );
    free( cipherBuffer );
    
    *n = plainLen;
    return plainBuffer;
    
}



    
    
    
    