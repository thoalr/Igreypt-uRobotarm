#ifndef robot_io_h
#define robot_io_h


// period is 16-bit number of PWM clock cycles in one period (3<=period)
// duty is number of PWM clock cycles output is high  (2<=duty<=period-1)
// PWM clock rate = processor clock rate/SYSCTL_RCC_PWMDIV
//                = BusClock/2
//                = 50 MHz/2 = 25 MHz (in this example)
/*   Clock rate ??????????????? 6Mhz 16Mhz
Pulse period 20ms - 50hz
Duty ratio 0.5ms-2.5ms - 40hz-8hz 
25 Mhz = 25000 clock pulses per second
1 clock pulse = 0.04 mikro sec
20 ms = x * 0.04mikro sec ---> x = 500000 = 0x7a120
so period = 0x7a120 however this is 20 bits
duty from 12500 to 62500 or 30D4 to F424

For 16Mhz - 6.25e-8 1 clock pulse
period = 320000 -> 0x4E200
duty = 8000-40000  ---> 0x1F40-0x9C40


Set PWM Clock speed to 25kHz
*/
#define _PWMperiod 0x01f4
#define _MaxDeg 0x003e
#define _MinDeg 0x000d

void PWM0A_Init(uint16_t period, uint16_t duty);

void PWM0A_Duty(uint16_t duty);


#endif //robot_io_h

