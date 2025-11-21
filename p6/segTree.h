/**
    @file segTree.h
    @author Jayani Sivakumar ( jsivaku )

    This file defines the interface for a generic segment tree data structure.
    It provides functions for creating and freeing a segment tree, as well as 
    inserting values, querying the best value in a range, modifying values, and
    handling errors.
*/
#include <stdlib.h>
#include <setjmp.h>

/** type for a segment tree. */
typedef struct SegTreeStruct SegTree;

/** Representation of the segment tree. */
struct SegTreeStruct {
    size_t vSize;                     
    int capacity;
    int size;
    void *vList;
    int *tree;
    int leafStart;
    int (*vComp)( void const *, void const * );
};

/** Constant value for longjmp() to indicate an invalid call to a
    segTree function. */
#define SEGTREE_ERROR 100

/**
    Creates a new segment tree that stores elements of the given size and 
    uses the specified comparison function to determine the best value.

    @param vSize size of each element in bytes
    @param vComp pointer to a comparison function that returns:
           a positive value if the first is better, a negative value if the second is better
           else zero if equal
    @return pointer to the newly allocated segment tree
 */
SegTree *makeST( size_t vSize, int (*vComp)( void const *, void const * ) );

/**
    Frees all memory associated with the given segment tree.

    @param st pointer to the segment tree to be freed
 */
void freeST( SegTree *st );

/**
    Returns the number of elements currently stored in the segment tree.

    @param st pointer to the segment tree
    @return the number of stored elements
 */
int sizeST( SegTree *st );

/**
    Adds a new value to the end of the segment tree and updates internal nodes accordingly.

    @param st pointer to the segment tree
    @param valPtr pointer to the value to be added
    @return the index at which the value was added
 */
int addST( SegTree *st, void *valPtr );

/**
    Removes the most recently added value from the segment tree.
    If called on an empty tree, this function will invoke longjmp() with SEGTREE_ERROR.

    @param st pointer to the segment tree
    @param env jump buffer to handle errors via longjmp
 */
void removeST( SegTree *st, jmp_buf *env );

/**
    Retrieves the value at the specified index in the segment tree.
    If the index is out of bounds, this function will invoke longjmp() with SEGTREE_ERROR.

    @param st pointer to the segment tree
    @param idx index of the value to retrieve
    @param env jump buffer to handle errors via longjmp
    @return pointer to the requested value
 */
void *getST( SegTree *st, int idx, jmp_buf *env );

/**
    Replaces the value at the specified index in the segment tree with the given value.
    If the index is out of bounds, this function will invoke longjmp() with SEGTREE_ERROR.

    @param st pointer to the segment tree
    @param idx index of the value to replace
    @param valPtr pointer to the new value
    @param env jump buffer to handle errors via longjmp
 */
void setST( SegTree *st, int idx, void *valPtr, jmp_buf *env );

/**
    Returns the index of the best value in the given range [i, j].
    If the range is invalid, this function will invoke longjmp() with SEGTREE_ERROR.

    @param st pointer to the segment tree
    @param i start index of the range
    @param j end index of the range
    @param env jump buffer to handle errors via longjmp
    @return index of the best value within the range
 */
int queryST( SegTree *st, int i, int j, jmp_buf *env );