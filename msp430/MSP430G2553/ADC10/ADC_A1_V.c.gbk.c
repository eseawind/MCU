/**********************************************
 *
 * 		Author	:		Shawn Guo
 * 		Date	:		2013/4/16
 *      Last    :       2013/6/4
 * 		Notes	:		ADC10, Sample A1 and display 12864
 * 		Tool    :	    MSP430G2553
 *
 *
 *
 **********************************************/
#include "msp430g2553.h"
#include "LCD12864.h"

#define ADC_VREF1   3.555
#define ADC_VREF2   0
#define V_OFF_SET 30    //AD输入为0时的偏置CODE
#define V_GAIN  1000            //电压显示增益


void ADC10_Init(void);
void V2char(unsigned int temp);      // dat from int to char for LCD5110
void Config_Clocks();
void Config_Timer1(); //设置定时器1以用作AD定时采样


char STR_F[7] = "0.000v"; // LCD buffer, (x.xxxv eg. 3.000v)
unsigned int AD_SAMPLE_DAT = 0; //AD sample data
unsigned int AD_MAX_DAT = 0; // feng zhi

void main(void)
{


    WDTCTL = WDTPW + WDTHOLD;// Stop WDT

    Config_Clocks();
    Config_Timer1();
    ADC10_Init();

    LCD_IO_set();
    LCD_Init();
    LCD_Write_String(0,0,"v = ");

    _EINT();
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
    P1SEL2 |= BIT1;   // open the P1.1 to sample as A1
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

void Config_Timer1() //设置定时器
{
    //TACTL,Timer_A Control Register
    //TASSELx, Timer_A clock source select
    //00 TACLK    01 ACLK    	10 SMCLK

    //MCx, Mode control.
    //00 Stop mode: the timer is halted.
    //01 Up mode: the timer counts up to TACCR0.
    //10 Continuous mode: the timer counts up to 0FFFFh.
    //11 Up/down mode: the timer counts up to TACCR0 then down to 0000h.

    //TAIE ,Timer_A interrupt enable.
    //0 Interrupt disabled
    //1 Interrupt enabled

    //TACLR ,Timer_A clear.

    //IDx , 7-6 Input divider.
    //00 /1           01 /2       10 /4         11 /8

    //      SMCLK     continuous               /8
    TA1CTL = TASSEL_2 + MC_2 + TAIE + TACLR + ID_1; //TA基准时钟为SMCLK，且SMCLK与MCLK频率相同
} //连续计数模式、无分频、开启定时中断


// ADC10 interrupt service routine
#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR(void)
{
    AD_SAMPLE_DAT = (unsigned int)((ADC10MEM * (ADC_VREF1 - ADC_VREF2) / 1023 + ADC_VREF2) * V_GAIN);
    //AD_SAMPLE_DAT -= V_OFF_SET;
    if(AD_SAMPLE_DAT > AD_MAX_DAT)
    {
        AD_MAX_DAT = AD_SAMPLE_DAT;
        V2char(AD_MAX_DAT);
        LCD_Write_String(0,2,STR_F);
    }
}


//   Timer_A中断程序
#pragma vector=TIMER1_A1_VECTOR
__interrupt void Timer1_A(void)
{
    switch(TA1IV)
    {
    case 10:
        ADC10CTL0 |= ENC + ADC10SC;
        break;
    case 2:             // 中断源为R1
    case 4:             // 中断源为R2
        break;
    }
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
