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


PWM_Handle pwmPF2 = NULL;
PWM_Handle pwmPF3 = NULL;
PWM_Params params;

/*
 *  ======== pwmLEDinit ========
 *  Initializes the PWM modeules for the on board LED.
 */
void pwmLEDinit(uint16_t pwmPeriod, uint16_t dutyInit)
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


    PWM_init();

    // This is my part
    PWM_Params_init(&params);
    params.period = pwmPeriod;
    pwmPF2 = PWM_open(Board_PWM0, &params);
    if (pwmPF2 == NULL) {
        System_abort("Board_PWM0 did not open");
    }
    PWM_setDuty(pwmPF2, dutyInit);
//    if (Board_PWM1 != Board_PWM0) {
//        params.polarity = PWM_POL_ACTIVE_HIGH;
//        PWM_Handle pwmPF3 = PWM_open(Board_PWM1, &params);
//        if (pwmPF3 == NULL) {
//            System_abort("Board_PWM1 did not open");
//        }
//    }
    //PWM_setDuty(pwmPF3, duty2Init);
}

