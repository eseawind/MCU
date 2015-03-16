/**********************************************
 *
 * 		Author	:		Shawn Guo
 *      Mail    :       iguoxiaopeng@gmail.com
 * 		Date	:       2013/8/19
 *      Last    :       2013/8/19       国赛真题模拟第一弹,完成第一阶段
                        2013/8/21       开始第二阶段编程工作
 * 		Notes	:       简易智能小车
 * 		Tool    :       MSP-EXP430F2618 / 智能小车
 **********************************************/

#include <msp430f2618.h>
#include "clock.h"
#include "LCD5110.h"
#include "Timer.h"
#include "drive.h"
#include "Board.h"
#include "track.h"

//当码盘计数达到ZHIDAO_MAPAN,表示小车绝对已脱离直道,因此需要将下一次的金属探测设定为停止模式.
#define     ZHIDAO_MAPAN        (200)
#define     BEEP_LED_1S         (BOARD_LED3_1, BOARD_BEEP_1, DELAY_MS(500), BOARD_LED3_0, BOARD_BEEP_0, DELAY_MS(500))

/******码盘模块全局变量**************/
// 码盘模块:用于路程记录.
extern int SPEED_COUNT_L, SPEED_COUNT_R;
//统计码盘总数
long int SPEED_SUM_L = 0, SPEED_SUM_R = 0;

/******金属探测全局变量**************/
// 金属探测模块: 探测金属, 记录码盘信息并发生声光提示.
// 0:未探测到金属                 1:探测到第一块金属
// 2:已探测1块,目前未探测到       3:探测到第二块金属
// 4:已探测2块,目前未探测到       5:探测到第三块金属(直道金属检测完毕)
// 6:已探测3块,目前未探测到       7:探测到第四块金属(停车地点C处)
extern unsigned char JSTC_FLAG; //金属探测标志位
unsigned long JSTC_SITE = 0; //统计直道金属的位置(金属与起跑线的距离)

/********LCD5110模块全局变量************/
//预设的LCD显示内容, 右边从5*6开始可以写入其他内容
char LCD5110_MAIN_BUFFER0[] = "M 1 :";
char LCD5110_MAIN_BUFFER1[] = "M 2 :";
char LCD5110_MAIN_BUFFER2[] = "M 3 :";
char LCD5110_MAIN_BUFFER3[] = "Time:";

//MAIN_FLAG 0   第一阶段
//          2   第二阶段
unsigned char MAIN_FLAG = 0;        //第二阶段标志,到达第二阶段置2.

int speed = 60;

