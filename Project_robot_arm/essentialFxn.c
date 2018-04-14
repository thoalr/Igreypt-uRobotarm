/*
 * essentialFxn.c
 *      Author: thors_000
 *  Created on: 3 Mar 2018
 *     version: 1.3
 *     Comment: Contains any essential functions
 */

/*
 *  ======== essentialFxn.c ========
 */

/* Our written headers  */
#include "essentialFxn.h"

/*
 *  ======== rad2deg ========
 *  Map radians to degrees
 */
double rad2deg(double rad) {
    // matlab code @(x,a,b,c,d)   c+(x-a)*(d-c)/(b-a)
    return rad*180.0/PI;
}

/*
 *  ======== deg2rad ========
 *  Map degrees to radians
 */
double deg2rad(double deg) {
    // matlab code @(x,a,b,c,d)   c+(x-a)*(d-c)/(b-a)
    return deg*PI/180;
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
 *  Doe nothing for m ms
 */
void waitIdle(int m) {
    int i;
    for(i = 0; i < 1600*m; i++) { }
}

