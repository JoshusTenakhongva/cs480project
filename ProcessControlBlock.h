#ifndef PCB_H
#define PCB_H

// header files
#include <stdio.h>
#include "StringUtils.h"
#include "MetaDataAccess.h"
#include "ConfigAccess.h"

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
int getProcessCount( PCB* processControlBlock );
Process_node* copyProcess( Process_node* sourceNode );
void runProcesses( PCB* processControlBlock, Output_list* outputList,
                                     int outputCode, ConfigDataType* cfgData );
void runFCFS( PCB* processControlBlock, Output_list* outputList,
                                                             int outputCode );
void runOpCodes( Process_node* currProc, Output_list* outputList,
                                                              int outputCode );
void spinOpCode( OpCodeType* opCode, double spinTime, Process_node* currProc );
void incrementCurrentProcess( PCB* ProcessControlBlock );

// Output Methods 
void outputOS( Process_node* process, Output_list* outputList, char string[],
                                                              int outputCode );
void outputProcess( Process_node* process, int outputCode,
																                     Output_list* outputList );
void initializeOutputList( Output_list* outputList,
																							ConfigDataType* configDataPtr );
Boolean checkOutputFile( int outputCode );
Boolean checkOutputPrint( int outputCode );
void addOutputNode( Output_list* outputList, char info[] );
void saveToFile( String_node* runningNode, char fileName[] );

// Methods for cleaning and freeing mallocs
void clearOutputList( String_node* runningNode );
void clearProcessNodes( Process_node* runningNode );

void* threadTimer( int code, char fileName[] );




#endif // PCB_H
