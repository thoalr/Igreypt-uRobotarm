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

/* Global variables */
int count1 = 0;
int count2 = 0;
/* PWM constants */
#define maxPWMwidth  2500.0
#define minPWMwidth  400.0   // For our big ones 350.0 for the little one 500.0
#define maxRad       PI
#define minRad       0.0
/* PWM handles and necessary initialization parameters  */


//PWM_Handle pwmPF2 = NULL;
//PWM_Handle pwmPF3 = NULL;
#define dutyInit     (maxPWMwidth+minPWMwidth)/2  //mapRad(PI/2)
/* PWM values */
uint16_t   pwmPeriod = 20000;      // Period and duty in microseconds
uint16_t   duty1 = (uint16_t) minPWMwidth;
uint16_t   duty2 = (uint16_t) minPWMwidth;
uint16_t   dutyInc = 40;

/*
 *  ======== gpioButtonFxn0 ========
 *  Callback function for the GPIO interrupt on Board_BUTTON0.
 */
void gpioButtonFxn0(unsigned int index)
{
    /* Clear the GPIO interrupt and toggle an LED */
    //GPIO_toggle(Board_LED0);
    PWM_setDuty(pwmPF2, minPWMwidth);

    if (count1++ >= 20) {
        count1 = -20;
    }
    duty1 = dutyInc*count1+1000;
}

/*
 *  ======== gpioButtonFxn1 ========
 *  Callback function for the GPIO interrupt on Board_BUTTON1.
 *  This may not be used for all boards.
 */
void gpioButtonFxn1(unsigned int index)
{
    /* Clear the GPIO interrupt and toggle an LED */
    //GPIO_toggle(Board_LED1);

    if (count1-- <= 0) {
        count1 = 20;
    }
    duty1 = dutyInc*count1+1000;

    PWM_setDuty(pwmPF2, maxPWMwidth);
}


/*
 *  ======== mapRad ========
 *  Map radians to pwm width value
 */
uint16_t mapRad(double rad) {
    // matlab code @(x,a,b,c,d)   c+(x-a)*(d-c)/(b-a)
    return (uint16_t) (minPWMwidth + (rad*(maxPWMwidth-minPWMwidth)/(maxRad)) );
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
    while(1) {
        //uint16_t inc = 430;
        double rad = 0;
        double inc = PI;
        while(count1 == 0) {

            System_printf("degrees = %f\n",rad2deg(rad));
            System_printf("duty = %d\n",(int) duty1);
            System_flush();

            duty1 = mapRad(rad);

            PWM_setDuty(pwmPF2, duty1);

            if(rad >= PI) {
                rad = 0;
                inc = inc / 2;
                PWM_setDuty(pwmPF2, minPWMwidth);
                Task_sleep((UInt) (arg0*2) );
            }
            else {
                rad = min(rad+inc,PI);
            }
            Task_sleep((UInt) (arg0 *(4*inc/PI)) );
            //        if (pwmPF3) {
            //            PWM_setDuty(pwmPF3, duty2);
            //        }

//            if(count1++ > (maxPWMwidth-minPWMwidth) / inc ) {
//                count1 = 0;
//                inc = inc/2;
//                PWM_setDuty(pwmPF2, minPWMwidth);
//                Task_sleep((UInt) (arg0));
//            }
//
//            duty1 = inc*count1+minPWMwidth;
//
//            //        if (count2++ >= 10) {
//            //            count2 = 1;
//            //        }
//            //        duty2 = dutyInc*count2 +500;
//
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
    pwmLEDinit(pwmPeriod,dutyInit);

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

    /* Start BIOS */
    BIOS_start();

    return (0);
}
