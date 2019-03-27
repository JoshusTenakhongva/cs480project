// Header files
#include <stdio.h>
#include <stdlib.h>
#include "StringUtils.h"
#include "simtimer.h"

/*
Function name: getStringLength
Algorithm: Find the lsength of the string
Precondition: given a C-style string with null character at end
Postcondition: return the number of characters from the beginning to the end
Exceptions: none
Note: Limit test loop to maximum characters for safety
*/

int getStringLength( char* testStr )
	{

	// initialize variables
	int length = 0;
	char* currentChar = testStr;

	// Loop through the string we are testing
	while ( *currentChar != NULL_CHAR && length < MAX_CHAR_LIMIT )
		{

		// Add one to the length
		length++;

		// Increment the
		currentChar++;
		}
	// end loop

	return length;
	}


/*
Function name: copyString
Algorithm: Create a copy of another string
Precondition: given a C-style string with null character at end
Postcondition: return a clone of the parameter string
Exceptions: none
Note: Limit test loop to maximum characters for safety
*/

void copyString( char* destination, char* source )
	{
	// initialize functions/variables
	int index = 0;

	// loop until null character is found in source string
	// 		loop limits to MAX_STR_LEN
	while( index < MAX_STR_LEN && source[ index ] != NULL_CHAR )
		{
		// Assign source character to destination element
		destination[ index ] = source[ index ];

		// increment index
		index++;

		// assign null character to next destination element
		destination[ index ] = NULL_CHAR;

		}
	// end loop

	// No return required
	}

void concatenateString( char* destination, char* source )
	{

	// Initialize function/variables

		// set destination index to length of destination string
		int destIndex = getStringLength( destination );

		// set source index to zero
		int sourceIndex = 0;

	// loop to end of source index (null character)
	// 		loop limited to MAX_STR_LEN
	while( sourceIndex < MAX_STR_LEN && source[ sourceIndex ] != NULL_CHAR )
		{

		// assign source character to destinatino at destination index
		destination[ destIndex ] = source[ sourceIndex ];

		// increment source and destination indices
		sourceIndex++; destIndex++;

		// assign null character to next destination element
		destination[ destIndex ] = NULL_CHAR;

		}
	// end loop

	}

int compareString( char* oneStr, char* otherStr )
	{
	// initialize function/variables

		// initialize index to zero
		int index = 0;

		// initialize other variables
		int difference;

	// loop; to end of one of the two strings
	// 	loop limited to MAX_STR_LEN
	while( index < MAX_STR_LEN
				&& oneStr[ index ] != NULL_CHAR
				&& otherStr[ index ] != NULL_CHAR )
		{

		// find the difference between the currently aligned characters
		difference = oneStr[ index ] - otherStr[ index ];

		// check for non-zero difference
		if( difference != 0 )
			{

			// return non-zero difference
			return difference;
			}

		// increment index
		index++;
		}

	// end loop

	// assume string are equal at this point, reutrn string length difference
		// function: getStringLength
	return ( getStringLength( oneStr ) - getStringLength( otherStr ) );

	}

void getSubString( char* destStr, char* sourceStr, int startIndex,
                                                          int endIndex )
  {
	// initialize function/variables

		// set length of source string
		int sourceStrLen = getStringLength( sourceStr );

		// initialize destination index to zero
		int destIndex = 0;

		// initialize source index to start index parameter
		int sourceIndex = startIndex;

		// create pointer to temp string
		char* tempSourceStr;

	// check for indices within limits
	if ( startIndex >= 0 && startIndex <= endIndex && endIndex < sourceStrLen )
		{

		// create temporary string, copy source string to it
			// function: malloc, copyString
		tempSourceStr = ( char* ) malloc( sourceStrLen + 1 );
		copyString( tempSourceStr, sourceStr );

		// loop across requrested substring ( indices)
		while( sourceIndex <= endIndex )
			{

			// assign source character to destination element
			destStr[ destIndex ] = tempSourceStr[ sourceIndex ];

			// increment indices
			destIndex++; sourceIndex++;

			// add null character to next destination string element
			destStr[ destIndex ] = NULL_CHAR;
			}

		// end loop

		// release memory used for temp source string
			// function: free
		free( tempSourceStr );
		}

	// otherwise, assume indices not in limits
	else
		{

		// create empty string with null character
		destStr[ 0 ] = NULL_CHAR;

		}
	}

/*
Function name: findSubString
Algorithm: Linear search for given substring within a given test string
Precondition: given a C-style test string, having a null character ('\0')
							at end of string, and given search string with a null character
							('\n') at the end of that string
Postcondition: index of substring location returned, or
							 SUBSTRING_NOT_FOUND constant is returned
Exceptions: none
Note: none
*/

