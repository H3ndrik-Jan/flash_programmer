#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "hardware.h"

#define XT25F04B

#ifdef XT25F04B

#define CMD_WREN	0x06
#define CMD_WRDI	0x04
#define CMD_WENVOL	0x50
#define CMD_RDSR 0x05
#define CMD_WRSR 0x01
#define CMD_READ 0x03
#define CMD_FREAD 0x0B
#define CMD_PP 0x02
#define CMD_SE 0x20
#define CMD_BE 0xD8
#define CMD_CE 0xC7	//Unclear??
#define CMD_REMS	0x90
#define CMD_RDID 0x9F
#endif

#define FLASH_SIZE 512000

uint8_t readStatusRegister(void);
uint8_t writeStatusRegister(uint8_t data);

void enableWrite(void);
void disableWrite(void);

void chipErase(void);

void readData(uint32_t address, uint32_t length, uint8_t data[]);
void pageProgram(uint32_t address, uint32_t length, uint8_t data[]);
