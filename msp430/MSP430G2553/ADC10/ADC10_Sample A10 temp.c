/**********************************************
 *
 * 		Author	:		Shawn Guo
 * 		Date	:		2013/4/16
 * 		Notes	:		ADC10, Sample A10 Temp温度传感器, Set P1.0 if 温度变化2度以上
 * 		Tool    :	    MSP430G2553
 *
 **********************************************/
#include  "msp430g2553.h"

static unsigned int FirstADCVal;            // 记录第一次的温度数值，作为比较

#define ADCDeltaOn       3                  //自定义的delta温度       ~ 2 Deg C delta for LED on

void ADC10_Init(void);      // ADC10初始化配置

void main(void)
{
    WDTCTL = WDTPW + WDTHOLD;                 // Stop watchdog

    ADC10_Init();      // ADC10初始化配置

    //这里使用Timer_A的捕获比较器R0，延时一段时间等待ADC10的参考电源设置
    __enable_interrupt();                     // Enable interrupts.
    TACCR0 = 30;                              // Delay to allow Ref to settle
    TACCTL0 |= CCIE;                          // Compare-mode interrupt.
    TACTL = TASSEL_2 | MC_1;                  // TACLK = SMCLK, Up mode.
    LPM0;                                     // Wait for delay.
    TACCTL0 &= ~CCIE;                         // Disable timer Interrupt
    __disable_interrupt();


    ADC10CTL0 |= ENC;
    TACCTL1 = OUTMOD_4;                       // Toggle on EQU1 (TAR = 0)
    TACTL = TASSEL_2 + MC_2;                  // SMCLK, cont-mode

    while (!(ADC10IFG & ADC10CTL0));          // First conversion?
    FirstADCVal = ADC10MEM;                   // Read out 1st ADC value

    P1OUT = 0x00;                             // Clear P1
    P1DIR = 0x01;                             // P1.0 as output
    __bis_SR_register(LPM0_bits + GIE);       // Enter LPM0 w/ interrupt
}

#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR (void)
{
    if (ADC10MEM >= FirstADCVal + ADCDeltaOn)
        P1OUT |= 0x01;                          // LED on
    else
        P1OUT &= ~0x01;                         // LED off
}

#pragma vector=TIMER0_A0_VECTOR
__interrupt void ta0_isr(void)
{
    TACTL = 0;
    LPM0_EXIT;                                // Exit LPM0 on return
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

    //  SREF_1 VR+ = VREF+ and VR- = VSS
    //  过采样率16x ADC10CLKs, 过采样率越高，采样速度越慢，获得的有效分辨率越高
    //  参考电压VREF+ = 2.5V

    ADC10CTL0 = SREF_1 + ADC10SHT_3 + REF2_5V + ADC10IE + REFON + ADC10ON;


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
    //  三分频，通道10温度传感器，使用TimerA OUT1作为采样转换启动位, 单通道重复采样
    ADC10CTL1 = ADC10DIV_3 + INCH_10 + SHS_1 + CONSEQ_2;
}

