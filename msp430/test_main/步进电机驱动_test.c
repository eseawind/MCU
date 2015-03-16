/**********************************************
 *
 * 		Author	:		Shawn Guo
 *      Mail    :       iguoxiaopeng@gmail.com
 * 		Date	:       2013/8/26
 *      Last    :       2013/8/26
 * 		Notes	:       两相四线步进电机驱动
 * 		Tool    :       MSP430F2618
 **********************************************/



#include <msp430f2618.h>
#include "clock.h"


void main()
{
    WDTCTL = WDTPW + WDTHOLD;
    Clock_Init(0,0,0,CPU_FF); // 1M DCO


    P1DIR |= BIT0 + BIT1 + BIT2 + BI3;
    P1OUT &= ~(BIT0 + BIT1 + BIT2 + BIT3);

    while(1)
    {
        P1OUT |= BIT0;
        DELAY_MS(10);
        P1OUT &= ~BIT0;
        DELAY_MS(10);
    }
}

#pragma vector=PORT2_VECTOR
__interrupt void P2_ISR (void)
{

}
