/*
 * essentialFxn.c
 *      Author: thors_000
 *  Created on: 3 Mar 2018
 *     Comment: Contains any functions not exclusive to this project
 */

/*
 *  ======== essentialFxn.c ========
 */

/* Our written headers  */
#include "essentialFxn.h"

/*
 *  ======== mapRad ========
 *  Map radians to pwm width value
 */
double rad2deg(double rad) {
    // matlab code @(x,a,b,c,d)   c+(x-a)*(d-c)/(b-a)
    return rad/pi * 180.0;
}
