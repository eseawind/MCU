/**********************************************
 *
 * 		Author	:		Shawn Guo
 *      Mail    :       iguoxiaopeng@gmail.com
 * 		Date	:       2013/9/3
 *      Last    :       2013/9/3
 * 		Notes	:       NAZA LITE起飞降落
 * 		Tool    :       NAZA LITE / MSP430X2XX
 *
 *
 *
 *
 *
 **********************************************/


#include <msp430f2618.h>
#include "clock.h"
#include "Timer.h"
#include "Plane_Drive.h"

int main()
{
    WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT

    Clock_Init(4,4,0,CPU_FF); // 12M XT2 MCLK/SMCLK, 12M DCO ACLK





    while(1);
}
