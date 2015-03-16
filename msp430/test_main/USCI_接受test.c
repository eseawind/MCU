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

unsigned char temp = 0;

int main()
{
    WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
    Clock_Init(0,0,0,1); // 1M DCO

    USCI_SPI_Init(0, 0, 1, 0, 0); //涓绘満A0
    USCI_SPI_Init(1, 0, 0, 0, 0);
    USCI_A1RX_ENABLE;
    UCA0TXBUF = 1;
    _EINT();
    while(1);

}
#pragma vector=USCIAB1RX_VECTOR
__interrupt void USCIA1RX_ISR (void)
{
    temp = UCA1RXBUF;
    UCA0TXBUF = temp + 1;
}
