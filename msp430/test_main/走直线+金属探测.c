/**********************************************
 *
 * 		Author	:		Shawn Guo
 *      Mail    :       iguoxiaopeng@gmail.com
 * 		Date	:       2013/8/18
 *      Last    :       2013/8/18
 * 		Notes	:       走直线, 没有绝对的校准只靠记录码盘数量是走不出绝对的直线, 只能走出近似的直线.
 *                      绝对的直线必须加入校正功能. 此时应该考虑陀螺仪/电子指南/磁传感器进行绝对方向定位.
 * 		Tool    :       MSP-EXP430F2618 / 智能小车  / 测速模块
 **********************************************/

#include <msp430f2618.h>
#include "clock.h"
#include "LCD5110.h"
#include "Timer.h"
#include "drive.h"
#include "Board.h"


unsigned char LCD_BUFFER1[] = "Speed:";

extern int SPEED_COUNT_L, SPEED_COUNT_R, SPEED_BUF_L, SPEED_BUF_R;
extern unsigned char SPEED_DISPLAY_FLAG;
int SPEED_L = 0, SPEED_R = 0;
long int SPEED_SUM_L = 0, SPEED_SUM_R = 0;
int speed = 50;


extern unsigned char JSTC_FLAG;

int main()
{
    WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT

    Clock_Init(0,0,0,CPU_FF);

    LCD5110_Init();
    LCD5110_Write_String(0, 0, (char*)LCD_BUFFER1);

    //Drive_Init(0,50);
    Drive_IO_Init();
    Drive_PWM_Init();
    Drive_Servo_Init();
    Drive_Speed_IO();

    JSTC_IO_DIR;   //金属探测IO
    JSTC_ENABLE;
    BOARD_BEEP_IO;  //蜂鸣器
    BOARD_LED3_IO;  //LED灯


    Drive_Motor_L(0, speed);
    Drive_Motor_R(0, speed);

    _EINT();

    while(1)
    {
        if(SPEED_DISPLAY_FLAG == 1)
        {
            SPEED_DISPLAY_FLAG = 0;
            SPEED_SUM_L += SPEED_BUF_L;
            SPEED_SUM_R += SPEED_BUF_R;
            //SPEED_L = Drive_Speed_Journey_L();
            //SPEED_R = Drive_Speed_Journey_R();
            LCD5110_Long2Char(SPEED_SUM_L);
            LCD5110_Write_String(36, 0, 0);
            LCD5110_Long2Char(SPEED_SUM_R);
            LCD5110_Write_String(36, 1, 0);

            if(SPEED_SUM_L > SPEED_SUM_R)   Drive_Morto_Speed_RR();
            else if(SPEED_SUM_L < SPEED_SUM_R)   Drive_Morto_Speed_R_();
            else
                ;
        }
        if(JSTC_FLAG)
        {
            BOARD_LED3_1;
            BOARD_BEEP;
            BOARD_LED3_0;
            JSTC_FLAG = 0;
        }
    }
}

#pragma vector=PORT2_VECTOR
__interrupt void P2_ISR (void)
{
    if(P2IFG & SPEED_BITL)
    {
        SPEED_COUNT_L++;
    }
    if(P2IFG & SPEED_BITR)
    {
        SPEED_COUNT_R++;
    }
    if(JSTC_IFG)        //金属探测中断处理
    {
        JSTC_FLAG = 1;
    }

    // ....something else.
    P2IFG = 0;
}

#pragma vector=TIMERA0_VECTOR
__interrupt void TimerA0(void)  //5s中断显示,显示速度.
{
    static int i = 0;

    if(++i >= 500)
    {
        i = 0;
        SPEED_DISPLAY_FLAG = 1; //显示SPEED
        SPEED_BUF_L = SPEED_COUNT_L;
        SPEED_BUF_R = SPEED_COUNT_R;
        SPEED_COUNT_L = 0;
        SPEED_COUNT_R = 0;
    }
}


#pragma vector=TIMERB1_VECTOR           //舵机TimerB1中断, 20ms中断一次.
__interrupt void TimerB1(void)
{
    switch(TBIV)                //  TB中断源软件判断,R0有自己单独的中断向量
    {
    case 2:          // 中断源为R1, 超声波测距中断端口

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
        break;
    }

}
