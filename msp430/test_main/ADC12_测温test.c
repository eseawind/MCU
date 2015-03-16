/**********************************************
 *
 * 		Author	:		Shawn Guo
 *      Mail    :       iguoxiaopeng@gmail.com
 * 		Date	:       2013/8/11
 *      Last    :       2013/8/11
 * 		Notes	:       ADC12 - 温度采样
 * 		Tool    :
 **********************************************/
#include  <msp430f2618.h>
#include "ADC12.h"
#include "clock.h"
#include "LCD5110.h"
const char LCD_BUFFER1[] = "TEMP_AD:";
unsigned int temp = 0;
unsigned char temp_flag = 0;

void main(void)
{
    WDTCTL = WDTPW + WDTHOLD;                 // Stop watchdog
    Clock_Init(0,0,0,1); // 1M DCO, 9K VLO ACLK

    P1OUT = 0x00;                             // Clear P1
    P1DIR = 0x01;                             // P1.0 as output LED.
    LCD5110_Init();
    LCD5110_Write_String(0,0,(char*)LCD_BUFFER1);

    ADC12_Init(3,1,2,7);        // smclk, /1, 单通道序列采样, 采样保持时间2^7 adcclk.
    ADC12_Sample_Source(1, 1);  // TimeA.out 触发采样，采样定时器控制采样
    ADC12_Set_VREF2_5();        // 2.5V参考电压开启

    ADC12_MCTL(0, 1, 10);       // 内存单元0开启温度传感器通道A10, 参考电压选择VREF+.
    ADC12_Enable((BIT0));           //内存单元中断,存放值以后是否中断.
    ADC12_ENC;

    TACCTL1 = OUTMOD_4;                       // Toggle on EQU1 (TAR = 0)
    TACTL = TASSEL_2 + MC_2;                  // SMCLK, cont-mode

    _EINT();
    while(1);
}

#pragma vector=ADC12_VECTOR
__interrupt void ADC12ISR (void)
{
    temp = ADC12MEM0;
    temp = ADC12_Result_2_5(temp);
    temp = ADC12_T(temp);
    LCD5110_Long2char(temp);
    LCD5110_Write_String(0,1,LCD5110_BUFFER);
}
