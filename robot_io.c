/**************************************************************************//**
 * @file      robot_io.c
 * @brief     Contains functions for controlling the servos on a robotic arm
 * @version   V0.01
 * @date      6. Feb. 2018
 * @lastdate  6. Feb. 2018
 * @note      Implements the PWM contoll of servos
 ******************************************************************************/

#include "tm4c123gh6pm.h"
#include <stdint.h>
#include "robot_io.h"
#include "driverlib/pwm.h"

//********************************************************************************
//*************************         PWM section          *************************
//********************************************************************************

// Pins for PWM ?
// pin M0PWM0


uint32_t deg2width( double deg) {
	if( deg > 180)
		return 1;
	if ( deg < 0)
		return 0;
	uint32_t val = (_MaxDeg - _MinDeg) *(deg/180.0) + _MinDeg;
	return val;
}


// Take in degrees in radians probably and convert to PWM


//********************************************************************************
//*************************         PWM section ENDs         *********************
//********************************************************************************


//********************************************************************************
//******************           Servo control section            ******************
//********************************************************************************

void initServo0() {
	//PWM_OUT_0
	// what pin?
	//PWM0_Base
	//PWMGenEnable(uint32_t ui32Base, uint32_t ui32Gen)
	//PWMGenPeriodSet(uint32_t ui32Base, uint32_t ui32Gen,
    //            uint32_t ui32Period)
	
}

void servo0deg(double deg) {
	uint32_t width = deg2width(deg);
	//PWMPulseWidthSet(PWM0_BASE, PWM_OUT_0, width);  // PWM_OUT_0 ???? what pin
}


// Function that moves the desired distance in desired steps or speed or degrees


// Functions that move only one or any or all of the servos at a time



//********************************************************************************
//******************         Servo control section ENDs         ******************
//********************************************************************************



#define PWM0_ENABLE_R           (*((volatile uint32_t *)0x40028008))
#define PWM_ENABLE_PWM0EN       0x00000001  // PWM0 Output Enable
#define PWM0_0_CTL_R            (*((volatile uint32_t *)0x40028040))
#define PWM_0_CTL_ENABLE        0x00000001  // PWM Block Enable
#define PWM0_0_LOAD_R           (*((volatile uint32_t *)0x40028050))
#define PWM0_0_CMPA_R           (*((volatile uint32_t *)0x40028058))
#define PWM0_0_GENA_R           (*((volatile uint32_t *)0x40028060))
#define PWM_0_GENA_ACTCMPAD_ONE 0x000000C0  // Set the output signal to 1
#define PWM_0_GENA_ACTLOAD_ZERO 0x00000008  // Set the output signal to 0
#define GPIO_PORTB_AFSEL_R      (*((volatile uint32_t *)0x40005420))
#define GPIO_PORTB_ODR_R        (*((volatile uint32_t *)0x4000550C))
#define GPIO_PORTB_DEN_R        (*((volatile uint32_t *)0x4000551C))
#define GPIO_PORTB_AMSEL_R      (*((volatile uint32_t *)0x40005528))
#define GPIO_PORTB_PCTL_R       (*((volatile uint32_t *)0x4000552C))
#define SYSCTL_RCC_R            (*((volatile uint32_t *)0x400FE060))
#define SYSCTL_RCC_USEPWMDIV    0x00100000  // Enable PWM Clock Divisor
#define SYSCTL_RCC_PWMDIV_M     0x000E0000  // PWM Unit Clock Divisor
#define SYSCTL_RCC_PWMDIV_2     0x00000000  // PWM clock /2
#define SYSCTL_RCGCGPIO_R       (*((volatile uint32_t *)0x400FE608))
#define SYSCTL_RCGCGPIO_R1      0x00000002  // GPIO Port B Run Mode Clock
                                            // Gating Control
#define SYSCTL_RCGCPWM_R        (*((volatile uint32_t *)0x400FE640))
#define SYSCTL_RCGCPWM_R0       0x00000001  // PWM Module 0 Run Mode Clock
                                            // Gating Control
#define SYSCTL_PRGPIO_R         (*((volatile uint32_t *)0x400FEA08))
#define SYSCTL_PRGPIO_R1        0x00000002  // GPIO Port B Peripheral Ready
#define SYSCTL_PRPWM_R          (*((volatile uint32_t *)0x400FEA40))
#define SYSCTL_PRPWM_R0         0x00000001  // PWM Module 0 Peripheral Ready

// period is 16-bit number of PWM clock cycles in one period (3<=period)
// duty is number of PWM clock cycles output is high  (2<=duty<=period-1)
// PWM clock rate = processor clock rate/SYSCTL_RCC_PWMDIV
//                = BusClock/2
//                = 50 MHz/2 = 25 MHz (in this example)
void PWM0A_Init(uint16_t period, uint16_t duty){
                                   // 1) activate clock for PWM0
  SYSCTL_RCGCPWM_R |= SYSCTL_RCGCPWM_R0;
                                   // allow time to finish activating
  while((SYSCTL_PRPWM_R&SYSCTL_PRPWM_R0)==0){};
                                   // activate clock for Port B
  SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R1;
                                   // allow time to finish activating
  while((SYSCTL_PRGPIO_R&SYSCTL_PRGPIO_R1)==0){};
  GPIO_PORTB_AFSEL_R |= 0x40;      // 2) enable alt funct on PB6
  GPIO_PORTB_ODR_R &= ~0x40;       //    disable open drain on PB6
  GPIO_PORTB_DEN_R |= 0x40;        //    enable digital I/O on PB6
  GPIO_PORTB_AMSEL_R &= ~0x40;     //    disable analog function on PB6
                                   //    configure PB6 as PWM
  GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R&0xF0FFFFFF)+0x04000000;
  SYSCTL_RCC_R = SYSCTL_RCC_USEPWMDIV |        // 3) use PWM divider
    ((SYSCTL_RCC_R & (~SYSCTL_RCC_PWMDIV_M)) + //    clear PWM divider field
    SYSCTL_RCC_PWMDIV_2);                      //    configure for /2 divider
  PWM0_0_CTL_R = 0;                // 4) re-loading down-counting mode
                                   //    PB6 goes low on LOAD
                                   //    PB6 goes high on CMPA down
  PWM0_0_GENA_R = (PWM_0_GENA_ACTCMPAD_ONE|PWM_0_GENA_ACTLOAD_ZERO);
  PWM0_0_LOAD_R = period - 1;      // 5) cycles needed to count down to 0
  PWM0_0_CMPA_R = duty - 1;        // 6) count value when output rises
  PWM0_0_CTL_R |= PWM_0_CTL_ENABLE;// 7) start PWM0 Generator 0
  PWM0_ENABLE_R |= PWM_ENABLE_PWM0EN;//  enable PWM0 Generator 0
}
// change duty cycle
// duty is number of PWM clock cycles output is high  (2<=duty<=period-1)
void PWM0A_Duty(uint16_t duty){
  PWM0_0_CMPA_R = duty - 1;        // 6) count value when output rises
}