int main()
{
    WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT

    Clock_Init(0,0,0,CPU_FF);

    /******第一阶段*******
    以循迹为主
    开启LCD模块显示金属位置
    开启金属探测模块检测金属
    开启小车驱动模块
    ***********************/
    LCD5110_Init();
    LCD5110_Write_String(0,0,LCD5110_MAIN_BUFFER0);
    LCD5110_Write_String(0,1,LCD5110_MAIN_BUFFER1);
    LCD5110_Write_String(0,2,LCD5110_MAIN_BUFFER2);
    LCD5110_Write_String(0,3,LCD5110_MAIN_BUFFER3);

    Drive_IO_Init();
    Drive_PWM_Init();       //电机启动
    Track_IO_Init();        //循迹IO启动
    Drive_Speed_IO();       //码盘模块启动

    JSTC_IO_DIR;   //金属探测IO
    JSTC_ENABLE;
    BOARD_BEEP_IO;  //蜂鸣器
    BOARD_LED3_IO;  //LED灯

    Drive_Motor_L(0, speed);
    Drive_Motor_R(0, speed);

    _EINT();

    while(1)        //第一阶段while主循环
    {
        if(JSTC_FLAG % 2)   //金属探测处理程序
        {
            JSTC_SITE = SPEED_SUM_L + SPEED_COUNT_L;    //码盘总数

            if(JSTC_FLAG < 7 && JSTC_SITE < ZHIDAO_MAPAN)   //直道金属探测处理
            {
                BOARD_LED3_1;
                BOARD_BEEP_1;

                //声光响应的同时,记录金属码盘数据,直道以左电机码盘为准, 并显示在LCD上.
                JSTC_SITE = JSTC_SITE * SPEED_GRITH / 10;   //折算距离
                LCD5110_Long2Char(JSTC_SITE);
                LCD5110_Write_String(30,(JSTC_FLAG >> 1), 0);
                DELAY_MS(100);

                BOARD_LED3_0;
                BOARD_BEEP_0;
                JSTC_FLAG++;
            }
            else    //弯道金属探测处理
            {
                break; // 跳出第一阶段
            }
        }

        Track_Following2(); //循迹处理
    }

    /**********************结束第一阶段******************
    *   关闭电机, 关闭金属探测, 退出循迹, 停止码盘中断
    *   定时5s停止(断续光电信息)
    *****************************************************/
    MOTOR_RIGHT_STOP;
    MOTOR_LEFT_STOP;
    JSTC_DISABLE;
    SPEED_DISABLE;

    //粗略延时5s即可
    BEEP_LED_1S;
    BEEP_LED_1S;
    BEEP_LED_1S;
    BEEP_LED_1S;
    BEEP_LED_1S;

    /******第二阶段*******
    以避障,寻光,避黑线为主
    开启超声波测距功能
    开启寻光功能
    开启避免黑线功能
    ***********************/
    //while(1);

    LIGHT_IO_Init();
    TimerB_ConMode_Init(2, 4); // 3M的持续计数模式.
    Drive_UTL_Init();
    MAIN_FLAG = 2;  //第二阶段

    while(1)    //第二阶段.
    {
        if(UTL_FLAG == 2)   //超声波测距完成
        {
            Drive_UTL_DISTANCE();
            UTL_START_TIME = 0;
            if(UTL_BUFFER < UTL_SAFE)   //有障碍物
            {
                //有障碍物就后退+小左
                TRACK_BACK;
                TRACK_LEFT;
            }
            else
            {
                //没有障碍物就开始寻光
                TRACK_GO;
            }

            UTL_FLAG = 0;
        }
    }



}

#pragma vector=PORT2_VECTOR
__interrupt void P2_ISR (void)  //码盘中断计数,金属探测, 只在第一阶段使用.
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
        JSTC_FLAG++;
    }

    // ....something else.
    P2IFG = 0;
}

#pragma vector=TIMERA0_VECTOR
__interrupt void TimerA0(void)  // A0中断频率1000HZ, 1ms中断一次.
{
    static long TIMER_I = 0;

    ++TIMER_I;
    if(TIMER_I >= 90000)  // 90S紧急制动
    {
        //紧急制动程序
        MOTOR_RIGHT_STOP;
        MOTOR_LEFT_STOP;
        LCD5110_Write_String(0,5, "90s !!!");
        while(1);
    }
    if(MAIN_FLAG)   //第二阶段才不断进行超声波测距.
    {
        if((TIMER_I % 500) == 0)
        {
            if(UTL_FLAG == 0)    //超声波空闲
            {
                UTL_START;
            }
        }
    }
    else //第一阶段才统计码盘数目.
    {
        if((TIMER_I % 10000) == 0)    //10s处理一次
        {
            //90s用2字节int类型进行码盘记录一定会溢出, 换成4字节long每次自加都需两个指令.
            //故这里每10s叠加一次解决溢出问题.        处理时间间隔可实际调整
            SPEED_SUM_L += SPEED_COUNT_L;
            SPEED_SUM_R += SPEED_COUNT_R;
            SPEED_COUNT_L = 0;
            SPEED_COUNT_R = 0;

        }
    }

}


#pragma vector=TIMERB1_VECTOR           //舵机TimerB1中断, 20ms中断一次.
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
