/**
    @file database.c
    @author Jayani Sivakumar ( jsivaku )

    This file contains the implementation of the database component.
    It manages reading houses from files, storing them in a resizable
    array and sorting them.
 */
#include "database.h"
#include "input.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/** Initial size for house and date lists. */
#define INITIAL_CAPACITY 5

/** Maximum length for house names. */
#define MAX_NAME_LENGTH 30

/** Initial capacity for house date list. */
#define INITIAL_HOUSE_CAPACITY 5

/** Expansion of house date list. */
#define DATE_GROWTH 2

/** Expected fields for house details. */
#define EXPECTED_HOUSE_FIELDS 3  

/** Expected fields for a date. */
#define EXPECTED_DATE_FIELDS 2

/** Starting valid month for reservations - June. */
#define MIN_VALID_MONTH 6  

/** Ending valid month for reservations - November. */
#define MAX_VALID_MONTH 11  

/** Minimum valid day of the month. */
#define MIN_VALID_DAY 1  

/** Maximum valid day of the month. */
#define MAX_VALID_DAY 31  

/** Saturday. */
#define SAT_WEEKDAY 6

/** Year used for reservation. */
#define CURR_YEAR 2025

/** Number of days in a week. */
#define DAYS_IN_WEEK 7

/** Month calculation in Zeller’s Congruence. */
#define ZELLER_MONTH 14  

 /** Total months in a year. */
#define MONTHS_IN_YEAR 12 

/** Month calculation in Zeller’s formula. */
#define ZELLER_CONSTANT 2  

/** Used for day calculation. */
#define DAYS_IN_LONG_MONTH 31  

/** For finding leap years. */
#define LEAP_YEAR 4  

/** For finding non-leap century years. */
#define NON_LEAP_CENTURY_YEAR 100  

/** For finding leap years in centuries */
#define LEAP_CENTURY_YEAR 400 

/** Maximum length for email. */
#define MAX_EMAIL_LEN 30  

Database *makeDatabase() 
{
    // allocate memory for database
    Database *db = malloc( sizeof( Database ) );
    
    if ( !db ) {
        exit( EXIT_FAILURE );
    }
    
    db->count = 0;
    db->capacity = INITIAL_CAPACITY;
    
    // allocating memory for array of house pointers
    db->list = malloc( db->capacity * sizeof( House * ) );
    
    if ( !db->list ) {
        free( db );
        exit( EXIT_FAILURE );
    }
    return db;
}

void freeDatabase( Database *database ) 
{
    if ( !database ) {
        return;
    }

    // check if database contains a list of houses
    if ( database->list ) {
        for ( int i = 0; i < database->count; i++ ) {
            // pointer for each house
            House *house = database->list[ i ];
            if ( house ) {
                // iterating every house in the list
                for ( int j = 0; j < house->count; j++ ) {
                    if ( house->list[ j ] ) {
                        free( house->list[ j ] );
                    }
                }

                free( house->list );
                free( house );
            }
        }
        free( database->list );
    }

    free( database );
}

