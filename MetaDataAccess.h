#ifndef META_DATA_ACCESS_H
#define META_DATA_ACCESS_H

// header files
#include <stdio.h>
#include "StringUtils.h"

// global constants

typedef enum { MD_FILE_ACCESS_ERR = 3,
							 MD_CORRUPT_DESCRIPTOR_ERR,
							 OPCMD_ACCESS_ERR,
							 CORRUPT_OPCMD_LETTER_ERR,
							 CORRUPT_OPCMD_NAME_ERR,
							 CORRUPT_OPCMD_VALUE_ERR,
							 UNBALANCED_START_END_ERR,
							 COMPLETE_OPCMD_FOUND_MSG,
							 LAST_OPCMD_FOUND_MSG } opCodeMessages;
							 
typedef enum 
  { 
	
	PROCESSOR = 24, 
	IO
	
	} opCodeCycleType; 

typedef struct OpCodeType
	{
	
	char opLtr;
	char opName[ 100 ]; // length of op name - 99 characters
	int opValue;
	int runTime;
	
	int masterProcess; 
	
	struct OpCodeType* next;
	
	} OpCodeType;
	
// function prototypes
int getOpCodes( char* fileName, OpCodeType** opCodeDataHead );
void displayMetaData( OpCodeType* localPtr );
void displayMetaDataError( int code );
OpCodeType* clearMetaDataList( OpCodeType* localPtr );

int getOpCommand( FILE* filePtr, OpCodeType* intData );
int updateStartCount( int count, char* opString );
int updateEndCount( int count, char* opString );
OpCodeType* addNode( OpCodeType* localPtr, OpCodeType* newNode );
Boolean isDigit( char testChar );
Boolean checkOpString( char* testStr );

#endif // META_DATA_ACCESS_H