/**********************************************
 *
 * 		Author	:		Shawn Guo
 *      Mail    :       iguoxiaopeng@gmail.com
 * 		Date	:       2013/8/31
 *      Last    :       2013/8/31
 * 		Notes	:       单片机模拟PWM信号操作 NAZA 飞控板
 * 		Tool    :       MSP430F2618 /  NAZA LITE飞控 / 四旋翼F450
 *
 *
 *
 *
 *
 **********************************************/


#include <msp430f2618.h>
#include "clock.h"
#include "Timer.h"
#include "Plane_Drive.h"

int main()
{
    WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT

    Clock_Init(4,4,0,CPU_FF); // 12M XT2 MCLK/SMCLK, 12M DCO ACLK

    //飞控解锁test
    DELAY_MS(1000);   //延时50ms等待飞控板初始化.多延时以保证电池安装完毕以及飞控系统初始化
    Plane_Drive_Init_IO();
    DELAY_MS(1000); // 延时1s保证系统初始化和自检查时间，延时过短会出现 绿绿绿绿LED灯闪烁.

    PLANE_BREAK_CENTER; //掰杆后启动电机，电机有延迟性，不可低于2s
    DELAY_MS(2000);

    PLANE_BREAK_CENTER; //掰杆以关闭电机

    DELAY_MS(5000);

    //失控保护test
    PLANE_BREAK_CENTER; //掰杆以启动电机
    DELAY_MS(1000);

    PLANE_ACCELERATOR(80);  //80%油门
    DELAY_MS(3000);


    while(1)
    {
        PLANE_OUTCONTROL;       //切换到失控保护模式,油门变低
        DELAY_MS(5000);
        PLANE_DIS_OUTCONTROL;   //切换到姿态模式, 油门恢复
        DELAY_MS(5000);
    }




    /*
        while(1)
        {
            //Plane_Drive_Accele();
        }

        */
}