void readHouses( char const *filename, Database *database ) 
{
    // opening the file for reading
    FILE *file = fopen( filename, "r" );
    if ( !file ) {
        fprintf( stderr, "Can't open file: %s\n", filename );
        exit( EXIT_FAILURE );
    }

    char *line;
    
    // reading each house from the file
    while ( ( line = readLine( file ) ) != NULL ) {
        // skipping empty lines
        while ( line && strlen( line ) == 0 ) {
            free( line );
            line = readLine( file );
        }
        if ( !line ) {
            break;
        }

        int id, maxGuests, cost;
        // details of each house
        if ( sscanf( line, "%d %d %d", &id, &maxGuests, &cost ) != EXPECTED_HOUSE_FIELDS ) {
            fprintf( stderr, "Invalid house file: %s\n", filename ); 
            exit( EXIT_FAILURE );
        }
        if ( id <= 0 || maxGuests <= 0 || cost <= 0 ) {
            fprintf( stderr, "Invalid house file: %s\n", filename ); 
            exit( EXIT_FAILURE );
        }
        free( line );

        // reading the second line that has the house name
        line = readLine( file );
        if ( !line || strlen( line ) > MAX_NAME_LENGTH ) {
            fprintf( stderr, "Invalid house file: %s\n", filename ); 
            exit( EXIT_FAILURE );
        }

        char name[ MAX_NAME_LENGTH + 1 ];
        strncpy( name, line, MAX_NAME_LENGTH );
        name[ MAX_NAME_LENGTH ] = '\0';
        free( line );

        // checing for duplicate house ID
        for ( int i = 0; i < database->count; i++ ) {
            if ( database->list[ i ]->id == id ) {
                fprintf( stderr, "Invalid house file: %s\n", filename ); 
                exit( EXIT_FAILURE );
            }
        }

        // allocating memory for the new house
        House *house = malloc( sizeof( House ) );
        if ( !house ) {
            exit( EXIT_FAILURE );
        }
        house->id = id;
        house->maxGuests = maxGuests;
        house->cost = cost;
        strcpy( house->name, name );
        house->count = 0;
        house->capacity = INITIAL_HOUSE_CAPACITY;

        // allocating memory for the dates
        house->list = malloc( house->capacity * sizeof(Date *));
        if ( !house->list ) {
            free( house );
            exit( EXIT_FAILURE );
        }

        // reading list of available dates on the third line
        line = readLine( file );
        if ( !line ) {
            free( house->list );
            free( house );
            continue;
        }

        // pointer to traverse through the date string
        char *ptr = line;
        while ( *ptr ) {
            // skipping the leading spaces and tabs
            while ( *ptr == ' ' || *ptr == '\t' ) {
                ptr++;
            }

            int month, day;
            if ( isdigit( *ptr ) ) {
                if ( sscanf( ptr, "%d / %d", &month, &day ) != EXPECTED_DATE_FIELDS ) {
                    sscanf( ptr, "%d/%d", &month, &day );
                }
                // skipping this date but continue processing other dates
                if ( !isValidSaturday( month, day ) ) {
                    ptr++;
                    continue;
                }

                // allocating memory and storing the date
                if ( house->count >= house->capacity ) {
                    house->capacity *= DATE_GROWTH;
                    Date **temp = realloc( house->list, house->capacity * sizeof(Date *) );
                    if ( !temp ) {
                        free( line );
                        free( house->list );
                        free( house );
                        exit( EXIT_FAILURE );
                    }
                    house->list = temp;
                }
                // allocating memory for the new date
                Date *newDate = malloc( sizeof( Date ) );
                if ( !newDate ) {
                    continue;
                }
                newDate->month = month;
                newDate->day = day;
                newDate->isAvailable = true;
                newDate->email[ 0 ] = '\0';
                
                // storing the new date in the house list
                house->list[ house->count++ ] = newDate;
            }

            // moving to next date
            while ( *ptr && *ptr != ' ' ) {
                ptr++;
            }
        }
        free( line );

        // adding house to the database
        if ( database->count >= database->capacity ) {
            database->capacity *= DATE_GROWTH;
            House **temp = realloc( database->list, database->capacity * sizeof( House * ) );
            if ( !temp ) {
                free( house->list );
                free( house );
                exit( EXIT_FAILURE );
            }
            database->list = temp;
        }
        database->list[ database->count++ ] = house;
    }

    fclose( file );
}

bool isValidSaturday( int month, int day ) 
{
    if ( month < MIN_VALID_MONTH || month > MAX_VALID_MONTH || day < MIN_VALID_DAY || day > MAX_VALID_DAY ) {
        return false;
    }
    
    int year = CURR_YEAR;
    int w = year - ( ZELLER_MONTH - month ) / MONTHS_IN_YEAR;
    int x = w + w / LEAP_YEAR - w / NON_LEAP_CENTURY_YEAR + w / LEAP_CENTURY_YEAR;
    int z = month + MONTHS_IN_YEAR * ( ( ZELLER_MONTH - month ) / MONTHS_IN_YEAR ) - ZELLER_CONSTANT;
    int dayOfWeek = ( day + x + ( DAYS_IN_LONG_MONTH * z ) / MONTHS_IN_YEAR ) % DAYS_IN_WEEK;
    
    return ( dayOfWeek == SAT_WEEKDAY );
}

