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
#define pwmPeriod    20000;      // Period and duty in microseconds
#define maxPWMwidth  2500.0
#define minPWMwidth  400.0   // For our big ones 350.0 for the little one 500.0
#define maxRad       PI
#define minRad       0.0



/* Function declerations */
uint16_t mapRad2PWM(double rad);
double mapPWM2Rad(uint16_t width);
void pwm_servo_init();
void moveServo(double rad, int n);
double getServoPos(int n);
void moveHomePos(void);
int servoWaitT(double pos, int n);


#endif /* PWMCONTROL_H_ */
