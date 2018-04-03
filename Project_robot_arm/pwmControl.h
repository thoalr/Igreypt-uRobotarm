/*
 * ========== pwmControl.h ============
 *      Author: thors_000
 *  Created on: 15 Mar 2018
 *     version: 1.0
 *     Comment: Contains functions for controlling PWM output
 */

#ifndef PWMCONTROL_H_
#define PWMCONTROL_H_


/* PWM constants */
#define pwmPeriod    20000      // Period and duty in microseconds
#define maxPWMwidth  2500.0
#define minPWMwidth  400.0   // For our big ones 350.0 for the little one 500.0
#define maxRad       PI
#define minRad       0.0

// current number of active servos
#define numServo     5

// Default position of all servos
#define servo1Home  mapRad2PWM(PI/2) // pi/2, pi/3 for minpwmwidth 350
#define servo2Home  mapRad2PWM(PI/4)
#define servo3Home  mapRad2PWM(PI/4)
#define servo4Home  mapRad2PWM(PI*3/4)
#define servo5Home  mapRad2PWM(PI/2)
#define clawClosed  mapRad2PWM(PI*1.4/4)
#define clawOpen    mapRad2PWM(PI*1.8/4)

/* Function declerations */
uint16_t mapRad2PWM(double rad);
double mapPWM2Rad(uint16_t width);
void pwm_servo_init();
void moveServo(double rad, int n);
double getServoPos(int n);
void moveHomePos(void);
void moveClaw(int pos);
void slowMoveServo(uint32_t ui32Base, uint32_t ui32PWMOut, uint16_t w, int n);

#endif /* PWMCONTROL_H_ */
