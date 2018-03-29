/*
 * ================ empty.c ======================
 *      Author: thors_000
 *  Created on: 1 Mar 2018
 *     version: 2.2
 *     Comment: Contains any functions not exclusive to this project
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
// #include <ti/drivers/UART.h>

/* Board Header file */
#include "Board.h"

/* Our written headers  */
#include "essentialFxn.h"
#include "pwmControl.h"

/* Task parameters */
#define TASKSTACKSIZE   512
Task_Struct tsk0Struct;
UInt8 tsk0Stack[TASKSTACKSIZE];
Task_Handle task;

/* Flag bitfield structure to use to control programm */
struct Flag{
    unsigned int running; // 1: Run loop programm  0: pause programm
    unsigned int stopp; // 1: programm stopped. Needs to restart
} flag;

/* PWM duty variables */
uint16_t   duty1 = (uint16_t) minPWMwidth;
uint16_t   duty2 = (uint16_t) minPWMwidth;
uint16_t   dutyInc = 40;
/* Global variables */
int count1 = 0;
int count2 = 0;

/*
 *  ======== gpioButtonFxn0 ========
 *  Callback function for the GPIO interrupt on Board_BUTTON0.
 */
void gpioButtonFxn0(unsigned int index)
{
    waitIdle(80);
    if(flag.running == 0)
        flag.running = 1;
    else //if(flag.running == 1)
        flag.running = 0;
}

/*
 *  ======== gpioButtonFxn1 ========
 *  Callback function for the GPIO interrupt on Board_BUTTON1.
 *  This may not be used for all boards.
 */
void gpioButtonFxn1(unsigned int index)
{
    flag.stopp = 1;
    moveHomePos();
    // Suspend any other tasks that change pwm output HOW ?????
    while(1) {}
}

/*
 *  ======== heartBeatFxn ========
 *  Toggle the Board_LED0. The Task_sleep is determined by arg0 which
 *  is configured for the heartBeat Task instance.
 */
//Void heartBeatFxn(UArg arg0, UArg arg1)
//{
//    while (1) {
//        Task_sleep((UInt)arg0);
//        GPIO_toggle(Board_LED0);
//    }
//}

void loop(UArg arg0, UArg arg1) {
//    double offset1 = 0;
//    double offset2 = 0;
    double inc1 = 1;
    double inc2 = 1;
    double pos1 = 0;
    double pos2 = 0;
    double posV1[] = {0, PI/8, PI/6, PI/4, PI/2, 2*PI/3, 3*PI/4 ,5*PI/6, PI};
    double posV2[] = {0, PI/4, PI/2};
    while(1) {
//        System_printf("wait\n");
//        System_flush();
        while(flag.running == 0) {
//            Task_sleep((UInt) 400 );
//            System_printf("flag = %d\n",flag.running);
//            System_flush();
        }
//
        int i1 = 0;
        int i2 = 0;
//        System_printf("start\n");
//        System_flush();
        while(flag.running == 1 && flag.stopp == 0) {
            moveServo(posV1[i1],1); // move servo 1
            moveServo(posV2[i2],2); // move servo 2
            Task_sleep((UInt) 800 ); //max(servoWaitT(posV1[i1],1), servoWaitT(posV2[i2],2) ) );
            i1 = (i1+1) % 9;
            i2 = (i2+1) % 3;
        }
        System_printf("wait2\n");
        System_flush();
        while(flag.running == 0) {}
        System_printf("start2\n");
        System_flush();
        double n = 1;
        while(flag.running == 1 && flag.stopp == 0) {
//            System_printf("degrees = %f\n",rad2deg(rad));
//            System_printf("duty = %d\n",(int) duty1);
//            System_flush();
            moveServo(pos1,1); // move servo 1
            moveServo(pos2,2); // move servo 2
            Task_sleep((UInt) 800); // max(servoWaitT(pos1,1), servoWaitT(pos2,2) ) );

            inc1 = PI/n;
            inc2 = PI/(2*n);
            int i = 0;
            for(i = 0;i <= n; i++) {
                moveServo(i*inc1,1); // move servo 1
                Task_sleep((UInt) 400);
                moveServo(i*inc2,2); // move servo 2
                Task_sleep((UInt) 400); // max(servoWaitT(pos1,1), servoWaitT(pos2,2) ) );
            }
            n *= 2;
            if(n > 60)
                n = 1;
//            pos1 = getServoPos(1);
//            pos2 = getServoPos(2);
//            pos1 += PI/inc1;
//            pos2 += PI/(2*inc2);

            //            duty1 = inc*count1+minPWMwidth;
//            //        if (count2++ >= 10) {
//            //            count2 = 1;
//            //        }
//            //        duty2 = dutyInc*count2 +500;
//            count1++;
//            //        count2++;
//            if( inc < 10) {
//                inc = 430;
//            }
            //Task_sleep((UInt) (arg0 *(inc*2/430)) );
        }
//        System_printf("\nduty = %d\n",(int) duty1);
//        System_flush();
//        Task_sleep((UInt) (arg0*10));
    }
}


/*
 *  ======== main ========
 */
int main(void)
{
    Task_Params tskParams;

    /* Call board init functions */
    Board_initGeneral();
    Board_initGPIO();
    Board_initUART();
    // Board_initUSB(Board_USBDEVICE);
    // Board_initWatchdog();
    // Board_initPWM();

    /* Initialize pwm for onboard LEDs */ // Also rename this once PWM pins are moved
    pwm_servo_init();

    /* Construct LED Task thread */
    Task_Params_init(&tskParams);
    tskParams.stackSize = TASKSTACKSIZE;
    tskParams.stack = &tsk0Stack;
    tskParams.arg0 = 400; // sleep time
    Task_construct(&tsk0Struct, (Task_FuncPtr)loop, &tskParams, NULL);

    /* Obtain instance handle */
    //task = Task_handle(&tsk0Struct);

    System_printf("Starting the example\nSystem provider is set to SysMin. "
            "Halt the target to view any SysMin contents in ROV.\n");
    /* SysMin will only print to the console when you call flush or exit */
    System_flush();

    /* install Button callback */
    GPIO_setCallback(Board_BUTTON0, gpioButtonFxn0);
    /* Enable interrupts */
    GPIO_enableInt(Board_BUTTON0);
    /*
     *  If more than one input PIn is available for your device, interrupts
     *  will be enabled on Board_BUTTON1.
     */
    if (Board_BUTTON0 != Board_BUTTON1) {
        /* install Button callback */
        GPIO_setCallback(Board_BUTTON1, gpioButtonFxn1);
        GPIO_enableInt(Board_BUTTON1);
    }
    /* Initialize flag variables */
    flag.running = 0;
    flag.stopp = 0;

    /* Start BIOS */
    BIOS_start();

    return (0);
}
