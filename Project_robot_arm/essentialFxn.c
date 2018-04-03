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


///*
// *  ======== abs ========
// *  returns absolute value
// */
//double abs(double y) {
//    if(y>=0)
//        return y;
//    else
//        return -y;
//}

/*
 *  ======== invertInt ========
 *  Returns 1 if input is 0 else return 0
 */
int invertInt(int n) {
    if(n == 0)
        return 1;
    else
        return 0;
}

/*
 *  ======== waitIdle ========
 *  Does nothing for m ms
 */
void waitIdle(int m) {
    int i;
    for(i = 0; i < 16000*m; i++) { }
}

