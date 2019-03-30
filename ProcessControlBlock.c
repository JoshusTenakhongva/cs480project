// header files

#include <stdio.h>
#include "StringUtils.h"
#include "ProcessControlBlock.h"
#include "simtimer.h"
#include <math.h>
#include <pthread.h>
#include "MemoryManagement.h"

/****************************************************
*       Table of Contents
*****************************************************
*  Process related Methods
*     initializeProcesses
*     initializeProcessesHelper
*     opCode_A_start
*     opCode_A_end
*     incrementCurrentOpCode
*     setProcessesToReady
*     sortProcesses
*     getProcessCount
*     copyProcesses
*     runProcesses
*     runFCFS
*     runOpCodes
*     spinOpCode

*  Output Methods
*     outputOS
*     outputProcesses
*     initializeOutputList
*     checkOutputFile
*     checkOutputPrint
*     addOutputNode
*     saveToFile

*  Cleaning/Freeing methods
*     clearOutputList
*     clearProcessNodes

*  Other
*     threadTimer

****************************************************/

/****************************************************
*   Process related methods
****************************************************/

/*
* Desc:
*       This method creates all of the processes and saves them to the PCB

* Input:
*       @processControlBlock - This is the struct that will hold the information
*                              for the linkedList of processes
*       @mdData - This is the linkedLIst of the metadata opCodes

* Postcondition:
*       The opCodes will be separated into processes, and the processes will be
*       saved into the processControlBlock
*/
void initializeProcesses( PCB* processControlBlock, OpCodeType* mdData,
                                                        ConfigDataType* cfgData)
  {

  // Declare/Initialize function variables
  processControlBlock->processQueueHead = NULL;

  // Loop through the opCodes and assign them to processes
  initializeProcessesHelper( processControlBlock, 0, mdData, cfgData, NULL );

  // Set the program counter
  processControlBlock->programCounter =
                                    processControlBlock->processQueueHead->next;

  }

void initializeProcessesHelper( PCB* processControlBlock, int processID,
    OpCodeType* runningOpCode, ConfigDataType* cfgData, Process_node* currProc )
  {

  Process_node* newProcess;
  Process_node* currentProcess = currProc;
  char opCodeName[ MAX_STR_LEN ];
  int processingCycleTime = cfgData->procCycleRate;
  int IOCycleTime = cfgData->ioCycleRate;
  int processNumber = processID;

  // check if the current opCode is not NULL
  if( runningOpCode != NULL )
    {

    // Save the code name
    copyString( opCodeName, runningOpCode->opName );

    // Switch case the opcode letter
    switch( runningOpCode->opLtr )
      {

      // Check if the opCode is S
      case 'S':
        // Do nothing since S does nothing here
        break;

      // Check if the opCode is A
  		case 'A':

        // CHeck if the A code starts a process
        if( compareString( opCodeName, "start" ) == STR_EQ )
          {

          // Initialize the new process
          newProcess = ( Process_node* ) malloc( sizeof( Process_node ) );

           // function: opCode_A_start
          opCode_A_start( newProcess, processNumber, processControlBlock );

          // Set the node we have just created as the node we are working Switch
          currentProcess = newProcess;

          // Increment the process Number for the next process
          processNumber++;

          }

        // Check if the next opCode is A(end)
        if( compareString( opCodeName, "end" ) == STR_EQ )
          {

          opCode_A_end( currentProcess, runningOpCode );

          }

        // If it's an end A code, we handle it elsewhere
        break;

      // Check if the opCode is a memory operation
  		case 'M':
        if( currentProcess->startCode == NULL )
          {

          currentProcess->startCode = runningOpCode;
          }

        runningOpCode->masterProcess = processNumber - 1;
        break;

      // Check if the opCode is processing
  		case 'P':

        if( currentProcess->startCode == NULL )
          {

          currentProcess->startCode = runningOpCode;
          }

        // Find the runTime of the opCode
        runningOpCode->runTime = runningOpCode->opValue * processingCycleTime;

        currentProcess->timeRemaining = currentProcess->timeRemaining +
                                                        runningOpCode->runTime;

        break;

      // Check if the opCode is IO
  		case 'I':
  		case 'O':

        if( currentProcess->startCode == NULL )
          {

          currentProcess->startCode = runningOpCode;
          }

        // Find the runtime of the opCode
        runningOpCode->runTime = runningOpCode->opValue * IOCycleTime;

        currentProcess->timeRemaining += runningOpCode->runTime;

        break;
      //
      }

    // Recurse into the next opCode
    initializeProcessesHelper( processControlBlock, processNumber,
                                runningOpCode->next, cfgData, currentProcess );
    }

  // Otherwise, do nothing and end the recursion
  }

