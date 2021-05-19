#include "hardware.h"
#include "BCM2711macros.h"

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
		if((outByte>>index) & 0x01) GPIO_SET = 1<<PINSO;
		else GPIO_CLR = 1<<PINSO;

		GPIO_CLR = 1<<PINCLK;
		GPIO_SET = 1<<PINCLK;
		
		readVal |= (GET_GPIO(PINSI)?0x01:0x00)<<index;
	}
	
	return inByte;
}