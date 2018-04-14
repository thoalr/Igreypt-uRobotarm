/*
 * essentialFxn.h
 *      Author: thors_000
 *  Created on: 3 Mar 2018
 *     version: 1.3
 *     Comment: Contains any essential function definitions
 */

#ifndef ESSENTIALFXN_H_
#define ESSENTIALFXN_H_

/* Constants  */
#define PI    3.14159265359

/* Lambda functinos */
#define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

#define min(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })


#define signum(n) ({ (n < 0) ? -1 : (n > 0) ? +1 : 0; })
#define absnum(n) ({ (n < 0) ? -1*(n) : n; })



/* Function definition */
double rad2deg(double rad);
double deg2rad(double deg);
void waitIdle(int m);
int invertInt(int n);
//double abs(double y);

#endif /* ESSENTIALFXN_H_ */
