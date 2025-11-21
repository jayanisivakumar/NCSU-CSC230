/**
    @file input.c
    @author Jayani Sivakumar ( jsivaku )

    This program contains functions to check observations, validate integers and floats,
    format and check dates, and handle data types according to the map.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "data.h"
#include "input.h"

/** Error in column length or number of columns */
#define COLUMN_ERROR 103

/** Exit status for a missing or badly formatted date. */
#define DATE_ERROR 104

/** Exit status for unrealistic date. */
#define DATE_ERROR_UNREALISTIC 105

/** Length of nan string */
#define NAN_LENGTH 3

/** Definition of nan string */
#define NAN_STR "nan"

/** Number of digits in the short (2 digit) year. */
#define SHORT_YEAR 2

/** Number of digits in the full (4 digit) year. */
#define FULL_YEAR 4

/** Number of digits in the month. */
#define MONTH_DIGITS 2

/** Number of digits in the day. */
#define DAY_DIGITS 2

/** Maximum columns for dataset*/
#define COLUMN_MAX 25

/** FLOAT2 format */
#define FLOAT_FORMAT2 2

/** FLOAT5 format */
#define FLOAT_FORMAT5 3

/** Date format */
#define DATE_NUMBER 4

/** Date length */
#define DATE_LEN 10

/** YYYY-MM-DD format separator */
#define YYYY_MM_DD_SEP1 4

/** YYYY-MM-DD format separator */
#define YYYY_MM_DD_SEP2 7

/** MM/DD/YYYY format separator */
#define MM_DD_YYYY_SEP1 2

/** MM/DD/YYYY format separator */
#define MM_DD_YYYY_SEP2 5

/** Starting index of the year in MM/DD/YYYY or MM-DD-YYYY format */
#define YEAR_START_INDEX 6

/** Starting index of the day in MM/DD/YYYY or MM-DD-YYYY format */
#define DAY_START_INDEX 3 

/** Ten's digit of year in date output */
#define YEAR_TENS_PLACE 2

/** One's digit of year in date output */
#define YEAR_ONES_PLACE 3

/** First digit of day in date output. */
#define DAY_DIGIT_FIRST 8

/** Second digit of day in date output. */
#define DAY_DIGIT_SECOND 9

/** First digit of month in date output. */
#define MONTH_DIGIT_FIRST 5

/** Second digit of month in date output. */
#define MONTH_DIGIT_SECOND 6

/** String null terminator in date array. */
#define STRING_TERMINATE 10

/** Extra credit year cutoff. */
#define YEAR_CUTOFF 25

/** Extra credit minimum year. */
#define YEAR_MIN 1926

/** Extra credit maximum year. */
#define YEAR_MAX 2025

/** Maximum number of months. */
#define MONTH_MAX 12

/** Check if year is divisible by 4. */
#define LEAP_YEAR_CHECK1 4

/** Check if year is divisible by 100. */
#define LEAP_YEAR_CHECK2 100

/** Check if year is divisible by 400. */
#define LEAP_YEAR_CHECK3 400

/** Handling thousand's digit. */
#define THOUSANDS_DIGIT 1000

/** Handling hundred's digit. */
#define HUNDREDS_DIGIT 100

/** Handling one's digit. */
#define TENS_DIGIT 10

/** Number of days in February */
#define MONTH_DAYS_1 28

/** Number of days in 30-day months */
#define MONTH_DAYS_2 30

/** Number of days in 31-day months */
#define MONTH_DAYS_3 31

/** Number of days in 29-day leap year Feb */
#define MONTH_DAYS_4 29

/** The month February */
#define FEB_MONTH 2

/** tens digit of the two-digit year (YY) is stored */
#define FULLYEARSTR_1 3

/** fullYearStr null-terminator */
#define FULLYEARSTR_NULL 4

/** total size of the fullYearStr */
#define FULLYEARSTR_SIZE 5

/** multiplier used to convert a two digit year into an integer. */
#define TWODIGIT_YEAR 10

/** number of characters used to store a two-digit year. */
#define SHORT_YEAR 2

