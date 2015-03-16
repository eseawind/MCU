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
 *      油门行程设定: 油门最高, 电调上电, 延时2s, 油门拉低，油门行程
 **********************************************/


#include <msp430f2618.h>
#include "clock.h"
#include "Timer.h"
#include "Plane_Drive.h"


int main()
{
    WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
    Clock_Init(4,4,0,CPU_FF); // 1M DCO


    P4DIR |= BIT1 + BIT2 + BIT3 + BIT4;
    P4SEL |= BIT1 + BIT2 + BIT3 + BIT4;

    //SMCLK时钟,  分频 /4 , 默认TimerB 3M时钟. 配置TimerB 四路PWM信号，
    TimerB_UpMode_Init(2, 4, PLANE_TXCCR0);
    TimerB_Disable();   //关闭TBCCR0中断, 避免影响其它模块
    TimerB_COM_Init(0, 0);  //不改变TXCCR0的值, TimerB 7通道独立


    TimerB_COM1(PLANE_HIGH_PWM, 7);
    TimerB_COM2(PLANE_HIGH_PWM, 7);
    TimerB_COM3(PLANE_HIGH_PWM, 7);
    TimerB_COM4(PLANE_HIGH_PWM, 7);

    DELAY_MS(2000);

    PLANE_DT1(0);
    PLANE_DT2(0);
    PLANE_DT3(0);
    PLANE_DT4(0);

    DELAY_MS(1000);

    PLANE_DT1(30);
    PLANE_DT2(30);
    PLANE_DT3(30);
    PLANE_DT4(30);

}
