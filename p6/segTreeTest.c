/** 
    @file segTreeTest.c
    @author dbsturgi
    Unit tests for the segment tree component with integer values.
*/


#include <stdio.h>
#include <stdbool.h>
#include "segTree.h"

/** Number of tests we should have, if they're all turned on. */
#define EXPECTED_TOTAL 75

/** Total number or tests we tried. */
static int totalTests = 0;

/** Number of test cases passed. */
static int passedTests = 0;

/** Macro to check the condition on a test case, keep counts of
    passed/failed tests and report a message if the test fails. */
#define TestCase( conditional ) {\
  totalTests += 1; \
  if ( conditional ) { \
    passedTests += 1; \
  } else { \
    printf( "**** Failed unit test on line %d of %s\n", __LINE__, __FILE__ );    \
  } \
}

/** Comparison function for integer values.
    @param a Pointer to the first integer values.
    @param b Pointer to the other integer value.
    @return -1, 0 or 1 depending on whehter the first value is less than, equal or greater than the second.
*/
static int intComp( void const *a, void const *b )
{
  int aval = *(int const *)a;
  int bval = *(int const *)b;

  if ( aval < bval )
    return -1;
  if ( aval > bval )
    return 1;
  return 0;
}

/** Top-level function for the unit test program.
    @return exit status of the program.
*/
int main()
{
  // As you finish parts of your implementation, move this directive
  // down past the blocks of code below.  That will enable tests of
  // various functions you're expected to implement.


  // Test an empty segment tree.
  
  {
    SegTree *st = makeST( sizeof( int ), intComp );

    // Non-null pointer.
    TestCase( st );
    
    // Empty tree.
    TestCase( sizeST( st ) == 0 );

    // Make sure the destructor doesn't fail.
    freeST( st );
  }


  // Try add / get on a segment tree with 4 values.
  
  {
    int seq[] = { 3, 10, 9, 2 };
    int n = sizeof( seq ) / sizeof( seq[ 0 ] );
  
    SegTree *st = makeST( sizeof( int ), intComp );
    for ( int i = 0; i < n; i++ ) {
      int j = addST( st, seq + i );
      // Value gets added at the right index.
      TestCase( j == i );

      // Resulting size is right.
      TestCase( sizeST( st ) == i + 1 );
    }

    // Make sure the segment tree contains the right sequence of values.
    for ( int i = 0; i < sizeST( st ); i++ ) {
      int *p = getST( st, i, NULL );
      TestCase( *p == seq[ i ] );
    }

    freeST( st );
  }
  
  // Try set on a segment tree with 4 values.
  
  {
    int seq[] = { 100, 200, -100, 0 };
    int n = sizeof( seq ) / sizeof( seq[ 0 ] );
  
    SegTree *st = makeST( sizeof( int ), intComp );
    for ( int i = 0; i < n; i++ )
      addST( st, seq + i );

    // Check the original values.
    for ( int i = 0; i < sizeST( st ); i++ ) {
      int *p = getST( st, i, NULL );
      TestCase( *p == seq[ i ] );
    }

    // Change all the values.
    int seq2[ sizeof( seq ) / sizeof( seq[ 0 ] ) ] = { 80, 110, 40, 70 };
    for ( int i = 0; i < n; i++ )
      setST( st, i, seq2 + i, NULL );
    
    // Check the modified values.
    for ( int i = 0; i < n; i++ ) {
      int *p = getST( st, i, NULL );
      TestCase( *p == seq2[ i ] );
    }

    freeST( st );
  }
  
  // Try query on a segment tree with 4 values.
  
  {
    int seq[] = { 3, 10, 9, 2 };
    int n = sizeof( seq ) / sizeof( seq[ 0 ] );
  
    SegTree *st = makeST( sizeof( int ), intComp );
    for ( int i = 0; i < n; i++ )
      addST( st, seq + i );

    // Check some trivial ranges.
    TestCase( queryST( st, 0, 0, NULL ) == 0 );
    TestCase( queryST( st, 1, 1, NULL ) == 1 );
    TestCase( queryST( st, 2, 2, NULL ) == 2 );
    TestCase( queryST( st, 3, 3, NULL ) == 3 );

    // Check some two-element ranges.
    TestCase( queryST( st, 0, 1, NULL ) == 1 );
    TestCase( queryST( st, 1, 2, NULL ) == 1 );
    TestCase( queryST( st, 2, 3, NULL ) == 2 );

    // Check all four elements.
    TestCase( queryST( st, 0, 3, NULL ) == 1 );
    
    // Check three-element ranges.
    TestCase( queryST( st, 0, 2, NULL ) == 1 );
    TestCase( queryST( st, 1, 3, NULL ) == 1 );
    
    freeST( st );
  }
  
  // Try query on a segment tree with 8 values.
  
  {
    int seq[] = { 2, 8, 3, 7, 5, 4, 9, 6 };
    int n = sizeof( seq ) / sizeof( seq[ 0 ] );

    // Make a segment tree with 4 elements.
    SegTree *st = makeST( sizeof( int ), intComp );
    for ( int i = 0; i < 4; i++ )
      addST( st, seq + i );

    // Query for just the first 4 items.
    TestCase( queryST( st, 0, 3, NULL ) == 1 );
    
    // More checks for the next 4 elments.
    for ( int i = 4; i < n; i++ ) {
      int j = addST( st, seq + i );
      
      // Make sure this element was inserted in the right spot.
      TestCase( j == i );

      // Make sure the size is right.
      TestCase( sizeST( st ) == i + 1 );
    }

    // Check some range queries.
    TestCase( queryST( st, 0, 7, NULL ) == 6 );
    TestCase( queryST( st, 1, 6, NULL ) == 6 );
    TestCase( queryST( st, 0, 4, NULL ) == 1 );
    TestCase( queryST( st, 1, 5, NULL ) == 1 );
    TestCase( queryST( st, 2, 6, NULL ) == 6 );
    TestCase( queryST( st, 2, 5, NULL ) == 3 );
    
    freeST( st );
  }
  
  // Try query after using set to change some values.
  
  {
    // Put 8 elements in a segment tree.
    SegTree *st = makeST( sizeof( int ), intComp );
    int seq[] = { 2, 8, 3, 7, 5, 4, 9, 6 };
    int n = sizeof( seq ) / sizeof( seq[ 0 ] );
    for ( int i = 0; i < n; i++ )
      addST( st, seq + i );

    // Get the largest value.
    TestCase( queryST( st, 0, 7, NULL ) == 6 );

    // Make that value smaller then query again.
    int newVal = 1;
    setST( st, 6, &newVal, NULL );
    TestCase( queryST( st, 0, 7, NULL ) == 1 );

    // Increase a small value.
    newVal = 10;
    setST( st, 2, &newVal, NULL );
    TestCase( queryST( st, 0, 7, NULL ) == 2 );
    TestCase( queryST( st, 3, 6, NULL ) == 3 );
    TestCase( queryST( st, 4, 7, NULL ) == 7 );

    // Change a few values.
    newVal = 11;
    setST( st, 7, &newVal, NULL );
    newVal = 12;
    setST( st, 0, &newVal, NULL );
    newVal = 9;
    setST( st, 2, &newVal, NULL );
    
    TestCase( queryST( st, 0, 4, NULL ) == 0 );
    TestCase( queryST( st, 1, 5, NULL ) == 2 );
    TestCase( queryST( st, 2, 6, NULL ) == 2 );
    TestCase( queryST( st, 3, 7, NULL ) == 7 );
    
    freeST( st );
  }
  
  // Try calling remove / add
  
  {
    // Put 11 values in a segment tree.
    SegTree *st = makeST( sizeof( int ), intComp );
    int seq[] = { 2, 1, 4, 3, 6, 5, 7, 8, 10, 9, 11 };
    int n = sizeof( seq ) / sizeof( seq[ 0 ] );
    for ( int i = 0; i < n; i++ )
      addST( st, seq + i );

    TestCase( queryST( st, 0, 10, NULL ) == 10 );

    // Remove one item.
    removeST( st, NULL );
    TestCase( sizeST( st ) == 10 );
    TestCase( queryST( st, 0, 9, NULL ) == 8 );

    // Remove another item.
    removeST( st, NULL );
    TestCase( sizeST( st ) == 9 );
    TestCase( queryST( st, 0, 8, NULL ) == 8 );
    
    // Remove another item.
    removeST( st, NULL );
    TestCase( sizeST( st ) == 8 );
    TestCase( queryST( st, 0, 7, NULL ) == 7 );
    
    // Add an item.
    int newVal = 0;
    addST( st, &newVal );
    TestCase( sizeST( st ) == 9 );
    TestCase( queryST( st, 0, 8, NULL ) == 7 );
    
    // Add another item.
    newVal = 13;
    addST( st, &newVal );
    TestCase( sizeST( st ) == 10 );
    TestCase( queryST( st, 0, 9, NULL ) == 9 );
  }
  
  // Test error handling via longjmp()
  
  {
    jmp_buf env;
    
    // Empty segment tree.
    SegTree *st = makeST( sizeof( int ), intComp );

    // Can't remove from an empty tree. tree.
    int code;
    bool jumped = false;
    if ( ( code = setjmp( env ) ) == 0 ) {
      removeST( st, &env );
    } else {
      TestCase( code == SEGTREE_ERROR );
      jumped = true;
    }
    TestCase( jumped );

    // Add some elements.
    int seq[] = { 2, 8, -4 };
    int n = sizeof( seq ) / sizeof( seq[ 0 ] );
    for ( int i = 0; i < n; i++ )
      addST( st, seq + i );

    // Can't get an element before index zero.
    jumped = false;
    if ( ( code = setjmp( env ) ) == 0 ) {
      getST( st, -1, &env );
    } else {
      TestCase( code == SEGTREE_ERROR );
      jumped = true;
    }
    TestCase( jumped );

    // Can't set an element off the end fo the tree.
    jumped = false;
    int newVal = 99;
    if ( ( code = setjmp( env ) ) == 0 ) {
      setST( st, 4, &newVal, &env );
    } else {
      TestCase( code == SEGTREE_ERROR );
      jumped = true;
    }
    TestCase( jumped );

    // Can't query a range that ends before it starts.
    jumped = false;
    if ( ( code = setjmp( env ) ) == 0 ) {
      queryST( st, 2, 0, &env );
    } else {
      TestCase( code == SEGTREE_ERROR );
      jumped = true;
    }
    TestCase( jumped );

    freeST( st );
  }

  // Once you move the #ifdef DISABLE_TESTS to here, you've enabled
  // all the tests.
#ifdef DISABLE_TESTS
#endif
  
  // Report a message if some tests are still disabled.
  if ( totalTests < EXPECTED_TOTAL )
    printf( "** %d of %d tests currently enabled.\n", totalTests,
            EXPECTED_TOTAL );
  if ( totalTests > EXPECTED_TOTAL )
    printf( "** There were more passing unit tests than expected (%d vs %d).\n",
            totalTests, EXPECTED_TOTAL );

  // Exit successfully if all tests are enabled and they all pass.
  if ( passedTests != EXPECTED_TOTAL )
    return EXIT_FAILURE;
  else
    return EXIT_SUCCESS;
}

