/**********************************************
 *
 * 		Author	:		Shawn Guo
 * 		Date	:		2013/4/13
 * 		Last	:		2013/4/18
 * 		Notes	:		频率计——等精度测频法
 * 		Tool    :	MSP430G2553
 * 		信号->迟滞比较器--->0-3v方波---> P1.5
 * 		定时器Timer0用作标准时间,定阀门时间为2S
 * 		P1.5中断记录信号上升沿
 *
 **********************************************/

#include <msp430g2553.h>
#include <stdio.h>
#include <stdlib.h>
#include "LCD5110.h"

//        实际参数配置
#define SCLK_M 12                 // 定时器Timer0用的时钟源为SMCLK，设置SMCLK的频率
#define DCO_M 12160           // DCO实际频率，需实时校准
#define CALBC1_M CALBC1_12MHZ
#define CALDCO_M CALDCO_12MHZ


//======================================================================
unsigned long int TA_OverflowCnt = 0; // 定时器溢出计数器
unsigned long long int Period = 0, F = 0;  // 定时器总计数值，频率计算值
unsigned long long int COUNT = 0;            //   I/O中断计数值
char STR_F[10]; // LCD显存缓冲

void Config_Clocks(); //配置时钟
void Config_Port(); //设置端口
void Config_Timer0(); //设置定时器
void Long2char(long int temp); // long int to char...转换数字为字符串并保存在STR_F数组中

void main()
{

    //初始化
    WDTCTL = WDTPW + WDTHOLD; // Stop watchdog timer

    // LCD配置
    LCD_IO_set();
    LCD_Init(); //初始化液晶
    LCD_Clear();
    LCD_Write_String(0,0,"f =         HZ");
    STR_F[9] = '\0';

    Config_Port();    //频率计端口配置
    Config_Clocks(); //配置时钟
    Config_Timer0();

    _BIS_SR(GIE);
    //开始循环测试信号，并显示在液晶上
}

void Config_Clocks() //配置时钟，该时钟将决定 定时器 溢出的时间
{
	BCSCTL1 = (CALBC1_M); // Set range
	DCOCTL = (CALDCO_M); // Set DCO step + modulation
	BCSCTL3 |= LFXT1S_2;  /* Mode 2 for LFXT1 : VLO */

	IFG1 &= ~OFIFG; // Clear OSCFault flag

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

		//    MCLK=DC0       不分频
	BCSCTL2 |= SELM_0 + DIVM_0 + DIVS_0; // SMCLK = DCO
}

void Config_Timer0() //设置定时器
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

	//      SMCLK     continuous               /1
	TACTL = TASSEL_2 + MC_2 + TAIE + TACLR + ID_0; //TA基准时钟为SMCLK，且SMCLK与MCLK频率相同
} //连续计数模式、无分频、开启定时中断


void Config_Port() //设置端口
{
    P1DIR &= ~BIT5; //  P1.5设为输入
    P1IES &= ~BIT5; //  P1.5中断触发沿：上升沿；
    P1IE |= BIT5; //    开启P1.5 I/O中断
}



//  I/O中断程序，利用中断记录信号周期个数
#pragma vector=PORT1_VECTOR
__interrupt void P1_ISR (void)
{
    if(P1IFG & BIT5){                  // P1.5口产生中断标志，则计数器加一，记一周期
    	COUNT++;
        if(COUNT == 1) // 第一个信号上升沿来临的时候，开启定时器计数
        	TACTL |= TACLR;  //配置定时器Timer0

    }

    if(TA_OverflowCnt >= 50)  // 设置闸门时间，12M晶振，16位计数器溢出42
    {
        Period = (((unsigned long long int)TA_OverflowCnt) << 16) + TAR;//获得给定阀门时间内的时钟总计数值
        F = (COUNT) * DCO_M * 1000 / Period;

     /*   if(F >= 60000)
        	Long2char(F);
        else
        	sprintf(STR_F, "%d" , F); */

        Long2char(F);
        LCD_Write_String(18,0,STR_F);

        TA_OverflowCnt = 0; 	// 定时器溢出器清零，重新开始定时计数
        COUNT = 0;
        TACTL |= TACLR;            //   定时器清零
    }
    P1IFG = 0;         // P1端口的中断标志位清零，等待下一次P1.5中断
}


//   Timer_A中断程序
#pragma vector=TIMER0_A1_VECTOR
__interrupt void Timer_A(void)
{
    switch(TAIV)                //  TA中断源软件判断,R0有自己单独的中断向量
    {
    case 2:
        break;               // 中断源为R1
    case 4:
        break;               // 中断源为R2
    case 10:
        TA_OverflowCnt++;    //   中断源为Timer,计满数，溢出中断计数加1
        break;
    }
}

void Long2char(long int temp)      // long int to char...转换数字为字符串并保存在STR_F数组中
{
    int i = 8;

    while(i >= 0 || temp > 0)
    {
        STR_F[i] = temp % 10 + '0';
        temp /= 10;
        i--;
    }
}

