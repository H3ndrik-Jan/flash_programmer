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
  
  
 //Compile with: gcc programmer.c hardware.c flash.c -Wall -o prg
 
void printHelp(){
	 printf("\nFlash programmer help menu\n\n");
	 printf("\t-d:\tDump file contents to console\n");
	 printf("\t-v:\tEnable verbose output\n");
	 printf("\t-f <filename>:\tWrite file to flash\n");
	 printf("\t-r:\tRead the flash and write this to a file\n");
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

void writeFile(filecont_t *inFile){
	size_t i = 0;
	while(i<inFile->_length-255){
		uint8_t temp[256];
		for(int j = 0; j<256; j++){
			temp[j] = inFile->_data[i+j];
		}
		
		while(readStatusRegister() & 0x01) usleep(1);	//ensure that WIP is low
		
		static bool printDebug = true;
		
		enableWrite();
		while(!(readStatusRegister() & 0x02)) {
			usleep(100);
			enableWrite();
		}
		if(printDebug){
			printDebug = false;
			printf("Status register: 0x%02X\n",readStatusRegister());
		}
		/*while(1){
			printf("0x%02X ", readStatusRegister());
			fflush(stdout);
			usleep(100000);
		}*/
		//printf("hebben jullie WEL beeld?\n");
		//ensure that WEL is high
		/*for(int k = 0; k<256; k++){
			printf("0x%02X ", temp[k]);
		}*/
		pageProgram(i, 256, temp);
		
		i+=256;
		for(int s = 0; s<0xFE; s++);	//wait for a little
	}
	
	if(i<inFile->_length){
		uint8_t remaining = inFile->_length-i;
		uint8_t temp[remaining];
		for(int j = 0; j<remaining; j++){
			temp[j] = inFile->_data[i+j];
		}
		
		while(readStatusRegister() & 0x01) usleep(1);
		enableWrite();
		pageProgram(i, remaining, temp);
		while(readStatusRegister() & 0x01) usleep(1);
	}
}

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
		 printf("File length: %zu\n", myFile->_length);
		 fflush(stdout);
	 }
	 
	 myFile->_data = calloc(myFile->_length, sizeof(uint8_t));
	 
	if(verboseOutput){
		 printf("Allocated local buffer\n");
		 fflush(stdout);
	}
	 
	 size_t succesBytes = fread(myFile->_data, sizeof(uint8_t), myFile->_length, inputFile);

	fclose(inputFile);
	//free(ByteArray);
	return succesBytes;
}

bool verifyFlash(filecont_t *pFile, bool verboseOutput){
		uint8_t inBuffer[FLASH_SIZE];
		if(verboseOutput){
			printf("Reading back the flash contents to verify the content\n");
			fflush(stdout);
		}
		readData(0,FLASH_SIZE,inBuffer);
		bool succes = true;
		for(size_t i = 0; i< pFile->_length; i++){
			if(inBuffer[i] != pFile->_data[i]){
				printf("Found mismatch at byte %zu!\nWriting the flash did likely not succeed\n", i);
				fflush(stdout);
				succes = false;
				break;
			}
		}
	return succes;
}

int main(int argc, char *argv[]) 
{
    int opt;

	char * inputFileName = malloc(sizeof(*inputFileName));
	
	bool fileIsProvided = false;
	bool verboseOutput = false;
	bool dumpFileContents = false;
	bool readTheFlashPlease = false;
	
	struct timeval beginTime, endTime;
	
	signal(SIGINT, signalHandler);
	
	gettimeofday(&beginTime, NULL);	//get begin time
	setupFlashProgrammer();
	powerOn();
	
	
    while((opt = getopt(argc, argv, ":if:drpvhx")) != -1) 
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
			case 'r':
					readTheFlashPlease = true;
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
	
	
	if(!fileIsProvided && !readTheFlashPlease){
		printf("Program failed: Please provide at least a read (-r) or write (-f <filename>) argument\n");
		exitProgrammer(0);
	}
	
		//read flash (to file)
	if(readTheFlashPlease){
		if(verboseOutput){
		printf("Reading flash contents\n");
		fflush(stdout);
		}
		
		uint8_t inBuffer[FLASH_SIZE];
		readData(0,FLASH_SIZE,inBuffer);
		
		FILE *outFile;
		printf("Writing flash contents to file\n");
		fflush(stdout);
		outFile = fopen("output.bin","wb");  // write binary file
		fwrite(inBuffer,sizeof(inBuffer),1,outFile);
		fclose(outFile);
		
		if(dumpFileContents){
			for(size_t i = 0; i<FLASH_SIZE; i++){
				printf("%d ", inBuffer[i]);
			}
		}
	}
	
	//write file to flash
	if(fileIsProvided){
		filecont_t inFile;
		inFile._fileName = malloc(sizeof(char)*100);	//beunmanier :-(
		sprintf(inFile._fileName , "%s", inputFileName);
		fflush(stdout);
		size_t size = readFileToBuffer(&inFile, verboseOutput);
		free(inputFileName);
		 
		if(verboseOutput){
			 printf("Copied %zu bytes from file to local buffer\n", size);
			 fflush(stdout);
	//		  for(uint16_t abc= 0; abc<1000; abc++){
	//			printf("%02X ", inFile._data[abc]);
	//		}
		}

		writeFile(&inFile);
		if(verboseOutput){
			printf("Wrote the local buffer to the flash\n");
			fflush(stdout);
		}
		
		if(verifyFlash(&inFile, verboseOutput)){
			printf("Written flash contents verified and OK\n");
			fflush(stdout);
		}
	}
	
	gettimeofday(&endTime, NULL);
	printf ("\nExecution took %f seconds\n",
         (double) (endTime.tv_usec - beginTime.tv_usec) / 1000000 +
         (double) (endTime.tv_sec - beginTime.tv_sec));
	  
	//free(ByteArray);
	printf("Program is done\n");
	powerOff();
    return 0;
}