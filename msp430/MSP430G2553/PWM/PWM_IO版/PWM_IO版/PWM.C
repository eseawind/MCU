/**********************************************
 *
 * 		Author	:		Shawn Guo
 *      Mail    :       iguoxiaopeng@gmail.com
 * 		Date	:		2013/5/4
 *      Last    :       2013/5/4
 * 		Notes	:       PWM方波输出
 * 		Tool    :	    MSP430G2553
 *
 *      P1.2 ---> TA0.1
 *      Timer_A (DCO CLK)
 **********************************************/
#include <msp430g2553.h>
#include "PWM.h"

double PWM_F = 100; // 频率默认100HZ
double PWM_DC = 0.50; // 占空比默认50%
long int PWM_CLK = 1000000; // 1M DCO CLK

void PWM_IO_Init()   //配置IO端口
{
    P1DIR |= BIT2;      // 配置P1.2(TA0.1)为输出
    P1SEL |= BIT2;      // 设为第二功能
}

void PWM_Init()     // 配置方波频率，占空比
{
    PWM_IO_Init();
    CCTL1 = OUTMOD_7;  // MODE 7
    PWM_Set(PWM_F, PWM_DC);
}

void PWM_Set(int f, double dc)  // 由给定的频率F和定时器的时钟频率确定定时器TACCR0的值与TACCR1的值；
{
    CCR0 = (int)(PWM_CLK / f);      // PWM总周期， 即PWM波形频率设置
    CCR1 = (int)(CCR0 * dc);  // MODE 7, PWM高电平持续时间，决定占空比
}


