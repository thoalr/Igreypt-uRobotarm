/*
 * =============== pwmControl.c ================
 *      Author: thors_000
 *  Created on: 15 Mar 2018
 *     version: 2.1
 *     Comment: Contains functions for controlling PWM output
 */
#include <stdbool.h>

/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/runtime/System.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>


#include <inc/hw_ints.h>
#include <inc/hw_memmap.h>
#include <inc/hw_types.h>
#include <inc/hw_gpio.h>

#include <driverlib/gpio.h>
#include <driverlib/pin_map.h>
#include <driverlib/sysctl.h>
//#include "driverlib/rom.h"
#include "driverlib/pwm.h"

#include "EK_TM4C123GXL.h"

/* TI-RTOS Header files */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/gpio/GPIOTiva.h>
//#include <ti/drivers/PWM.h>
//#include <ti/drivers/pwm/PWMTiva.h>
//#include <ti/drivers/I2C.h>
//#include <ti/drivers/i2c/I2CTiva.h>

/* Board Header file */
#include "Board.h"

/* Our written headers  */
#include "essentialFxn.h"
#include "pwmControl.h"


/* Current position of servos */
//uint16_t servoPos[0];
//uint16_t servoPos[1];
uint16_t servoPos[numServo];
uint16_t servoInc  =   50; // pwm width
uint16_t servoSleep  =  10; // wait between increments
//PWM_Handle servoH[0]; // Servo1
//PWM_Handle servoH2; // Servo2
//PWM_Handle servoH[numServo];
//PWM_Params params;
//PWM_Params params2;
//PWM_Params params3;

/*
 *  ======== mapRad2PWM ========
 *  Map radians to pwm width value
 */
uint16_t mapRad2PWM(double rad) {
    // matlab code @(x,a,b,c,d)   c+(x-a)*(d-c)/(b-a)
    return (uint16_t) (minPWMwidth + (rad*(maxPWMwidth-minPWMwidth)/(maxRad)) );
}

/*
 *  ======== mapPWM2Rad ========
 *  Map pwm width to radians
 */
double mapPWM2Rad(uint16_t width) {
    // matlab code @(x,a,b,c,d)   c+(x-a)*(d-c)/(b-a)
    return ((width-minPWMwidth)*maxRad)/(maxPWMwidth-minPWMwidth);
}

/*
 *  ========= pwm_servo_init =========
 *  Initializes the PWM modeules for the board.
 */
void pwm_servo_init()
{
    //Set the clock
    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC |   SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    //Configure PWM Clock to match system
    SysCtlPWMClockSet(SYSCTL_PWMDIV_16);

    // Enable the peripherals used by this program.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1);  // The Tiva Launchpad has two modules (0 and 1).
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);  // Module 1 covers the LED pins

    // Configure Pins to be PWM
    GPIOPinConfigure(GPIO_PD0_M1PWM0);
    GPIOPinConfigure(GPIO_PD1_M1PWM1);
    GPIOPinConfigure(GPIO_PA6_M1PWM2);
    GPIOPinConfigure(GPIO_PA7_M1PWM3);
    GPIOPinConfigure(GPIO_PB4_M0PWM2);
    GPIOPinConfigure(GPIO_PB5_M0PWM3);
    GPIOPinTypePWM(GPIO_PORTD_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    GPIOPinTypePWM(GPIO_PORTA_BASE, GPIO_PIN_6 | GPIO_PIN_7);
    GPIOPinTypePWM(GPIO_PORTB_BASE, GPIO_PIN_4 | GPIO_PIN_5);

    // Configure PWM Options
    // See page 207 4/11/13 DriverLib doc
    PWMGenConfigure(PWM1_BASE, PWM_GEN_0, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);
    PWMGenConfigure(PWM1_BASE, PWM_GEN_1, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);
    PWMGenConfigure(PWM0_BASE, PWM_GEN_1, PWM_GEN_MODE_DOWN | PWM_GEN_MODE_NO_SYNC);

    //Set the Period (expressed in clock ticks)
    PWMGenPeriodSet (PWM1_BASE, PWM_GEN_0, pwmPeriod);
    PWMGenPeriodSet (PWM1_BASE, PWM_GEN_1, pwmPeriod);
    PWMGenPeriodSet (PWM0_BASE, PWM_GEN_1, pwmPeriod);
    //Set PWM duty to home position
    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_2,servo1Home);
    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_3,servo2Home);
    PWMPulseWidthSet(PWM1_BASE, PWM_OUT_0,servo3Home);
    PWMPulseWidthSet(PWM1_BASE, PWM_OUT_1,servo4Home);
    PWMPulseWidthSet(PWM1_BASE, PWM_OUT_2,servo5Home);
    PWMPulseWidthSet(PWM1_BASE, PWM_OUT_3,clawOpen);
    servoPos[0] = servo1Home;
    servoPos[1] = servo2Home;
    servoPos[2] = servo3Home;
    servoPos[3] = servo4Home;
    servoPos[4] = servo5Home;

    // Enable the PWM generator
    PWMGenEnable(PWM1_BASE, PWM_GEN_0);
    PWMGenEnable(PWM1_BASE, PWM_GEN_1);
    PWMGenEnable(PWM0_BASE, PWM_GEN_1);

    // Turn on the Output pins
    PWMOutputState(PWM1_BASE, PWM_OUT_0_BIT | PWM_OUT_1_BIT | PWM_OUT_2_BIT | PWM_OUT_3_BIT, true);
    PWMOutputState(PWM0_BASE, PWM_OUT_2_BIT | PWM_OUT_3_BIT, true);

}

