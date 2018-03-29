/*
 * =============== pwmControl.c ================
 *      Author: thors_000
 *  Created on: 15 Mar 2018
 *     version: 1.0
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

#include "EK_TM4C123GXL.h"

/* TI-RTOS Header files */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/gpio/GPIOTiva.h>
#include <ti/drivers/PWM.h>
#include <ti/drivers/pwm/PWMTiva.h>
#include <ti/drivers/I2C.h>
#include <ti/drivers/i2c/I2CTiva.h>

/* Board Header file */
#include "Board.h"

/* Our written headers  */
#include "essentialFxn.h"
#include "pwmControl.h"


/* Current position of servos */
uint16_t servo1Pos;
uint16_t servo2Pos;


PWM_Handle servoH1; // Servo1
PWM_Handle servoH2; // Servo2
PWM_Params params;

#define servo1Home  mapRad2PWM(PI/2) // pi/2, pi/3 for minpwmwidth 350
#define servo2Home  mapRad2PWM(PI/3)

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
 *  ======== pwmLEDinit ========
 *  Initializes the PWM modeules for the on board LED.
 */
void pwm_servo_init()
{
    //this is Board_initPWM()
    /* Enable PWM peripherals */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1);
    /*
     * Enable PWM output on GPIO PIns.  Board_LED1 and Board_LED2 are now
     * controlled by PWM peripheral - Do not use GPIO APIs.
     */
    GPIOPinConfigure(GPIO_PF2_M1PWM6);
    GPIOPinConfigure(GPIO_PF3_M1PWM7);
    GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_2 |GPIO_PIN_3);

//    GPIOPinConfigure(GPIO_PB6_M?PWM?);
//    GPIOPinConfigure(GPIO_PB?_M?PWM?);
//    GPIOPinTypePWM(GPIO_PORTB_BASE, GPIO_PIN_6 |GPIO_PIN_?);

    PWM_init(); // Allows the usage of pwm

    // This is my part
    PWM_Params_init(&params);
    params.period = pwmPeriod;
    servoH1 = PWM_open(Board_PWM0, &params);
    if (servoH1 == NULL) {
        System_abort("Board_PWM0 did not open");
    }
    PWM_setDuty(servoH1, servo1Home);
    servo1Pos = servo1Home;

    if (Board_PWM1 != Board_PWM0) {
        params.polarity = PWM_POL_ACTIVE_HIGH;
        servoH2 = PWM_open(Board_PWM1, &params);
        if (servoH2 == NULL) {
            System_abort("Board_PWM1 did not open");
        }
    }

    PWM_setDuty(servoH2, servo2Home);
    servo2Pos = servo2Home;
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
        PWM_setDuty(servoH1, w);
        servo1Pos = w;
        break;
    case 2:
        PWM_setDuty(servoH2, w);
        servo2Pos = w;
        break;
    }
}


/*
 *  ======== getServoPOS ========
 *  Get the position of servo n
 */
double getServoPos(int n) {
    switch(n) {
    case 1:
        return min(max( mapPWM2Rad( servo1Pos), 0),PI);
    case 2:
        return min(max( mapPWM2Rad( servo2Pos), 0),PI);
    default:
        return 0;
    }
}


/*
 *  ======== moveHomePOS ========
 *  Sets the positons of all servos to home position values
 */
void moveHomePos() {
    moveServo( mapPWM2Rad( servo1Home ), 1);
    moveServo( mapPWM2Rad( servo2Home ), 2);
}

/*
 *  ======== servoWaitT ========
 *  Returns a wait time necessary for the servo to move
 *  between the two positions given. For servo n.
 */
int servoWaitT(double pos, int n) {
    double tmp = pos - getServoPos(n);
    if(tmp < 0)
        tmp = -tmp;

    uint16_t d = mapRad2PWM(tmp);
    return 40*d;
}


