/**
    @file segTree.c
    @author Jayani Sivakumar ( jsivaku )

    This file contains the implementation of a generic segment tree that stores
    fixed-size elements. It supports insertion, removal, querying, updating values,
    and error handling using setjmp/longjmp. The segment tree is implemented using
    an array-based binary heap structure.
*/
#include "segTree.h"
#include <stdlib.h>
#include <string.h>

/** Initial capacity of the segment tree. */
#define INITIAL_CAP 4

/** Overhead of for the tree, relative to the capacity. */
#define TREE_OVERHEAD 2

/** Error code returned via longjmp on invalid operations. */
#define SEGTREE_ERROR 100

/** Gets the parent index of a node in the segment tree. */
#define PARENT(n) ((n) / 2)

/** Gets the left child index of a node in the segment tree. */
#define LEFT(n)   ((n) * 2)

/** Gets the right child index of a node in the segment tree. */
#define RIGHT(n)  ((n) * 2 + 1)

/** Constant used for left/right child index calculations. */
#define TREE_BRANCH_FACTOR 2

/** Growth factor for dynamic resizing of segment tree arrays. */
#define GROWTH_FACTOR 2

SegTree *makeST( size_t vSize, int (*vComp)( void const *, void const * ) )
{
    SegTree *st = malloc( sizeof( SegTree ) );
    st -> vSize = vSize;
    st -> capacity = INITIAL_CAP;
    st -> size = 0;
    
    // allocating the vList
    st -> vList = malloc( st -> capacity * st -> vSize );
    
    // allocating the capacity for the tree
    st -> tree = malloc( TREE_OVERHEAD * st -> capacity * sizeof( int ) );
    
    st -> leafStart = st -> capacity;
    // leaves are set to -1 initially
    for ( int i = 0; i < st -> capacity; i++ ) {
        st -> tree[ st -> leafStart + i ] = -1;
    }
    
    // setting default value for internal nodes
    for ( int i = st -> leafStart - 1; i > 0; i-- ) {
        st -> tree[ i ] = -1;
    }  
    st -> vComp = vComp;
    return st;   
}

void freeST( SegTree *st ) 
{
    free( st -> vList );
    free( st -> tree );
    free( st );
}

int sizeST( SegTree *st )
{
    return st -> size;
}

int addST( SegTree *st, void *valPtr )
{
    if ( st -> size >= st -> capacity ) {
        int newCapacity = st -> capacity * GROWTH_FACTOR;
        
        // resizing the vList array
        void *newData = realloc( st -> vList, newCapacity *st -> vSize );
        st -> vList = newData;
        
        // resizing the tree array
        int *newTree = realloc( st -> tree, TREE_OVERHEAD * newCapacity * sizeof( int ) );
        st ->tree = newTree;
        st -> capacity = newCapacity;
        st -> leafStart = newCapacity;
        
        // initializing the entire tree array to -1
        for ( int i = 0 ; i < TREE_OVERHEAD * newCapacity ; i++ ) {
            st -> tree[ i ] = -1;
        }
        
        // setting the leaf nodes 
        for ( int i = 0 ; i < st -> size ; i++ ) {
            st -> tree[ st -> leafStart + i ] = i;
        }
        
        // making the internal nodes
        for ( int pos = st -> leafStart - 1 ; pos >= 1 ; pos-- ) {
            int leftChild = st -> tree[ LEFT( pos ) ];
            int rightChild = st -> tree[ RIGHT( pos ) ];
            
            if ( leftChild == -1 ) {
                st -> tree[ pos ] = rightChild;
            }
            else if ( rightChild == -1 ) {
                st -> tree[ pos ] = leftChild;
            }
            else {
                void *leftVal = ( char * ) st -> vList + ( leftChild *st -> vSize );
                void *rightVal = ( char * ) st -> vList + ( rightChild *st -> vSize );
                if ( st -> vComp( leftVal, rightVal ) < 0 ) {
                    st -> tree[ pos ] = rightChild;
                } else {
                    st -> tree[ pos ] = leftChild;
                }
            }
        }
    }

    // copying the value into vList array
    memcpy( ( char * ) st -> vList + ( st -> size *st -> vSize ), valPtr, st -> vSize );
    
    // placing newly added value to corresponding leaf
    int index = st -> leafStart + st -> size;
    st -> tree[ index ] = st -> size; 
    st -> size++;

    // updating the internal nodes
    for ( int pos = index / TREE_BRANCH_FACTOR ; pos >= 1 ; pos /= TREE_BRANCH_FACTOR ) {
        int leftChild = st -> tree[ TREE_BRANCH_FACTOR * pos ];
        int rightChild = st -> tree[ TREE_BRANCH_FACTOR * pos + 1 ];
        
        if ( rightChild == -1 ) {
            st -> tree[ pos ] = leftChild;
        }
        else {
            void *leftVal = ( char * ) st -> vList + ( leftChild *st -> vSize );
            void *rightVal = ( char * ) st -> vList + ( rightChild *st -> vSize );
            if ( st -> vComp( leftVal, rightVal ) < 0 ) {
                st -> tree[ pos ] = rightChild;
            } else {
                st -> tree[ pos ] = leftChild;
            }
        }
    }
    
    // index where element was added
    return st -> size - 1;
}

