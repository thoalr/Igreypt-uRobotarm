/**************************************************************************//**
 * @file      stdf.c
 * @brief     Contains standard functions you always need
 * @version   V0.01
 * @date      6. Feb. 2018
 * @lastdate  6. Feb. 2018
 * @note      
 ******************************************************************************/

#include "TM4C123.h"

// bounds
int inBounds(double min, double max, double value) {
	if(value < max) {
		if(value > min) {
			return 1;
		}
	}
	return 0;
}


// delay n milliseconds (16 MHz CPU clock)
void delayMs(int n)
{
    int i, j;
    for(i = 0 ; i < n; i++)
        for(j = 0; j < 3180; j++)
            {}  // do nothing for 1 ms
}
