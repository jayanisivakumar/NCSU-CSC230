/** 
    @file TDES.h
    @author Jayani Sivakumar
    Header for the Triple DES Implementation.
*/

#include "IO.h"
#include "TDESinternal.h"

/**
    This function returns zero or one based on the value of the bit
    at index idx in the given array of bytes.
    @param data given array of bytes.
    @param idx bit index starting from 1.
    @return 0 if bit is 0, 1 if bit is 1.
 */
int getBit( byte const data[], int idx );

/**
    This function clears (if val is zero) or sets (if val is one)
    the bit at index idx of the data array.
    @param data byte array to modify.
    @param idx bit index starting from 1.
    @param val bit value to set 0 or 1.
 */
void putBit( byte data[], int idx, int val );

/**
    This function performs the permute operation, copying n bits from the
    given input array to output selected by the first n elements of perm.
    @param output output array for permuted bits.
    @param input input array containing bits to permute.
    @param perm permutation array that specifies which bits to copy.
    @param n number of bits to permute.
 */
void permute( byte output[], byte const input[], int const perm[], int n );

/**
    This function computes 16 subkeys based on the input key and stores each
    one in an element of the given K array. The resulting subkeys are stored in K[ 1 ] .. K[ 16 ].
    @param K two-dimensional array where subkeys will be stored. The subkeys are
    stored in K[1] through K[16] (K[0] is unused).
    @param key original 64-bit key (stored in BLOCK_BYTES bytes).
    
 */
void generateSubkeys( byte K[ ROUND_COUNT ][ SUBKEY_BYTES ], byte const key[ BLOCK_BYTES ] );

/**
    This function returns the result of an S-Box calculation in the four high-order bits of
    output[ 0 ]. The idx value ranges from 0 to 7. The result is an element of sBoxTable[ idx ]
    selected by bits idx * 6 + 1 to idx * 6 + 6 of B.
    @param output one-byte array where the 4-bit result is stored.
    @param input 48-bit input value stored in SUBKEY_BYTES bytes.
    @param idx index of the S-Box to use (0 to 7).
 */
void sBox( byte output[ 1 ], byte const input[ SUBKEY_BYTES ], int idx );

/**
    This function computes the f function based on the given 32-bit value R and the
    given 48-bit subkey, S. The result is stored in the result array.
    @param result 32-bit output stored in HALF_BLOCK_BYTES bytes.
    @param R 32-bit right half of the block (stored in HALF_BLOCK_BYTES bytes).
    @param K 48-bit subkey (stored in SUBKEY_BYTES bytes).
 */
void fFunction( byte result[ HALF_BLOCK_BYTES ], byte const R[ HALF_BLOCK_BYTES ], byte const K[ SUBKEY_BYTES ] );

/**
   This function performs a single block encrypt operation on the byte array in block,
   using the subkeys in the K array. The encrypted result is stored back in the given block. 
   @param block 64-bit block (BLOCK_BYTES bytes) to encrypt.
   @param K two-dimensional array of subkeys (each 48-bit, stored in SUBKEY_BYTES bytes) used in the 16 rounds.  
 */
void encryptBlock( byte block[ BLOCK_BYTES ], byte const K[ ROUND_COUNT ][ SUBKEY_BYTES ] );

/**
    This function performs a single block decrypt operation on the byte array in block,
    using the subkeys in the K array. The encrypted result is stored back in the given block.
    @param block 64-bit block (BLOCK_BYTES bytes) to decrypt.
    @param K two-dimensional array of subkeys (each 48-bit, stored in SUBKEY_BYTES bytes) used in the 16 rounds.
 */
void decryptBlock( byte block[ BLOCK_BYTES ], byte const K[ ROUND_COUNT ][ SUBKEY_BYTES ] );

/**
    This function adds padding if needed and encrypts the resulting array using the Triple
    DES algorithm. It returns a pointer to a dynamically allocated array containing the padded,
    encrypted result. The pass-by reference parameter, n is set to the length of the resulting array.
    @param input plaintext data to encrypt.
    @param inputLen length in bytes of the plaintext.
    @param key encryption key
    @param keyLen length in bytes of the key.
    @param n pointer to an integer that will hold the length of the encrypted output.
    @return byte* pointer to a dynamically allocated array containing the padded, encrypted data.
 */
byte *encryptTDES( byte input[], int inputLen, byte key[], int keyLen, int *n );

/**
    This function is like encryptTDES() but it performs the decrypt operation rather than encrypting.
    @param input ciphertext data to decrypt.
    @param inputLen length in bytes of the ciphertext.
    @param key decryption key.
    @param keyLen length in bytes of the key.
    @param n pointer to an integer that will hold the length of the decrypted output.
    @return byte* pointer to a dynamically allocated array containing the decrypted plaintext.
 */
byte *decryptTDES( byte input[], int inputLen, byte key[], int keyLen, int *n );