void * getST( SegTree *st, int idx, jmp_buf *env )
{
    if ( idx < 0 || idx >= st -> size ) {
        if ( env ) {
            longjmp( *env, SEGTREE_ERROR );
        }
    }
    return ( char * ) st -> vList + ( idx *st -> vSize );
}

void setST( SegTree *st, int idx, void *valPtr, jmp_buf *env )
{
    if ( idx < 0 || idx >= st -> size ) {
        longjmp( *env, SEGTREE_ERROR );
    }
    
    // copying new value into vList array
    memcpy( ( char * ) st -> vList + ( idx *st -> vSize ), valPtr, st -> vSize );
    
    // updating the leaf
    int index = st -> leafStart + idx;
    st -> tree[ index ] = idx;
    
    // making the changes to the tree
    for ( int pos = index / TREE_BRANCH_FACTOR ; pos >= 1 ; pos /= TREE_BRANCH_FACTOR ) {
        int leftChild = st -> tree[ LEFT( pos ) ];
        int rightChild = st -> tree[ RIGHT( pos ) ];
        
        if ( rightChild == -1 ) {
            st -> tree[ pos ] = leftChild;
        }
        else {
            void *leftVal = ( char * ) st -> vList + ( leftChild *st -> vSize );
            void *rightVal = ( char * ) st -> vList + ( rightChild *st -> vSize );
            if ( st -> vComp( leftVal, rightVal ) < 0 ) {
                st -> tree[ pos ] = rightChild;
            } else {
                st -> tree[ pos ] = leftChild;
            }
        }
    }
}

void removeST( SegTree *st, jmp_buf *env )
{
    if ( st -> size <= 0 ) {
        longjmp( *env, SEGTREE_ERROR );
    }   
    st -> size--;
    
    // setting the removed leaf to -1
    int index = st -> leafStart + st -> size;
    st -> tree[ index ] = -1; 
    
    for ( int pos = index / TREE_BRANCH_FACTOR ; pos >= 1 ; pos /= TREE_BRANCH_FACTOR ) {
        int leftChild = st -> tree[ LEFT( pos ) ];
        int rightChild = st -> tree[ RIGHT( pos ) ];
        
        if ( leftChild == -1 ) {
            st -> tree[ pos ] = rightChild;
        }
        else if ( rightChild == -1 ) {
            st -> tree[ pos ] = leftChild;
        }
        else {
            void *leftVal = ( char * ) st -> vList + ( leftChild *st -> vSize );
            void *rightVal = ( char * ) st -> vList + ( rightChild *st -> vSize );
            if ( st -> vComp( leftVal, rightVal ) < 0 ) {
                st -> tree[ pos ] = rightChild;
            } else {
                st -> tree[ pos ] = leftChild;
            }
        }
    }
}

int queryST( SegTree *st, int i, int j, jmp_buf *env )
{
    if ( i < 0 || j >= st -> size || i > j ) {
        if ( env ) {
            longjmp( *env, SEGTREE_ERROR );
        }
    }
   
    int i_leaf = st -> leafStart + i;
    int j_leaf = st -> leafStart + j;
    int element = -1; 
    while ( i_leaf <= j_leaf ) {
        // if its right child/odd index, proccess and move to next
        if ( i_leaf % TREE_BRANCH_FACTOR == 1 ) {
            if ( element == -1 ) {
                element = st -> tree[ i_leaf ];
            }
            else if ( st -> tree[ i_leaf ] != -1 ) {
                void *candVal = ( char * ) st -> vList + ( element *st -> vSize );
                void *currVal = ( char * ) st -> vList + ( st -> tree[ i_leaf ] *st -> vSize );
                if ( st -> vComp( candVal, currVal ) < 0 ) {
                    element = st -> tree[ i_leaf ];
                }
            }
            i_leaf++;
        }
        
        // if its left child/even index, process and move to previous
        if ( j_leaf % TREE_BRANCH_FACTOR == 0 ) {
            if ( element == -1 ) {
                element = st -> tree[ j_leaf ];
            }
            else if ( st -> tree[ j_leaf ] != -1 ) {
                void *candVal = ( char * ) st -> vList + ( element *st -> vSize );
                void *currVal = ( char * ) st -> vList + ( st -> tree[ j_leaf ] *st -> vSize );
                if ( st -> vComp( candVal, currVal ) < 0 ) {
                    element = st -> tree[ j_leaf ];
                }
            }
            j_leaf--;
        }
        
        // next upper level
        i_leaf /= TREE_BRANCH_FACTOR;
        j_leaf /= TREE_BRANCH_FACTOR;
    }
    
    return element;
}



    


