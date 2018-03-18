/*
 * ========== pwmControl.h ============
 *      Author: thors_000
 *  Created on: 15 Mar 2018
 *     version: 1.0
 *     Comment: Contains functions for controlling PWM output
 */

#ifndef PWMCONTROL_H_
#define PWMCONTROL_H_


/* Handles for changing duty cycle */
extern PWM_Handle pwmPF2;
extern PWM_Handle pwmPF3;

/* Function declerations */
void pwmLEDinit(uint16_t pwmPeriod, uint16_t dutyInit);


#endif /* PWMCONTROL_H_ */
