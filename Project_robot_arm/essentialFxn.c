/*
 * essentialFxn.c
 *      Author: thors_000
 *  Created on: 3 Mar 2018
 *     version: 1.2
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
    return rad*180.0/PI;
}


/*
 *  ======== waitIdle ========
 *  Does nothing for m ms
 */
void waitIdle(int m) {
    int c = 16*m;
    int i = 0;
    for(i = 0; i < m; i++) {
        c--;
    }
}

