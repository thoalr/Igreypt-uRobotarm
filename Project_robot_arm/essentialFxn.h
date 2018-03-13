/*
 * essentialFxn.h
 *      Author: thors_000
 *  Created on: 3 Mar 2018
 *     Comment: Contains any function defenitions not exclusive to this project
 */

#ifndef ESSENTIALFXN_H_
#define ESSENTIALFXN_H_

/* Constants  */
#define pi    3.14159265359

#define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

#define min(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })

double rad2deg(double rad);



#endif /* ESSENTIALFXN_H_ */
