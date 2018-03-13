/*
 * Copyright (c) 2015, Texas Instruments Incorporated
 */

/*
 *  ======== empty.c ========
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


#define TASKSTACKSIZE   512
//pwm
Task_Struct tsk0Struct;
UInt8 tsk0Stack[TASKSTACKSIZE];
Task_Handle task;

/* variable to be read by GUI Composer */
int count1 = 0;
int count2 = 0;
/* PWM constants */
#define maxPWMwidth  2500.0
#define minPWMwidth  502.0 // For our big ones 350.0 for the little one 500.0
#define maxRad       pi
#define minRad       0.0
PWM_Handle pwm1;
PWM_Handle pwm2 = NULL;
PWM_Params params;
uint16_t   pwmPeriod = 20000;      // Period and duty in microseconds
uint16_t   duty1 = (uint16_t) maxPWMwidth;
uint16_t   duty2 = (uint16_t) maxPWMwidth;
uint16_t   dutyInc = 10;
/*
 *  ======== pwmLEDinit ========
 *  Initializes the PWM modeules for the on board LED.
 */
Void pwmLEDinit()
{
    //this is Board_initPWM()
    /* Enable PWM peripherals */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1);

    /*
     * Enable PWM output on GPIO pins.  Board_LED1 and Board_LED2 are now
     * controlled by PWM peripheral - Do not use GPIO APIs.
     */
    GPIOPinConfigure(GPIO_PF2_M1PWM6);
    GPIOPinConfigure(GPIO_PF3_M1PWM7);
    GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_2 |GPIO_PIN_3);

    PWM_init();

    // This is my part
    PWM_Params_init(&params);
    params.period = pwmPeriod;
    pwm1 = PWM_open(Board_PWM0, &params);
    if (pwm1 == NULL) {
        System_abort("Board_PWM0 did not open");
    }
    PWM_setDuty(pwm1, duty1);
//    if (Board_PWM1 != Board_PWM0) {
//        params.polarity = PWM_POL_ACTIVE_HIGH;
//        pwm2 = PWM_open(Board_PWM1, &params);
//        if (pwm2 == NULL) {
//            System_abort("Board_PWM1 did not open");
//        }
//    }
    //PWM_setDuty(pwm2, duty2);
}

/*
 *  ======== gpioButtonFxn0 ========
 *  Callback function for the GPIO interrupt on Board_BUTTON0.
 */
void gpioButtonFxn0(unsigned int index)
{
    /* Clear the GPIO interrupt and toggle an LED */
    //GPIO_toggle(Board_LED0);
    PWM_setDuty(pwm1, duty1);

    if (count1++ >= 20) {
        count1 = -20;
    }
    duty1 = dutyInc*count1+2500;
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
    duty1 = dutyInc*count1+2500;

    PWM_setDuty(pwm1, duty1);
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
        double inc = pi;
        while(1) {

            //System_printf("deg = %f\n",rad2deg(rad));
            //System_printf("duty = %d\n",(int) duty1);
            //System_flush();

            duty1 = mapRad(rad);

            PWM_setDuty(pwm1, duty1);

            if(rad >= pi) {
                rad = 0;
                inc = inc / 2;
                PWM_setDuty(pwm1, minPWMwidth);
                Task_sleep((UInt) (arg0*2) );
            }
            else {
                rad = min(rad+inc,pi);
            }
            Task_sleep((UInt) (arg0 *(4*inc/pi)) );
            //        if (pwm2) {
            //            PWM_setDuty(pwm2, duty2);
            //        }

//            if(count1++ > (maxPWMwidth-minPWMwidth) / inc ) {
//                count1 = 0;
//                inc = inc/2;
//                PWM_setDuty(pwm1, minPWMwidth);
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

    /* Initialize pwm for onboard LEDs */
    pwmLEDinit();

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
     *  If more than one input pin is available for your device, interrupts
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
