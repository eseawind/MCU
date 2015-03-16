/**********************************************
 *
 * 		Author	:		Shawn Guo
 *      Mail    :       iguoxiaopeng@gmail.com
 * 		Date	:       2013/8/6
 *      Last    :       2013/8/17
 * 		Notes	:       80cm测距没有问题，再远就有8度的误差导致无法测量. 目前只有到超声波避障，这里暂不要求精确度.
 * 		Tool    :
 **********************************************/

#include <msp430f2618.h>
#include "clock.h"
#include "Timer.h"
#include "drive.h"
#include "LCD5110.h"


char LCD_BUFFER1[] = "distance:";

int main()
{
    WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
    Clock_Init(0,0,0,12); // 12M DCO
    TimerB_ConMode_Init(2, 4); // 3M的持续计数模式.

    LCD5110_Init();
    LCD5110_Write_String(0,0,LCD_BUFFER1);

    Drive_UTL_Init();

    _EINT();

    while(1)    //间隔1s 进行一次超声波测距
    {

        ULT_TRIG1;
        DELAY_US(30);
        ULT_TRIG0;

        DELAY_MS(300);

        Drive_UTL_DISTANCE();
        UTL_START_TIME = 0;
        LCD5110_Long2Char(UTL_BUFFER);
        LCD5110_Write_String(0,1,0);
        UTL_FLAG = 0;
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