void opCode_A_start( Process_node* currProcess, int processNumber,
                                                      PCB* processControlBlock )
  {

  // Set everything to null except cycle time which is 0
  currProcess->next = NULL;
  currProcess->timeRemaining = 0;
  currProcess->startCode = NULL;
  currProcess->state = NEW;
  currProcess->processID = processNumber;

  // Check if we have not already made a process, making this the first
  if( processControlBlock->processQueueHead == NULL )
    {

    // Set this process as the head
    processControlBlock->processQueueHead = currProcess;
    }

  // Otherwise, assume this isn't the first process
  else
    {

    // Have the previous process point to our new process as its next one
    processControlBlock->currentProcess->next = currProcess;
    }

  // update the PCB's current process to the one we just made
  processControlBlock->currentProcess = currProcess;

  }

void opCode_A_end( Process_node* currProcess, OpCodeType* opCode )
  {

  // Set the A opCode as the end of the process's opCodes
  currProcess->endCode = opCode;

  // Reset the opCode pointer, so processing can begin at the start of the codes
  currProcess->currCode = currProcess->startCode;

  }

void setProcessesToReady( Process_node* runningProcess )
  {
  // check if the runningProcess is NULL
  if( runningProcess != NULL )
    {

    runningProcess->state = READY;

    // recurse to the next processStates
    setProcessesToReady( runningProcess->next );
    }

  }

void sortProcesses( PCB* processControlBlock )
  {

  /* Initialize variables */
  int outerCount, innerCount;

  // Create a new head for our sorted list of processes
  Process_node* sortedListHead = NULL;

  // Create a tempNode for sorting
  Process_node* tempNode;

  // Create a process node to increment through the unsorted list of processes
  Process_node* currPCBNode = processControlBlock->processQueueHead;

  // Variable to hold the tail of the sorted processes list
  Process_node* currSortedNode;

  // Find the number of process in the PCB
  int numOfProcesses = getProcessCount( processControlBlock );

  // Loop through the processes, sorting one process for one loop
  for( outerCount = 0; outerCount < numOfProcesses; outerCount++ )
    {

    // Reset our tempNode at the beginning each loop
    tempNode = NULL;

    // Reset the unsorted process list node we are at
    currPCBNode = processControlBlock->processQueueHead;

    // Loop through the processes
    for( innerCount = 0; innerCount < numOfProcesses; innerCount++ )
      {

      // Check if the tempNode is null and if this process is viable
      if( tempNode == NULL && currPCBNode->state != READY )
        {

        // If so, assign it ot the tempNode
        tempNode = currPCBNode;
        }

      // else check if this node has a lowertimeRemaining than our tempNode
      else if( tempNode != NULL && currPCBNode->state != READY)
        {

        if( currPCBNode->timeRemaining < tempNode->timeRemaining )
          {

          // If so, assign it to the temp node
          tempNode = currPCBNode;
          }
        }

      // increment the PCBNode
      currPCBNode = currPCBNode->next;
      }
    /* end Inner loop */

    // Set the tempNode -therefore, the unsorted node- to ready
    tempNode->state = READY;

    // Create a copy of the node we've chosen, so we don't mess with the order
    tempNode = copyProcess( tempNode );

    // check if the head of the sorted list is NULL
    if( sortedListHead == NULL )
      {

      // assign the tempNode to the head
      sortedListHead = tempNode ;
      }
    else
      {

      currSortedNode->next =  tempNode ;
      }

    // set the current sorted node to the tempNode
    currSortedNode = tempNode;
    }
  /* end Outer loop */

  // Free the unsorted processes
  clearProcessNodes( processControlBlock->processQueueHead );

  // Save the head and program counter of the sorted list to the PCB
  processControlBlock->processQueueHead = sortedListHead;
  processControlBlock->programCounter = sortedListHead->next;
  }

