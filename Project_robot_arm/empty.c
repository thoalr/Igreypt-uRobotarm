/*
 * ================ empty.c ======================
 *      Author: thors_000
 *  Created on: 1 Mar 2018
 *     version: 3.0
 *     Comment: Contains the highest level code
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
#define TASKSTACKSIZE   1000
Task_Struct tsk0Struct;
UInt8 tsk0Stack[TASKSTACKSIZE];
Task_Handle taskLoop1;
Task_Struct tsk1Struct;
UInt8 tsk1Stack[TASKSTACKSIZE];
Task_Handle taskMain;

/* Global variables */
/* Flag bitfield structure to use to control programm */
struct Flag{
    unsigned int volatile running; // 1: Run loop programm  0: pause programm
    unsigned int volatile stopp; // 1: programm stopped. Needs to restart
    unsigned int volatile task;
} flag;


// Function declaration
void ledFxn();

/*
 *  ======== gpioButtonFxn0 ========
 *  Callback function for the GPIO interrupt on Board_BUTTON0.
 */
void gpioButtonFxn0(unsigned int index)
{
    //count1 = min(count1+1,4);
    flag.running = 1;
    ledFxn();
    waitIdle(100); // debounce
}

/*
 *  ======== gpioButtonFxn1 ========
 *  Callback function for the GPIO interrupt on Board_BUTTON1.
 *  This may not be used for all boards.
 */
void gpioButtonFxn1(unsigned int index)
{
    //count1 = max(count1-1,0);
    flag.stopp = 1;
    flag.running = 0;
    ledFxn();
    moveHomePos();
    while(1) {} // complete stop
}



