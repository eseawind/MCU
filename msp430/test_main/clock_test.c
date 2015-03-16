/**********************************************
 *
 * 		Author	:		Shawn Guo
 *      Mail    :       iguoxiaopeng@gmail.com
 * 		Date	:
 *      Last    :
 * 		Notes	:
 * 		Tool    :
 **********************************************/

#include <msp430f2618.h>
#include "clock.h"
#include "Timer.h"

int main()
{
    WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT

    Clock_Init(4,4,0,CPU_FF); // 12M XT2 MCLK/SMCLK, 12M DCO ACLK


    P5DIR |= BIT4 + BIT5;                            // P5.4= output direction
    P5SEL |= BIT4 + BIT5;                            // P5.4= MCLK option select

    while(1)
    {

    }

}
