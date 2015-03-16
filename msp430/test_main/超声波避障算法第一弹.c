/**********************************************
 *
 * 		Author	:		Shawn Guo
 *      Mail    :       iguoxiaopeng@gmail.com
 * 		Date	:       2013/8/6
 *      Last    :       2013/8/18
 * 		Notes	:       80cm测距没有问题，再远就有8度的误差导致无法测量. 目前只有到超声波避障，这里暂不要求精确度.
 * 		Tool    :
 **********************************************/

#include <msp430f2618.h>
#include "clock.h"
#include "Timer.h"
#include "drive.h"
#include "track.h"
#include "LCD5110.h"


const char LCD_BUFFER1[] = "distance:";
const char LCD_BUFFER2[] = "Speed:";

extern int SPEED_COUNT_L, SPEED_COUNT_R, SPEED_BUF_L, SPEED_BUF_R;
int SPEED_L = 0, SPEED_R = 0;

int UTL_ANGLE = 0;
unsigned char UTL_DIR = 3;      // 0,1,2,3,4,5,6  七个方向测距, 初始默认居中

unsigned char speed = 60;       //小车速度.
int flag = 0;

int main()
{
    WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
    Clock_Init(0,0,0,CPU_FF); // 12M DCO

    LCD5110_Init();             // LCD模块
    LCD5110_Write_String(0,0,(char*)LCD_BUFFER1);
    LCD5110_Write_String(0,2,(char*)LCD_BUFFER2);

    //电机驱动pwm占用TimerA up mode, 1ms中断,
    //码盘测速占用P2.4 P2.5端口中断, TimerA 1ms中断
    //舵机占用P4.5(TB5) 比较模式, up mode 20ms中断.
    //超声波占用端口P7.4。 占用P4.1 捕获模式, 利用TimerA计时.
    Drive_Init(2, speed);               //仅配置速度，不前进
    Drive_Servo_Move(SERVO_MEDIUM);     //舵机归中

    _EINT();
    while(1)                //超声波避障开始
    {
        if(UTL_FLAG == 2)   //超声波测距距离显示
        {
            Drive_UTL_DISTANCE();
            UTL_START_TIME = 0;
            //舵机根据现在的方向UTL_DIR确定下一步操作.
            switch(UTL_DIR)
            {
            case 3:     //前进中方向
                if(UTL_BUFFER < UTL_SAFE) //前进中遇到障碍，停止小车, 向左前转.
                {
                    MOTOR_LEFT_STOP;
                    MOTOR_RIGHT_STOP;
                    UTL_DIR = 2;
                    Drive_Servo_Turn(90, 120);
                    flag = 0;
                }
                else
                {
                    if(flag == 0)   // 前方无障碍，左前测试一次.
                    {
                        Drive_Servo_Turn(90, 135);
                        flag = 1;
                    }
                    else if(flag == 1)  //左前无障碍, 重新确定前方.
                    {
                        Drive_Servo_Move(SERVO_MEDIUM);     //舵机归中
                        flag = 2;
                    }
                    else if(flag == 2)  //右前
                    {
                        Drive_Servo_Turn(90, 45);
                        flag = 3;
                    }
                    else if(flag == 3)
                    {
                        Drive_Servo_Move(SERVO_MEDIUM);     //舵机归中
                        flag = 0;
                    }
                }
                break;
            case 2: //左前方向
                if(UTL_BUFFER < UTL_SAFE) //左前方向遇到障碍, 向小左方向转.
                {
                    Drive_Servo_Turn(120, 150);
                    UTL_DIR = 1;
                }
                else    //左前无障碍，则小车转30度前进.
                {
                    Drive_Motor_L(1, speed);
                    Drive_Motor_R(0, speed);
                    DELAY_MS(220);
                    Drive_Motor_L(0, speed);
                    Drive_Motor_R(0, speed);
                    Drive_Servo_Move(SERVO_MEDIUM);     //舵机归中
                    UTL_DIR = 3;
                }
                break;
            case 1:
                if(UTL_BUFFER < UTL_SAFE) //小左方向遇到障碍, 向大左方向转.
                {
                    Drive_Servo_Turn(150, 180);
                    UTL_DIR = 0;
                }
                else    //小左无障碍，则小车转60度前进.
                {
                    Drive_Motor_L(1, speed);
                    Drive_Motor_R(0, speed);
                    DELAY_MS(440);
                    Drive_Motor_L(0, speed);
                    Drive_Motor_R(0, speed);
                    Drive_Servo_Move(SERVO_MEDIUM);     //舵机归中
                    UTL_DIR = 3;
                }
                break;
            case 0: //大左方向
                if(UTL_BUFFER < UTL_SAFE) // 左边都有障碍，右前测试
                {
                    Drive_Servo_Turn(180, 60);
                    UTL_DIR = 4;
                }
                else    //大左方向无障碍，则小车左转90度前进.
                {
                    Drive_Motor_L(1, speed);
                    Drive_Motor_R(0, speed);
                    DELAY_MS(660);
                    Drive_Motor_L(0, speed);
                    Drive_Motor_R(0, speed);

                    Drive_Servo_Move(SERVO_MEDIUM);     //舵机归中
                    UTL_DIR = 3;
                }
                break;
            case 4:
                if(UTL_BUFFER < UTL_SAFE) //右前有障碍
                {
                    Drive_Servo_Turn(60, 30);
                    UTL_DIR = 5;
                }
                else    //右前无障碍，右转30度
                {
                    Drive_Motor_L(0, speed);
                    Drive_Motor_R(1, speed);
                    DELAY_MS(220);
                    Drive_Motor_L(0, speed);
                    Drive_Motor_R(0, speed);

                    Drive_Servo_Move(SERVO_MEDIUM);     //舵机归中
                    UTL_DIR = 3;
                }
                break;
            case 5:
                if(UTL_BUFFER < UTL_SAFE)   //小右有障碍
                {
                    Drive_Servo_Turn(30, 0);
                    UTL_DIR = 6;
                }
                else    //右转60度前进
                {
                    Drive_Motor_L(0, speed);
                    Drive_Motor_R(1, speed);
                    DELAY_MS(440);
                    Drive_Motor_L(0, speed);
                    Drive_Motor_R(0, speed);

                    Drive_Servo_Move(SERVO_MEDIUM);     //舵机归中
                    UTL_DIR = 3;
                }
                break;
            case 6:
                if(UTL_BUFFER < UTL_SAFE) //有障碍, 先后退, 等周围无障碍在原路转180度返回
                {
                    Drive_Motor_L(1, speed);
                    Drive_Motor_R(1, speed);
                    DELAY_MS(300);
                    UTL_DIR = 2;
                }
                else
                {
                    Drive_Motor_L(0, speed);
                    Drive_Motor_R(1, speed);
                    DELAY_MS(700);
                    Drive_Motor_L(0, speed);
                    Drive_Motor_R(0, speed);

                    Drive_Servo_Move(SERVO_MEDIUM);     //舵机归中
                    UTL_DIR = 3;
                }
                break;
            }

            UTL_FLAG = 0;
        }

        if(SPEED_DISPLAY_FLAG == 1) //码盘测速显示
        {

            SPEED_DISPLAY_FLAG = 0;

            SPEED_L = Drive_Speed_Journey_L();
            SPEED_R = Drive_Speed_Journey_R();
            LCD5110_Long2Char(SPEED_L);
            LCD5110_Write_String(0, 3, 0);
            LCD5110_Long2Char(SPEED_R);
            LCD5110_Write_String(0, 4, 0);
        }
    }
}

