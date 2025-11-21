/**
    @file data.h
    @author Jayani Sivakumar ( jsivaku )

    This program contains function declarations to check observations, validate integers and floats,
    format and check dates, and handle data types according to the map.
*/
#include "input.h"
#include <stdbool.h>
#include <string.h>

/** total size of the fullYearStr */
#define FULLYEARSTR_SIZE 5

/** number of characters used to store a two-digit year. */
#define SHORT_YEAR 2

/**
    This function checks and processes the obervation. Reads and processes a
    row of data according to the column map.

    @param len number of columns
    @param map array containing column type map
    @return true if a row was successfully processed, false if EOF was reached
 */
bool checkObservation( int len, int map[ COLUMN_MAX ] );

/**
    Checks if a date string is valid and in the correct format.

    @param date date string to check.
 */
void checkDate( char date[ FIELD_MAX + 1 ] );

/**
    Checks if a string represents a valid integer.

    @param str string to validate
    @return true if string is a valid integer, else false
 */
bool isValidInteger( char str[ ] );

/**
    Checks if a string represents a valid float.

    @param str string to validate
    @return true if string is a valid float, else false
 */
bool isValidFloat( char str[ ] );

/**
    Formats a date string to YYYY-MM-DD, MM/DD/YYYY or MM-DD-YYYY format.

    @param date date string to format
    @return true if date was successfully formatted, else false
 */
bool formatDate( char date[ ] );

/**
    Checks if given date is in valid format or not. Exits with DATE_ERROR otherwise.

    @param date array to be checked if formatted or not
 */
void checkDate( char date[ FIELD_MAX + 1 ] );

/**
    Checks if a given character is a numeric digit (0-9).

    @param ch character to check.
    @return true if the character is a digit (0-9), otherwise 0.
*/
int isDigit( char ch );

/**
    Checks if a year is a leap year.

    @param year the year to check
    @return true if the year is a leap year, otherwise 0.
*/
int isLeapYear( int year );

/** 
    Retrieves the maximum number of days in a given month for a specific year.

    @param month The month
    @param year The year
    @return The number of days in the given month.	
*/
int getMaxDays( int month, int year );

/**
    Converts a two-digit year into a four-digit year.

    @param yearStr character array containing a two-digit year.
    @param fullYearStr character array where the expanded four-digit year.
*/
void expandTwoDigitYear( char yearStr[ SHORT_YEAR ], char fullYearStr[ FULLYEARSTR_SIZE ] );