#include <stdio.h>
#include <pthread.h>

#include "ProcessControlBlock.h"
#include "StringUtils.h"
#include "ConfigAccess.h"
#include "MetaDataAccess.h"
#include "simtimer.h"
#include "MemoryManagement.h"

/*
Function name: main
Algorithm: main
Precondition:
Postcondition:
Exceptions: none
Note:
*/

int main( int argc, char** argv )
  {

  // initialize function/variables for Config Files
  int configAccessResult;
  char configFileName[ MAX_STR_LEN ];
  ConfigDataType *configDataPtr;

  // initialize function/variables for MetaData
  int mdAccessResult;
  char mdFileName[ MAX_STR_LEN ];
  OpCodeType* mdData;

  // initialize function/variables for the Process Control Block
  PCB* processControlBlock = ( PCB* ) malloc( sizeof( PCB ) );
  Memory_management_unit* mmu = ( Memory_management_unit* )
                                      malloc( sizeof( Memory_management_unit));
  Output_list* outputList = ( Output_list* ) malloc( sizeof( Output_list ) );
  outputList->head = NULL;
  int outputCode;
  Process_node* BEFORE_PROCESSING = ( Process_node* )
                                              malloc( sizeof( Process_node ) );
  *BEFORE_PROCESSING = ( Process_node ) { .state = EMPTY };
  char fileName[ MAX_STR_LEN ];

  // initialize araibles for other things
  Boolean noErrors = True;

  // display title
	 // function: printf
  printf( "\nSimulator Program\n" );
	printf( "=========================================\n" );

  // check for not correct number of command line arguments (two)
  if( argc < 2 )
	  {

    // print missing command line argument error
     // function: printf
    printf( "ERROR: Program requires file name for config file " );
    printf( "as commandline argument\n" );
    printf( "Program Terminated\n\n" );

    // return non-normal program result
    return 1;

    }

  // get data from configuration file
	  // function: copyString, getConfigData
  copyString( configFileName, argv[ 1 ] );

  configAccessResult = getConfigData( configFileName, &configDataPtr );

  // check for successful upload
  if ( configAccessResult == NO_ERR )
    {

    // display config file uploading
     // function: printf
    printf( "\nUploading Configuration Files\n" );

    // Save the output code from the config file
    outputCode = configDataPtr->logToCode;

    // Check if the program is writing to a file
    if( outputCode == LOGTO_FILE_CODE || outputCode == LOGTO_BOTH_CODE )
      {

      // Initialize the output linked list
       // function: initializeOutputList
      initializeOutputList( outputList, configDataPtr );
      }
    }

	// otherwise assume failed upload
  else
    {

    // display configuration upload error
     // function: displayConfigError
    displayConfigError( configAccessResult );

    noErrors = False;

    // clear configuration data
     // function: clearConfigData
    clearConfigData( &configDataPtr );

    }

  /*********************************************
  * Begin Metadata section                     *
  *********************************************/

  if( noErrors )
    {

    // get data from meta data file
     // function: copyString, getOpCodes
    copyString( mdFileName, configDataPtr->metaDataFileName );
    mdAccessResult = getOpCodes( mdFileName, &mdData );


    // check for successful upload
    if( mdAccessResult == NO_ERR )
      {

      // display meta data file uploading
       // function: printf
      printf( "\nUploading Meta Data Files\n" );

      }

    // otherwise, assume unsuccessful upload
    else
      {

      // display meta data error message
       // function: displayMetaDataError
      displayMetaDataError( mdAccessResult );

      noErrors = False;

      // clear configuration data
       // function: clearConfigData
      clearConfigData( &configDataPtr );

      // clear meat data
       // function: clearMetaDataList
      mdData = clearMetaDataList( mdData );

      }
    }

  /******************************************
  * Begin Processing Section                *
  ******************************************/

  if( noErrors )
    {
    // Seprate the uploading of files from the process(es)
    printf( "\n================\n" );
    printf( "Begin Simulation\n\n" );

    accessTimer( ZERO_TIMER, fileName );

    // Print the system start
    outputOS( BEFORE_PROCESSING, outputList, "System Start", outputCode );

    /*
    Create the process control block
    */

    // Change the BEFOR_PROCESSING because I'm bad at programming
    BEFORE_PROCESSING->state = EMPTY;

    // Print that we've created the PCB
     // function: outputOS
    outputOS( BEFORE_PROCESSING, outputList, "Create Process Control Blocks",
                                                                  outputCode );

    /*
    Create the processes
    */

    // function: initializeProcesses
    initializeProcesses( processControlBlock, mdData, configDataPtr );

    /*
    Decide the scheduling
    Since it's FCFS, I haven't done anythings here other than set the proccesses
    to ready
    */

    // Print to the monitor that the processes are ready
     // function: outputOS
    outputOS( BEFORE_PROCESSING, outputList,
                          "All processes initialized in New State", outputCode );

    // Check if the schedule code is SJF
    if( configDataPtr->cpuSchedCode == CPU_SCHED_SJF_N_CODE )
      {

      // Sort the processes and set them to ready
      sortProcesses( processControlBlock );
      }

    else
      {

      // Set all the processses to ready
       // function: setProcesesToReady
      setProcessesToReady( processControlBlock->processQueueHead );
      }

    // Print to the monitor that the processes are ready
     // function: outputOS
    outputOS( BEFORE_PROCESSING, outputList,
                          "All processes now set in Ready state", outputCode );

    /*
    * Run through the processes
    */

    // Run the simulation
     // function: runProcesses
    runProcesses( processControlBlock, outputList, outputCode, configDataPtr,
                                                                        mmu );

    // Print that the system has stopped
    outputOS( BEFORE_PROCESSING, outputList, "System stop", outputCode );

    // Print that the simulation has ended
     // function: printf
    printf( "\nEnd Simulation - Complete\n" );
    printf( "=========================\n") ;


    if( checkOutputFile( outputCode ) )
      {

      // Save the end simulation to the outputfile
      addOutputNode( outputList, "\nEnd Simulation - Complete" );
      addOutputNode( outputList, "=========================");

      // Save the name of the file we will write to
      copyString( fileName, configDataPtr->logToFileName );

      // Write to the file
       // function: saveToFile
      saveToFile( outputList, fileName );
      }

    /*
    shut down, clean up program
    */

    // Free this because I'm bad at programming
    free( BEFORE_PROCESSING );

    // clear meta data
     // function: clearMetaDataList
    mdData = clearMetaDataList( mdData );

    // clear the PCB nodes
     // function: clearProcessNodes
    clearProcessNodes( processControlBlock->processQueueHead );

    // clear the PCB
     // function: free
    free( processControlBlock );

    // clear the outputList nodes
     // function: clear
    clearOutputList( outputList->head );

    // clear the outputList
     // function: free
    free( outputList );

    // clear configuration data
     // function: clearConfigData
    clearConfigData( &configDataPtr );

    clearMemory( mmu );

    }

  // add endline for vertical spacing
   // function: printf
  printf( "\n" );
  }
