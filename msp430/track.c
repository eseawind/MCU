/**********************************************
 *
 * 		Author	:		Shawn Guo
 *      Mail    :       iguoxiaopeng@gmail.com
 * 		Date	:       2013/7/29
 *      Last    :       2013/8/3        移植到f2618
 *                      2013/8/20       应用于《简易智能小车》,完善状态图算法
 * 		Notes	:       黑线探测(躲避黑线/循迹黑线) 默认黑线正常情况不可能被相邻两个传感器探测到.
 * 		Tool    :       MSP430f2618
 *                      红外线发射管+接受管  --> LM339迟滞比较器
 *                      有反射光线(输出0), 无反射光线【黑线】(1)
 *
 *
 **********************************************/


/*****************************************************************************
* @file       track.c
* @addtogroup TRACK for smart car
* @{
******************************************************************************/
#include <msp430f2618.h>
#include "clock.h"
#include "track.h"
#include "drive.h"



void Track_IO_Init()
{
    P5DIR &= ~(BIT4 + BIT5 + BIT6 + BIT7);
}

void LIGHT_IO_Init()
{
    P6DIR &= ~(BIT0 + BIT1 + BIT2 + BIT3);
}


/*************************************************************
    TRACK_OUT4    TRACK_OUT3    TRACK_OUT2    TRACK_OUT1    操作
    0       0       0       0       直走
    0       0       0       1       小左
    0       0       1       0       大左
    0       0       1       1       大左
    0       1       0       0       大右
    0       1       0       1       后退+大右
    0       1       1       0       后退+大右
    0       1       1       1       大大左

    1       0       0       0       小右
    1       0       0       1       后退+大右
    1       0       1       0       后退+大右
    1       0       1       1       后退+大右
    1       1       0       0       大右
    1       1       0       1       后退+大右
    1       1       1       0       大大右
    1       1       1       1       后退+大左
***************************************************************/
void Track_Avoiding(unsigned char speed)      //死循环避黑线模式
{
    while(1)
    {
        if(TRACK_OUT4 == 0)
        {
            if(TRACK_OUT3 == 0)
            {
                if(TRACK_OUT2 == 0)  //情况1: 直走
                {
                    if(TRACK_OUT1 == 0)
                    {
                        Drive_Motor_L(0, speed);
                        Drive_Motor_R(0, speed);
                        DELAY_MS(3);
                    }
                    else
                    {
                        Drive_Motor_L(1, speed);
                        Drive_Motor_R(0, speed);
                        DELAY_MS(3);
                    }
                }
                else    //情况3,4: 大左
                {
                    Drive_Motor_L(1, speed);
                    Drive_Motor_R(0, speed);
                    DELAY_MS(5);
                }
            }
            else
            {
                if(TRACK_OUT2 == 0 && TRACK_OUT1 == 0)  //情况5: 大右
                {
                    Drive_Motor_L(0, speed);
                    Drive_Motor_R(1, speed);
                    DELAY_MS(5);
                }
                else if(TRACK_OUT2 == 1 && TRACK_OUT1 == 1) // 情况8: 大大左
                {
                    Drive_Motor_L(1, speed);
                    Drive_Motor_R(0, speed);
                    DELAY_MS(10);
                }
                else    //情况 6, 7: 后退+大右
                {
                    Drive_Motor_L(1, speed);
                    Drive_Motor_R(1, speed);
                    DELAY_MS(500);
                    Drive_Motor_L(0, speed);
                    Drive_Motor_R(1, speed);
                    DELAY_MS(300);
                }
            }
        }
        else
        {
            if(TRACK_OUT3 == 0)
            {
                if(((TRACK_OUT2) == 0) && ((TRACK_OUT1) == 0))  //情况9: 小右
                {
                    Drive_Motor_L(0, speed);
                    Drive_Motor_R(1, speed);
                    DELAY_MS(3);
                }
                else    //情况10,11,12： 后退+大右
                {
                    Drive_Motor_L(1, speed);
                    Drive_Motor_R(1, speed);
                    DELAY_MS(500);
                    Drive_Motor_L(0, speed);
                    Drive_Motor_R(1, speed);
                    DELAY_MS(300);
                }
            }
            else
            {
                if(TRACK_OUT1 == 0) //情况13,15: 大右
                {
                    Drive_Motor_L(0, speed);
                    Drive_Motor_R(1, speed);
                    DELAY_MS(5);
                }
                else    //情况14,16： 后退+大右
                {
                    Drive_Motor_L(1, speed);
                    Drive_Motor_R(1, speed);
                    DELAY_MS(500);
                    Drive_Motor_L(1, speed);
                    Drive_Motor_R(0, speed);
                    DELAY_MS(300);
                }

            }

        }
    }
}