#pragma vector=TIMERB1_VECTOR
__interrupt void TimerB1(void)
{
    switch(TBIV)                //  TB中断源软件判断,R0有自己单独的中断向量
    {
    case 2:          // 中断源为R1      //捕获上升下降沿.
        if(UTL_FLAG == 0)     //开启超声波计时
        {
            UTL_FLAG = 1;
            UTL_START_TIME = TBR;
            UTL_TIME_OVERFLOW = 0;
        }
        else if(UTL_FLAG == 1)    // 关闭计时，等待处理数据
        {
            UTL_FLAG = 2;
            UTL_END_TIME = TBR;
        }
        else
            ;
        break;
    case 4:          // 中断源为R2
        break;
    case 6:             // R3
        break;
    case 8:             // R4
        break;
    case 10:             // R5
        break;
    case 12:             // R6
        break;
    case 14:         // 中断源为Timer_B
        if(UTL_FLAG == 1)  //如果开始超声波计时，则记录此后的中断溢出次数
            UTL_TIME_OVERFLOW++;
        break;
    }
}


#pragma vector=TIMERA0_VECTOR
__interrupt void TimerA0(void)      // 1ms中断.1000HZ
{
    static unsigned int i = 0;
    ++i;
    if(i % 300 == 0)  //1s进行测速一次, 测距一次
    {
        /* SPEED_DISPLAY_FLAG = 1; //显示SPEED
        SPEED_BUF_L = SPEED_COUNT_L;
        SPEED_BUF_R = SPEED_COUNT_R;
        SPEED_COUNT_L = 0;
        SPEED_COUNT_R = 0;
        */
        if(UTL_FLAG == 0)   //若超声波空闲，则测距.
            UTL_START;
    }
    else if(i >= 10000)
    {
        i = 0;
    }
    else
        ;
}

#pragma vector=PORT2_VECTOR
__interrupt void P2_ISR (void)
{
    /*
    if(P2IFG & BIT4)
    {
        SPEED_COUNT_L++;
    }
    if(P2IFG & BIT5)
    {
        SPEED_COUNT_R++;
    }
    // ....something else.
    */
    P2IFG = 0;

}
