 /**********************************************
 *
 * 		Author	:		Shawn Guo
 *      Mail    :       iguoxiaopeng@gmail.com
 * 		Date	:       2013/8/15
 *      Last    :       2013/8/15
 * 		Notes	:       IO_test
 * 		Tool    :       MSP430F2618
 **********************************************/



#include <msp430f2618.h>

void main()
{
    WDTCTL = WDTPW + WDTHOLD;

    P1DIR |= 0XFF;
    P1OUT |= 0XFF;

    P2DIR |= 0XFF;
    P2OUT |= 0XFF;

    P3DIR |= 0XFF;
    P3OUT |= 0XFF;

    P4DIR |= 0XFF;
    P4OUT |= 0XFF;

    P5DIR |= 0XFF;
    P5OUT |= 0XFF;

    P6DIR |= 0XFF;
    P6OUT |= 0XFF;

    P7DIR |= 0XFF;
    P7OUT |= 0XFF;

    P8DIR |= 0XFF;
    P8OUT |= 0XFF;
    while(1);
}

#pragma vector=PORT2_VECTOR
__interrupt void P2_ISR (void)
{

}
