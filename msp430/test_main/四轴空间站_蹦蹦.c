/**********************************************
 *
 * 		Author	:		Shawn Guo
 *      Mail    :       iguoxiaopeng@gmail.com
 * 		Date	:       2013/9/4
 *      Last    :       2013/9/4
 * 		Notes	:       四旋翼空间站——蹦蹦
 *                      地面站的单片机按键对应响应的功能,
 *                      发射信号给四轴空间站,四轴接受信号以后, 执行响应操作.
 *
 * 		Tool    :       MSP430X2XX  / NRF905
 **********************************************/

#include "clock.h"
#include "USCI.h"
#include "NRF905.h"
#include "Timer.h"
#include "CJMCU.h"
#include "Plane_Drive.h"

#include <msp430f2618.h>



/************************地面站命令******************************
*
*   1. 自动起飞(0x01):  起飞四旋翼, 将自动进入悬停模式, 保持自稳.
*   2. 失控降落(0x02):  调整四旋翼进入失控模式, 自动降落.
*   3. 强制关机(0x03):  立刻关闭四旋翼电机, 并强制死循环.
*
*
*
******************************************************************/



#define     CODE_AUTO_LAUNCH         (0x01)
#define     CODE_AUTO_LAND           (0x02)
#define     CODE_POWEROFF            (0x03)

#define     DT_MAIN_DEFAULT          (4000)     //默认电调油门

//NRF905无线模块 全局变量
//TXADDRESS: 飞行器无线地址 0xaa,0xbb,0xcc,0xdd
//RXADDRESS: 地面站无线地址 0xdd,0xcc,0xbb,0xaa

unsigned char NRF905_RXDATA[2] = {0x00, 0x00};
unsigned char NRF905_READ_FLAG = 0;

int yaw = 0, pitch = 0, roll = 0; //姿态角

//PID控制后得到的输出. 将输出对应PWM信号
int yaw_pid_out = 0, pitch_pid_out = 0, roll_pid_out = 0;


//主循环标志位, 0: 空闲. 1: 读取姿态并PID控制
unsigned char MAIN_FLAG = 0;

int temp = 0;

int main()
{
    WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
    Clock_Init(4,4,0,CPU_FF); // 12M DCO

    //RF905 接受数据中断配置
    P1DIR &= ~BIT7; //  设为输入
    P1IES &= ~BIT7; //  中断触发沿：上升沿；
    P1IE  |=  BIT7; //    开启 I/O中断
    P1IFG &= ~BIT7;
    //等待按键中断进行发射命令.

    NRF905_Init();
    NRF905_Set_RevAddress(0xdd,0xcc,0xbb,0xaa);     //修改接受地址
    NRF905_Set_SendAddress(0xaa, 0xbb, 0xcc, 0xdd); //配置发送地址: 四轴无线地址

    //初始化飞控板PWM信号,解锁飞控
    Plane_Drive_Init_IO();
    Plane_Drive_Lock();

    //开环控制...

    _EINT();

    //九轴姿态传感器初始化

    NRF905_Set_Mode(NRF905_RCV);   //切换到发送模式, 从此一直重复发送数据



    while(1)
    {
        if(NRF905_READ_FLAG == 1)
        {
            //读取标志位清零
            NRF905_READ_FLAG = 0;
            //读取数据
            NRF905_Read_Data(NRF905_RRP, NRF905_RXDATA, 2);

            //进行校验一次
            switch(NRF905_RXDATA[0])
            {
            case CODE_AUTO_LAUNCH:


                //起飞操作.
                PLANE_DT_LOCK = 0;

                PLANE_DT1(60);
                PLANE_DT2(60);
                PLANE_DT3(60);
                PLANE_DT4(60);


                DELAY_MS(500);
                /*
                PLANE_DT1(20);
                PLANE_DT2(20);
                PLANE_DT3(20);
                PLANE_DT4(20);

                DELAY_MS(2000);
                */

                PLANE_DT1(42);
                PLANE_DT2(40);
                PLANE_DT3(41);
                PLANE_DT4(40);

                DELAY_MS(500);

                break;

            case CODE_AUTO_LAND:

                PLANE_DT1(20);
                PLANE_DT2(20);
                PLANE_DT3(20);
                PLANE_DT4(20);

                DELAY_MS(3000);


                PLANE_DT1(0);
                PLANE_DT2(0);
                PLANE_DT3(0);
                PLANE_DT4(0);
                PLANE_DT_LOCK = 1;

                break;

            case CODE_POWEROFF:
                PLANE_DT_LOCK = 0;
                Plane_Drive_Lock();

                break;
            }
        }
        Plane_Drive_Lock();
    }
}




#pragma vector=PORT1_VECTOR
__interrupt void P1_ISR (void)
{
    if(P1IFG & BIT7)
    {
        NRF905_READ_FLAG = 1;
    }

    P1IFG = 0;
}


/*******************************

//起飞操作

//降落操作

//前进操作

//后退操作


*******************************/