void loop(UArg arg0, UArg arg1) {
    // Causes weird behaviour
    //    Rpos startP = getRobotPos();
    //    while(0) {
    //        while(flag.running == 0) {}
    //        // start
    //        startP.servo2 = deg2rad(50);
    //        startP.servo3 = deg2rad(90);
    //        moveRobotPos(startP,2);
    //        // move 1
    //        Task_sleep((UInt) 800 );
    //        while(flag.running == 0) {}
    //        Rpos Pos1 = getRobotPos();
    //        Pos1.servo4 = 0;
    //        moveRobotPos(Pos1,0);
    //        // move 2 move to item direction
    //        Rpos Pos2 = getRobotPos();
    //        Pos2.servo1 = 0.9226; // atan(20.6/15.6);
    //        Pos2.servo3 = deg2rad(110);
    //        Pos2.servo4 = deg2rad(45);
    //        moveRobotPos(Pos2,2);
    //        Task_sleep((UInt) 200 );
    //        while(flag.running == 0) {}
    //        // move 3 move over item
    //        Rpos Pos3 = getRobotPos();
    //        Pos3.servo2 = deg2rad(50);
    //        Pos3.servo3 = deg2rad(160);
    //        Pos3.servo4 = deg2rad(30);
    //        moveRobotPos(Pos3,2);
    //        Task_sleep((UInt) 200 );
    //        while(flag.running == 0) {}
    //
    //
    //        // move 4 move closer
    //        Rpos Pos4 = getRobotPos();
    //        Pos4.servo2 = deg2rad(48);
    //        moveRobotPos(Pos4,1);
    //        Rpos Pos5 = getRobotPos();
    //        // close claw
    //        Pos5.claw = 2;
    //        moveRobotPos(Pos5,0);
    //        Task_sleep((UInt) 200 );
    //        while(flag.running == 0) {}
    //
    //        // move 6 Lift item
    //        Rpos Pos6 = getRobotPos();
    //        Pos6.claw = 2;
    //        Pos6.servo1 = startP.servo1;
    //        Pos6.servo2 = deg2rad(70);
    //        Pos6.servo3 = deg2rad(120);
    //        moveRobotPos(Pos6,0);
    //        Task_sleep((UInt) 400 );
    //        while(flag.running == 0) {}
    //
    //        // move to drop zone
    //        Rpos Pos7 = getRobotPos();
    //        Pos7.servo1 = 2.119345; // atan(18/-11) + pi
    //        Pos7.servo2 = deg2rad(70);
    //        Pos7.servo3 = startP.servo3;
    //        moveRobotPos(Pos7,1);
    //        Task_sleep((UInt) 200 );
    //        while(flag.running == 0) {}
    //
    //        // drop item
    //        Rpos Pos8 = getRobotPos();
    //        Pos8.claw = 0;
    //        Pos8.servo2 = deg2rad(46);
    //        Pos8.servo3 = deg2rad(110);
    //        moveRobotPos(Pos8,0);
    //        Task_sleep((UInt) 800 );
    //        while(flag.running == 0) {}
    //
    //        while(flag.running == 0) {}
    //        moveRobotPos(startP,2);
    //        Task_sleep((UInt) 1000 );
    //    }
    /* Shown during presentation */
    int i = 0;
    while(flag.task == 0) {
        while(flag.running == 0) {}
        if(flag.task != 0)
            break;
        moveServo(deg2rad(50),2);
        moveServo(PI/2,3);
        moveServo(0,4);
        Task_sleep((UInt) 200 );
        // get item
        // move 1 move to item direction
        moveServo( 0.9226,1); // atan(20.6/15.6);
        moveServo(deg2rad(45),4);
        moveServo(deg2rad(110),3);
        Task_sleep((UInt) 200 );
        // move 2 move over item
        moveServo(deg2rad(50),2);
        moveServo(deg2rad(120),3);
        moveServo(deg2rad(40),4);
        Task_sleep((UInt) 200 );
        if(flag.task != 0)
            break;
        while(flag.running == 0) {}

        // move 3 move closer
        //        moveServo(deg2rad(48.5),2);
        moveServo(deg2rad(35),2);
        moveServo(deg2rad(130),3);
        Task_sleep((UInt) 200 );
        if(flag.task != 0)
            break;
        while(flag.running == 0) {}

        // move 4 Pick up item
        moveClaw(2);
        Task_sleep((UInt) 200 );
        if(flag.task != 0)
            break;
        while(flag.running == 0) {}
        // move up
        moveServo(deg2rad(70),2);
        moveServo(deg2rad(120),3);
        Task_sleep((UInt) 200 );
        if(flag.task != 0)
            break;
        while(flag.running == 0) {}
        // move to drop zone
        moveServo(2.119345,1); // atan(18/-11) + pi
        Task_sleep((UInt) 200 );
        if(flag.task != 0)
            break;
        while(flag.running == 0) {}

        // drop item
        moveServo(deg2rad(122-i),3);
        Task_sleep((UInt) 200 );
        moveServo(deg2rad(32+2*i),2);
        Task_sleep((UInt) 200 );
        moveServo(deg2rad(30-2*i),4);
        i++;
        Task_sleep((UInt) 400 );
        if(flag.task != 0)
            break;
        while(flag.running == 0) {}
        moveClaw(0);
        Task_sleep((UInt) 200 );
        if(flag.task != 0)
            break;
        while(flag.running == 0) {}

        // move to starting position
        moveServo(deg2rad(60),2);
        moveServo(PI/2,3);
        moveServo(PI/2,1);
        Task_sleep((UInt) 400 );
        if(i == 7) {
            flag.running = 0;
            i = 0;
            ledFxn();
        }

    }
    i = 0;
    /* shown in video */
    while(flag.task == 1) {
        while(flag.running == 0) {}
        if(flag.task != 1)
            break;
        moveServo(deg2rad(50),2);
        moveServo(PI/2,3);
        moveServo(0,4);
        Task_sleep((UInt) 200 );
        // get item
        // move 1 move to item direction
        moveServo( 0.9226,1); // atan(20.6/15.6);
        moveServo(deg2rad(45),4);
        moveServo(deg2rad(110),3);
        Task_sleep((UInt) 200 );
        // move 2 move over item
        moveServo(deg2rad(50),2);
        moveServo(deg2rad(120),3);
        moveServo(deg2rad(40),4);
        Task_sleep((UInt) 200 );
        if(flag.task != 1)
            break;
        while(flag.running == 0) {}

        // move 3 move closer
        //        moveServo(deg2rad(48.5),2);
        moveServo(deg2rad(35),2);
        moveServo(deg2rad(130),3);
        Task_sleep((UInt) 200 );
        if(flag.task != 1)
            break;
        while(flag.running == 0) {}

        // move 4 Pick up item
        moveClaw(2);
        Task_sleep((UInt) 200 );
        if(flag.task != 1)
            break;
        while(flag.running == 0) {}
        // move up
        moveServo(deg2rad(70),2);
        moveServo(deg2rad(120),3);
        Task_sleep((UInt) 200 );
        if(flag.task != 1)
            break;
        while(flag.running == 0) {}
        // move to drop zone
        moveServo(2.119345,1); // atan(18/-11) + pi
        Task_sleep((UInt) 200 );
        if(flag.task != 1)
            break;
        while(flag.running == 0) {}

        // drop item
        moveServo(deg2rad(70+2*i),2);
        Task_sleep((UInt) 200 );
        moveServo(deg2rad(165-2*i),3);
        Task_sleep((UInt) 200 );
        moveServo(deg2rad(21-3*i),4);
        i++;
        Task_sleep((UInt) 400 );
        if(flag.task != 1)
            break;
        while(flag.running == 0) {}
        moveClaw(0);
        Task_sleep((UInt) 200 );
        while(flag.running == 0) {}

        // move to starting position
        moveServo(PI/2,3);
        moveServo(deg2rad(60),2);
        moveServo(PI/2,1);
        Task_sleep((UInt) 400 );
        if(i == 7) {
            flag.running = 0;
            i = 0;
            ledFxn();
        }
    }
}

