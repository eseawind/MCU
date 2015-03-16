/**********************************************
 *
 * 		Author	:		Shawn Guo
 *      Mail    :       iguoxiaopeng@gmail.com
 * 		Date	:		2013/5/4
 *      Last    :       2013/5/4
 * 		Notes	:       PWM方波输出
 * 		Tool    :	    MSP430G2553
 *
 **********************************************/

#ifndef __PWM__
#define __PWM__


void PWM_IO_Init();   //配置IO端口
void PWM_Init();     // 配置定时器，配置方波频率，占空比
void PWM_Set(int f, double dc);  // 由给定的频率F和定时器的时钟频率确定定时器TACCR0的值与TACCR1的值；

#endif
