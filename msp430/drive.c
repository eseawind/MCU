/**********************************************
 *
 * 		Author	:		Shawn Guo
 *      Mail    :       iguoxiaopeng@gmail.com
 * 		Date	:       2013/7/27
 *      Last    :       2013/8/3   确定f2618 IO端口及PWM配置
 *                      2013/8/14  优化代码,增强可读性
 *                      2013/8/18  金属探测功能
 *
 *                      2013/8/    光源追踪功能
 *
 *
 * 		Notes	:       L293N for Smart Car
 * 		Tool    :       MSP430X2XX
 **********************************************/

/*****************************************************************************
* @file       drive.c
* @addtogroup SMART CAR
* @{
******************************************************************************/

#include <msp430f2618.h>
#include "clock.h"
#include "Timer.h"
#include "drive.h"

/***************************************************************************//**
 * 软硬隔离层_START, 下面的代码与硬件有关, 硬件改动时需要配置
 ******************************************************************************/

void Drive_PWM_Init()   //默认PWM频率1000HZ
{
    TimerA_R_Init(2, TIMERA_DIV, 1, MOTOR_PWM_NUM,0,0); // TimerA Up mode , 1ms定时
    TACCTL2 = OUTMOD_7;
    TACCTL1 = OUTMOD_7;
}


void Drive_Servo_Init() //开启了TimerB1中断,若开_EINT(),则一定要写中断函数防止跑飞.
{
    SERVO_IO;
    TimerB_UpMode_Init(2, TIMERB_DIV, SERVO_PWM_NUM); // 20ms的PWM信号
    TBCCTL5 = OUTMOD_7;
}

void Drive_UTL_Init()  //超声波模块IO初始化
{
    UTL_IO_INIT;

    //接受端口ECHO, 只配置捕获模式  TB1 P4.1
    // 需另行配置定时器TIMERB工作方式和TBCCRx的值以及中断处理函数, 可结合舵机配置一次TimerB
    TimerB_CAP1_Init(1, 3, 1);
    TBCCTL1 &= ~CCIFG;
}

/***************************************************************************//**
 * 软硬隔离层_END,下面的代码与硬件无关
 ******************************************************************************/
void Drive_Init(unsigned char mode, unsigned int speed)
{
    Drive_IO_Init();
    Drive_PWM_Init();
    Drive_Servo_Init();
    Drive_Speed_IO();
    Drive_UTL_Init();

    Drive_Motor_L(mode, speed);
    Drive_Motor_R(mode, speed);
}

void Drive_IO_Init()
{
    // 输入端口
    INL_DIR;
    INR_DIR;
    IN1_0;
    IN2_0;
    IN4_0;
    IN5_0;

    // 使能端口
    ENL_IO;
    ENR_IO;
}

void Drive_Motor_L(unsigned char mode, unsigned int speed)
{
    if(mode == 0)   //forward
        MOTOR_LEFT_GO;
    else if(mode == 1)           // backward
        MOTOR_LEFT_BACK;
    else                // stop
    {
        MOTOR_LEFT_STOP;
        return;
    }

    if(speed)       // set the new speed by pwm
        MOTOR_PWM_TXCCRL = (int)(MOTOR_PWM_NUM / 100 * speed);  // MODE 7, PWM高电平持续时间，决定占空比
}

void Drive_Motor_R(unsigned char mode, unsigned int speed)
{
    if(mode == 0)   //forward
        MOTOR_RIGHT_GO;
    else if(mode == 1)           // backward
        MOTOR_RIGHT_BACK;
    else                // stop
    {
        MOTOR_RIGHT_STOP;
        return;
    }

    if(speed)       // set the new speed by pwm
        MOTOR_PWM_TXCCRR = (int)(MOTOR_PWM_NUM / 100 * speed );  // MODE 7, PWM高电平持续时间，决定占空比
}


void Drive_Morto_Speed_LL()    //将左电机的速度增加一个最小刻度,最小刻度可以编程设置.
{
    MOTOR_PWM_TXCCRL += MOTOR_MINI_PWM_SCALE;
}

void Drive_Morto_Speed_L_()    //将左电机的速度增加一个最小刻度,最小刻度可以编程设置.
{
    MOTOR_PWM_TXCCRL -= MOTOR_MINI_PWM_SCALE;
}

void Drive_Morto_Speed_RR()
{
    MOTOR_PWM_TXCCRR += MOTOR_MINI_PWM_SCALE;
}
void Drive_Morto_Speed_R_()
{
    MOTOR_PWM_TXCCRR -= MOTOR_MINI_PWM_SCALE;
}


