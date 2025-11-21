/**
    @file database.h
    @author Jayani Sivakumar ( jsivaku )

    This file contains the header definitions for the database component.
    The file also provides function prototypes for managing and manipulating
    the database by storing house rental data, including
    houses, available dates, and the database containing the list of houses.
*/
#include <stdbool.h>

/** Maximum length for email. */
#define MAX_EMAIL_LEN 30  

/**
    Represents a date when a house is for rent.
*/
typedef struct DateRep {
    int month;
    int day;
    bool isAvailable;
    char email[ MAX_EMAIL_LEN + 1 ];
} Date;

/**
    Represents a house for rent.
*/
typedef struct HouseRep {
    Date **list;
    int count;
    int capacity;
    int id;
    int maxGuests;
    int cost;
    char name[ MAX_EMAIL_LEN + 1 ];
} House;

/**
    Represents the database that stores all houses for rent.
*/
typedef struct DatabaseRep {
    House **list;
    int count;
    int capacity;
} Database;

/**
    This function dynamically allocates storage for the Database,
    initializes its fields (to store a resizable array).

    @return pointer to new database instance
*/
Database *makeDatabase();

/**
    This function frees the memory used to store the given Database,
    including freeing space for all the Houses and their Date arrays,
    freeing the resizable array of pointers and freeing space for
    the Database struct itself.

    @param database pointer to database to be freed
*/
void freeDatabase( Database *database );

/**
    This function reads all the houses from a house file with the given
    name. It makes an instance of the House struct for each one and stores
    a pointer to that house in the resizable array in database.

    @param filename name of the house file to read
    @param database database where houses should be stored
*/
void readHouses( char const *filename, Database *database );

/**
    Sorts houses in the database using custom comparison functions.
    It uses the qsort() function together with the function pointer
    parameter to order the houses.

    @param database pointer to database containing the houses
    @param compare pointer to comparison function which determines the sorting order
*/
void sortHouses( Database *database, int ( *compare )( void const *va, void const *vb ) );

/**
    Lists houses in the database that satisfy the given test function.
    This function prints houses in a formatted output with headers.

    @param database pointer to database containing houses
    @param test pointer to function that determines whether a house should be printed
    @param val1 first integer passed to function
    @param val2 Second integer passed to function
*/
void listHouses( Database *database, bool ( *test )( House const *house, int val1, int val2 ), int val1, int val2 );

/**
    Checks if a given date falls on a Saturday between June 7 and November 29, 2025.
    Zeller’s algorithm may be used to determine whether a date falls on a Saturday.

    @param month month of the date
    @param day day of the month
    @return true if date is a valid Saturday, else false
*/
bool isValidSaturday( int month, int day );

/**
    Checks if the house is available for a specific date.
    
    @param house pointer house
    @param month month of requested date
    @param day day of requested date
    @return true if house is available on the given date, else false
*/
bool isHouseAvailableOnDate( House const *house, int month, int day );

/**
    Checks if house is available for any date.
    
    @param house pointer to house
    @param val1 parameter
    @param val2 parameter
    @return true if house has at least one available date, else false
*/
bool isAvailable( House const *house, int val1, int val2 );

/**
    Function to compare two dates (used for sorting) and order them in ascending order.
    
    @param a pointer to first date pointer
    @param b pointer to second date pointer
    @return -1 if first date is earlier than second,
    1 if first date is later, and 0 if they are the same.
*/
int sortDates( const void *a, const void *b );

/**
    Checks if a house is reserved by customer.
    
    @param house pointer to house
    @param val1 parameter
    @param val2 parameter
    @return true if house is reserved, else false
*/
bool isReserved( House const *house, int val1, int val2 );