/**************************************************************************//**
 * @file      main.c
 * @brief     Contains the highest level code of the robotarm project
 *            and calls all other code
 * @version   V0.01
 * @startdate 6. Feb. 2018
 * @lastdate  6. Feb. 2018
 * @note      Not finished
 ******************************************************************************/
//#include "TM4C123.h"                    // Device header
#include "tm4c123gh6pm.h"
#include <stdint.h>
//#include "Texas.h"
//#include "CortexM.h"
//#include "RTE_Components.h"             // Component selection
// cannot include .c files
#include "stdf.h"
//#include "robotf.c"
#include "robot_io.h"
//#include "other_io.c"
#include "driverlib/pwm.h"

// state of running code ie process - P
// state of robot arm - R
// state of claw - C
//enum state {
//	workingP,runningP,errorP,restartingP,stoppedP,pausedP,	
//	movingR,errrorR,pausedR,stoppedR,homeposR,shutdownR,
//	openC,closedC,errorC
//};

int main() {
	// For small servo period = 0x0008 and duty = 0x0002 is the only thing that seems to work
	// However goes from 0 to 180 in each step
	//int pwmcl = SysCtlPWMClockGet();
	
	uint16_t period = 0x0000018f;
	uint16_t duty = 0x0000000a;
	uint16_t increase = 0x00000005;
	PWM0A_Init(period, duty);
	delayMs(1000);
	int count = 0;
	while(count < 10) {
		
		PWM0A_Duty( duty);
		delayMs(1000);
		if( duty > 0x00000032)
			duty = 0x0000000a  - increase;
		duty += increase;
		count++;
	}
	
	while(1) {
		// stops unexpected behaviour when programm finishes
		// by making the led blinck
		// enable clock to GPIOF at clock gating control register
		SYSCTL->RCGCGPIO |= 0x20;
		// enable the GPIO pins for the LED (PF3, 2 1) as output
		GPIOF->DIR = 0x0e;
		// enable the GPIO pins for digital function
		GPIOF->DEN = 0x0e;
		int i = 0x02;
		while(1)
		{
			// red 0x02 blue 0x04 
			GPIOF->DATA = i;     // turn on red LED
			delayMs(1000);
        
			GPIOF->DATA = 0;        // turn off red LED
			delayMs(500); 
//			i = i<<1;
			i += 2;
			if(i >= 15) i = 2;
//			if(i > 9) i = 3;
//			else if(i > 8) i = 2;
		}
	}
}
