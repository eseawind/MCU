/**********************************************
 *
 * 		Author	:		Shawn Guo
 *      Mail    :       iguoxiaopeng@gmail.com
 * 		Date	:
 *      Last    :
 * 		Notes	:
 * 		Tool    :
 **********************************************/

#include <msp430g2553.h>
#include "clock.h"
#include "Timer.h"
#include "drive.h"


int FLAG = 1;
long int i = 0;

int main()
{
    WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT

    Clock_Init(0,0,0,1); // 1M DCO
    Drive_IO_Init();
    Drive_PWM_Init();

    _EINT();

    	while(1){
    		Drive_Motor_L(0, 60);
    		Drive_Motor_R(1, 60);
    		DELAY_MS(333);
    		Drive_Motor_L(0, 10);
    		Drive_Motor_R(1, 10);
    		DELAY_MS(333);
    	}
}
