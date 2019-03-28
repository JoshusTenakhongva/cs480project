// header files

#include <stdio.h>
#include <math.h>
#include "MemoryManagement.h"
#include "StringUtils.h"

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
*/

void allocateMemory( Memory_management_unit mmu, opCodeType opCode )
  {

  // Initialize variables

  /* Check if the memory allocation is valid */
  // Check if the allocation is possible with our memory size

  // Check if the allocation segfaults


  // Check if the head of the MMU is null

    // If so, set the current memory allocation as the head

  // Otherwise

    // Set the current allocation as the next in the list

  }

void separateMemoryAddress( Memory_unit memoryUnit, opCodeType opCode )
  {

  /* Initialize variables */
  // Save the address the opCode wants to allocate to
  int address = opCode

  // Chop off the kilobyte allocation and save it
  memoryUnit->memoryBase = o

  // Amputate the megabyte offset and save it

  // Save the left over numbers as the identifier
  }
