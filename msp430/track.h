/**********************************************
 *
 * 		Author	:		Shawn Guo
 *      Mail    :       iguoxiaopeng@gmail.com
 * 		Date	:       2013/7/29
 *      Last    :       2013/8/3        移植到f2618
 *                      2013/8/19       应用于《简易智能小车》
 *                      2013/8/20       实现寻光功能
 * 		Notes	:       黑线探测(躲避黑线/循迹黑线) 默认黑线正常情况不可能被相邻两个传感器探测到.
 *                      寻找光源(遇到光则为低电平.)
 *
 * 		Tool    :       MSP430f2618
 *                      红外线发射管+接受管  --> LM339迟滞比较器
 *                      有反射光线(输出0), 无反射光线【黑线】(1)
 *
 *                      光敏二极管           ---> 电压比较器
 *                      有光线入射(输出0), 无光线为1
 *
 *
 **********************************************/


#ifndef __TRACK_H__
#define __TRACK_H__

#include "drive.h"



/********************************
    物理位置: OUT4      OUT3    OUT2    OUT1
                左       <------>        右
*********************************/

/************************** 红外探测端口IO配置 *************************/
#define     TRACK_OUT1      (P5IN & 0X10)     // P5 BIT4
#define     TRACK_OUT2      (P5IN & 0X20)     // P5 BIT5
#define     TRACK_OUT3      (P5IN & 0X40)     // P5 BIT6
#define     TRACK_OUT4      (P5IN & 0X80)     // P5 BIT7

/*************************** 循迹基本操作 *******************************/
#define     TRACK_GO_SPEED         (60)                //循迹直道速度
#define     TRACK_TURN_SPEED       (50)                //循迹弯道速度
#define     TRACK_BASIC_TIME       (10)                 // delay  x ms

#define     TRACK_GO            (Drive_Motor_L(0, TRACK_GO_SPEED), Drive_Motor_R(0, TRACK_GO_SPEED), DELAY_MS(TRACK_BASIC_TIME))//直走

#define     TRACK_LEFT          (MOTOR_LEFT_BACK, MOTOR_RIGHT_GO, DELAY_MS(TRACK_BASIC_TIME))//左
#define     TRACK_BIG_LEFT      (TRACK_LEFT, DELAY_MS(TRACK_BASIC_TIME * 2))

#define     TRACK_RIGHT         (MOTOR_LEFT_GO, MOTOR_RIGHT_BACK, DELAY_MS(TRACK_BASIC_TIME))//右
#define     TRACK_BIG_RIGHT     (TRACK_RIGHT, DELAY_MS(TRACK_BASIC_TIME * 2))

#define     TRACK_BACK          (MOTOR_LEFT_BACK,   MOTOR_RIGHT_BACK,   DELAY_MS(TRACK_BASIC_TIME))//后退
#define     TRACK_STOP          (MOTOR_LEFT_STOP,   MOTOR_RIGHT_STOP,   DELAY_MS(TRACK_BASIC_TIME))//停止

void Track_IO_Init();


void Track_Avoiding(unsigned char speed);        //避黑线模式
/*************************************************************
    OUT4    OUT3    OUT2    OUT1    操作
    0       0       0       0       直走
    0       0       0       1       小左
    0       0       1       0       小左
    0       0       1       1       大左
    0       1       0       0       小右
    0       1       0       1       -
    0       1       1       0       小左
    0       1       1       1       大左

    1       0       0       0       小右
    1       0       0       1       -
    1       0       1       0       -
    1       0       1       1       -
    1       1       0       0       大右
    1       1       0       1       -
    1       1       1       0       大右
    1       1       1       1       后退
***************************************************************/



void Track_Following();
////跟随黑线模式(左导向模式)
/*************************************************************
    OUT4    OUT3    OUT2    OUT1    操作
    0       0       0       0       后退+大右       //冲出黑线了, 后退, 自定义大右
    0       0       0       1       大右            //严重偏离，大右
    0       0       1       0       直走            //直线行驶中
    0       0       1       1       小右            //轻微右偏, 小右
    0       1       0       0       直走            //直线行驶中
    0       1       0       1       后退+小左       //前面是大弯道
    0       1       1       0       直走            //直线行驶中
    0       1       1       1       直走            //直线行驶中

    1       0       0       0       大左            //严重偏离，大左
    1       0       0       1       后退+小左       //大弯道
    1       0       1       0       直走
    1       0       1       1       直走
    1       1       0       0       小左
    1       1       0       1       小左
    1       1       1       0       直走
    1       1       1       1       停止
***************************************************************/





/*****************************
TRACK_FLAG  状态
0           直走
1           左转
2           大左
3           右转
4           大右
5           停止
******************************/