int getProcessCount( PCB* processControlBlock )
  {

  // Initialize variables
  Process_node* runningNode = processControlBlock->processQueueHead;
  int count = 0;

  // Loop until we reach the final process, counting each
  while( runningNode != NULL )
    {

    runningNode = runningNode->next;
    count++;
    }

  return count;
  }

/*
* Desc:
*   Copies the a process

* Input:
*   @sourceNode - The node that is being copied

* Postcondition:
*   Returns a copy of the input process
*/
Process_node* copyProcess( Process_node* sourceNode )
  {

  // Create the copy of the sourceNode
  Process_node* nodeCopy = ( Process_node* ) malloc( sizeof( Process_node ) );

  // Check if the sourceNode is null
  if( sourceNode == NULL )
    {

    return NULL;
    }

  /* Otherwise, continue as usual */
  // Copy the data of the node
  nodeCopy->timeRemaining = sourceNode->timeRemaining;
  nodeCopy->startCode = sourceNode->startCode;
  nodeCopy->endCode = sourceNode->endCode;
  nodeCopy->currCode = sourceNode->currCode;
  nodeCopy->state = sourceNode->state;
  nodeCopy->processID = sourceNode->processID;
  nodeCopy->next = NULL;

  // return the copy of the sourceNode
  return nodeCopy;

  }

/*
* Desc:
*    Runs through the processes and simulates run time by spinning

* Input:
*    @processControlBlock - The manager of our queue of processes
*    @outputList - The linked list of the output to the file
*    @outputCode - The decider of what kind of output is used

* Postcondition:
*   The simulation of the processes being run will be completed.
*/
void runProcesses( PCB* processControlBlock, Output_list* outputList,
        int outputCode, ConfigDataType* cfgData, Memory_management_unit* mmu )
  {

  // Set the process that is currently being worked on as the first on the queue
  processControlBlock->currentProcess = processControlBlock->processQueueHead;

  // Determine the scheduling type
  switch( cfgData->cpuSchedCode )
    {

    // Since we have already sorted the process list, we just run it in order
    case CPU_SCHED_SJF_N_CODE:
    case CPU_SCHED_FCFS_N_CODE:
    // FCFS-N is the default cpu scheduling order
    default:
      runFCFS( processControlBlock, outputList, outputCode, mmu );
      break;
    }
  }

/*
* Desc:
*   Runs through the processes using the First Come First Serve algorithm

* Input:
*   @processControlBlock - The top-level manager of the processStates
*   @outputList - The linkedlist of outputNodes for file output
*   @outputCode - Determines the type of output: to the screen, file, or both

* Postcondition:
*   All of the processes will have been run through
*
*/
void runFCFS( PCB* processControlBlock, Output_list* outputList,
                                int outputCode, Memory_management_unit* mmu )
  {

  // Initialize variables
  Process_node* currProc = processControlBlock->currentProcess;

  // Check if the current process is NULL
  if( currProc != NULL )
    {

    // Set this process to running
    currProc->state = RUNNING;

    // Print the process cycle time and that this process is running
     // function: outputOS
    outputOS( currProc, outputList, EMPTY_STRING, outputCode );

    // Loop through the opCodes of this process recursively
    runOpCodes( currProc, outputList, outputCode, mmu );

    // Set the current processes state as terminated
    currProc->state = TERMINATED;

    // Extra line for clarity
    if( checkOutputPrint( outputCode ) )
      {

      printf( "\n" );
      }

    if( checkOutputFile( outputCode ))
      {

      addOutputNode( outputList, "\n" );
      }

    // Print that the process has ended
    outputOS( currProc, outputList, EMPTY_STRING, outputCode );

    // set the next process as the current process
    incrementCurrentProcess( processControlBlock );

    runFCFS( processControlBlock, outputList, outputCode, mmu );
    }

  // end loop
  }

