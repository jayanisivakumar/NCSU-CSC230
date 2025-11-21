/**
    @file driver.c
    @author Jayani Sivakumar ( jsivaku )

    This file implements the command-line driver program for a generic
    segment tree that stores dynamically allocated strings. It supports
    commands to add, set, get, remove, and query string values from the
    segment tree, and uses setjmp/longjmp for error handling.
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <setjmp.h>
#include <unistd.h> // Unix-specific isatty() function.
#include "segTree.h"
#include "input.h"

/**
    Frees all dynamically allocated strings from the segment tree.

    @param st pointer to the segment tree
*/
static void freeTree( SegTree *st );

/** Print out a usage message and exit unsuccessfully. */
static void usage()
{
  fprintf( stderr, "Usage: driver [-term]\n" );
  exit( EXIT_FAILURE );
}

/** Maximum size of a string, for regular credit. */
#define STRING_LIMIT 20

/** Longjump value for an invalid command. */
#define INVALID_COMMAND 101

/** Invalid command parameters */
#define INVALID_PARAM 100

/** Length of the command. */
#define CMD_LEN 32

/** Length of value */
#define VALUE_LEN 128

/** Length of extra. */
#define EXTRA_LEN 16

/** Parameter count. */
#define PARAM_COUNT_TWO 2

/**
    Parses and executes a user command on the segment tree.

    @param st pointer to the segment tree
    @param line full input line from user
    @param env jump buffer for error handling
    @return true if command is "quit", false otherwise
 */
bool handleCommand( SegTree *st, const char *line, jmp_buf *env );

/**
    Comparison function for lexicographic min segment tree.

    @param a pointer to the first string
    @param b pointer to the second string
    @return 1 if a is better (comes first), -1 if b is better, 0 if equal
 */
static int strComp( void const *a, void const *b )
{
    char *const *pa = a;
    char *const *pb = b;
    int cmp = strcmp( *pa, *pb );
    if ( cmp < 0 ) {
        return 1;
    }
    if ( cmp > 0 ) {
        return -1;
    }
    return 0;
}

/**
   Starting point for the program.
   @param argc number of command-line arguments.
   @param argv array of strings given as command-line arguments.
   @return exit status for the program.
 */
int main( int argc, char *argv[] )
{
    // See if our input is from a terminal.
    bool interactive = isatty( STDIN_FILENO );

    // Parse command-line arguments.
    int apos = 1;
    while ( apos < argc ) {
        if ( strcmp( argv[ apos ], "-term" ) == 0 ) {
            interactive = true;
            apos += 1;
        } else {
            usage();
        }
    }
  
    SegTree *st = makeST( sizeof(char *), strComp );

    jmp_buf env;
    char *line;

    if ( interactive ) {
        printf( "cmd> " );
    }

    while ( ( line = readLine( stdin ) ) != NULL ) {
        if ( strlen( line ) == 0 ) {
            free( line );
            if ( interactive ) {
                printf( "cmd> " );
            }
            continue;
        }

        int code = setjmp( env );
        if ( code == INVALID_COMMAND ) {
            if ( interactive ) {
                printf( "Invalid command\n" );
                free( line );
                printf( "cmd> " );
                continue;
            } else {
                fprintf( stderr, "Invalid command: %s\n", line );
                free( line );
                freeTree( st );
                freeST( st );
                exit( EXIT_FAILURE );
            }
        } else if ( code == INVALID_PARAM ) {
            if ( interactive ) {
                printf( "Invalid parameters\n" );
                free( line );
                printf( "cmd> " );
                continue;
            } else {
                fprintf( stderr, "Invalid parameters: %s\n", line );
                free( line );
                freeTree( st );
                freeST( st );
                exit( EXIT_FAILURE );
            }

        }

        bool quit = handleCommand( st, line, &env );
        free( line );
        if ( quit ) {
            break;
        }

        if ( interactive ) {
            printf( "cmd> " );
        }
    }

    freeTree( st );
    freeST(st);
    return EXIT_SUCCESS;
}

bool handleCommand( SegTree *st, char const *line, jmp_buf *env ) 
{
    char cmd[ CMD_LEN ];
    int parsed = sscanf( line, " %31s", cmd );

    if ( strcmp( cmd, "quit" ) == 0 ) {
        return true;
    }

    if ( strcmp( cmd, "add" ) == 0 ) {
        char val[ VALUE_LEN ], extra[ EXTRA_LEN ];
        parsed = sscanf( line, " add %127s %15s", val, extra );
        if ( parsed != 1 || strlen( val ) < 1 ) {
            longjmp( *env, INVALID_COMMAND );
        }
        char *copy = malloc( strlen( val ) + 1 );
        strcpy( copy, val );
        addST( st, &copy );
        return false;
    }

    if ( strcmp( cmd, "size" ) == 0 ) {
        char extra[ EXTRA_LEN ];
        if ( sscanf( line, " size %15s", extra ) == 1 ) {
            longjmp( *env, INVALID_COMMAND );
        }
        printf( "%d\n", sizeST( st ) );
        return false;
    }

    if ( strcmp( cmd, "get" ) == 0 ) {
        int idx;
        char extra[ EXTRA_LEN ];
        if ( sscanf( line, " get %d %15s", &idx, extra ) != 1 ) {
            longjmp( *env, INVALID_COMMAND );
        }
        char **resPtr = getST( st, idx, env );
        printf( "%s\n", *resPtr );
        return false;
    }

    if ( strcmp( cmd, "set" ) == 0 ) {
        int idx;
        char val[ VALUE_LEN ], extra[ EXTRA_LEN ];
        if ( sscanf( line, " set %d %127s %15s", &idx, val, extra ) != PARAM_COUNT_TWO ||
             strlen( val ) < 1 ) {
            longjmp( *env, INVALID_COMMAND );
        }
        char *newStr = malloc( strlen( val ) + 1 );
        strcpy( newStr, val );

        if ( idx >= 0 && idx < sizeST( st ) ) {
            char **oldPtr = getST( st, idx, NULL );
            free( *oldPtr );
            setST( st, idx, &newStr, env );
        } else {
            jmp_buf localEnv;
            int code = setjmp( localEnv );
            if ( code == 0 ) {
                setST( st, idx, &newStr, &localEnv );
            } else {
                free( newStr );
                longjmp( *env, code );
            }
        }
        return false;
    }

    if ( strcmp( cmd, "remove" ) == 0 ) {
        char extra[ EXTRA_LEN ];
        if ( sscanf( line, " remove %15s", extra ) == 1 ) {
            longjmp( *env, INVALID_COMMAND );
        }
        if ( sizeST( st ) > 0 ) {
            int lastIdx = sizeST( st ) - 1;
            char **lastPtr = getST( st, lastIdx, NULL );
            free( *lastPtr );
        }
        removeST( st, env );
        return false;
    }

    if ( strcmp( cmd, "query" ) == 0 ) {
        int i, j;
        char extra[ EXTRA_LEN ];
        if ( sscanf( line, " query %d %d %15s", &i, &j, extra ) != PARAM_COUNT_TWO ) {
            longjmp( *env, INVALID_COMMAND );
        }
        int idx = queryST( st, i, j, env );
        char **resPtr = getST( st, idx, env );
        printf( "%s\n", *resPtr );
        return false;
    }

    longjmp( *env, INVALID_COMMAND );
}

static void freeTree( SegTree *st ) {
    int n = sizeST( st );
    for ( int i = 0; i < n; i++ ) {
        char **ptr = getST( st, i, NULL );
        if ( ptr && *ptr ) {
            free( *ptr );
        }
    }
}
