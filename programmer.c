#include <stdio.h> 
#include <unistd.h> 
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>
#include <signal.h>
#include <sys/time.h>
 
#include "hardware.h"
  
  
 //Compile with: gcc programmer.c hardware.c -Wall -o prg
 
void printHelp(){
	 printf("\nFlash programmer help menu\n\n");
	 printf("\t-d:\tDump file contents to console\n");
	 printf("\t-v:\tEnable verbose output\n");
	 printf("\t-f:\tFilename\n");
	 printf("\t-h:\tPrint this help menu\n");
	 fflush(stdout);
}



void exitProgrammer(int signal){
	if(signal == SIGINT){
	powerOff();
	exit(0);
	}
}

int main(int argc, char *argv[]) 
{
    int opt;

	char * inputFileName = malloc(sizeof(*inputFileName));
	
	bool fileIsProvided = false;
	bool verboseOutput = false;
	bool dumpFileContents = false;
	
	struct timeval beginTime, endTime;
	
	signal(SIGINT, exitProgrammer);
	
	gettimeofday(&beginTime, NULL);	//get begin time
	setupFlashProgrammer();
	powerOn();
	
	
    while((opt = getopt(argc, argv, ":if:dvhx")) != -1) 
    { 
        switch(opt) 
        { 
            case 'i': break;
            case 'd': dumpFileContents = true; break;
            case 'v': 
                verboseOutput = true;
                break; 
			case 'h': 
                printHelp();
				exitProgrammer();
                break; 
            case 'f': 
				sprintf(inputFileName, "%s", optarg);
				printf("Opening file: %s\n", inputFileName); 
				fileIsProvided = true;
                break; 
            case ':': 
                printf("option needs a value\n"); 
				printHelp();
                break; 
				
			default:
            case '?': 
                printf("unknown option: %c\n", optopt);
				printHelp();
                break; 
        } 
    } 
      
    // optind is for the extra arguments
    // which are not parsed
    for(; optind < argc; optind++){     
        printf("extra arguments: %s\n", argv[optind]); 
    }
	fflush(stdout);
	
	if(!fileIsProvided){
		printf("Program failed: Please provide a file using -f\n");
		exitProgrammer();
	}
	
	 FILE *inputFile;
	 
	 inputFile = fopen(inputFileName, "rb");
	 if(inputFile == NULL){
		 perror("Opening file failed");
		 exitProgrammer();
	 }
	 else if(verboseOutput){
		 printf("Opening file was succesful\n");
		 fflush(stdout);
	 }
	 free(inputFileName);
	 
	 fseek(inputFile, 0L, SEEK_END);
	 size_t size = ftell(inputFile);
	 fseek(inputFile, 0L, SEEK_SET);
	 if(verboseOutput){
		 printf("File length: %I64u\n", size);
		 fflush(stdout);
	 }
	 uint8_t* ByteArray = malloc(sizeof(uint8_t)*size);
	 
	if(verboseOutput){
		 printf("Allocated local buffer\n");
		 fflush(stdout);
	}
	 
	 size_t succesBytes = fread(ByteArray, sizeof(uint8_t), size, inputFile);

	fclose(inputFile);
	 
	if(verboseOutput){
		 printf("Copied %I64u bytes from file to local buffer\n", succesBytes);
		 fflush(stdout);
	}
	
	writeCS(0);
	 for (size_t i = 0; i < size; i++) {
		 softSpiTransfer(ByteArray[i]);
			if(dumpFileContents){	
				printf("%02X ", ByteArray[i]);
			}
	 }
	 writeCS(1);
	  
	  gettimeofday(&endTime, NULL);

	printf ("Execution took %f seconds\n",
         (double) (endTime.tv_usec - beginTime.tv_usec) / 1000000 +
         (double) (endTime.tv_sec - beginTime.tv_sec));
	  
	free(ByteArray);
	printf("Program is done\n");
	powerOff();
    return 0;
}