/*
 *  ======== moveServo ========
 *  Set the position of servo n
 */
void moveServo(double rad, int n) {
    rad = min(max(rad,0),PI);
    uint16_t w = mapRad2PWM(rad);
    switch(n) {
    case 1:
        slowMoveServo(PWM0_BASE, PWM_OUT_2, w, n);
        break;
    case 2:
        slowMoveServo(PWM0_BASE, PWM_OUT_3, w, n);
        break;
    case 3:
        slowMoveServo(PWM1_BASE, PWM_OUT_0, w, n);
        break;
    case 4:
        slowMoveServo(PWM1_BASE, PWM_OUT_1, w, n);
        break;
    case 5:
        w = (uint16_t) (500 + (rad*(maxPWMwidth-500)/(maxRad)) );
        PWMPulseWidthSet(PWM1_BASE, PWM_OUT_2,w);
        servoPos[n-1] = w;
    }
}

/*
 *  ======== slowMoveServo ========
 *  Set the position of servo n in increments with wait time
 */
void slowMoveServo(uint32_t ui32Base, uint32_t ui32PWMOut, uint16_t w, int n) {
    double inc = servoInc;
    n = n-1;
    uint16_t pos = servoPos[n];
    while( 1 ) {
        PWMPulseWidthSet(ui32Base, ui32PWMOut,pos);
        if( absnum( w - pos ) > servoInc )
            inc = servoInc*signum(w-pos);
        else if(inc < 10) {
            servoPos[n] = pos;
            return;
        }
        else
            inc = inc/2;
        pos += inc;
        waitIdle(servoSleep);
    }
}



/*
 *  ======== moveClaw ========
 *  Set the position of claw.
 *  0 means closed position.
 *  1 means opened position.
 */
void moveClaw(int pos) {
    switch(pos) {
    case 0:
        PWMPulseWidthSet(PWM1_BASE, PWM_OUT_3,clawClosed);
        break;
    case 1:
        PWMPulseWidthSet(PWM1_BASE, PWM_OUT_3,clawOpen);
    }
}



/*
 *  ======== getServoPOS ========
 *  Get the position of servo n
 */
double getServoPos(int n) {
    if(n<numServo)
        return min(max( mapPWM2Rad( servoPos[n]), 0),PI);
    else
        return PI/2;
}


/*
 *  ======== moveHomePOS ========
 *  Sets the positons of all servos to home position values
 */
void moveHomePos() {
    moveServo( mapPWM2Rad( servo1Home ), 1);
    moveServo( mapPWM2Rad( servo2Home ), 2);
    moveServo( mapPWM2Rad( servo3Home ), 3);
    moveServo( mapPWM2Rad( servo4Home ), 4);
    moveServo( mapPWM2Rad( servo5Home ), 5);
    // claw is open
    moveClaw(1);
}

///*
// *  ======== servoWaitT ========
// *  Returns a wait time necessary for the servo to move
// *  between the two positions given. For servo n.
// */
//int servoWaitT(double pos, int n) {
//    double tmp = pos - getServoPos(n);
//    if(tmp < 0)
//        tmp = -tmp;
//
//    uint16_t d = mapRad2PWM(tmp);
//    return 40*d;
//}

// old and decrepid

//    PWM_init(); // Allows the usage of pwm
//
//    // This is my part
//    PWM_Params_init(&params);
//    params.period = pwmPeriod;
//    servoH[0] = PWM_open(EK_TM4C123GXL_PWM6, &params);
//    if (servoH[0] == NULL) {
//        System_abort("Board_PWM0 did not open");
//    }
//    PWM_setDuty(servoH[0], servo1Home);
//    servoPos[0] = servo1Home;
//
//    params.polarity = PWM_POL_ACTIVE_HIGH;
//    servoH[1] = PWM_open(EK_TM4C123GXL_PWM7, &params);
//    if (servoH[1] == NULL) {
//        System_abort("Board_PWM1 did not open");
//    }
//    PWM_setDuty(servoH[1], servo2Home);
//    servoPos[1] = servo2Home;
//
//    // new pwm pins
//    params.polarity = PWM_POL_ACTIVE_HIGH;
//    servoH[2] = PWM_open(EK_TM4C123GXL_PWM0, &params);
//    if (servoH[2] == NULL) {
//        System_abort("Board_PWM1 did not open");
//    }
//    PWM_setDuty(servoH[2], servo3Home);
//    servoPos[2] = servo3Home;
//
//    params.polarity = PWM_POL_ACTIVE_HIGH;
//    servoH[3] = PWM_open(EK_TM4C123GXL_PWM1, &params);
//    if (servoH[3] == NULL) {
//        System_abort("Board_PWM1 did not open");
//    }
//    PWM_setDuty(servoH[3], servo4Home);
//    servoPos[3] = servo4Home;