/*************************************************************
    FLAG    OUT4    OUT3        OUT2        OUT1    操作

    0直走    0    OUT3 == 1 || OUT2 == 1     0      直走
    0        1       1                              小左
    0        1       0                              大左
    0                           1             1     小右
    0                           0             1     大右
    0        1        1         1             1     停止
    0        0       0          0             0     (直走突然没有黑线了, 只可能是黑线在两传感器间，继续直走)

    1小左    1       1                              小左
    1        1       0                              大左
    1        0       1                              直走
    1        0       0                              (小左突然没有黑线, 说明冲出去或小左力度不够, 后退+大左)

    2大左    1       0                              大左
    2        1       1                              小左
    2        0       1                              直走
    2        0       0                              后退+大左

    3小右                        1             1    小右
    3                            1             0    直走
    3                            0             1    大右
    3                            0             0    (后退+大右)

    4大右                        1             0    直走
    4                            1             1    小右
    4                            0             1    大右
    4                            0             0    后退+大右
***************************************************************/

void Track_Following2();        ////死循环跟随黑线模式 (状态图)






/******************寻找光源**************
    物理位置: OUT1      OUT2    OUT3    OUT4
                左       <------>        右
*********************************/

/************************** 寻光端口IO配置 *************************/
#define     LIGHT_OUT1      (P6IN & 0X01)     // P6 BIT0
#define     LIGHT_OUT2      (P6IN & 0X02)     // P6 BIT1
#define     LIGHT_OUT3      (P6IN & 0X04)     // P6 BIT2
#define     LIGHT_OUT4      (P6IN & 0X08)     // P6 BIT3

#define     LIGHT_GO_SPEED         (60)                //寻光直道速度
#define     LIGHT_TURN_SPEED       (50)                //寻光转弯速度

//寻光动作编辑
#define     LIGHT_GO            (MOTOR_LEFT_GO,     MOTOR_RIGHT_GO,     DELAY_MS(5)) //直走
#define     LIGHT_LEFT          (MOTOR_LEFT_BACK,   MOTOR_RIGHT_GO,     DELAY_MS(5))//左
#define     LIGHT_BIG_LEFT      (LIGHT_LEFT,        DELAY_MS(5))
#define     LIGHT_RIGHT         (MOTOR_LEFT_GO,     MOTOR_RIGHT_BACK,   DELAY_MS(5))//右
#define     LIGHT_BIG_RIGHT     (LIGHT_RIGHT,       DELAY_MS(5))
#define     LIGHT_BACK          (MOTOR_LEFT_BACK,   MOTOR_RIGHT_BACK,   DELAY_MS(10))//后退
#define     LIGHT_STOP          (MOTOR_LEFT_STOP,   MOTOR_RIGHT_STOP,   DELAY_MS(10))//停止

void LIGHT_IO_Init();

//状态机
/*********状态(有光0,没光1)**************
    0   直走  (10x1)(1x01)
    1   小左  (00xx)
    2   大左  (10xx)
    3   小右  (xx00)
    4   大右  (xx10)
    5   停止  (0000)
默认:     光源不会关闭
***************************/
/*************************************************************
    FLAG    OUT1    OUT2        OUT3        OUT4        操作
    0直走     1      (OUT2 或 OUT3 == 0)       1        直走
    0         0       0         0              0        停止
    0         0       0                                 小左
    0         0       1                                 大左
    0                           0               0       小右
    0                           1               0       大右
    0          1      1         1               1       (直走突然检测不到,说明光线在传感器中间,继续直走等待检测)

    1小左
    1         0       0                                 小左
    1         0       1                                 大左
    1         1       0                                 直走
    1         1       1                                 小左过头，小右

    2大左
    2         0       0                                 小左
    2         0       1                                 大左
    2         1       0                                 直走
    2         1       1                                 大左过头, 小右

    3小右                       0               0        小右
    3                           0               1        直走
    3                           1               0        大右
    3                           1               1        小左


    4大右                       0               0        小右
    4                           0               1        直走
    4                           1               0        大右
    4                           1               1        小左

    5停止     0               0       0       0           停止

***************************************************************/
void LIGHT_Following();




void LIGHT_Following2();
////寻光源 组合逻辑
/*************************************************************
    OUT1    OUT2    OUT3    OUT4    操作
    0       0       0       0       停止            //到达光源，停止
    0       0       0       1       直走
    0       0       1       0       小左            //忽略右边
    0       0       1       1       小左            //向左偏
    0       1       0       0       小右            //向右偏
    0       1       0       1       直走            //当前不确定，前进一段进一步确定
    0       1       1       0       大左            //情况不明，默认左偏
    0       1       1       1       大左            //向左偏移

    1       0       0       0       直走            //精确光源位置
    1       0       0       1       直走            //确定光源在正前方
    1       0       1       0       直走            //情况不确定
    1       0       1       1       直走
    1       1       0       0       小右
    1       1       0       1       直走
    1       1       1       0       大右
    1       1       1       1       直走             //情况不明，默认向左
***************************************************************/

#endif /* __TRACK_H__ */
