#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "hardware.h"

#define XT25F04B

#ifdef XT25F04B

#define CMD_WEN	0x06
#define CMD_WDIS	0x04
#define CMD_WENVOL	0x50
#define CMD_READSTATUS 0x05
#define CMD_WRITESTATUS 0x01
#define CMD_RDAT 0x03
#define CMD_FREAD 0x08
#define CMD_PAGEPRG 0x02
#define CMD_SECERASE 0x20
#define CMD_BLCKERASE 0xD8
#define CMD_CHIPERASE 0xC7	//Unclear??
#define CMD_ID	0x90
#define CMD_RID 0x9F
#endif

uint8_t readStatusRegister(void);
uint8_t writeStatusRegister(uint8_t data);

void enableWrite(void);
void disableWrite(void);

void readData(uint32_t address, uint32_t length, uint8_t data[]);
