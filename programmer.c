#include <stdio.h> 
#include <unistd.h> 
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>
#include <signal.h>
#include <sys/time.h>
 
#include "hardware.h"
#include "flash.h"
  
  
 //Compile with: gcc programmer.c hardware.c -Wall -o prg
 
void printHelp(){
	 printf("\nFlash programmer help menu\n\n");
	 printf("\t-d:\tDump file contents to console\n");
	 printf("\t-v:\tEnable verbose output\n");
	 printf("\t-f:\tFilename\n");
	 printf("\t-h:\tPrint this help menu\n");
	 fflush(stdout);
}



void exitProgrammer(int status){
	
	powerOff();
	
	exit(status);
}

void signalHandler(int signo){
	if(signo == SIGINT || signo == SIGKILL || signo == SIGSTOP){
		exitProgrammer(0);
	}
}


typedef struct{
	
	uint8_t *_data;
	char * _fileName;
	size_t _length;
	
} filecont_t;



size_t readFileToBuffer(filecont_t *myFile, bool verboseOutput){
	 FILE *inputFile;
	 
	 inputFile = fopen(myFile->_fileName, "rb");
	 if(inputFile == NULL){
		 perror("Opening file failed");
		 exitProgrammer(0);
	 }
	 else if(verboseOutput){
		 printf("Opening file was succesful\n");
		 fflush(stdout);
	 }
	 
	 
	 fseek(inputFile, 0L, SEEK_END);
	 myFile->_length = ftell(inputFile);
	 fseek(inputFile, 0L, SEEK_SET);
	 if(verboseOutput){
		 printf("File length: %I64u\n", myFile->_length);
		 fflush(stdout);
	 }
	 
	 myFile->_data = malloc(sizeof(uint8_t)*myFile->_length);
	 
	if(verboseOutput){
		 printf("Allocated local buffer\n");
		 fflush(stdout);
	}
	 
	 size_t succesBytes = fread(myFile->_data, sizeof(uint8_t), myFile->_length, inputFile);

	fclose(inputFile);
	//free(ByteArray);
	return succesBytes;
}

int main(int argc, char *argv[]) 
{
    int opt;

	char * inputFileName = malloc(sizeof(*inputFileName));
	
	bool fileIsProvided = false;
	bool verboseOutput = false;
	bool dumpFileContents = false;
	
	struct timeval beginTime, endTime;
	
	signal(SIGINT, signalHandler);
	
	gettimeofday(&beginTime, NULL);	//get begin time
	setupFlashProgrammer();
	powerOn();
	
	
    while((opt = getopt(argc, argv, ":if:dpvhx")) != -1) 
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
				exitProgrammer(0);
                break; 
			case 'p':
					
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
		exitProgrammer(0);
	}
	
	filecont_t inFile;
	inFile._fileName = malloc(sizeof(char)*100);
	sprintf(inFile._fileName , "%s", inputFileName);
	//fileSize = getFileSize(inputFileName);
	printf("copied filename: %s\n", inFile._fileName);
	fflush(stdout);
	//uint8_t *ByteArray = malloc(sizeof(uint8_t)*fileSize);
	size_t size = readFileToBuffer(&inFile, verboseOutput);

	free(inputFileName);
	 
	if(verboseOutput){
		 printf("Copied %I64u bytes from file to local buffer\n", size);
		 fflush(stdout);
	}
	
	softSpiTransfer(0xAA);
	//write to flash chip
	size_t i = 0;
	while(i<inFile._length-255){

		uint8_t temp[256];
		for(int j = 0; j<256; j++){
			temp[j] = inFile._data[i+j];
		}
		enableWrite();
		pageProgram(i, 256, temp);
		i+=256;
	}
	
	if(i<inFile._length){
		uint8_t remaining = inFile._length-i;
		uint8_t temp[remaining];
		for(int j = 0; j<remaining; j++){
			temp[j] = inFile._data[i+j];
		}
		enableWrite();
		pageProgram(i, remaining, temp);
	}
	

	
	gettimeofday(&endTime, NULL);
	printf ("Execution took %f seconds\n",
         (double) (endTime.tv_usec - beginTime.tv_usec) / 1000000 +
         (double) (endTime.tv_sec - beginTime.tv_sec));
	  
	//free(ByteArray);
	printf("Program is done\n");
	powerOff();
    return 0;
}