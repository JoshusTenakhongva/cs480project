#ifndef MM_H
#define MM_H

// header files

#include <stdio.h>
#include "StringUtils.h"
#include "MetaDataAccess.h"

// global constants
#define REMOVE_KB 1000
#define REMOVE_MB 1000

typedef struct Memory_address
  {

  int memoryID;
  int memoryBase;
  int memoryOffset;

  struct Memory_address* next;
	
	int processWithAccess; 
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
void stringifyMemoryAddress( Memory_address* memoryAddress, char* string );
Boolean checkAccess(); 
Memory_address* createMemoryAddress( OpCodeType* opCode ); 
Memory_address* memoryAddressSearch( Memory_management_unit* mmu,
                                              Memory_address* addressSearch ); 
Boolean checkSegfault( Memory_management_unit* mmu, Memory_address* address ); 	 

#endif //MM_H
