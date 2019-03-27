// header files
#include "MetaDataAccess.h"

int getOpCodes( char* fileName, OpCodeType** opCodeDataHead )
	{
	
	// initialize function/variables
	
		// initialize read only constant
		const char READ_ONLY_FLAG[] = "r";
		
		// initialize start and end counts for balanced app operations
		int startCount = 0, endCount = 0;
		
		// initialize local head pointer to null
		OpCodeType* localHeadPtr = NULL;
		
		// initialize variables
		int accessResult;
		char dataBuffer[ MAX_STR_LEN ];
		OpCodeType* newNodePtr;
		FILE* fileAccessPtr;
		
	// initialize op code data pointer in case of return error
	*opCodeDataHead = NULL;
	
	// open file for reading
	 // function: fopen
	fileAccessPtr = fopen( fileName, READ_ONLY_FLAG );
	 
	// check for file open failure
	if( fileAccessPtr == NULL )
		{
	
		// return file access error
		return MD_FILE_ACCESS_ERR;
		}
		
	// check first line for correct leader
	 // function: getLineTo, compareString
	if( getLineTo( fileAccessPtr, MAX_STR_LEN, COLON, dataBuffer, 
																					IGNORE_LEADING_WS ) != NO_ERR 
			|| compareString( dataBuffer, "Start Program Meta-Data Code" ) 
																														!= STR_EQ )
		
		{
		// close file
		 // function: fclose
		fclose( fileAccessPtr );
		 
		// return corrupt descriptor error 
		return MD_CORRUPT_DESCRIPTOR_ERR;
		}
		
	// alloocate memory for the temoprary data structure
	 // function: malloc
	newNodePtr = ( OpCodeType* ) malloc( sizeof( OpCodeType ) ) ;
	 
	// get the first op command
	 // function: getOpCommand'
	accessResult = getOpCommand( fileAccessPtr, newNodePtr );
	 
	// get start and end counts for later comparison
	 // function: updateStartCount, updateEndCount
	startCount = updateStartCount( startCount, newNodePtr->opName );
	endCount = updateEndCount( endCount, newNodePtr->opName );
	 
	// check for failure of first complete op command
	if( accessResult != COMPLETE_OPCMD_FOUND_MSG )
		{
	
		// close file
		 // function: fclose
		fclose( fileAccessPtr );
		 
		// clear data from the structure list
		 // function: clearMetaDataList 
		*opCodeDataHead = clearMetaDataList( localHeadPtr );
		 
		// free temporary structure memory
		 // function: free 
		free( newNodePtr );
		 
		// return result of operation
		return accessResult;
		}
		
	// loop across all remaining op commmands
	//		(while complete op commands are found)
	while( accessResult == COMPLETE_OPCMD_FOUND_MSG )
		{
	
		// add the new op command to the linked list
		 // function: addNode
		localHeadPtr = addNode( localHeadPtr, newNodePtr );
		 
		// get a new op command
		 // function: getOpCommand 
		accessResult = getOpCommand( fileAccessPtr, newNodePtr );
		 
		// update start and end counts for later comparison
		 // function: updateStartCount, updateEndCount
		startCount = updateStartCount( startCount, newNodePtr->opName );
		endCount = updateEndCount( endCount, newNodePtr->opName );
		}
		 
	// after loop completion, check for last op command found
	if( accessResult == LAST_OPCMD_FOUND_MSG )
		{
	
		// check for start and end op code counts equal
		if( startCount == endCount )
			{
		
			// add the last node to the linked list
			 // function: addNode
			localHeadPtr = addNode( localHeadPtr, newNodePtr );
			 
			// set access result to no error for later operation
			accessResult = NO_ERR;
			
			// check last line for incorrect and descriptor
			 // function: getLineTo, compareString 
			if( getLineTo( fileAccessPtr, MAX_STR_LEN, PERIOD, dataBuffer, 
																		IGNORE_LEADING_WS ) != NO_ERR 
					|| compareString( dataBuffer, "End Program Meta-Data Code" ) 
																														!= STR_EQ )
				{
					
				// set access result to corrupted descriptor error
				accessResult = MD_CORRUPT_DESCRIPTOR_ERR;
				}
			}
			
		// otherwise, assume start count not equal to end count
		else
			{
		
			// set access result to unbalance start/end error
			accessResult = UNBALANCED_START_END_ERR;
			}
		}
		
	// check for any errors found( not no error )
	if( accessResult != NO_ERR )
		{
	
		// clear the op command list 
		 // function: clearMetaDataList
		localHeadPtr = clearMetaDataList( localHeadPtr );
		}
		 
	// close access file
	 // function: fclose
	fclose( fileAccessPtr );
	 
	// release temporary structure memory
	 // function: free
	free( newNodePtr );
	 
	// assign temporary local head pointer to parameter return pointer
	*opCodeDataHead = localHeadPtr;
	
	// report function operation for step four 
		
	return 0;
	}
	
	
	
	
	
	
	
	
