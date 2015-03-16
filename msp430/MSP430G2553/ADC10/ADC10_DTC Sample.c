/**********************************************
 *
 * 		Author	:		Shawn Guo
 * 		Date	:		2013/4/16
 * 		Notes	:		ADC10, DTC Sample A1 32x, AVcc, Repeat Single, DCO
 * 		Tool    :	    MSP430G2553
 *
 **********************************************/
//  Description: Use DTC to sample A1 32 times with reference to AVcc. Software
//  writes to ADC10SC to trigger sample burst. In Mainloop MSP430 waits in LPM0
//  to save power until ADC10 conversion burst complete, ADC10_ISR(DTC) will
//  force exit from any LPMx in Mainloop on reti. ADC10 internal oscillator
//  times sample period (16x) and conversion (13x). DTC transfers conversion
//  code to RAM 200h - 240h. P1.0 set at start of conversion burst, reset on
//  completion.
//
//                MSP430G2x33/G2x53
//             -----------------
//         /|\|              XIN|-
//          | |                 |
//          --|RST          XOUT|-
//            |                 |
//        >---|P1.1/A1      P1.0|-->LED


#include  "msp430g2553.h"

void ADC10_Init(void);      // ADC10初始化配置

void main(void)
{
    WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT

    ADC10_Init();// ADC10初始化配置

    P1DIR |= 0x01;                            // Set P1.0 to output direction

    for (;;)
    {
        ADC10CTL0 &= ~ENC;                      // 关闭ADC10采样转换
        while (ADC10CTL1 & BUSY);               // Wait if ADC10 core is active
        ADC10SA = 0x200;                        // Data buffer start
        P1OUT |= BIT1;                          // Set P1.0 LED on
        ADC10CTL0 |= ENC + ADC10SC;             // Sampling and conversion start
        __bis_SR_register(CPUOFF + GIE);        // LPM0, ADC10_ISR will force exit
        P1OUT &= ~0x01;                         // Clear P1.0 LED off
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
    // MSC:Multiple SampleConversion
    //----------------ADC10CTL0----END------
    //  过采样率16x ADC10CLKs, 过采样率越高，采样速度越慢，获得的有效分辨率越高


    ADC10CTL0 = ADC10SHT_2 + MSC + ADC10ON + ADC10IE; // ADC10ON, interrupt enabl

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

    //CONSEQx Bits 2-1 Conversion sequence mode select
    //00 Single-channel-single-conversion
    //01 Sequence-of-channels
    //10 Repeat-single-channel
    //11 Repeat-sequence-of-channels

    //SHSx , Sample-and-hold source select.
    //00 ADC10SC bit
    //01 Timer_A.OUT1(1)        //10 Timer_A.OUT0(1)
    //11 Timer_A.OUT2 (Timer_A.OUT1 on MSP430F20x0, MSP430G2x31, and MSP430G2x30 devices)(1)

    //ADC10BUSY Bit 0 ADC10 busy. This bit indicates an active sample or conversion operation
    //0 No operation is active.
    //1 A sequence, sample, or conversion is active.

    //----------------ADC10CTL1----END------
    ADC10CTL1 = CONSEQ_2 + INCH_1;            // Repeat single channel, A1

    //ADC10DTC1 Transfers ,transfers. These bits define the number of transfers in each block.
    ADC10DTC1 = 0x20;                         // 32 conversions

    ADC10AE0 |= 0x02;                         // P1.1 ADC option select
}

