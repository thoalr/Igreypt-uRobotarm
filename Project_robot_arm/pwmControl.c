/*
 * =============== pwmControl.c ================
 *      Author: thors_000
 *  Created on: 15 Mar 2018
 *     version: 2.4
 *     Comment: Contains functions for controlling PWM output
 *     Following is a table of possible inputs into the servos in degrees
 *     Servo 1:  0 - 180
 *     Servo 2: 10 -  90
 *     Servo 3: 80 - 220
 *     Servo 4:  0 - 180
 *     Servo 5:  0 - 180
 *     Claw position: 0 - Open, 1 - large width closed, 2 - small width closed
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

/* Board Header file */
#include "Board.h"

/* Our written headers  */
#include "essentialFxn.h"
#include "pwmControl.h"


/* Current position of servos */
uint16_t servoPos[numServo];
uint16_t servoInc  =   35; // pwm width
uint16_t servoSleep  =  35; // wait between increments
/*
 *  ======== mapRad2PWM ========
 *  Map radians to pwm width value
 */
uint16_t mapRad2PWM(double rad) {
    // matlab code @(x,a,b,c,d)   c+(x-a)*(d-c)/(b-a)
    return (uint16_t) max( min( (minPWMwidth + (rad*(maxPWMwidth-minPWMwidth)/(maxRad)) ), maxPWMwidth) ,minPWMwidth);
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
 *  Initializes the PWM modules for the board.
 */
void pwm_servo_init()
{
    //Set the clock
    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC |   SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    //Configure PWM Clock to match system
    SysCtlPWMClockSet(SYSCTL_PWMDIV_16); // set pwm clock to 1MHz

    // Enable the peripherals used by this program.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1);  // The Tiva Launchpad has two modules (0 and 1).
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);

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
    servoPos[5] = 0;  // Claw position is special
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
    uint16_t w;
    switch(n) {
    case 1:
        w = mapRad2PWM(rad);
        slowMoveServo(w, 1); // move in increments
        break;
    case 2:
        rad = min(rad,PI/2); // is max 90 degrees input value
        w = mapRad2PWM(rad);
        slowMoveServo(w, 2);
        break;
    case 3:
        w = mapRad2PWM(rad);
        slowMoveServo(w, 3);
        break;
    case 4:
        w = mapRad2PWM(rad);
        slowMoveServo(w, 4);
        break;
    case 5:
        rad = min(max(rad,0),PI);
        w = (uint16_t) (500 + (rad*(maxPWMwidth-500)/(maxRad)) );
        // small servo has different minimum pwm width
        setPWMwidth(w,5);
        servoPos[4] = w; // record servo 5 position
    }
}

/*
 *  ======== slowMoveServo ========
 *  Set the position of servo n in increments with wait time
 */
void slowMoveServo(uint16_t w, int n) {
    n = n-1; // decrement to use in array
    double inc = servoInc*signum(w-servoPos[n]); // sign of increment
    int count = 0;
    while( 1 ) {
        setPWMwidth(servoPos[n],n+1);
        // close enough put PWM to desired width
        if( absnum( w - servoPos[n] ) <= servoInc || count > 50)
        {
            setPWMwidth(w,n+1);
            servoPos[n] = w; // record servo position
            return;
        }
        servoPos[n] += inc;
        inc = servoInc*signum(w-servoPos[n]);
        waitIdle(servoSleep);
        count++;
    }
}



/*
 *  ======== moveClaw ========
 *  Set the position of claw.
 *  0: opened position.
 *  1: slightly closed position.
 *  2: more closed positon.
 */
