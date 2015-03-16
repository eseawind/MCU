/**********************************************
 *
 * 		Author	:		Shawn Guo
 *      Mail    :       iguoxiaopeng@gmail.com
 * 		Date	:       2013/8/27
 *      Last    :       2013/8/27
 * 		Notes	:       四旋翼-无刷直流电机测试
 * 		Tool    :       MSP430F2618/好盈20A PWM电调/朗宇2212 KV980电机
 *
 *
 *
 *
 *
 **********************************************/


#include <msp430f2618.h>
#include "clock.h"
#include "Timer.h"

int main()
{
    WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
    Clock_Init(0,0,0,CPU_FF); // 1M DCO

    //PWM IO配置
    P4DIR |= BIT1 + BIT2 + BIT3 + BIT4;
    P4SEL |= BIT1 + BIT2 + BIT3 + BIT4;


    //1M时钟,  配置TimerB 四路PWM信号， R0 为20ms
    TimerB_UpMode_Init(2, 1, 20000);
    TimerB_COM_Init(0, 0);
    TimerB_COM1(1500, 7); //副翼
    TimerB_COM2(1500, 7); //升降舵
    TimerB_COM3(1000, 7); //油门,开始时最低
    TimerB_COM4(1500, 7); //方向舵,开始居中，等待1s拉最右，解锁飞控板

    DELAY_MS(1000);

    TBCCR4 = 2000;  //方向舵最右，飞控解锁.

    DELAY_MS(2000);

    while(1)
    {
        if(TBCCR3 < 2100)
        {
            TBCCR3 += 100;
        }
        else
        {
            TBCCR3 = 1000;
        }
        DELAY_MS(1000);
    }

}
