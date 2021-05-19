#include <stdio.h> 
#include <unistd.h> 
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>
 
#define COMPUTEMODULE4

#include "BCM2711macros.h"

#include "pinout.h"
  
void printHelp(){
	 printf("\nFlash programmer help menu\n\n");
	 printf("\t-d:\tDump file contents to console\n");
	 printf("\t-v:\tEnable verbose output\n");
	 printf("\t-f:\tFilename\n");
	 printf("\t-h:\tPrint this help menu\n");
	 fflush(stdout);
}

void setupFlashProgrammer(void){
	
	setup_io();
	
	INP_GPIO(PINWP);
	OUT_GPIO(PINWP);
	
	INP_GPIO(PINHOLD);
	OUT_GPIO(PINHOLD);
	
	INP_GPIO(PINSTATLED);
	OUT_GPIO(PINSTATLED);
	
	INP_GPIO(PINSO);
	OUT_GPIO(PINSO);
	
	INP_GPIO(PINPWRON);
	OUT_GPIO(PINPWRON);
	
	INP_GPIO(PINCLK);
	OUT_GPIO(PINCLK);
	
	INP_GPIO(PINCS);
	OUT_GPIO(PINCS);
	
	INP_GPIO(PINSI);
	//Serial input pin...
}

void powerOn(void){
	GPIO_SET = (1<<PINPWRON) | (1<<PINSTATLED);
}

void powerOff(){
	GPIO_CLR = (1<<PINPWRON) | (1<<PINSTATLED);
}

int main(int argc, char *argv[]) 
{
    int opt;
	setupFlashProgrammer();
	powerOn();
	char * inputFileName = malloc(sizeof(*inputFileName));
	
	bool fileIsProvided = false;
	bool verboseOutput = false;
	bool dumpFileContents = false;
	
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
				exit(0);
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
		exit(0);
	}
	
	 FILE *inputFile;
	 
	 inputFile = fopen(inputFileName, "rb");
	 if(inputFile == NULL){
		 perror("Opening file failed");
		 exit(0);
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
	
	if(dumpFileContents){	
		 for (size_t i = 0; i < size; i++) {
				printf("%02X ", ByteArray[i]);
		 }
	}
	  
	free(ByteArray);
	printf("Program is done\n");
	powerOff();
    return 0;
}