void runOpCodes( Process_node* currProc, Output_list* outputList,
                                int outputCode, Memory_management_unit* mmu )
  {

  OpCodeType* currOpCode = currProc->currCode;

  // Check if the current opCode is not the ending of the process
  if( compareString( currOpCode->opName, "end") != STR_EQ )
    {

    // Check if the opcode is a memory unit
    if( currOpCode->opLtr == 'M' )
      {

      allocateMemory( mmu, currOpCode );
      }

    // otherwise, act as any other opCode

    // Print that this opCode has started
     // function: outputProcess
    outputProcess( currProc, outputCode, outputList );

    // Spin the opCode
     // function: spinOpCode
    spinOpCode( currOpCode, currOpCode->runTime, currProc );

    // Print that the opCode has ended
     // function: outputProcess
    outputProcess( currProc, outputCode, outputList );

    // Increment the process's currrent opCode to the next one
    incrementCurrentOpCode( currProc );

    // recurse to the next opCode
    runOpCodes( currProc, outputList, outputCode, mmu );
    }

  // Otherwise, end the recursion
  }

/*
* Desc:
*   Spins an opCode for the desired amount of time and subtracts the time spun
*   from the overall process remaining run time

* Input:
*   @opCode - The opCode that we want to spin
*   @spinTime - The amount of time that we want to spin the opCode for
*   @currProc - The process that the opCode belongs to; therefore, the current
*     process being run

* Postcondition:
*   No output is done, but, for the system, the opCode has finished "processing"
*/
void spinOpCode( OpCodeType* opCode, double spinTime, Process_node* currProc )
  {

  // Spin for the amount of time
  runTimer( spinTime );

  // Subtrack the amount of time spun from he current process
  currProc->timeRemaining = currProc->timeRemaining - spinTime;

  // Subtract the opCode's cycleTime by the amount of time it needed to spin
  opCode->runTime = opCode->runTime - spinTime;

  if( currProc->timeRemaining <= 0 )
    {

    currProc->state = TERMINATED;
    }

  }

void incrementCurrentOpCode( Process_node* process )
  {

  process->currCode = process->currCode->next;
  }

/*
* Desc:
*   Increments the currentProcess of the PCB and sets the next program
*   counter.

* Input:
*   @processControlBlock - The manager of all of the processes

* Postcondition:
*   The current process and program counter have been changed to be the next
*   in line of the queue.
*/
void incrementCurrentProcess( PCB* processControlBlock )
  {

  processControlBlock->currentProcess = processControlBlock->programCounter;

  // Check if the current process is not null
  if( processControlBlock->currentProcess != NULL )
    {

    processControlBlock->programCounter =
                                    processControlBlock->currentProcess->next;
    }

  // otherwise, set the programCounter to null
  else
    {

    processControlBlock->programCounter = NULL;
    }

  }

/**************************************************************
*   Output Methods
***************************************************************/