////跟随黑线模式(左导向), OUT2 OUT3为趋势预测传感器, OUT1 OUT4为严重偏离传感器
/*************************************************************
    OUT4    OUT3    OUT2    OUT1    操作
    0       0       0       0       后退+大右       //冲出黑线了, 后退, 自定义大左
    0       0       0       1       大右            //严重偏离，大右
    0       0       1       0       直走            //直线行驶中
    0       0       1       1       小右            //轻微右偏, 小右
    0       1       0       0       直走            //直线行驶中
    0       1       0       1       直走
    0       1       1       0       直走            //直线行驶中
    0       1       1       1       直走            //直线行驶中

    1       0       0       0       大左            //严重偏离，大左
    1       0       0       1       后退+大左       //大弯道
    1       0       1       0       直走
    1       0       1       1       直走
    1       1       0       0       小左
    1       1       0       1       小左
    1       1       1       0       直走
    1       1       1       1       停止
***************************************************************/
void Track_Following()        ////死循环跟随黑线模式
{
    if(TRACK_OUT4 == 0)
    {
        if(TRACK_OUT3 == 0)
        {
            if((TRACK_OUT2 == 0) )
            {
                if(TRACK_OUT1 == 0)  // case 1: 后退+大左
                {
                    TRACK_STOP;
                    TRACK_BIG_LEFT;
                }
                else //case 2:大右
                {
                    TRACK_BIG_RIGHT;
                }
            }
            else
            {
                if(TRACK_OUT1 == 0) //case 3:直走
                {
                    TRACK_GO;
                }
                else        // case 4: 小右
                {
                    TRACK_RIGHT;
                }
            }
        }
        else        //CASE 5，6，7，8
        {
            TRACK_GO;
        }
    }
    else
    {
        if(TRACK_OUT3 == 0)
        {
            if((TRACK_OUT2 == 0))
            {
                if(TRACK_OUT1 == 0) // CASE 9： 大左
                {
                    TRACK_BIG_LEFT;
                }
                else    //case 10:后退+大左
                {
                    TRACK_BACK;
                    TRACK_BIG_LEFT;
                }
            }
            else    //case 11,12: 小左
            {
                TRACK_GO;
            }
        }
        else
        {

            if(TRACK_OUT2 == 0) // case  13,14:小左
            {
                TRACK_LEFT;
            }
            else
            {
                if(TRACK_OUT1 == 0) // case 15:直走
                {

                    TRACK_GO;

                }
                else    //CASE 16: STOP
                {
                    TRACK_STOP;
                }
            }
        }
    }

    DELAY_MS(5);
}


static unsigned char TRACK_FLAG = 0;   //默认直走

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


void Track_Following2()
{
    switch(TRACK_FLAG)
    {
    case 0: //直走
        if(TRACK_OUT4)
        {
            if(TRACK_OUT3)  //小左
            {
                TRACK_LEFT;
                TRACK_FLAG = 1;
            }
            else            //大左
            {
                TRACK_BIG_LEFT;
                TRACK_FLAG = 2;
            }
        }
        else if((TRACK_OUT1))
        {
            if(TRACK_OUT2)  //小右
            {
                TRACK_RIGHT;
                TRACK_FLAG = 3;
            }
            else    //大右
            {
                TRACK_BIG_RIGHT;
                TRACK_FLAG = 4;
            }
        }
        else{ //直走
            TRACK_GO;
        }

        break;
    case 1: //小左
        if(TRACK_OUT4)
        {
            if(TRACK_OUT3 == 0) //大左
            {
                TRACK_BIG_LEFT;
                TRACK_FLAG = 2;
            }
        }
        else
        {
            if(TRACK_OUT3)  //直走
            {
                TRACK_GO;
                TRACK_FLAG = 0;
            }
            else            //后退+大左
            {
                TRACK_BACK;
                TRACK_BIG_LEFT;
                TRACK_FLAG = 2;
            }
        }
        break;
    case 2: //大左
        if(TRACK_OUT4)
        {
            if(TRACK_OUT3)  //小左
            {
                TRACK_LEFT;
                TRACK_FLAG = 1;
            }
        }
        else
        {
            if(TRACK_OUT3)      //直走
            {
                TRACK_GO;
                TRACK_FLAG = 0;
            }
            else    //后退+大左
            {
                TRACK_BACK;
                TRACK_BIG_LEFT;
            }
        }
        break;
    case 3: //小右
        if(TRACK_OUT2)
        {
            if(TRACK_OUT1 == 0)  // 直走
            {
                TRACK_GO;
                TRACK_FLAG = 0;
            }
        }
        else
        {
            if(TRACK_OUT1)  //BIG_RIGHT
            {
                TRACK_BIG_RIGHT;
                TRACK_FLAG = 4;
            }
            else    //后退+大右
            {
                TRACK_BACK;
                TRACK_BIG_RIGHT;
                TRACK_FLAG = 4;
            }

        }
        break;
    case 4: //大右
        if(TRACK_OUT2)
        {
            if(TRACK_OUT1)  //小右
            {
                TRACK_RIGHT;
                TRACK_FLAG = 3;
            }
            else    //直走
            {
                TRACK_GO;
                TRACK_FLAG = 0;
            }
        }
        else
        {
            if(!TRACK_OUT1) //后退+大右
            {
                TRACK_BACK;
                TRACK_BIG_RIGHT;
            }
        }
        break;
    case 5: //停止
        break;
    }
}




