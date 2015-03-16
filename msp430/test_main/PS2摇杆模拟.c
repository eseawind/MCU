/**********************************************
 *
 * 		Author	:		Shawn Guo
 *      Mail    :       iguoxiaopeng@gmail.com
 * 		Date	:       2013/11/2
 *      Last    :       2013/11/2
 * 		Notes	:       PS2 JoyStick摇杆模拟
 * 		Tool    :       MSP430F2618
 **********************************************/
#include  <msp430f2618.h>
#include "ADC12.h"
#include "clock.h"
#include "LCD5110.h"
const char LCD_BUFFER1[] = "dy:";

int temp = 0;

void main(void)
{
    WDTCTL = WDTPW + WDTHOLD;                 // Stop watchdog
    Clock_Init(0,0,0,CPU_FF);  // 1M DCO, 9K VLO ACLK

    LCD5110_Init();
    LCD5110_Write_String(0,0,(char*)LCD_BUFFER1);


    P6SEL = 0x03;                             // Enable A/D channel inputs
    ADC12CTL0 = ADC12ON + MSC + SHT0_8;           // Turn on ADC12, extend sampling time
    // to avoid overflow of results
    ADC12CTL1 = SHP + CONSEQ_3;                 // Use sampling timer, repeated sequence
    ADC12MCTL0 = INCH_0;                      // ref+=AVcc, channel = A0
    ADC12MCTL1 = INCH_1 + EOS;                  // ref+=AVcc, channel = A1, end seq.
    ADC12IE = 0x02;                           // Enable ADC12IFG.3
    ADC12CTL0 |= ENC;                         // Enable conversions
    ADC12CTL0 |= ADC12SC;                     // Start convn - software trigger
    _BIS_SR(LPM0_bits + GIE);                 // Enter LPM0, Enable interrupts
}

#pragma vector=ADC12_VECTOR
__interrupt void ADC12ISR (void)
{
    temp = ADC12MEM0;
    LCD5110_Long2Char(temp);
    LCD5110_Write_String(0,1,0);

    temp = ADC12MEM1;
    LCD5110_Long2Char(temp);
    LCD5110_Write_String(0,2,0);
}
