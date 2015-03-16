/**********************************************
 *
 * 		Author	:		Shawn Guo
 *      Mail    :       iguoxiaopeng@gmail.com
 * 		Date	:       2013/8/14
 *      Last    :
 * 		Notes	:
 * 		Tool    :       MSP430X2XX  smart car 测速test
 **********************************************/

#include <msp430f2618.h>
#include "clock.h"
#include "Timer.h"
#include "drive.h"
#include "LCD5110.h"

const char LCD_BUFFER1[] = "Speed:";
extern char LCD_5110_BUFFER[] = "00000000\0"; // buffer 8-bit
int SPEED_L = 0, SPEED_R = 0, SPEED_DISPLAY_FLAG = 0;

int main()
{
    WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT

    Clock_Init(0,0,0,1); // 1M DCO
    Drive_Init(0, 100);

    LCD5110_Init();
    LCD5110_Write_String(0,0,LCD_BUFFER1);
    _EINT();
    while(1)
    {
        if(SPEED_DISPLAY_FLAG == 1)
        {
            SPEED_DISPLAY_FLAG = 0;

            SPEED_L = Drive_Speed_L();
            SPEED_R = Drive_Speed_R();
            LCD5110_Long2char(SPEED_L / 5);
            LCD5110_Write_String(0,1,LCD5110_BUFFER);
            LCD5110_Long2char(SPEED_R / 5);
            LCD5110_Write_String(0,2,LCD5110_BUFFER);
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

    // ....something else.
}

#pragma vector=TIMERA0_VECTOR
__interrupt void TimerA0(void)  //5s中断显示,显示速度.
{
    static int i = 0;
    if(++i >= 5000) //5s
    {
        SPEED_DISPLAY_FLAG = 1; //显示SPEED
    }
}