int findSubString( char* testStr, char* searchSubStr )
	{
	// initialize function/variables

		// initialize test string length
			// function: getStringLength
		int testStrLen = getStringLength( testStr );

		// initialize master index - location of sub string start point
		int masterIndex = 0;

		// initialize other variables
		int searchIndex, internalIndex;

	// loop across test string
	while( masterIndex < testStrLen )
		{

		// set internal loop index to current test string index
		internalIndex = masterIndex;

		// set internal search index to zero
		searchIndex = 0;

		// loop to end of test string
		// 		while test string/sub string character are the same
		while( internalIndex <= testStrLen
					 && testStr[ internalIndex ] == searchSubStr[ searchIndex ] )
			{

			// increment test string, substring indices
			internalIndex++; searchIndex++;

			// check for end of substring( search completed )
			if ( searchSubStr[ searchIndex ] == NULL_CHAR )
				{

				// return current test string index
				return masterIndex;
				}

		// end loop
			}

		// increment current test string index
		masterIndex++;
		}

	// end loop

	// assume tests have failed at this point, return SUBSTRING_NOT_FOUND
	return SUBSTRING_NOT_FOUND;
	}

/*
Function Name: setStringToLowercase
Algorithm: Iterates through string, setting all uppercase letters to lowercase
*/

void setStrToLowerCase( char* destStr, char* sourceStr )
	{
	// initialize function/variables

		// create temporary source string
			// function: getStringLength, malloc
	int strLen = getStringLength( sourceStr );
	char* tempStr = ( char* ) malloc( strLen + 1 );

		// initialize source string index to zero
	int index = 0;

	// copy source string to temoprary string
		// function: copyString
	copyString( tempStr, sourceStr );

	// loop to end of temp/source string
	//		loop limited to MAX_STR_LEN
	while( index < MAX_STR_LEN && tempStr[ index ] != NULL_CHAR )
		{

		// change letter to lower case as needed and assign
		// 	to destination string
			// function: setCharToLowerCase
		destStr[ index ] = setCharToLowerCase( tempStr[ index ] );

		// incremet index
		index++;

		// add null character to next destination string element
		destStr[ index ] = NULL_CHAR;
		}

	// end loop

	// release temp string memory
		// function: free
	free( tempStr );

	}

char setCharToLowerCase( char testChar )
	{
	// initialize function/variables

		// none

	// check for character between 'A' and 'Z' inclusive
	if( testChar >= 'A' && testChar <= 'Z' )
		{

		// convert upper case letter to lower case
		testChar = ( char ) ( testChar - 'A' + 'a' );
		}

	// reutrn character
	return testChar;
	}

int getLineTo( FILE* filePtr, int bufferSize, char stopChar, char* buffer,
																							Boolean omitLeadingWhiteSpace )
 {
	// initialize function/variables

		// initialize character index
		int charIndex = 0;

		// initialize status return to NO_ERR
		int statusReturn = NO_ERR;

		// initialize bugger size available flag to true
		Boolean bufferSizeAvailable = True;

		// initialize other variables
		int charAsInt;

	// get the first character
		// function: fgetc
	charAsInt = fgetc( filePtr );

	// use a loop to consume leading white space, if flagged
	while( omitLeadingWhiteSpace == True
					&& charAsInt != ( int ) stopChar
				  && charIndex < bufferSize
					&& charAsInt <= ( int ) SPACE )
		{

		// get next character (as integer)
			// function: fgetc
		charAsInt = fgetc( filePtr );
		}
	// end loop

	// capture string
	// loop while character is not stop character and bugger size is available
	while( charAsInt != ( int ) stopChar && bufferSizeAvailable == True )
		{

		// check for input failure
			// function: isEndOfFile
		if( isEndOfFile( filePtr ) == True )
			{

			// return incomplete file error
			return INCOMPLETE_FILE_ERR;
			}

		// check for usable (printable) character
		if ( charAsInt >= ( int ) SPACE )
			{

			// assign input character to buffer string
			buffer[ charIndex ] = ( char ) charAsInt;

			// increment index
			charIndex++;
			}

		// set next buffer element to null character
		buffer[ charIndex ] = NULL_CHAR;

		// check for not at end of buffer size
		if ( charIndex < bufferSize - 1 )
			{

			// get a new character
			charAsInt = fgetc( filePtr );
			}

		// otherwise, assume end of buffer size
		else
			{

			// set buffer size Boolean to false to end loop
			bufferSizeAvailable = False;

			// set status return to buffer overrun error
			statusReturn = INPUT_BUFFER_OVERRUN_ERR;
			}
		}
	// end loop

	// return status data
	return statusReturn;

 }

Boolean isEndOfFile( FILE* filePtr )
 {
	// initialize function/variables

	// check for feof end of file response
		// function: feof
	if ( feof( filePtr ) != 0 )
		{

		// return true
		return True;
		}

	// assume test failed at this point, return false
	return False;
 }