/**
    This function checks and processes the obervation. Reads and processes a
    row of data according to the column map.

    @param len number of columns
    @param map array containing column type map
    @return true if a row was successfully processed, false if EOF was reached
*/
bool checkObservation( int len, int map[ COLUMN_MAX ] ) 
{
    char field[ FIELD_MAX + 1 ];
    int result, col = 0;

    while ( col <= len ) {
        result = readString( field );

        // Handle missing values
        if ( strcmp( field, "nan" ) == 0 || strlen( field ) == 0 ) {
            strcpy( field, "nan" );
        } 
        else {
            // process based on format type
            if ( map[ col ] == 1 ) {  // Integer
                if ( !isValidInteger( field ) ) {
                    exit( COLUMN_ERROR );
                }
                printf( "%s", field );
            } 
            else if ( map[col] == FLOAT_FORMAT2 ) {  // Float2
                if ( !isValidFloat( field ) ) {
                    exit( COLUMN_ERROR );
                }
                printf( "%.2f", atof( field ) );
            } 
            else if ( map[ col ] == FLOAT_FORMAT5 ) {  // Float5
                if ( !isValidFloat( field ) ) {
                    exit( COLUMN_ERROR );
                }
                printf( "%.5f", atof( field ) );
            } 
            else if ( map[ col ] == DATE_NUMBER ) {  // Date
                if ( !formatDate( field ) ) {
                    exit( COLUMN_ERROR );
                }
                printf( "%s", field );
            } 
            else if ( map[ col ] == 0 ) {  // String
                printf( "%s", field );
            } 
            else {
                printf( "%s", field );
            }
        }

        col++;

        if ( col <= len ) {
            if ( result == '\n' || result == EOF ) {
                return 0;
            }
            printf( "," );
        }
    }

    printf( "\n" );

    if ( result != '\n' && result != EOF ) {
        exit( COLUMN_ERROR );
    }

    return result != EOF;
}

/**
    Checks if a string represents a valid integer.

    @param str string to validate
    @return true if string is a valid integer, else false
*/
bool isValidInteger( char str[ ] ) 
{
    int i = 0;

    if ( str[ 0 ] == '-' ) {
        i++;
    }

    // empty string
    if ( str[ i ] == '\0' ) {
        return false;
    }

    while ( str[ i ] != '\0' ) {
        if ( str[ i ] < '0' || str[ i ] > '9' ) {
            return false;
        }
        i++;
    }
    return true;
}

/**
    Checks if a string represents a valid float.

    @param str string to validate
    @return true if string is a valid float, else false
*/
bool isValidFloat( char str[ ] ) 
{
    int i = 0;
    bool hasDecimal = false;

    if ( str[ 0 ] == '-' ) {
        i++;
    }

    // empty string
    if ( str[ i ] == '\0' ) {
        return false;
    }

    while ( str[ i ] != '\0' ) {
        if ( str[ i ] == '.' ) {
            if ( hasDecimal ) {
                return false; // multiple decimals
            }
            hasDecimal = true;
        } else if ( str[ i ] < '0' || str[ i ] > '9' ) {
            return false;
        }
        i++;
    }
    return true;
}

/**
    Checks if a given character is a numeric digit (0-9).

    @param ch character to check.
    @return true if the character is a digit (0-9), otherwise 0.
*/
int isDigit( char ch ) 
{
    return ch >= '0' && ch <= '9';
}

/**
    Checks if a year is a leap year.

    @param year the year to check
    @return true if the year is a leap year, otherwise 0.
*/
int isLeapYear( int year ) 
{
    return ( year % LEAP_YEAR_CHECK1 == 0 && year % LEAP_YEAR_CHECK2 != 0 ) || ( year % LEAP_YEAR_CHECK3 == 0 );
}

/** 
    Retrieves the maximum number of days in a given month for a specific year.

    @param month The month
    @param year The year
    @return The number of days in the given month.	
*/
int getMaxDays( int month, int year ) 
{
    int days_in_month[] = { MONTH_DAYS_3, MONTH_DAYS_1, MONTH_DAYS_3, MONTH_DAYS_2, MONTH_DAYS_3, MONTH_DAYS_2, MONTH_DAYS_3, MONTH_DAYS_3, MONTH_DAYS_2, MONTH_DAYS_3, MONTH_DAYS_2, MONTH_DAYS_3 };
    if ( month == FEB_MONTH && isLeapYear( year ) ) {
        return MONTH_DAYS_4;
    }
    return days_in_month[ month - 1 ];
}

/**
    Converts a two-digit year into a four-digit year.

    @param yearStr character array containing a two-digit year.
    @param fullYearStr character array where the expanded four-digit year.
*/
void expandTwoDigitYear( char yearStr[ FULLYEARSTR_1 - 1 ], char fullYearStr[ FULLYEARSTR_SIZE ] ) 
{
    int year = ( yearStr[ 0 ] - '0' ) * TWODIGIT_YEAR + ( yearStr[ 1 ] - '0' );
    
    if ( year <= YEAR_CUTOFF ) {
        fullYearStr[ 0 ] = '2';
        fullYearStr[ 1 ] = '0';
    } else {
        fullYearStr[ 0 ] = '1';
        fullYearStr[ 1 ] = '9';
    }

    fullYearStr[ FULLYEARSTR_1 - 1 ] = yearStr[ 0 ];
    fullYearStr[ FULLYEARSTR_1 ] = yearStr[ 1 ];
    fullYearStr[ FULLYEARSTR_NULL ] = '\0';
}

