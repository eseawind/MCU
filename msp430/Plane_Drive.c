/**********************************************
 *
 *      Author  :       Shawn Guo
 *      Mail    :       iguoxiaopeng@gmail.com
 *      Date    :       2013/8/27
 *      Last    :       2013/8/31
 *      Notes   :       四旋翼驱动模块 for NAZA LITE
 *      Tool    :       MSP430X2XX  / 好盈天行者20A电调    / 朗宇2212 KV980无刷电机
 *
 **********************************************/

/*****************************************************************************
* @file       Plane_Drive.c
* @addtogroup Plane
* @{
******************************************************************************/

#include <msp430f2618.h>
#include "clock.h"
#include "Timer.h"
#include "Plane_Drive.h"


//电调锁定标志, 置0表示电调未锁定, 置1表示已锁定.
extern char PLANE_DT_LOCK = 1;


//开启4路PWM信号IO, 输出默认的4路PWM信号.
void Plane_Drive_Init_IO()
{
    P4DIR |= BIT1 + BIT2 + BIT3 + BIT4;
    P4SEL |= BIT1 + BIT2 + BIT3 + BIT4;

    //SMCLK时钟,  分频 /4 , 默认TimerB 3M时钟. 配置TimerB 四路PWM信号，
    TimerB_UpMode_Init(2, 4, PLANE_TXCCR0);
    TimerB_Disable();   //关闭TBCCR0中断, 避免影响其它模块
    TimerB_COM_Init(0, 0);  //不改变TXCCR0的值, TimerB 7通道独立

    //将油门拉至最低,解锁.
    TimerB_COM1(PLANE_LOW_PWM, 7);
    TimerB_COM2(PLANE_LOW_PWM, 7);
    TimerB_COM3(PLANE_LOW_PWM, 7);
    TimerB_COM4(PLANE_LOW_PWM, 7);

    DELAY_MS(3000);

    PLANE_DT_LOCK = 0;

}

//解锁飞控板
void Plane_Drive_Unlock()
{
    //好盈电调解锁(油门拉最低)
    if(PLANE_DT_LOCK == 1)
    {
        PLANE_DT1(0);
        PLANE_DT2(0);
        PLANE_DT3(0);
        PLANE_DT4(0);
        DELAY_MS(3000);
        PLANE_DT_LOCK = 0;
    }
}


//锁定飞控板
void Plane_Drive_Lock()
{
    if(PLANE_DT_LOCK == 0)
    {
        PLANE_DT1(0);
        PLANE_DT2(0);
        PLANE_DT3(0);
        PLANE_DT4(0);
        PLANE_DT_LOCK = 1;
    }

}


//油门从0%遍历到100%,并返回. 每次变换10%, 保持1s.
void Plane_Drive_Accele()
{
    unsigned char i = 0;

    for(i = 2; i <= 10; i++)
    {
        PLANE_DT1(i * 10);
        PLANE_DT2(i * 10);
        PLANE_DT3(i * 10);
        PLANE_DT4(i * 10);

        DELAY_MS(1000);

        PLANE_DT1(10);
        PLANE_DT2(10);
        PLANE_DT3(10);
        PLANE_DT4(10);

        DELAY_MS(1000);
    }
}

//最短的延迟, 只保证平稳起飞, 不包括自稳
void Plane_Auto_Launch()
{
    Plane_Drive_Unlock();
    while(PLANE_DT_LOCK == 1);
}


/***************************************************************************//**
 * @brief
 * @param
 * @return none
 *******************************************************************************/


/***************************************************************************//**
 * @}
 ******************************************************************************/