void sortHouses( Database *database, int (*compare)( void const *va, void const *vb ) ) 
{
    if ( !database || !database->list ) {
        return;
    }
    
    // no sorting if 1 or less houses
    if ( database->count <= 1 ) {
        return;
    }
    
    // using qsort to sort using comparison functions
    qsort( database->list, database->count, sizeof( House * ), compare );
}

void listHouses( Database *database, bool (*test)( House const *house, int val1, int val2 ), int val1, int val2 ) 
{
    if ( !database || !database->list ) {
        return;
    }
    
    if ( database->count == 0 ) {
        return;
    }
    
    // checking the reservation dates
    bool listDates = ( test == isAvailable || test == isReserved );  
    bool listSingleDate = ( test == isHouseAvailableOnDate );  

    if ( listDates ) {
        printf( "ID  Name                           Sleeps   Cost  Date\n" );
    } else {
        printf( "ID  Name                           Sleeps   Cost\n" );
    }
 
    // iterating through all houses
    for ( int i = 0; i < database->count; i++ ) {
        if ( !database->list[ i ] ) {
            continue;
        }

        House *house = database->list[ i ];
        bool houseListed = false;

        // sorting the dates if we are able to
        if ( listDates ) {
            qsort( house->list, house->count, sizeof(Date *), sortDates );
            
            for ( int j = 0; j < house->count; j++ ) {
                if ( house->list[ j ]->isAvailable && test == isAvailable ) {
                    printf( "%3d %-30s %6d %6d %02d/%02d\n",
                           house->id, house->name, house->maxGuests, house->cost,
                           house->list[ j ]->month, house->list[ j ]->day);
                    houseListed = true;
                }
                else if ( test == isReserved && !house->list[ j ]->isAvailable && isReserved( house, 0, 0 ) ) {
                    printf( "%3d %-30s %6d %6d %02d/%02d\n",
                           house->id, house->name, house->maxGuests, house->cost,
                           house->list[ j ]->month, house->list[ j ]->day);
                    houseListed = true;
                }
            }
        } 
        else if ( listSingleDate ) {  
            if ( test( house, val1, val2 ) && !houseListed ) {
                printf( "%3d %-30s %6d %6d\n",
                       house->id, house->name, house->maxGuests, house->cost );
                houseListed = true;
            }
        } 
        else {  
            if ( test == NULL || test( house, val1, val2 ) ) {
                printf( "%3d %-30s %6d %6d\n",
                       house->id, house->name, house->maxGuests, house->cost );
            }
        }
    }

}

int sortDates( const void *a, const void *b ) 
{
    Date *date1 = *(Date **)a;
    Date *date2 = *(Date **)b;

    // sorting by month
    if ( date1->month != date2->month ) {
        return date1->month - date2->month;
    }
    
    // sorting by day if months are the same
    return date1->day - date2->day;
}

bool isHouseAvailableOnDate( House const *house, int month, int day ) 
{
    for ( int i = 0; i < house->count; i++ ) {
        if ( house->list[ i ]->month == month && house->list[ i ]->day == day && house->list[ i ]->isAvailable ) {
            return true;
        }
    }
    return false;
}

bool isAvailable( House const *house, int val1, int val2 ) 
{
    for ( int i = 0; i < house->count; i++ ) {
        if ( house->list[ i ]->isAvailable ) {
            return true;
        }
    }
    return false;
}

bool isReserved( House const *house, int val1, int val2 ) 
{
    static char lookupEmail[ MAX_EMAIL_LEN + 1 ] = "";

    if ( val1 == -1 ) {  
        // storing email before looking up
        strncpy( lookupEmail, (char *)house, MAX_EMAIL_LEN );
        lookupEmail[ MAX_EMAIL_LEN ] = '\0';
        return false;
    }

    bool foundReservation = false;
    for ( int i = 0; i < house->count; i++ ) {
        if ( !house->list[ i ]->isAvailable ) {
            if ( strcmp( house->list[ i ]->email, lookupEmail ) == 0 ) {
                foundReservation = true;
            }
        }
    }

    return foundReservation;
}