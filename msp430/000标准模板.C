/**********************************************
 *
 * 		Author	:		Shawn Guo
 *      Mail    :       iguoxiaopeng@gmail.com
 * 		Date	:		2013/
 *      Last    :       2013/
 * 		Notes	:
 * 		Tool    :	    MSP430F2618
 **********************************************/
#include <msp430f5529.h>

void main(void)
{
  WDTCTL = WDTPW+WDTHOLD;                   // Stop WDT
}
