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

int i = 0;

int main()
{
    WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT

    Clock_Init(0,0,0,1); // 1M DCO
    Drive_Init(0, 100);

    P1DIR &= ~BIT3; //  P1.3设为输入
    P1REN |= BIT3;
    P1OUT |= BIT3;

    P1IES |= BIT3; //  P1.3中断触发沿：上升沿；
    P1IE |= BIT3; //    开启P1.3 I/O中断
    P1IFG &= ~BIT3;

    _EINT();
    while(1);
}

#pragma vector=PORT1_VECTOR
__interrupt void P1_ISR (void)
{

	if(P1IFG & BIT3){
		for(i = 2; i < 11; i++)
		{
		    Drive_Motor_L(0, 10 * i);
		    Drive_Motor_R(0, 10 * i);
		    DELAY_MS(5000);
		}
	}

	 P1IFG = 0;         // P1端口的中断标志位清零
}
