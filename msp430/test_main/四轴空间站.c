/**********************************************
 *
 * 		Author	:		Shawn Guo
 *      Mail    :       iguoxiaopeng@gmail.com
 * 		Date	:       2013/9/4
 *      Last    :       2013/9/4
 * 		Notes	:       四旋翼空间站
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
#include "PID.h"

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

    //四轴PID控制初始化. 初始化3个PID控制器
    //开启TimerA 1ms中断, 最后在中断置标志位以处理数据
    PID_Plane_Init();
    //电调未解锁之前先关闭TACCR0中断.
    TimerA_R0_Disable();

    _EINT();

    //九轴姿态传感器初始化

    DELAY_MS(3000);
    CJMCU_INIT();
    CJMCU_Aline();  //自动校准

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
                PLANE_DT_LOCK = 0;
                PLANE_DT_CCR1(PID_DEFAULT);
                PLANE_DT_CCR2(PID_DEFAULT);
                PLANE_DT_CCR3(PID_DEFAULT);
                PLANE_DT_CCR4(PID_DEFAULT);
                DELAY_MS(1000);

                TimerA_R0_Enable();         //打开定时器, 开始定时PID控制
                break;

            case CODE_AUTO_LAND:

                TimerA_R0_Disable(); //关闭定时器, 取消定时PID控制
                PLANE_DT1(20);
                PLANE_DT2(20);
                PLANE_DT3(20);
                PLANE_DT4(20);

                DELAY_MS(3000);

                break;

            case CODE_POWEROFF:
                TimerA_R0_Disable(); //关闭定时器, 取消定时PID控制
                Plane_Drive_Lock();
                DELAY_MS(1000);
                break;
            }
        }

        if(MAIN_FLAG == 1 )  //读取姿态并进行PID控制
        {

            MAIN_FLAG = 0;

            CJMCU_START(); //只读取一次。每次读取数据时都要调用该函数
            //while(CJMCU_RCV_FLAG == 0);
            DELAY_MS(10);
            CJMCU_GET_VALUE(&yaw, &pitch, &roll);

            pitch_pid_out = PID_Calc(&Pitch_PID, pitch);
            roll_pid_out = PID_Calc(&Roll_PID, roll);


            //将PID的输出转换为PWM信号给电调.
            //-500-0-500的PID输出对应着给定的油门信号(PWM: 3000(0%), 4500(50%))


            // PITCH(上负下正,控制1,4电机)
            //PLANE_DT_CCR1( PID_DEFAULT + pitch_pid_out);
            PLANE_DT_CCR1(PID_DEFAULT);       //1号电机为仰卧轴基准.
            PLANE_DT_CCR4(PID_DEFAULT - pitch_pid_out);

            // ROLL (左正右负, 控制2,3电机)
            PLANE_DT_CCR2( PID_DEFAULT);   // 2号电机为翻滚轴基准.
            PLANE_DT_CCR3( PID_DEFAULT - roll_pid_out);
        }

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


//TimerA0 10ms中断, 用作读取姿态并PID控制
#pragma vector=TIMERA0_VECTOR
__interrupt void TimerA0(void)
{
    static char tt = 0;
    static char safe_flag = 0;  //安全标志, 每60秒自动锁定电调,保证安全.

    if(tt++ >= 5 && PLANE_DT_LOCK == 0)   //四轴解锁并且定时到了,则进行姿态测量
    {
        MAIN_FLAG = 1;
        tt = 0;
        if(safe_flag++ >= 30) // 20s ！！！自动关闭电调
        {
            //TimerA_R0_Disable(); //关闭定时器, 取消定时PID控制
            //Plane_Drive_Lock();
            //safe_flag = 0;
            //MAIN_FLAG = 0;
        }
    }

    TACCTL1 &= ~CCIFG;
}