/*
* Desc:
*   Controls the output to the screen and file done to reflect the actions
*   of the simulated operating system. Output like processes being begun and
*   ending, the start of the system, and the end of the system.

* Input:
*   @process - The process that is currently being run. It is used to output
*     which process has begun or ended and is output accordingly
*   @outputList - A linked list of nodes that hold the information that will be
*     output to a file at tne end of the program. If the config file determines
*     that the output will be done to the file, the line will be added to this
*     list
*   @string - If a unique string must be output, this will be that string
*   @outputCode - The code that determines if the output will be to the screen,
*     to the file, or both.

* Postcondition:
*   Appropriate output will have been created to the monitor, file, or both.
*/
void outputOS( Process_node* process, Output_list* outputList, char string[],
                                                                int outputCode )
  {

  /* Initialize the variables */
  // Create a string to hold the time of an operation
  char time[ MAX_STR_LEN ];

  // Create a string to hold the entire output string
  char outputString[ MAX_STR_LEN ] = "  ";

  // Create a string for the beginning of the display line
  char processNumber[ MAX_STR_LEN ];

  // Create a string to hold the remaining time of a process
  char timeRemaining[ MAX_STR_LEN ];

  // Save the time that this process is processing
  accessTimer( LAP_TIMER, time );

  // Add the time to the output string
  concatenateString( outputString, time );
  concatenateString( outputString, ", OS: " );

  // check if process state is RUNNING
  if( process->state == RUNNING )
    {

    concatenateString( outputString, "Process " );

    sprintf( processNumber, "%d", process->processID );

    concatenateString( outputString, processNumber );

    // Concatenate it with the "selectd with"
    concatenateString( outputString, " selected with " );

    sprintf( timeRemaining, "%d", process->timeRemaining );

    // Concatenate it with the time remaining
    concatenateString( outputString, timeRemaining );

    // Contatenate it with "ms remaining"
    concatenateString( outputString, " ms remaining" );

    // Add to the thing

    accessTimer( LAP_TIMER, time );

    printf( "%s\n", outputString );

    // Add this string to our output node if we must
    if( outputCode == LOGTO_BOTH_CODE || outputCode == LOGTO_FILE_CODE )
      {

      addOutputNode( outputList, outputString );
      }

    copyString( outputString, "  " );

    concatenateString( outputString, time );
    concatenateString( outputString, ", OS: " );

    concatenateString( outputString, "Process " );

    sprintf( processNumber, "%d", process->processID );

    concatenateString( outputString, processNumber );

    concatenateString( outputString, " set in RUNNING state\n" );

    }

  // check if the process has EXITed
  else if( process->state == TERMINATED )
    {

    concatenateString( outputString, "Process " );

    sprintf( processNumber, "%d", process->processID );

    concatenateString( outputString, processNumber );

    concatenateString( outputString, " ended and set in EXIT state" );

    }

  // check if the process state is EMPTY
  else if ( process->state == EMPTY )
    {

    concatenateString( outputString, string );
    }

  // print the string
   // function: printf
  if( outputCode == LOGTO_BOTH_CODE || outputCode == LOGTO_MONITOR_CODE )
    {

    printf( "%s\n", outputString );
    }

  // Save the output to the linkedList if it can
   // function: saveForFile
  if( outputCode == LOGTO_BOTH_CODE || outputCode == LOGTO_FILE_CODE )
    {
    // Add the node to the outputlist
    addOutputNode( outputList, outputString );
    }

	}

/*
* Desc:
*   Controls the output to the screen and file done to reflect the actions
*   of a process and its opCodes. Output like input and output operations,
*   memory allocation, and processing operations.

* Input:
*   @process - The process that is currently being run. It is used to keep
*     track of the opCodes that are being run.
*   @outputCode - The code that determines if the output will be to the screen,
*     to the file, or both.
*   @outputList - A linked list of nodes that hold the information that will be
*     output to a file at tne end of the program. If the config file determines
*     that the output will be done to the file, the line will be added to this
*     list

* Postcondition:
*   Appropriate output will have been created to the monitor, file, or both.
*/
void outputProcess( Process_node* process, int outputCode,
																                      Output_list* outputList )
  {

  // Initialize the variables
  char time[ MAX_STR_LEN ];
  char outputString[ MAX_STR_LEN ] = "  ";
  OpCodeType* currentOpCode = process->currCode;

  // Create a string for the beginning of the display line
  char processNumber[ MAX_STR_LEN ];

  /*
  Create the string to be about the cycle time remaining

  Parts must be added incrementally using concatenate string since we cannot
  just easily concatenate the entire string at once and put everything in it.
  */

  accessTimer( LAP_TIMER, time );

  // Create the string that holds the process and its number
  concatenateString( outputString, time );

  concatenateString( outputString, ", Process: " );

  sprintf( processNumber, "%d", process->processID );

  concatenateString( outputString, processNumber );

  // Because my SPACE constant doesn't want to work
  concatenateString( outputString, " " );

  //printf( "opName: %s", currentOpCode->opName );

  // Create the second half with the process number and opCode information
  concatenateString( outputString, currentOpCode->opName );

  // Switch statement checking the opLetter
  switch( currentOpCode->opLtr )
    {

    case 'M':
      concatenateString( outputString, " memory operation " );
      break;

    case 'P':
      concatenateString( outputString, " operation " );
      break;

    case 'I':
      concatenateString( outputString, " input " );
      break;

    case 'O':
      concatenateString( outputString, " output " );
      break;

    }

  // Check if the cycleTime is above 0
  if( currentOpCode->runTime > 0 )
    {

    concatenateString( outputString, "start" );
    }

  else
    {

    concatenateString( outputString, "end" );
    }

  // display the string if the output code designates the monitor
   // function: printf
  if( checkOutputPrint( outputCode ) )
    {

    printf( "%s\n", outputString );
    }

  // save the output to our list if it can
   // function: savetoOutputList
  if( checkOutputFile( outputCode ) )
    {

    addOutputNode( outputList, outputString );
    }

  }

