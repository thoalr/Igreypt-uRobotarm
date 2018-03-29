/*
 * essentialFxn.h
 *      Author: thors_000
 *  Created on: 3 Mar 2018
 *     version: 1.2
 *     Comment: Contains any function definitions not exclusive to this project
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





/* Function definition */
double rad2deg(double rad);
void waitIdle(int m);
//double abs(double y);

#endif /* ESSENTIALFXN_H_ */
