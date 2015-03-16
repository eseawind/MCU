/**********************************************
 *
 * 		Author	:		Shawn Guo
 * 		Date	:		2013/4/16
 * 		Notes	:		ADC10, Sample A1, AVcc Ref, Set P1.0 if > 0.5*AVcc
 * 		Tool    :	    MSP430G2553
 *
 *
 *
 **********************************************/
#include "msp430g2553.h"

void ADC10_Init(void);      // ADC10初始化配置

void main(void)
{
    WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT

    ADC10_Init();// ADC10初始化配置

    P1DIR |= 0x01;                            // Set P1.0 to output direction

    for (;;)
    {
        ADC10CTL0 |= ENC + ADC10SC;             // Sampling and conversion start
        __bis_SR_register(CPUOFF + GIE);        // LPM0, ADC10_ISR will force exit
        if (ADC10MEM < 0x1FF)
            P1OUT &= ~0x01;                       // Clear P1.0 LED off
        else
            P1OUT |= 0x01;                        // Set P1.0 LED on
    }
}

// ADC10 interrupt service routine
#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR(void)
{
    __bic_SR_register_on_exit(CPUOFF);        // Clear CPUOFF bit from 0(SR)
}

void ADC10_Init(void)      // ADC10初始化配置
{
    //----------------ADC10CTL0
    //ADC10SC,  Start conversion.
    // ADC10IE          ADC10IFG
    //ENC:    Enable conversion,if ENC=1, ADC10 enabled

    //---Can be modified only when ((   ENC = 0     ))
    //ADC10SHTx : ADC10 sample-and-hold time
    // 00 4 × ADC10CLKs   01  8xCLK       10      16xCLKS       11  64xCLKS
    //SREFx,    Select reference.
    //ADC10SR,  ADC10 sampling rate.    0,~200 ksps     1,~50 ksps
    //----------------ADC10CTL0----END------

    ADC10CTL0 = ADC10SHT_2 + ADC10ON + ADC10IE; // ADC10ON, interrupt enabled


    //----------------ADC10CTL1
    // INCHx , Input channel select.
    //0000 A0  //0001 A1  //0010 A2    //0011 A3    //0100 A4
    //0101 A5    //0110 A6    //0111 A7    //1000 VeREF+    //1001 VREF-/VeREF-
    //1010 Temperature sensor    //1011 (VCC - VSS) / 2

    // ADC10DIVx Bits 7-5 ADC10 clock divider
    //000 /1    //001 /2    //010 /3    //011 /4
    //100 /5    //101 /6    //110 /7    //111 /8

    //ADC10SSELx Bits 4-3 ADC10 clock source select
    //00 ADC10OSC   //01 ACLK    //10 MCLK    //11 SMCLK

    //SHSx Bits 11-10 Sample-and-hold source select.
    //00 ADC10SC bit
    //01 Timer_A.OUT1(1)        //10 Timer_A.OUT0(1)
    //11 Timer_A.OUT2 (Timer_A.OUT1 on MSP430F20x0, MSP430G2x31, and MSP430G2x30 devices)(1)

    //CONSEQx Bits 2-1 Conversion sequence mode select
    //00 Single-channel-single-conversion
    //01 Sequence-of-channels
    //10 Repeat-single-channel
    //11 Repeat-sequence-of-channels

    //ADC10BUSY Bit 0 ADC10 busy. This bit indicates an active sample or conversion operation
    //0 No operation is active.
    //1 A sequence, sample, or conversion is active.

    //----------------ADC10CTL1----END------
    ADC10CTL1 = INCH_1;                       // input A1

    ADC10AE0 |= 0x02;                         // PA.1 ADC option select
}