/*
* Desc:
*   Initializes our linked list that will contain the information that
*   will be put into the output file at the end of the program with the
*   data from the config file.

* Input:
*   @listContainer - The pointer to the linked list struct declared outside of
*     the function
*   @configDataPtr - The pointer to the struct that contains the information
*     from config file

* Output:
*   The listPointer will now have the first segment of the output for the file,
*   the information from the configuration file
*/
void initializeOutputList( Output_list* outputList,
                                                ConfigDataType* configDataPtr )
  {

  // Initialize variables
  int code;
  char outputString[ MAX_STR_LEN ];
  char infoString[ MAX_STR_LEN ];

  /* Create the beginning nodes */

  // Create a string_node with the line of =
   // function: createNewNode
  addOutputNode( outputList, "======================" );

  // Create a string_node with the "Simulator Log File Header"
   // function: createNewNode
  addOutputNode( outputList, "Simulator Log File Header" );

  // Add a blank line
   // function: createNewNode
  addOutputNode( outputList, "\n" );

  // Loop through the config codes starting at CFG_MD_FILE_NAME_CODE
  for( code = 8; code < 14; code++ )
    {
    // Switch case: loopCode
    switch( code )
      {

      // check case metaData code
       // function: createNewNode
      case CFG_MD_FILE_NAME_CODE:
        copyString( outputString, "File Name                       : " );

        concatenateString( outputString, configDataPtr->metaDataFileName);

        break;

      // check case CPU schedule code
       // function: createNewNode
      case CFG_CPU_SCHED_CODE:
        copyString( outputString, "CPU Scheduling                  : " );

        switch( configDataPtr->cpuSchedCode )
          {

          case CPU_SCHED_SJF_N_CODE:
            copyString( infoString, "SJF-N" );
            break;

          case CPU_SCHED_SRTF_P_CODE:
            copyString( infoString, "SRTF-P" );
            break;

          case CPU_SCHED_FCFS_P_CODE:
            copyString( infoString, "FCFS-P" );
            break;

          case CPU_SCHED_RR_P_CODE:
            copyString( infoString, "RR-P" );
            break;

          case CPU_SCHED_FCFS_N_CODE:
            copyString( infoString, "FCFS-N" );
            break;
          }

        concatenateString( outputString, infoString );

        break;

      // check case Quantum cycles code
       // function: createNewNode
      case CFG_QUANT_CYCLES_CODE:
        copyString( outputString, "Quantum Cycles                  : " );

        sprintf( infoString, "%d", configDataPtr->quantumCycles );

        concatenateString( outputString, infoString );
        break;

      // check case Memory Available (KB)
       // function: createNewNode
      case CFG_MEM_AVAILABLE_CODE:
        copyString( outputString, "Memory Available (KB)           : " );

        sprintf( infoString, "%d", configDataPtr->memAvailable );

        concatenateString( outputString, infoString );
        break;

      // check case Processor Cycle Rate
       // function: createNewNode
      case CFG_PROC_CYCLES_CODE:
        copyString( outputString, "Processor Cycle Rate (ms/cycle) : " );

        sprintf( infoString, "%d", configDataPtr->procCycleRate );

        concatenateString( outputString, infoString );
        break;

      // check case IO cycle rate
       // function: createNewNode
      case CFG_IO_CYCLES_CODE:
        copyString( outputString, "I/O Cycle Rate (ms/cycle)       : " );

        sprintf( infoString, "%d", configDataPtr->ioCycleRate );

        concatenateString( outputString, infoString );
        break;

      // default case end the loop
       // function: createNewNode
      }

    // Add the string we created to the output list
    addOutputNode( outputList, outputString );
    }
  // End loop

  // Add = line
   // function: createNewNode
  addOutputNode( outputList, "==================" );

  // Add "Begin Simulation" line
   // function: createNewNode
  addOutputNode( outputList, "Begin Simulation\n" );
  }

