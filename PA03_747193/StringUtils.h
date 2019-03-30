// Pre-compiler directive
#ifndef STRING_UTILS_H
#define STRING_UTILS_H

// Header files
#include <stdio.h>
#include <stdlib.h>

// Create global constants across files
#define MAX_STR_LEN 200
#define STD_STR_LEN 60
#define SUBSTRING_NOT_FOUND -101
#define MAX_CHAR_LIMIT 200
#define STR_EQ 0

#define SPACE ' '
#define NULL_CHAR '\0'
#define COLON ':'
#define PERIOD '.'
#define SEMICOLON ';'
#define LEFT_PAREN '('
#define RIGHT_PAREN ')'
#define EMPTY_STRING "  "

// These could be wrong 
#define IGNORE_LEADING_WS True
#define ACCEPT_LEADING_WS False

typedef enum { False, True } Boolean;
typedef enum { NO_ERR, INCOMPLETE_FILE_ERR, INPUT_BUFFER_OVERRUN_ERR }
																												StringManipCode;
																												
int getStringLength( char* testStr );
void copyString( char* destination, char* source );
void concatenateString( char* destination, char* source );
int compareString( char* oneString, char* twoString );
void getSubString( char* destString, char* sourceString, int startIndex, int endIndex );
int compareString( char* oneString, char* twoString );
int findSubString( char* testString, char* searchString );
void setStrToLowerCase( char* destString, char* sourceString );
char setCharToLowerCase( char testChar );
int getLineTo( FILE* filePtr, int bufferSize, char stopChar, char* buffer, 
																							Boolean omitLeadingWhiteSpace );
Boolean isEndOfFile( FILE* filePtr );

#endif // STRING_UTILS_H