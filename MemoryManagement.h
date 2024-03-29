#ifndef MM_H
#define MM_H

// header files

#include <stdio.h>
#include "StringUtils.h"
#include "MetaDataAccess.h"

// global constants
#define REMOVE_KB 1000
#define REMOVE_MB 1000
#define ALLOCATE True
#define ACCESS False
#define NOT_FOUND NULL

typedef enum {
	
	BEFORE_ACTION = 1029, 
	ACCESS_SUCCESS, 
	ACCESS_FAIL, 
	ALLOCATE_SUCCESS, 
	ALLOCATE_FAIL	
	
} memoryCodes; 

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
	int memAvailable; 

  } Memory_management_unit;

/* functions */ 
Boolean checkSegfault( Memory_management_unit* mmu, Memory_address* address,
                                                      Boolean operationFlag );
Memory_address* memoryAddressSearch( Memory_management_unit* mmu,
                                              Memory_address* addressSearch );
Memory_address* memoryAddressSearchHelper( Memory_address* runningNode,
                                               Memory_address* addressSearch );
int allocateMemory( Memory_management_unit* mmu, OpCodeType* opCode ); 
void separateMemoryAddress( Memory_address* memoryUnit, OpCodeType* opCode );
void stringifyMemoryAddress( Memory_address* memoryAddress, char* string );
Boolean checkAccess(); 
Memory_address* createMemoryAddress( OpCodeType* opCode ); 

void clearMemoryAddress( Memory_address* address ); 
void clearMemory( Memory_management_unit* mmu ); 

#endif //MM_H