/*
 *  ======== ledFxn ========
 */
void ledFxn() {
    if(flag.running == 1)
        GPIO_write(Board_LED0, Board_LED_ON);
    else
        GPIO_write(Board_LED0, Board_LED_OFF);
    if(flag.stopp == 1)
        GPIO_write(Board_LED2, Board_LED_ON);
    else
        GPIO_write(Board_LED2, Board_LED_OFF);
    if(flag.task == 0)
        GPIO_write(Board_LED1,Board_LED_ON);
    else
        GPIO_write(Board_LED1,Board_LED_OFF);
}


/*
 *  ======== gpioButtonFxn1 ========
 *  Callback function for the GPIO interrupt on Board_BUTTON1.
 */
void intPC4(unsigned int index)
{
    flag.running = invertInt(flag.running);
    ledFxn();
    waitIdle(1000);
}

/*
 *  ======== gpioButtonFxn1 ========
 *  Callback function for the GPIO interrupt on Board_BUTTON1.
 */
void intPC5(unsigned int index)
{
    flag.task = invertInt(flag.task);
    ledFxn();
    waitIdle(1000); // debounce
    //while(GPIOPinRead(GPIO_PORTC_BASE,GPIO_INT_PIN_4) & (1<<4) != GPIO_INT_PIN_4 ) {} // wait until start button is pressed
    //flag.running = 1;
    //ledFxn();
}

/*
 *  ======== gpioButtonFxn1 ========
 *  Callback function for the GPIO interrupt on Board_BUTTON1.
 */
void intPC6(unsigned int index)
{
    flag.stopp = 1;
    flag.running = 0;
    ledFxn();
    moveHomePos();
    while(1) {} // complete stop
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
    //Board_initWatchdog();

    /* Initialize pwm */
    pwm_servo_init();

    /* Construct loop Task thread */
    Task_Params_init(&tskParams);
    tskParams.stackSize = TASKSTACKSIZE;
    //tskParams.priority = 10;
    tskParams.stack = &tsk0Stack;
    tskParams.arg0 = 400; // sleep time
    Task_construct(&tsk0Struct, (Task_FuncPtr)loop, &tskParams, NULL);

    /* Obtain instance handle */
    //task = Task_handle(&tsk0Struct);

    System_printf("Starting the example\nSystem provider is set to SysMin. "
            "Halt the target to view any SysMin contents in ROV.\n");
    /* SysMin will only print to the console when you call flush or exit */
    System_flush();

    //    /* install Button callback */
    GPIO_setCallback(Board_BUTTONPC4, intPC4);
    //    /* Enable interrupts */
    GPIO_enableInt(Board_BUTTONPC4);
    //    /* install Button callback */
    GPIO_setCallback(Board_BUTTONPC5, intPC5);
    //    /* Enable interrupts */
    GPIO_enableInt(Board_BUTTONPC5);
    //    /* install Button callback */
    GPIO_setCallback(Board_BUTTONPC6, intPC6);
    //    /* Enable interrupts */
    GPIO_enableInt(Board_BUTTONPC6);

    //    /* install Button callback */
    GPIO_setCallback(Board_BUTTON0, gpioButtonFxn0);
    //    /* Enable interrupts */
    GPIO_enableInt(Board_BUTTON0);

    /* install Button callback */
    GPIO_setCallback(Board_BUTTON1, gpioButtonFxn1);
    GPIO_enableInt(Board_BUTTON1);
    /* Initialize flag variables */
    flag.running = 0;
    flag.stopp = 0;
    flag.task = 0;
    ledFxn();

    /* Start BIOS */
    BIOS_start();

    return (0);
}