static unsigned char LIGHT_FLAG = 0;   //默认直走
/*********状态(有光0,没光1)**************
    0   直走  (10x1)(1x01)
    1   小左  (00xx)
    2   大左  (10xx)
    3   小右  (xx00)
    4   大右  (xx10)
    5   停止  (0000)
默认:     光源不会关闭
***************************/

void LIGHT_Following()
{
    switch(LIGHT_FLAG)
    {
    case 0: //直走
        if((!LIGHT_OUT1) && (!LIGHT_OUT2) && (!LIGHT_OUT3) && (!LIGHT_OUT4))    //停止
        {
            LIGHT_FLAG = 5;
            LIGHT_STOP;
        }
        if(!LIGHT_OUT1)
        {
            if(!LIGHT_OUT2) //00xx,小左
            {
                LIGHT_LEFT;
                LIGHT_FLAG = 1;
            }
            else    //01xx，大左
            {
                LIGHT_BIG_LEFT;
                LIGHT_FLAG = 2;
            }
        }
        else if(!LIGHT_OUT4)
        {
            if(!LIGHT_OUT3) //xx00,小右
            {
                LIGHT_RIGHT;
                LIGHT_FLAG = 3;
            }
            else    //xx10，大右
            {
                LIGHT_BIG_RIGHT;
                LIGHT_FLAG = 4;
            }
        }
        else
            ;
        break;
    case 1: //小左
        if(!LIGHT_OUT1)
        {
            if(LIGHT_OUT2) //01xx，大左
            {
                LIGHT_BIG_LEFT;
                LIGHT_FLAG = 2;
            }
            else    //00xx,小左
            {

            }
        }
        else
        {
            if(LIGHT_OUT2) //11xx，小右
            {
                LIGHT_RIGHT;
                LIGHT_FLAG = 3;
            }
            else    //10xx,直走
            {
                LIGHT_GO;
                LIGHT_FLAG = 0;
            }

        }
        break;
    case 2: //大左
        if(!LIGHT_OUT1)
        {
            if(LIGHT_OUT2) //01xx，大左
            {
            }
            else    //00xx,小左
            {
                LIGHT_LEFT;
                LIGHT_FLAG = 1;
            }
        }
        else
        {
            if(LIGHT_OUT2) //11xx
            {
                LIGHT_RIGHT;
                LIGHT_FLAG = 3;
            }
            else    //10xx,直走
            {
                LIGHT_GO;
                LIGHT_FLAG = 0;
            }

        }
        break;
    case 3: //小右
        if(!LIGHT_OUT4)
        {
            if(LIGHT_OUT3) //xx10，大右
            {
                LIGHT_BIG_RIGHT;
                LIGHT_FLAG = 4;
            }
            else    //xx00,小右
            {
            }
        }
        else
        {
            if(LIGHT_OUT3) //XX11
            {
                LIGHT_LEFT;
                LIGHT_FLAG = 1;
            }
            else    //XX01,直走
            {
                LIGHT_GO;
                LIGHT_FLAG = 0;
            }

        }
        break;
    case 4: //大右
        if(!LIGHT_OUT4)
        {
            if(LIGHT_OUT3) //xx10，大右
            {
            }
            else    //xx00,小右
            {
                LIGHT_RIGHT;
                LIGHT_FLAG = 3;
            }
        }
        else
        {
            if(LIGHT_OUT3) //XX11
            {
                LIGHT_LEFT;
                LIGHT_FLAG = 1;
            }
            else    //XX01,直走
            {
                LIGHT_GO;
                LIGHT_FLAG = 0;
            }

        }
        break;
    }
}



