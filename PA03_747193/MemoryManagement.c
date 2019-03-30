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

Boolean checkSegfault( Memory_management_unit* mmu, Memory_address* address )
  {

  /* initialize variables */
  return True;

  }

Memory_address* memoryAddressSearch( Memory_management_unit* mmu,
                                               Memory_address* addressSearch )
  {

  return NULL;
  }

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

Boolean checkAccess()
  {

  return False;
  }

void stringifyMemoryAddress( Memory_address* memoryAddress, char* string )
  {

  /* initialize variables */
  char tempString[ MAX_STR_LEN ];

  // Convert the memory ID to a string
  sprintf( tempString, "%d/", memoryAddress->memoryID );

  // Add the string to the string we plan to print
  concatenateString( string, tempString );

  // Convert the memory base to a string
  sprintf( tempString, "%d/", memoryAddress->memoryBase );

  concatenateString( string, tempString );

  sprintf( tempString, "%d", memoryAddress->memoryOffset );

  concatenateString( string, tempString );
  }

Boolean allocateMemory( Memory_management_unit* mmu, OpCodeType* opCode )
  {

  /* Initialize variables */
  // Create a memory address for the opCode
  Memory_address* opCodeAddress = createMemoryAddress( opCode );

  return False;

  /* Check if the memory allocation is valid */
  // Check if the allocation is possible with our memory size

  // Check if the allocation segfaults


  // Check if the head of the MMU is null

    // If so, set the current memory allocation as the head

  // Otherwise

    // Set the current allocation as the next in the list

  }

void clearMemoryAddress( Memory_address* address )
  {

  //

  }
