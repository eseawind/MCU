/**********************************************
 *
 * 		Author	:		Shawn Guo
 * 		Date	:		2013/4/16
 *      Last    :       2013/4/27
 * 		Notes	:		ADC10, Sample A1 and display LCD5110
 * 		Tool    :	    MSP430G2553s
 *
 *
 *
 **********************************************/
#include "msp430g2553.h"
#include "LCD5110.h"

#define ADC_VREF1   3.545
#define ADC_VREF2   0
#define V_OFF_SET 40    //AD输入为0时的偏置CODE
#define V_GAIN  1000            //电压显示增益


void ADC10_Init(void);
void V2char(unsigned int temp);      // dat from int to char for LCD5110
void Config_Clocks();


char STR_F[7] = "0.000v"; // LCD buffer, (x.xxxv eg. 3.000v)
unsigned int dat = 0; //AD sample data
unsigned int MAX_DAT = 0; // feng zhi

void main(void)
{


    WDTCTL = WDTPW + WDTHOLD;// Stop WDT

    Config_Clocks();
    ADC10_Init();
    P1SEL2 |= BIT1;   // open the P1.1 to sample as A1

    LCD_IO_set();
    LCD_Init();
    LCD_Clear();
    LCD_Write_String(0,0,"v = ");

    _EINT();
    for (;;)
    {
        ADC10CTL0 |= ENC + ADC10SC;             // Sampling and conversion start
        V2char(MAX_DAT);
        LCD_Write_String(24,0,STR_F);
    }
}


void ADC10_Init(void)
{
    //----------------ADC10CTL0
    //ADC10SC,  Start conversion.
    // ADC10IE          ADC10IFG
    //ENC:    Enable conversion,if ENC=1, ADC10 enabled

    //---Can be modified only when ((   ENC = 0     ))
    //ADC10SHTx : ADC10 sample-and-hold time
    // 00 4 脳 ADC10CLKs   01  8xCLK       10      16xCLKS       11  64xCLKS
    //SREFx,    Select reference.
    //ADC10SR,  ADC10 sampling rate.    0,~200 ksps     1,~50 ksps
    //----------------ADC10CTL0----END------
    //  SREF_1 VR+ = AVCC and VR- = VSS

    ADC10CTL0 = SREF_0 + ADC10SHT_0 + ADC10IE + ADC10ON;

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
    ADC10CTL1 = INCH_1 + ADC10SSEL_3;  // input A1

    ADC10AE0 |= 0x02;                         // PA.1 ADC option select
}


void Config_Clocks()
{
    BCSCTL1 = CALBC1_16MHZ; // Set range
    DCOCTL = CALDCO_16MHZ; // Set DCO step + modulation
    BCSCTL3 |= LFXT1S_2;
// BCSCTL2, Basic Clock System Control Register 2

    // SELMx , Select MCLK. These bits select the MCLK source.
    // 00 DCOCLK    01 DCOCLK  11 LFXT1CLK or VLOCLK
    // 10 XT2CLK when XT2 oscillator present on-chip.
    //LFXT1CLK or VLOCLK when XT2 oscillator not presenton-chip.

    //SELS , Select SMCLK. This bit selects the SMCLK source.
    // 0 DCOCLK
    //1 XT2CLK when XT2 oscillator present.
    //LFXT1CLK or VLOCLK when XT2 oscillator not present

    //DIVMx , Divider for MCLK
    // 00 /1	01 /2	10 /4	11 /8
    //DIVSx , Divider for SMCLK
    //00 /1		01 /2	10 /4	11 /8

    //DCOR , DCO resistor select. Not available in all devices. See the device-specific data sheet.
    //0 Internal resistor
    //1 External resistor

    //    MCLK=DC0
    BCSCTL2 |= SELM_0 + DIVM_0 + DIVS_0; // SMCLK = DCO
}

// ADC10 interrupt service routine
#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR(void)
{
    dat = (unsigned int)((ADC10MEM * (ADC_VREF1 - ADC_VREF2) / 1023 + ADC_VREF2) * V_GAIN);
    dat -= V_OFF_SET;
    if(dat > MAX_DAT) MAX_DAT = dat;
}

void V2char(unsigned int temp)      // dat from int to char for LCD5110
{
    STR_F[4] = temp % 10 + '0';
    temp /= 10;
    STR_F[3] = temp % 10 + '0';
    temp /= 10;
    STR_F[2] = temp % 10 + '0';
    temp /= 10;
    // STR_F[1] = '.';
    STR_F[0] = temp % 10 + '0';
}
