/**********************************************
 *
 * 		Author	:		Shawn Guo
 *      Mail    :       iguoxiaopeng@gmail.com
 * 		Date	:       2013/8/12
 *      Last    :       2013/8/12
 * 		Notes	:       USCI_test
 * 		Tool    :       MSP430F2618
 **********************************************/

#include <msp430f2618.h>
#include "clock.h"
#include "LCD5110.h"
#include "USCI.h"

const unsigned char LCD_BUFFER1[] = "GXPISGOOD";

int main()
{
    WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
    Clock_Init(0,0,0,1); // 1M DCO

    USCI_SPI_Init(3, 0, 1, 0, 0);
    USCI_B1TX_ENABLE;
    UCB1TXBUF = 0xAA;                     // Transmit first character
    _EINT();
    while(1);

}
#pragma vector=USCIAB1TX_VECTOR
__interrupt void USCIB1TX_ISR (void)
{
    UCB1TXBUF = 0xAA;
}
