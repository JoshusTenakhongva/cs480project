// header files

#include <stdio.h>
#include <math.h>
#include "MemoryManagement.h"
#include "StringUtils.h"
#include "MetaDataAccess.h"

/*******************
* Table of Contents
***********************************
*
*     allocateMemory
*     separateMemoryAddress
*     accessMemory
*     checkMemoryAddress
*     clearMemory
*     throwSegFault
*     clearMMU
*/

Boolean checkSegfault( Memory_management_unit* mmu, Memory_address* address,
                                                      Boolean operationFlag )
  {
  /* initialize variables */

  // Search through the memoryAddresses to see if we find a match
  Memory_address* searchedAddress = memoryAddressSearch( mmu, address );

  // Check if the operation is allocating
  if( operationFlag == ALLOCATE )
    {

    // return segfault if the address was found
    return searchedAddress != NOT_FOUND;
    }

  // Otherwise, assume the operation is accessing
  else // ( operationFlag == ACCESS )
    {

    // return segfault true if the address was not found
    return searchedAddress == NOT_FOUND;
    }
  }

/*
* Desc:
* Input:
* Postcondition:
*/
Boolean checkMemoryAvailable( Memory_management_unit* mmu,
                                                    Memory_address* allocMem )
  {

  return mmu->memAvailable >= allocMem->memoryOffset;
  }


/*
* Desc:
* Input:
* Postcondition:
*/
Memory_address* memoryAddressSearch( Memory_management_unit* mmu,
                                              Memory_address* addressSearch )
  {

  return memoryAddressSearchHelper( mmu->head, addressSearch );
  }

/*
* Desc:
* Input:
* Postcondition:
*/
Memory_address* memoryAddressSearchHelper( Memory_address* runningNode,
                                               Memory_address* addressSearch )
  {

  // Check if the runningNode is the last node or if there were never nodes
  if( runningNode == NULL )
    {

    return NULL;
    }

  // Check if the runningNode's base is equal to the address search
  if( runningNode->memoryBase == addressSearch->memoryBase )
    {

    // Return this address
    return runningNode;
    }

  // Otherwise, return the incremented recusion
  return memoryAddressSearchHelper( runningNode->next, addressSearch );
  }

/*
* Desc:
*     Creates a memory address based on the information from the opCode input.
*     This address is not added to the MMU yet. It is just created.

* Input:
*     @opCode - The opCode from which we will create a memory address from

* Postcondition:
*     @return - We will return a memory address with the information from the
*       opCode;
*/
Memory_address* createMemoryAddress( OpCodeType* opCode )
  {

  /* Initialize variables */
  // Create the memory address we're going to return
  Memory_address* memoryUnit = ( Memory_address* )
                                            malloc( sizeof( Memory_address ) );

  // Save the address the opCode wants to allocate to
  int address = opCode->opValue;

  // Chop off the kilobyte allocation and save it
  memoryUnit->memoryOffset = address % REMOVE_KB;
  address = ( int ) floor( address / REMOVE_KB );

  // Amputate the megabyte offset and save it
  memoryUnit->memoryBase = address % REMOVE_MB;
  address = ( int ) floor( address / REMOVE_MB );

  // Save the left over numbers as the identifier
  memoryUnit->memoryID = address;

  memoryUnit->processWithAccess = opCode->masterProcess;

  return memoryUnit;
  }

/*
* Desc:
* Input:
* Postcondition:
*/
Boolean checkAccess( Memory_address* memoryAddress, OpCodeType* opCode )
  {

  return memoryAddress->processWithAccess == opCode->masterProcess;

  }

/*
* Desc:
* Input:
* Postcondition:
*/
void stringifyMemoryAddress( Memory_address* memoryAddress, char* string )
  {

  /* initialize variables */
  char tempString[ MAX_STR_LEN ];

  // Convert the memory ID to a string
  sprintf( tempString, "%d/", memoryAddress->memoryID );

  //printf( "%s", tempString );
  // Add the string to the string we plan to print
  concatenateString( string, tempString );

  printf( "%s", string );

  // Convert the memory base to a string
  sprintf( tempString, "%d/", memoryAddress->memoryBase );

  concatenateString( string, tempString );

  sprintf( tempString, "%d", memoryAddress->memoryOffset );

  concatenateString( string, tempString );
  }

/*
* Desc:
* Input:
* Postcondition:
*/
int allocateMemory( Memory_management_unit* mmu, OpCodeType* opCode )
  {

  /* Initialize variables */
  // Create a memory address for the opCode
  Memory_address* opCodeAddress = createMemoryAddress( opCode );

  /* Check if the memory allocation is valid */
  // Check if the allocation is possible with our memory size
  if ( !checkMemoryAvailable( mmu, opCodeAddress ) )
    {

    return ALLOCATE_FAIL;
    }

  // Check if the allocation segfaults
  if( checkSegfault( mmu, opCodeAddress, ALLOCATE ) )
    {

    printf( "bitch segfaulted\n" );
    return ALLOCATE_FAIL;
    }

  /* assume that we are free to continue allocating */

  // Check if the head of the MMU is null
  if( mmu->head == NULL )
    {

    // If so, set the current memory allocation as the head
    mmu->head = opCodeAddress;
    mmu->currentUnit = opCodeAddress;
    }
  // Otherwise
  else
    {

    // Set the current allocation as the next in the list
    mmu->currentUnit->next = opCodeAddress;
    }

  return ALLOCATE_SUCCESS;
  }

/*
* Desc:
* Input:
* Postcondition:
*/
void clearMemoryAddress( Memory_address* address )
  {

  /* Initailize variables */
  Memory_address* nextAddress;

  // Check if the address is not null
  if( address != NULL )
    {

    // Save the next address
    nextAddress = address->next;

    // Clear this address
    free( address );

    // recurse to the next address
    clearMemoryAddress( nextAddress );
    }

  }

/*
* Desc:
* Input:
* Postcondition:
*/
void clearMemory( Memory_management_unit* mmu )
  {

  clearMemoryAddress( mmu->head );

  free( mmu );
  }