/*
* Desc:
*   Checks if the output code has determined that the output will be done
*   to the monitor

* Input:
*   @outputCode - The code that determines if the output will be to the screen,
*     to the file, or both.

* Postcondition:
*   Return a boolean determining whether or not the output will be done to the
*   monitor
*/
Boolean checkOutputPrint( int outputCode )
  {

  return outputCode == LOGTO_BOTH_CODE || outputCode == LOGTO_MONITOR_CODE;
  }

/*
* Desc:
*   Checks if the output code has determined that the output will be done
*   to a file

* Input:
*   @outputCode - The code that determines if the output will be to the screen,
*     to the file, or both.

* Postcondition:
*   Return a boolean determining whether or not the output will be done to a
*   file
*/
Boolean checkOutputFile( int outputCode )
  {

  return outputCode == LOGTO_BOTH_CODE || outputCode == LOGTO_FILE_CODE;
  }

/*
* Desc:
*       Takes in information, stores it into a newly created node, and sets
*       that node as the new tail for the listContainer
* Input:
*       @listContainer - The struct that holds the head and tail for the
*                        linked list that holds the information that will be
*                        written to the file at the end of the program
*       @info - The string that we want to insert into the new node
* Output:
*       The listContainer will now have a new node at its tail with the
*       information that we entered into this function.
*/
void addOutputNode( Output_list* outputList, char info[] )
  {

  // Create a new node while saving the info to the new node
  String_node* outputNode = ( String_node* ) malloc( sizeof( String_node ));

  // Save the string information
  copyString( outputNode->string, info );

  // Check if this node is the first
  if ( outputList->head == NULL )
    {

    // Set the new node as the head and the tail of the outputList
    outputList->head = outputNode;
    outputList->tail = outputNode;
    }

  // Otherwise, assume this isn't the first node
  else
    {

    // Have the current tail point to our new node as its next
    outputList->tail->next = outputNode;

    // update the tail
    outputList->tail = outputNode;

    }

  // Set the new tail to point to null
  outputList->tail->next = NULL;
  }


void saveToFile( Output_list* outputList, char fileName[] )
  {

  // Create a file to write to
  FILE* filePointer = fopen( fileName, "w" );

  // resurcse through the outputList and save the nodes to the file
  saveToFileHelper( outputList->head, filePointer );

  // close the file
  fclose( filePointer );

	}

void saveToFileHelper( String_node* runningNode, FILE* filePointer )
  {

  // Check if the running node is not null
  if ( runningNode != NULL )
    {

    // Save the text to the filePointer
    fprintf( filePointer, "%s\n", runningNode->string );

    // Recurse to the next node
    saveToFileHelper( runningNode->next, filePointer );
    }
  }

/*******************************************************
*   Methods for cleaning and freeing mallocs
********************************************************/

/**
* Desc:
*   Recursively loops through the linked list that contains the information
*   that will be written to the lgf file at the end of the program and frees
*   every node.

* Input:
*   @runningNode - The current node of the recursion that is being freed

* Postcondition:
*   All of the nodes in the outputList linked list will be freed
*/
void clearOutputList( String_node* runningNode )
  {

  /* initialize variables */
  String_node* nextNode;

	// Check if the runningNode is not null
  if( runningNode != NULL )
    {

		// Save the address of the next node
    nextNode = runningNode->next;

		// Free the runningNode
     // function: free
    free( runningNode );

		// clearOutputList( nextNode )
    clearOutputList( nextNode );
    }
  }

void clearProcessNodes( Process_node* runningNode )
  {

  Process_node* nextNode;

  // Check if the node is not NULL
  if( runningNode != NULL )
    {

    // Save the address of the next node
    nextNode = runningNode->next;

    // Free the runningNode
     // function: free
    free( runningNode );

    // Recurse to the next node
    clearProcessNodes( nextNode );
    }
  }
