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
	GPIO_SET = (1<<PINPWRON) | (1<<PINSTATLED) | (1<<PINCS);
}

void powerOff(){
	GPIO_CLR = (1<<PINPWRON) | (1<<PINSTATLED) | (1<<PINCS) | (1<<PINHOLD) | (1<<PINSO) | (1<<PINCLK) | (1<<PINWP);
}