/**********************************************
 *
 *              Author	:		Shawn Guo
 *              Mail    :       iguoxiaopeng@gmail.com
 *              Date	:       2013/8/18
 *              Last    :       2013/8/18
 *              Notes	:
 *              Tool    :
 **********************************************/

#include <msp430f2618.h>
#include "clock.h"
#include "LCD5110.h"
#include "Timer.h"
#include "drive.h"
#include "Board.h"

extern unsigned char JSTC_FLAG;


int main()
{
    WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT

    Clock_Init(0,0,0,CPU_FF);

    JSTC_IO_DIR;   //金属探测IO
    JSTC_ENABLE;
    BOARD_BEEP_IO;  //蜂鸣器
    BOARD_LED3_IO;  //LED灯



    _EINT();

    while(1)
    {
        if(JSTC_FLAG)
        {
            BOARD_LED3_1;
            BOARD_BEEP;
            DELAY_MS(100);
            BOARD_LED3_0;
            JSTC_FLAG = 0;
        }
    }


}

#pragma vector=PORT2_VECTOR
__interrupt void P2_ISR (void)
{

    if(JSTC_IFG)        //金属探测中断处理
    {
        JSTC_FLAG = 1;
    }
    // ....something else.
    P2IFG = 0;
}