//组合逻辑寻光源LIGHT_BIG_LEFT;
void LIGHT_Following2()
{
    if(!LIGHT_OUT1)
    {
        if(!LIGHT_OUT2)
        {
            if(!LIGHT_OUT3)
            {
                if(!LIGHT_OUT4) // 0000
                {
                    LIGHT_STOP;
                }
                else        //0001
                {
                    LIGHT_GO;
                }
            }
            else
            {
                if(!LIGHT_OUT4) // 0010
                {
                    LIGHT_LEFT;
                }
                else        //0011
                {
                    LIGHT_LEFT;
                }
            }
        }
        else
        {
            if(!LIGHT_OUT3)
            {
                if(!LIGHT_OUT4) // 0100
                {
                    LIGHT_RIGHT;
                }
                else        //0101
                {
                    LIGHT_GO;
                }
            }
            else
            {
                if(!LIGHT_OUT4) // 0110
                {
                    LIGHT_BIG_LEFT;
                }
                else        //0111
                {
                    LIGHT_BIG_LEFT;
                }
            }

        }
    }
    else
    {
        if(!LIGHT_OUT2)
        {
            if(!LIGHT_OUT3)
            {
                if(!LIGHT_OUT4) // 1000
                {
                    LIGHT_GO;
                }
                else        //1001
                {
                    LIGHT_GO;
                }
            }
            else
            {
                if(!LIGHT_OUT4) // 1010
                {
                    LIGHT_GO;
                }
                else        //1011
                {
                    LIGHT_GO;
                }
            }
        }
        else
        {
            if(!LIGHT_OUT3)
            {
                if(!LIGHT_OUT4) // 1100
                {
                    LIGHT_RIGHT;
                }
                else        //1101
                {
                    LIGHT_GO;
                }
            }
            else
            {
                if(!LIGHT_OUT4) //1110
                {
                    LIGHT_BIG_RIGHT;
                }
                else        //1111
                {
                    LIGHT_GO;
                }
            }

        }

    }
}















/******************************************************************
//pid MODE

unsigned char  SetValue = 0;     //定义设定值, PID控制器将调解执行器件从ActualValue最终达到该值
unsigned char  ActualValue = 0;  //定义实际值
int  err = 0;          //定义偏差值         error--->err_next--->err_last
int  err_next = 0;     //定义上一个偏差值
int  err_last = 0;     //记录更上一个偏差值
unsigned char  kp,ki,kd = 0;     //定义比例/积分/微分系数
unsigned char temp = 0;

**/



//根据传感器的值转换为响应的ActualValue;
//这里SetValue = 0, AcutalValue -30~30.
//OUT4  OUT3     OUT2    OUT1
// -30   -10  0  10       30
/*************************************************************
    OUT4    OUT3    OUT2    OUT1    ActualValue
    0       0       0       0       -30
    0       0       0       1       大右
    0       0       1       0       小右
    0       0       1       1       大右
    0       1       0       0       小左
    0       1       0       1       直走
    0       1       1       0       直走
    0       1       1       1       直走

    1       0       0       0       大左
    1       0       0       1       直走
    1       0       1       0       小右
    1       0       1       1       大右
    1       1       0       0       大左
    1       1       0       1       大左
    1       1       1       0       直走
    1       1       1       1       停止
******************************************************/

/**
void PID_Get_ActualValue()
{
    temp = 0;
    if(TRACK_OUT4 == 1) temp += 8;
    if(TRACK_OUT3 == 1) temp += 4;
    if(TRACK_OUT2 == 1) temp += 2;
    if(TRACK_OUT1 == 1) temp += 1;

    switch(temp)
    {

    }
}

void PID_Realize()      //增量型PID算法
{
    int increment_speed = 0;

    err = SetValue - ActualValue;    // 由设定值与实际值计算误差

    //无需累加误差，只需存储最近3次的误差即可
    increment_speed = kp * (err - err_next) + ki * err +  kd * ( err - 2 *  err_next +  err_last);
    if(increment_speed > 30) increment_speed = 30;
    else if(increment_speed < -30) increment_speed = -30;
    else ;

    ActualSpeed += increment_speed;
    err_last =  err_next;                       //记录误差值
    err_next =  err;

    //得到输出值,这里进行对应的操作
}

void Track_PID_Following(unsigned char speed)
{

}

**/

/***************************************************************************//**
 * @}
 ******************************************************************************/

/***************************************************************************//**
 * @brief
 * @param
 * @return none
 *******************************************************************************/
