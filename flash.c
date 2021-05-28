#include "flash.h"

uint8_t readStatusRegister(void){
	writeCS(0);
	softSpiTransfer(CMD_RDSR);
	uint8_t status = softSpiTransfer(0x00);
	writeCS(1);
	return status;
}

uint8_t writeStatusRegister(uint8_t data){
	writeCS(0);
	softSpiTransfer(CMD_WRSR);
	uint8_t status = softSpiTransfer(data);
	writeCS(1);
	return status;
}

void enableWrite(void){
	writeCS(0);
	softSpiTransfer(CMD_WREN);
	writeCS(1);
}

void disableWrite(void){
	writeCS(0);
	softSpiTransfer(CMD_WRDI);
	writeCS(1);
}

void readData(uint32_t address, uint32_t length, uint8_t data[]){
	writeCS(0);
	softSpiTransfer(CMD_READ);
	softSpiTransfer((address>>16) & 0xFF);
	softSpiTransfer((address>>8) & 0xFF);
	softSpiTransfer(address & 0xFF);
	
	for(uint32_t i = 0; i<length; i++){
		data[i] = softSpiTransfer(0x00);
	}
	
	writeCS(1);
}

void pageProgram(uint32_t address, uint32_t length, uint8_t data[]){
	writeCS(0);
	softSpiTransfer(CMD_PP);
	softSpiTransfer((address>>16) & 0xFF);
	softSpiTransfer((address>>8) & 0xFF);
	softSpiTransfer(address & 0xFF);
	
	for(uint32_t i = 0; i<length; i++){
		softSpiTransfer(data[i]);
	}
	
	writeCS(1);
}