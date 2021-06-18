#include "hardware.h"
#include "BCM2711macros.h"
#include <time.h>
#include "pinout.h"

void setupFlashProgrammer(void){
	
	setup_io();
	
	INP_GPIO(PINWP);
	OUT_GPIO(PINWP);
	
	INP_GPIO(PINHOLD);
	OUT_GPIO(PINHOLD);
	
	INP_GPIO(PINSTATLED);
	OUT_GPIO(PINSTATLED);
	
	INP_GPIO(PINSI);
	OUT_GPIO(PINSI);
	
	INP_GPIO(PINPWRON);
	OUT_GPIO(PINPWRON);
	
	INP_GPIO(PINCLK);
	OUT_GPIO(PINCLK);
	
	INP_GPIO(PINCS);
	OUT_GPIO(PINCS);
	
	INP_GPIO(PINSO);
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

void writeWP(bool state){
	if(state){
		GPIO_SET = 1<<PINWP;
	}
	else{
		GPIO_CLR = 1<<PINWP;
	}
}

void writeHold(bool state){
	if(state){
		GPIO_SET = 1<<PINHOLD;
	}
	else{
		GPIO_CLR = 1<<PINHOLD;
	}
}

uint8_t softSpiTransfer(uint8_t outByte){
	uint8_t inByte = 0;
	for(uint8_t i = 0; i<0xFE; i++);//wait
	
	for(int8_t index = 7; index>=0; index--){
		if((outByte>>index) & 0x01){
			GPIO_SET = 1<<PINSI;
		}
		else {
			GPIO_CLR = 1<<PINSI;
		}

		GPIO_CLR = 1<<PINCLK;
		for(uint8_t i = 0; i<70; i++);	//for loop to act as delay
		GPIO_SET = 1<<PINCLK;
		for(uint8_t i = 0; i<70; i++);
		inByte<<=1;
		inByte |= (GET_GPIO(PINSO)?0x01:0x00);
	}
	
	
	return inByte;
}