void moveClaw(int pos) {
    servoPos[numServo-1] = pos;
    switch(pos) {
    case 0:
        PWMPulseWidthSet(PWM1_BASE, PWM_OUT_3,clawOpen);
        break;
    case 1:
        PWMPulseWidthSet(PWM1_BASE, PWM_OUT_3,clawClosed1);
        break;
    case 2:
        PWMPulseWidthSet(PWM1_BASE, PWM_OUT_3,clawClosed2);
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
 *  ======== moveHomePos ========
 *  Sets the positons of all servos to home position values
 */
void moveHomePos() {
    moveServo( mapPWM2Rad( servo5Home ), 5);
    moveServo( mapPWM2Rad( servo4Home ), 4);
    moveServo( mapPWM2Rad( servo2Home ), 2);
    moveServo( mapPWM2Rad( servo3Home ), 3);
    moveServo( mapPWM2Rad( servo1Home ), 1);
    // claw is open
    moveClaw(0);
}

/*
 *  ======== moveRobotPos ========
 *  Sets the positons of all servos to values contained in the Rpos struct pos
 *  mode = 0: move servos one at a time starting with highest highest servo
 *  mode = 1: move servos one at a time starting with lowest servo
 *  mode = 2: move servos all at once
 */
void moveRobotPos(Rpos pos, int mode) {
    waitIdle(pos.wait1);
    if( mode == 0) {
        moveClaw(  pos.claw);
        moveServo( pos.servo5, 5);
        moveServo( pos.servo4, 4);
        moveServo( pos.servo3, 3);
        moveServo( pos.servo2, 2);
        moveServo( pos.servo1, 1);
    }
    if( mode == 1) {
        moveServo( pos.servo1, 1);
        moveServo( pos.servo2, 2);
        moveServo( pos.servo3, 3);
        moveServo( pos.servo4, 4);
        moveServo( pos.servo5, 5);
        moveClaw(  pos.claw);
    }
    if( mode == 2) {
        uint16_t posV[] = {mapRad2PWM(pos.servo1),
                           mapRad2PWM(pos.servo2),
                           mapRad2PWM(pos.servo3),
                           mapRad2PWM(pos.servo4),
                           mapRad2PWM(pos.servo5)};
        int inc[] = {servoInc*signum(posV[0]-servoPos[0]),
                        servoInc*signum(posV[1]-servoPos[1]),
                        servoInc*signum(posV[2]-servoPos[2]),
                        servoInc*signum(posV[3]-servoPos[3])};
        int active[] = {1,1,1,1};
        int activeC = 4;
        int count = 0;
        while( activeC > 0 ) {
            PWMPulseWidthSet(PWM0_BASE, PWM_OUT_2,servoPos[0]);
            PWMPulseWidthSet(PWM0_BASE, PWM_OUT_3,servoPos[1]);
            PWMPulseWidthSet(PWM1_BASE, PWM_OUT_0,servoPos[2]);
            PWMPulseWidthSet(PWM1_BASE, PWM_OUT_1,servoPos[3]);
            int i;
            for(i=0; i < 4; i++) {
                servoPos[i] += inc[i];
                if( active[i] == 1 && absnum( posV[i] - servoPos[i] ) <= servoInc)
                {
                    setPWMwidth(posV[i],i+1);
                    servoPos[i] = posV[i];
                    active[i] = 0;
                    activeC--;
                    inc[i] = 0;
                }
                else if(active[i] == 1) {
                    inc[i] = servoInc*signum(posV[i]-servoPos[i]);
                }
            }
            //        if( active[1] == 1 && absnum( mapRad2PWM(pos.servo1) - servoPos[1] ) <= servoInc)
            //        {
            //            PWMPulseWidthSet(PWM0_BASE, PWM_OUT_3,mapRad2PWM(pos.servo1));
            //            servoPos[1] = mapRad2PWM(pos.servo1);
            //            active[1] = 0;
            //            activeC--;
            //        }
            //        else
            //            inc[1] = servoInc*signum(mapRad2PWM(pos.servo2)-servoPos[1])*active[1];
            if(activeC == 0  || count > 50)
                break;
            waitIdle(servoSleep);
            count++;
        }
        moveServo( pos.servo5, 5);
        moveClaw(pos.claw);
        servoPos[5] = pos.claw;
    }
    waitIdle(pos.wait2);
}

/*
 *  ======== setPWMwidth ========
 *  sets the PWM width of the specified servo pin
 */
void setPWMwidth(uint16_t width, int n) {
    switch(n) {
    case 1:
        PWMPulseWidthSet(PWM0_BASE, PWM_OUT_2,width);
        break;
    case 2:
        PWMPulseWidthSet(PWM0_BASE, PWM_OUT_3,width);
        break;
    case 3:
        PWMPulseWidthSet(PWM1_BASE, PWM_OUT_0,width);
        break;
    case 4:
        PWMPulseWidthSet(PWM1_BASE, PWM_OUT_1,width);
        break;
    case 5:
        PWMPulseWidthSet(PWM1_BASE, PWM_OUT_2,width);
        break;
    case 6:
        PWMPulseWidthSet(PWM1_BASE, PWM_OUT_3,width);
    }
}



/*
 *  ======== getRobotPos ========
 *  returns a Rpos struct with the current robot positions
 */
Rpos getRobotPos() {
    Rpos pos;
    pos.claw =  servoPos[5];
    pos.servo5 = mapPWM2Rad(servoPos[4]);
    pos.servo4 = mapPWM2Rad(servoPos[3]);
    pos.servo3 = mapPWM2Rad(servoPos[2]);
    pos.servo2 = mapPWM2Rad(servoPos[1]);
    pos.servo1 = mapPWM2Rad(servoPos[0]);
    pos.wait1 = 0;
    pos.wait2 = 0;
    return pos;
}


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