int getOpCommand( FILE* filePtr, OpCodeType* inData )
	{
		
	// initialize function/variables
	
		// initialize local constants - max op name and op value lengths ( 10 & 9 )
		const int MAX_OP_NAME_LENGTH = 10; 
		const int MAX_OP_VALUE_LENGTH = 9;
		
		// initialize integer buffer value to zero
		int intBuffer = 0;
		
		// initialize source and destination indices to zero 
		int sourceIndex = 0, destIndex = 0;
		
		// initialize other variales
		int accessResult;
		char strBuffer[ STD_STR_LEN ];
		
	// get whole op comand as a string
	 // function: getLineTo
	accessResult = getLineTo( filePtr, STD_STR_LEN, SEMICOLON, strBuffer, 
																											IGNORE_LEADING_WS );
	 
	// check for successful access
	if( accessResult == NO_ERR )
		{
	
		// assign op command letter to struct component
		inData->opLtr = strBuffer[ sourceIndex ];
		}
		
	// otherwise, assume unsuccessful access
	else	
		{
		
		// set pointer to data structure to null
		inData = NULL;
		
		// return op command access failure
		return OPCMD_ACCESS_ERR;
		}
		
	// verify op command letter
	switch( inData->opLtr )
		{
	
		// check for all correct cases
		case 'S':
		case 'A':
		case 'P': 
		case 'M':
		case 'I':
		case 'O':
			break;
		
		// otherwise, assume not a correct case
		default:
		
			// set op command pointer to null
			inData = NULL;
			
			// return op command letter error
			return CORRUPT_OPCMD_LETTER_ERR;
		}
			
	// loop until left paren found
	while( sourceIndex < STD_STR_LEN && strBuffer[ sourceIndex ] != LEFT_PAREN )
		{
	
		// increment source index 
		sourceIndex++;
		}
		
	// skip left paren element, increment source index  
	sourceIndex++;
	
	// set op command text 
	// loop until right paren found 
	while( sourceIndex < STD_STR_LEN && destIndex < MAX_OP_NAME_LENGTH 
																&& strBuffer[ sourceIndex ] != RIGHT_PAREN )
		{
	
		// acquire letter
		inData->opName[ destIndex ] = strBuffer[ sourceIndex ];
		
		// increment source and destinatino indices
		destIndex++; sourceIndex++;
		
		// set and end/null character to current end of string 
		inData->opName[ destIndex ] = NULL_CHAR;
		}
		
	// check for incorrect op string
	 // function: checkOpString
	if( checkOpString( inData->opName ) == False )
		{
	 
		// set struct to null
		inData = NULL;
		
		// return corrupt on command found 
		return CORRUPT_OPCMD_NAME_ERR;
		}
		
	// skip right paren element - increment source index, reset dest index
	sourceIndex++;
	destIndex = 0;
	
	// get integer value
	// loop while digits are found
	 // function: isDigit
	while( sourceIndex < STD_STR_LEN 
			&& destIndex < MAX_OP_VALUE_LENGTH 
			&& isDigit( strBuffer[ sourceIndex ] ) == True )
		{
	 
		// multiply current buffer by ten
		intBuffer *= 10;
		
		// add next integer value, converted frmo character to integer
		intBuffer += ( int )( strBuffer[ sourceIndex ] - '0' );
		
		// increment indices
		destIndex++; sourceIndex++;
		}
		
	// check for loop overrn failure, check specified lengths
	if( sourceIndex == STD_STR_LEN || destIndex == MAX_OP_VALUE_LENGTH )
		{
	
		// set struct to null 
		inData = NULL;
		
		// return corrupt op command value found 
		return CORRUPT_OPCMD_VALUE_ERR;
		}
		
	// set value to data structure component
	inData->opValue = intBuffer;
	
	// check for last op command *S(end)0"
	 // function: compareString 
	if( inData->opLtr == 'S' 
										&& compareString( inData->opName, "end" ) == STR_EQ )
		{
	 
		// return last op command found message 
		return LAST_OPCMD_FOUND_MSG;
		}
		
	// return complete op command found message
		
	return COMPLETE_OPCMD_FOUND_MSG;
	}
	
	
	
	
	
