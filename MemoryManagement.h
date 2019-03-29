#ifndef MM_H
#define MM_H

// header files

#include <stdio.h>

// global constants

typedef struct Memory_address
  {

  int memoryID;
  int memoryBase;
  int memoryOffset;

  struct Memory_address* next;
	
	int processesWithAccess[]; 
} Memory_address; 

typedef struct Memory_management_unit
  {

  //
  Memory_address* head;
  Memory_address* currentUnit;

  } Memory_management_unit;

/* functions */ 
Boolean allocateMemory( Memory_management_unit* mmu, OpCodeType* opCode );
void separateMemoryAddress( Memory_address* memoryUnit, OpCodeType* opCode );



#endif //MM_H
