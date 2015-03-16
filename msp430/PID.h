/**********************************************
 *
 *      Author  :       Shawn Guo
 *      Mail    :       iguoxiaopeng@gmail.com
 *      Date    :       2013/9/5
 *      Last    :       2013/9/5
 *      Notes   :       PID控制模块
 *      Tool    :       MSP430X2XX
 **********************************************/

#ifndef __PID_H__
#define __PID_H__

#include "CJMCU.h"

/************************
*       控制思想:
*           以1号电机，2号电机为基准，根据仰卧角和翻滚角结合PID控制器调节平衡.
*           通过改变1/2电机的转速可以控制四轴实现任意姿态.
*
*************************/


// 挂绳悬摆时, P = 1, D = 40, 油门40% 悬浮稳定. pid_pitch_out / 2.




//PID的采样频率,使用TimerA定时中断(10ms中断), 固定频率采样并进行一次PID处理.
#define     PID_F           (100)

#define     PID_DEFAULT     (4200)  //默认四个电机初始转速4500(50%)

//四旋翼 PID的预设值(YAW, PITCH, ROLL)
#define     PID_YAW_SETPOINT        (CJMCU_YAW_OFFSET)
#define     PID_YAW_P               (0)
#define     PID_YAW_I               (0)
#define     PID_YAW_D               (0)
#define     PID_YAW_MULTIPLE        (1) //PID系数放大倍率, 最后结果除以该值.

#define     PID_PITCH_SETPOINT      (CJMCU_PITCH_OFFSET)
#define     PID_PITCH_P             (2)
#define     PID_PITCH_I             (0)
#define     PID_PITCH_D             (40)
#define     PID_PITCH_MULTIPLE      (1)

#define     PID_ROLL_SETPOINT       (CJMCU_ROLL_OFFSET)
#define     PID_ROLL_P              (2)
#define     PID_ROLL_I              (0)
#define     PID_ROLL_D              (40)
#define     PID_ROLL_MULTIPLE       (1)

//PID的精度, 即将输出结果划分为1000个档位. 正PID_ROLL_SETPOINT负各500档 划分越精细, 控制越精细.
//输入：姿态角度变化: -2000 ~ 2000(已经放大100倍的值)
//输出: PID档位变化:  -500 ~ 500档
//执行: 电机PWM变化:  -500 ~ 500油门变化  (中心值在PID_DEFAULT. 1%油门 = 30定时器值.)
#define     PID_ACCURACY    (500)

#define     PID_YAW_RANGE       (20)    //航向角YAW输入范围, +-2000度
#define     PID_PITCH_RANGE     (20)    //俯仰角PITCH输入范围
#define     PID_ROLL_RANGE      (20)    //翻滚角ROLL输入范围
#define     PID_MULTIPLE        (10)    // 输入+-50 至输出+-500的映射

typedef struct PID
{
    int SetPoint;       //设定目标值

    int Proportion;     //比例常数
    int Integral;       //积分常数
    int Derivative;     //微分常数

    int Multiple;         //PID系数的放大倍数, 最后需要处于该值.

    int LastError;      //Error[-1],前一误差值记录
    int PrevError;      //Error[-2],更前一误差值记录
    long SumError;       //Sums of Errors

}   PID;


//输入一个PID结构指针, 以及下一个测量值, 计算出对应的控制量.
int PID_Calc(PID *pp, int NextPoint);

//四轴PID控制初始化. 初始化3个PID控制器, 开启TimerA 1ms中断, 最后在中断置标志位以处理数据
void PID_Plane_Init();
void PID_Plane();



/*********  全局变量  ***********************/
extern PID Yaw_PID , Pitch_PID , Roll_PID ;


/***************************************************************************//**
    示例代码:
 *******************************************************************************/

#endif /* __MODULE_H__ */