//==================================================	

	
	
	
	
int updateStartCount( int count, char* opString )
	{
	// check for "start" in op string
	 // function: compareString 
	if ( compareString( opString, "start" ) == STR_EQ )
		{
	 
		// return incremented start count 
		return count + 1;
		}
		
	// return unchanged start count 		
	return count;
	}
	
int updateEndCount( int count, char* opString )
	{
		
	// check for "end" in op string
	 // function: compareString 
	if( compareString( opString, "end" ) == STR_EQ )
		{
	 
		// return incremented end count 
		return count + 1;
		}
		
	// return unchaged end count 
	return count;
	}
	
OpCodeType* addNode( OpCodeType* localPtr, OpCodeType* newNode )
	{
		
	// check for local pointer assigned to null 
	if( localPtr == NULL )
		{
	
		// access memory for new link/node
		 // function: malloc
		localPtr = ( OpCodeType* ) malloc( sizeof( OpCodeType ) );
		 
		// assign all three values to newly created node
		// assign next pointer to null
		 // function: copyString 
		localPtr->opLtr = newNode->opLtr;
		copyString( localPtr->opName, newNode->opName );
		localPtr->opValue = newNode->opValue;
		localPtr->next = NULL;
		 
		// return current local pointer
		return localPtr;
		}
		
	// assume end of list not found yet
	// assign recursive function to current's next link/node
	 // function: addNode
	localPtr->next = addNode( localPtr->next, newNode );
	 
	// return current local pointer		
	return localPtr;
	}
	
Boolean checkOpString( char* testStr )
	{
	// check for all possible op namespace
	 // function: compareString 
	if( compareString( testStr, "access" ) 				== STR_EQ 
			|| compareString( testStr, "allocate" ) 		== STR_EQ
			|| compareString( testStr, "end" ) 					== STR_EQ
			|| compareString( testStr, "hard drive" ) 	== STR_EQ
			|| compareString( testStr, "keyboard" ) 		== STR_EQ
			|| compareString( testStr, "printer" ) 			== STR_EQ
			|| compareString( testStr, "monitor" ) 			== STR_EQ
			|| compareString( testStr, "run" ) 					== STR_EQ
			|| compareString( testStr, "start" ) 				== STR_EQ )
		{
	 
		// return found - true
		return True;
		}
		
	// return failure - false		
	return False;
	}
	
Boolean isDigit( char testChar )
	{
		
	// check for test character between characters '0' - '9' inclusive
	if( testChar >= '0' && testChar <= '9' )
		{
	
		// return true
		return True;
		}
		
	// otherwise, assume character is not digit, return false		
	return False;
	}
	
void displayMetaData( OpCodeType* localPtr )
	{
		
	// display title, with underlien
	 // function: printf
	printf( "\nMeta-Data File Display\n" );
	printf( "=================================\n" );
	 
	// loop to end of linked list
	while( localPtr != NULL )
		{
	
		// print op code letter 
		 // function: printf
		printf( "Op code letter: %c\n", localPtr->opLtr );
		 
		// print op code name
		 // function: printf
		printf( "Op code name  : %s\n", localPtr->opName );
		 
		// print op code value
		 // function: printf
		printf( "Op code value : %d\n\n", localPtr->opValue );
		
		// assign local pointer to next mode
		localPtr = localPtr->next;
		}
		
	// no return, no data setting
	}
	
void displayMetaDataError( int code )
	{
		
	// create string error list, 10 items, max 35 letters
	// includes 3 errors from StringManipError
	char errList[ 10 ][ 35 ] = 
											{ "No Error", 
												"Incomplete File Error", 
												"Input Buffer Overrun", 
												"MD File Access Error", 
												"MD Corrupt Descriptor Error", 
												"Op Command Access Error", 
												"Corrupt Op Command Letter Error", 
												"Corrupt Op Command Value Error", 
												"Unbalanced Start-End Code Error" };
	
	// display error to monitor with selected error string 
	 // function: printf
	printf( "\nFATAL ERROR: %s, Program aborted\n", errList[ code ] );
	
	// no return, no data setting 
	}
	
OpCodeType* clearMetaDataList( OpCodeType* localPtr )
	{
		
	// check for local pointer not set to null (list not empty)
	if( localPtr != NULL )
		{
	
		// check for local pointer's next node not null
		if( localPtr->next != NULL )
			{
		
			// call recursive function with next pointer
			 // function: clearMetaDataList
			clearMetaDataList( localPtr->next );
			}
			 
		// after recursive call, release memory to OS
		 // function: free
		free( localPtr );
		}
		
	return NULL;
	}