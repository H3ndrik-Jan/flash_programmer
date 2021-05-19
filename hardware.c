#include "hardware.h"
#include "BCM2711macros.h"
#include <time.h>


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
	usleep(1);
	GPIO_SET = (1<<PINCS) | (1<<PINCLK) ;
	
}

void powerOff(){
	GPIO_CLR = (1<<PINPWRON) | (1<<PINSTATLED) | (1<<PINCS) | (1<<PINHOLD) | (1<<PINSO) | (1<<PINCLK) | (1<<PINWP);
}

void writeCS(bool state){
	if(state){
		GPIO_SET = 1<<PINCS;
	}
	else{
		GPIO_CLR = 1<<PINCS;
	}
}

uint8_t softSpiTransfer(uint8_t outByte){
	uint8_t inByte = 0;
	
	//read current value
	for(uint8_t index = 0; index<8; index++){
		if((outByte>>index) & 0x01) GPIO_SET = 1<<PINSI;
		else GPIO_CLR = 1<<PINSI;

		GPIO_CLR = 1<<PINCLK;
		for(uint16_t i = 0; i<0xFFFE; i++);
		GPIO_SET = 1<<PINCLK;
		for(uint16_t i = 0; i<0xFFFE; i++);
		inByte |= (GET_GPIO(PINSO)?0x01:0x00)<<index;
	}
	
	return inByte;
}