/**
    Formats a date string to YYYY-MM-DD, MM/DD/YYYY or MM-DD-YYYY format.

    @param date date string to format
    @return true if date was successfully formatted, else false
*/
bool formatDate( char date[ ] ) 
{
    int day, month, year;
    char temp[ DATE_LEN + 1 ];

    // YYYY-MM-DD
    if ( date[ YYYY_MM_DD_SEP1 ] == '-' && date[ YYYY_MM_DD_SEP2 ] == '-' ) {
        return true;
    }

    // MM/DD/YYYY or MM-DD-YYYY
    if ( ( date[ MM_DD_YYYY_SEP1 ] == '/' && date[ MM_DD_YYYY_SEP2 ] == '/' ) ||
        ( date[ MM_DD_YYYY_SEP1 ] == '-' && date[ MM_DD_YYYY_SEP2 ] == '-' ) ) {
        
        for ( int i = 0; i < DATE_LEN; i++ ) {
            if ( ( i == MM_DD_YYYY_SEP1 || i == MM_DD_YYYY_SEP2 ) ) {
                if ( date[ i ] != '/' && date[ i ] != '-' ) {
                    exit( DATE_ERROR );
                }
            } else if ( !isDigit( date[ i ] ) ) {
                exit( DATE_ERROR );
            }
        }

        // Extract month, day, and year
        month = ( date[ 0 ] - '0' ) * TWODIGIT_YEAR + ( date[ 1 ] - '0');
        day = ( date[ DAY_START_INDEX ] - '0') * TWODIGIT_YEAR + ( date[ DAY_START_INDEX + 1 ] - '0' );
        char yearStr[ FULLYEARSTR_SIZE ];

        // Handle 4-digit year
        if ( isDigit( date[ YEAR_START_INDEX ] ) && isDigit( date[ YEAR_START_INDEX + YEAR_ONES_PLACE ] ) ) {
            year = ( date[ YEAR_START_INDEX ] - '0' ) * THOUSANDS_DIGIT +
                   ( date[ YEAR_START_INDEX + 1 ] - '0' ) * HUNDREDS_DIGIT +
                   ( date[ YEAR_START_INDEX + YEAR_TENS_PLACE ] - '0' ) * TENS_DIGIT +
                   ( date[ YEAR_START_INDEX + YEAR_ONES_PLACE ] - '0' );
        }
        // Handle 2-digit year
        else if ( isDigit( date[ YEAR_START_INDEX ] ) && isDigit( date[ YEAR_START_INDEX + 1 ] ) ) {
            char shortYear[ SHORT_YEAR ] = { date[ YEAR_START_INDEX ], date[ YEAR_START_INDEX + 1 ] };
            expandTwoDigitYear( shortYear, yearStr );
            year = ( yearStr[0] - '0' ) * THOUSANDS_DIGIT +
                   ( yearStr[1] - '0' ) * HUNDREDS_DIGIT +
                   ( yearStr[ YEAR_TENS_PLACE ] - '0' ) * TENS_DIGIT +
                   ( yearStr[ YEAR_ONES_PLACE ] - '0' );
        } 
        else {
            exit( DATE_ERROR );
        }

        // Validate year
        if ( year < YEAR_MIN || year > YEAR_MAX ) {
            exit( DATE_ERROR_UNREALISTIC );
        }

        // Validate month
        if ( month < 1 || month > MONTH_MAX ) {
            exit( DATE_ERROR_UNREALISTIC );
        }

        // Validate day
        if ( day < 1 || day > getMaxDays( month, year ) ) {
            exit( DATE_ERROR_UNREALISTIC );
        }

        temp[ 0 ] = date[ YEAR_START_INDEX ];
        temp[ 1 ] = date[ YEAR_START_INDEX + 1 ];
        temp[ YEAR_TENS_PLACE ] = date[ YEAR_START_INDEX + YEAR_TENS_PLACE ];
        temp[ YEAR_ONES_PLACE ] = date[ YEAR_START_INDEX + YEAR_ONES_PLACE ];
        temp[ YYYY_MM_DD_SEP1 ] = '-';
        temp[ MONTH_DIGIT_FIRST ] = date[ 0 ];
        temp[ MONTH_DIGIT_SECOND ] = date[ 1 ];
        temp[ YYYY_MM_DD_SEP2 ] = '-';
        temp[ DAY_DIGIT_FIRST ] = date[ DAY_START_INDEX ];
        temp[ DAY_DIGIT_SECOND ] = date[ DAY_START_INDEX + 1 ];
        temp[ STRING_TERMINATE ] = '\0';

        for ( int i = 0; i <= DATE_LEN; i++ ) {
            date[ i ] = temp[ i ];
        }

        return true;
    }

    exit( DATE_ERROR ); 
}

/**
    Checks if given date is in valid format or not. Exits with DATE_ERROR otherwise.

    @param date array to be checked if formatted or not
*/
void checkDate( char date[ FIELD_MAX + 1 ] ) 
{
    if ( !formatDate( date ) ) {
        exit( DATE_ERROR ); 
    }
}