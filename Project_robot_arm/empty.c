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
//Task_Struct tsk1Struct;
//UInt8 tsk1Stack[TASKSTACKSIZE];
//Task_Handle task1;

/* Flag bitfield structure to use to control programm */
struct Flag{
    unsigned int running; // 1: Run loop programm  0: pause programm
    unsigned int stopp; // 1: programm stopped. Needs to restart
    unsigned int clawPos; // 1: claw is open     0: claw is closed
} flag;

/* Global variables */


// Function declaration
void ledFxn();

/*
 *  ======== gpioButtonFxn0 ========
 *  Callback function for the GPIO interrupt on Board_BUTTON0.
 */
void gpioButtonFxn0(unsigned int index)
{
    waitIdle(80);
    //count1 = min(count1+1,4);
    flag.running = invertInt(flag.running);
    ledFxn();
}

/*
 *  ======== gpioButtonFxn1 ========
 *  Callback function for the GPIO interrupt on Board_BUTTON1.
 *  This may not be used for all boards.
 */
void gpioButtonFxn1(unsigned int index)
{
    waitIdle(80);
    //count1 = max(count1-1,0);
    flag.stopp = 1;
    flag.running = 0;
    ledFxn();
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


//void moveToHomePos() {
//    double inc = 0.1;
//    double pos = PI;
//    double v[] = {0, PI/4, PI/2, PI*3/4, PI};
//    while( pos > 0) {
//        moveServo(pos,0);
//        pos -= inc;
//        Task_sleep((UInt) 200 );
//    }
//    while( 1 ) {
//        moveServo(pos,0);
//        if( absnum( v[count1] - pos ) > 0.1 )
//            inc = 0.1*signum(v[count1]-pos);
//        else
//            inc = 0;
//        pos += inc;
//        Task_sleep((UInt) 200 );
//        if(count1 == 1)
//            moveServo(PI*1.2/4,1);
//        if(count1 == 0)
//            moveServo(PI*1.6/4,1);
//    }
//}



void loop(UArg arg0, UArg arg1) {
    //    double offset1 = 0;
    //    double offset2 = 0;
    //    double inc1 = 1;
    //    double inc2 = 1;
    int pos1 = 0;
    int pos2 = 0;
    int pos3 = 0;
    int pos4 = 0;
    int pos5 = 0;
    int count6 = 0;
    double posV1[] = {0, PI/8, PI/6, PI/4, PI/2, 2*PI/3, 3*PI/4 ,5*PI/6, PI};
    int l1 = 9;
    double posV2[] = {0, PI/4, PI/2};
    int l2 = 3;
    double posV3[] = {0, PI/4, PI/2};
    int l3 = 3;
    double posV4[] = {0, PI/4, PI/2};
    int l4 = 3;
    double posV5[] = {0, PI/4, PI/2};
    int l5 = 3;
    while(1) {
        while(flag.running == 1 && flag.stopp == 0) {
//            moveServo(posV1[pos1],1);
//            pos1 = (pos1+1)%l1;
//            moveServo(posV2[pos1],2);
//            pos2 = (pos2+1)%l2;
//            moveServo(posV3[pos3],3);
//            pos3 = (pos3+1)%l3;
//            moveServo(posV4[pos4],4);
//            pos4 = (pos4+1)%l4;
//            moveServo(posV5[pos5],5);
//            pos5 = (pos5+1)%l5;
            count6++;
            //if(count6 % 4 == 0) {
                flag.clawPos = invertInt(flag.clawPos);
                moveClaw(flag.clawPos);
            //}
            Task_sleep((UInt) 800 );
        }
    }
    //    moveToHomePos();
    //    while(1) {
    ////        System_printf("wait\n");
    ////        System_flush();
    //        while(flag.running == 0) {
    ////            Task_sleep((UInt) 400 );
    ////            System_printf("flag = %d\n",flag.running);
    ////            System_flush();
    //        }
    ////
    //        int i1 = 0;
    //        int i2 = 0;
    ////        System_printf("start\n");
    ////        System_flush();
    //        while(flag.running == 1 && flag.stopp == 0) {
    //            moveServo(posV1[i1],1); // move servo 1
    //            moveServo(posV2[i2],2); // move servo 2
    //            Task_sleep((UInt) 800 ); //max(servoWaitT(posV1[i1],1), servoWaitT(posV2[i2],2) ) );
    //            i1 = (i1+1) % 9;
    //            i2 = (i2+1) % 3;
    //        }
    //        System_printf("wait2\n");
    //        System_flush();
    //        while(flag.running == 0) {}
    //        System_printf("start2\n");
    //        System_flush();
    //        double n = 1;
    //        while(flag.running == 1 && flag.stopp == 0) {
    ////            System_printf("degrees = %f\n",rad2deg(rad));
    ////            System_printf("duty = %d\n",(int) duty1);
    ////            System_flush();
    //            moveServo(pos1,0); // move servo 1
    //            moveServo(pos2,1); // move servo 2
    //            Task_sleep((UInt) 800); // max(servoWaitT(pos1,1), servoWaitT(pos2,2) ) );
    //
    //            inc1 = PI/n;
    //            inc2 = PI/(2*n);
    //            int i = 0;
    //            for(i = 0;i <= n; i++) {
    //                moveServo(i*inc1,1); // move servo 1
    //                Task_sleep((UInt) 400);
    //                moveServo(i*inc2,2); // move servo 2
    //                Task_sleep((UInt) 400); // max(servoWaitT(pos1,1), servoWaitT(pos2,2) ) );
    //            }
    //            n *= 2;
    //            if(n > 60)
    //                n = 1;
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
    //        }
    //        System_printf("\nduty = %d\n",(int) duty1);
    //        System_flush();
    //        Task_sleep((UInt) (arg0*10));
    //    }
}

/*
 *  ======== ledFxn ========
 */
void ledFxn() {
    //while(1) {
        if(flag.running == 1)
            GPIO_write(Board_LED0, Board_LED_ON);
        else
            GPIO_write(Board_LED0, Board_LED_OFF);
        if(flag.stopp == 1)
            GPIO_write(Board_LED2, Board_LED_ON);
        else
            GPIO_write(Board_LED2, Board_LED_OFF);
        //Task_sleep((UInt) (arg0*10));
    //}
}

/*
 *  ======== main ========
 */
int main(void)
{
    Task_Params tskParams;
//    Task_Params tskParams1;
    /* Call board init functions */
    Board_initGeneral();
    Board_initGPIO();
    Board_initUART();
    // Board_initUSB(Board_USBDEVICE);
    //Board_initWatchdog();

    /* Initialize pwm */
    pwm_servo_init();

    /* Construct loop Task thread */
    Task_Params_init(&tskParams);
    tskParams.stackSize = TASKSTACKSIZE;
    tskParams.stack = &tsk0Stack;
    tskParams.arg0 = 400; // sleep time
    Task_construct(&tsk0Struct, (Task_FuncPtr)loop, &tskParams, NULL);

//    /* Construct idle Task thread */
//    Task_Params_init(&tskParams1);
//    tskParams1.stackSize = TASKSTACKSIZE;
//    tskParams1.stack = &tsk1Stack;
//    tskParams1.arg0 = 400; // sleep time
//    Task_construct(&tsk1Struct, (Task_FuncPtr)idleTasks, &tskParams1, NULL);


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
    flag.clawPos = 1;
    ledFxn();
    /* Start BIOS */
    BIOS_start();

    return (0);
}
