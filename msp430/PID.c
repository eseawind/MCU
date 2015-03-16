/**********************************************
 *
 *      Author  :       Shawn Guo
 *      Mail    :       iguoxiaopeng@gmail.com
 *      Date    :       2013/9/5
 *      Last    :       2013/9/5
 *      Notes   :       PID控制模块
 *      Tool    :       MSP430X2XX
 *
 **********************************************/

/*****************************************************************************
* @file       PID.c
* @addtogroup PID
* @{
******************************************************************************/
#include <msp430f2618.h>
#include "clock.h"
#include "PID.h"
#include "Timer.h"
#include "CJMCU.h"

/*********  全局变量  ***********************/
extern PID Yaw_PID = {0}, Pitch_PID = {0}, Roll_PID = {0};


//姿态传感器校准的静止误差
extern int CJMCU_YAW_OFFSET, CJMCU_PITCH_OFFSET, CJMCU_ROLL_OFFSET;


//输入一个PID结构指针, 以及下一个测量值, 计算出对应的控制量.
int PID_Calc(PID *pp, int NextPoint)
{

    int error = 0;
    int derror = 0;   //绝对PID计算
    int result = 0;



    error = (pp->SetPoint - NextPoint) / 100;   //// 由设定值与实际值计算误差
    pp->SumError += error;              //累加误差用作积分
    derror = pp->LastError - pp->PrevError; //当前微分项误差

    pp->PrevError = pp->LastError;      //误差前移
    pp->LastError = error;

    result = (pp->Proportion * error + (pp->Integral * pp->SumError) + pp->Derivative * derror);
    result /= pp->Multiple; //除以放大倍数, 计算出真值.

    if(result > PID_ACCURACY ) result = PID_ACCURACY ;
    else if(result < ((-1) * PID_ACCURACY )) result = (-1 * PID_ACCURACY );

    //返回绝对值.
    return (result);

}


//四轴PID控制初始化. 初始化3个PID控制器
//开启TimerA 1ms中断, 最后在中断置标志位以处理数据
void PID_Plane_Init()
{
    //陀螺仪的航向角只需读取间隔时间内的偏差即可, YAW不准
    Yaw_PID.SetPoint = PID_YAW_SETPOINT;
    Yaw_PID.Proportion = PID_YAW_P;
    Yaw_PID.Integral = PID_YAW_I;
    Yaw_PID.Derivative = PID_YAW_D;
    Yaw_PID.Multiple = PID_YAW_MULTIPLE;
    Yaw_PID.LastError = Yaw_PID.PrevError = 0;

    Pitch_PID.SetPoint = PID_PITCH_SETPOINT;
    Pitch_PID.Proportion = PID_PITCH_P;
    Pitch_PID.Integral = PID_PITCH_I;
    Pitch_PID.Derivative = PID_PITCH_D;
    Pitch_PID.Multiple = PID_PITCH_MULTIPLE;
    Pitch_PID.LastError = Pitch_PID.PrevError = 0;

    Roll_PID.SetPoint = PID_ROLL_SETPOINT;
    Roll_PID.Proportion = PID_ROLL_P;
    Roll_PID.Integral = PID_ROLL_I;
    Roll_PID.Derivative = PID_ROLL_D;
    Roll_PID.Multiple = PID_ROLL_MULTIPLE;
    Roll_PID.LastError = Roll_PID.PrevError = 0;

    // 12M SMCLK, /4, 这里之用到R0, 也只开启R0中断.
    TimerA_R_Init(2, 4, 1,  (TIMERA_F / PID_F), 0, 0);
}


/***************************************************************************//**
 * @brief
 * @param
 * @return none
 *******************************************************************************/


/***************************************************************************//**
 * @}
 ******************************************************************************/