void Drive_Around(unsigned char mode, unsigned char angle)
{
    if(mode == 0)       //左转        1M时钟下测得。
    {
        while(angle--)
        {
            Drive_Motor_L(0, 60);
            Drive_Motor_R(1, 60);
            DELAY_MS(333);
            MOTOR_LEFT_STOP;
            MOTOR_RIGHT_STOP;
        }
    }
    else                //右转
    {
        while(angle--)
        {
            Drive_Motor_L(1, 60);
            Drive_Motor_R(0, 60);
            DELAY_MS(333);
            MOTOR_LEFT_STOP;
            MOTOR_RIGHT_STOP;
        }
    }
}

//===========码盘测速================

void Drive_Speed_IO()
{
    SPEED_PXDIR &= ~(SPEED_BITL + SPEED_BITR);
    SPEED_PXREN |=  (SPEED_BITL + SPEED_BITR);
    SPEED_PXOUT |=  (SPEED_BITL + SPEED_BITR);    //上拉电阻
    SPEED_PXIES |=  (SPEED_BITL + SPEED_BITR);    //中断触发沿：下降沿
    SPEED_PXIE  |=  (SPEED_BITL + SPEED_BITR);
    SPEED_PXIFG &= ~(SPEED_BITL + SPEED_BITR);
}

unsigned int Drive_Speed_Journey_L()
{
    //SPEED_BUF_L   码盘总数
    //SPEED_GRITH   一个码盘代表的距离的十倍
    return (SPEED_BUF_L * SPEED_GRITH / 10);    //返回一段时间内走过的路程, mm
}

unsigned int Drive_Speed_Journey_R()
{
    return (SPEED_BUF_R * SPEED_GRITH / 10);
}

//=============舵机驱动=======================

void Drive_Servo_Move(unsigned int angle)
{
    SERVO_TXCCR = SERVO_MIN + (((SERVO_MAX - SERVO_MIN) / SERVO_FULL_SCALE) * angle);
}

void Drive_Servo_Turn(unsigned int angle1, unsigned int angle2)  //慢慢从angle1转动到angle2，速度由SERVO_FULL_SCALE 、SERVO_DELAY决定
{
    angle1 = (SERVO_FULL_SCALE) / 180 * angle1;
    angle2 = (SERVO_FULL_SCALE) / 180 * angle2;

    if(angle1 > angle2) //逆转
        for(; angle1 > angle2; angle1--)
        {
            SERVO_DELAY;
            Drive_Servo_Move(angle1);
        }
    else
    {
        for(; angle1 < angle2; angle1++)
        {
            SERVO_DELAY;
            Drive_Servo_Move(angle1);
        }
    }
}


//====================================

//根据UTL_START_TIME, UTL_END_TIME, UTL_TIME_OVERFLOW和时钟频率，返回距离值 cm.
void Drive_UTL_DISTANCE()
{
    unsigned long int time = (unsigned long int)(UTL_END_TIME + UTL_TIME_OVERFLOW * UTL_OVERFLOW_TIMER - UTL_START_TIME);

    UTL_BUFFER = (int)((time * 17) / (TIMERB_F / 1000));
}

/***************************************************************************//**
 * 自定义的模块寄存器,用于特定用处,请勿修改
 ******************************************************************************/
//码盘计数器,每经过一个码盘空隙, 下降沿中断使得SPEED_COUNT_x ++;
extern int SPEED_COUNT_L = 0, SPEED_COUNT_R = 0, SPEED_BUF_L = 0, SPEED_BUF_R = 0;
//速度显示标志位, 当速度计算出来, 置一表示可以显示.
extern unsigned char SPEED_DISPLAY_FLAG = 0;


//记录超声波模块返回的脉冲时间
// UTL_FLAG = 0  超声波空闲，可以开始计时
// UTL_FLAG = 1  超声波正在计时, 若中断来临表示计时完成
// UTL_FLAG = 2  超声波计时完成, 等待处理
extern unsigned int UTL_START_TIME = 0, UTL_END_TIME = 0, UTL_TIME_OVERFLOW = 0, UTL_BUFFER = 0, UTL_FLAG = 0;
extern unsigned int UTL_LLEFT = 0, UTL_LEFT = 0, UTL_FRONT = 0, UTL_RIGHT = 0, UTL_RRIGHT = 0;

//JSTC_FALG = 0, 没有探测到金属
//JSTC_FLAG = 1, 探测到金属
extern unsigned char JSTC_FLAG = 0;


/***************************************************************************//**
 * @}
 ******************************************************************************/


/***************************************************************************//**
 * @brief
 * @param
 * @return none
 *******************************************************************************/
