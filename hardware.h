#define COMPUTEMODULE4

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>


void setupFlashProgrammer(void);

void powerOn(void);

void powerOff(void);

void writeCS(bool state);
void writeWP(bool state);
void writeHold(bool state);
uint8_t softSpiTransfer(uint8_t outByte);