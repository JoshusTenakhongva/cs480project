#ifndef MM_H
#define MM_H

// header files

#include <stdio.h>

// global constants

typedef struct Memory_unit
  {

  int memoryID;
  int memoryBase;
  int memoryOffset;
	Process_node* accessProcesses; 

  struct Memory_unit* next;
} Memory_unit; 

typedef struct Memory_management_unit
  {

  //
  Memory_unit* head;
  Memory_unit* currentUnit;

  } Memory_management_unit;



#endif //MM_H
