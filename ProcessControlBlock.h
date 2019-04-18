#ifndef PCB_H
#define PCB_H

// header files
#include <stdio.h>
#include "StringUtils.h"
#include "MetaDataAccess.h"
#include "ConfigAccess.h"
#include "MemoryManagement.h"

// global constants
typedef enum { EMPTY = 101,
               NEW,
               READY,
               RUNNING,
               TERMINATED,
             } processStates;


typedef struct Process_node
  {

  int timeRemaining;
  OpCodeType* startCode;
  OpCodeType* endCode;
  OpCodeType* currCode;
  int state;
  int processID;
  struct Process_node* next;

  } Process_node;

typedef struct PCB
  {

  Process_node* processQueueHead;
  Process_node* currentProcess;
  Process_node* programCounter;
	
	Memory_management_unit* mmu; 

  } PCB;

typedef struct String_node
  {

  char string[ MAX_STR_LEN ];
  struct String_node* next;

  } String_node;

typedef struct Output_list
  {

  String_node* head;
  String_node* tail;

  } Output_list;

/* function prototypes */
// Process related methods 
void initializeProcesses( PCB* processControlBlock, OpCodeType* mdData,
																											ConfigDataType* cfgData);
void initializeProcessesHelper( PCB* processControlBlock, int processID,
  OpCodeType* runningOpCode, ConfigDataType* cfgData, Process_node* currProc );
void opCode_A_start( Process_node* currProcess, int processNumber,
                                                    PCB* processControlBlock );
void opCode_A_end( Process_node* currProcess, OpCodeType* opCode );
void incrementCurrentOpCode( Process_node* process );
void setProcessesToReady( Process_node* runningProcess );
void sortProcesses( PCB* processControlBlock ); 



Process_node* findShortestProcess( PCB* processControlBlock, 
                                                          int unwantedState );
Process_node* findShortestHelper( Process_node* runningNode,
                              Process_node* shortestNode, int unwantedState );

															
															
															
															
int getProcessCount( PCB* processControlBlock );
Process_node* copyProcess( Process_node* sourceNode );
void runProcesses( PCB* processControlBlock, Output_list* outputList,
        int outputCode, ConfigDataType* cfgData, Memory_management_unit* mmu );
void runFCFS( PCB* processControlBlock, Output_list* outputList,
                                int outputCode, Memory_management_unit* mmu );
void runOpCodes( Process_node* currProc, Output_list* outputList,
                                int outputCode, Memory_management_unit* mmu );
void spinOpCode( OpCodeType* opCode, double spinTime, Process_node* currProc );
void incrementCurrentProcess( PCB* ProcessControlBlock );

// Output Methods 
void outputOS( Process_node* process, Output_list* outputList, char string[],
                                                              int outputCode );
void memoryOutput( Process_node* process, int outputCode,
															         Output_list* outputList, int memCode );
void nonMemoryOutput( Process_node* process, int outputCode,
																                     Output_list* outputList );						
void processOutputPrefix( Process_node* process, char* outputString ); 
void initializeOutputList( Output_list* outputList,
																							ConfigDataType* configDataPtr );
Boolean checkOutputFile( int outputCode );
Boolean checkOutputPrint( int outputCode );
void addOutputNode( Output_list* outputList, char info[] );
void saveToFile( Output_list* outputList, char fileName[] );
void saveToFileHelper( String_node* runningNode, FILE* filePointer );

// Methods for cleaning and freeing mallocs
void clearOutputList( String_node* runningNode );
void clearProcessNodes( Process_node* runningNode );

void* threadTimer( int code, char fileName[] );



#endif // PCB_H
