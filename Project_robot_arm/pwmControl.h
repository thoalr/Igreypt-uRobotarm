/*
 * ========== pwmControl.h ============
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

#ifndef PWMCONTROL_H_
#define PWMCONTROL_H_


/* PWM constants */
#define pwmPeriod    20000      // Period and duty in microseconds
#define maxPWMwidth  2500.0
#define minPWMwidth  400.0   // For our big ones 400.0 for the little one 500.0
#define maxRad       PI
#define minRad       0.0

// current number of active servos
#define numServo     6
typedef struct Rpos{
    double servo1; // position of servo1
    double servo2; // position of servo2
    double servo3; // position of servo3
    double servo4; // position of servo4
    double servo5; // position of servo5
    int    claw;  // value of position of claw
    int    wait1; // delay before moving to position
    int    wait2; // delay after moving to position
} Rpos; // Structure for moving robot to position


// Default position of all servos on startup
#define servo1Home  mapRad2PWM(PI/2) // pi/2, pi/3 for minpwmwidth 350
#define servo2Home  mapRad2PWM(PI*1.5/4) //
#define servo3Home  mapRad2PWM(PI) // 135°
#define servo4Home  mapRad2PWM(PI*3/4)  // 135°
#define servo5Home  mapRad2PWM(PI/2)
#define clawOpen    mapRad2PWM(PI*2.6/4)
// closed Claw positions
#define clawClosed1 mapRad2PWM(PI*2.0/4)
#define clawClosed2 mapRad2PWM(PI*1.8/4)

/* Function declerations */
uint16_t mapRad2PWM(double rad);
double mapPWM2Rad(uint16_t width);
void pwm_servo_init();
void moveServo(double rad, int n);
double getServoPos(int n);
void moveHomePos(void);
void moveClaw(int pos);
void slowMoveServo(uint16_t w, int n);
void moveRobotPos(Rpos pos, int mode);
Rpos getRobotPos();
void moveRobotPos(Rpos pos, int mode);
void setPWMwidth(uint16_t width, int n);

#endif /* PWMCONTROL_H_ */
