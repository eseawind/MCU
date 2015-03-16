/**********************************************
 *
 *      Author  :       Shawn Guo
 *      Mail    :       iguoxiaopeng@gmail.com
 *      Date    :       2013/8/18
 *      Last    :
 *      Notes   :       拓展版模块功能
 *      Tool    :       MSP-EXP430F2618拓展板
 **********************************************/



#include <msp430f2618.h>
#include "clock.h"
#include "Board.h"


void main()
{
    WDTCTL = WDTPW + WDTHOLD;
    Clock_Init(0,0,0,CPU_FF);

    BOARD_LED1_IO;
    BOARD_LED2_IO;
    BOARD_LED3_IO;

    BOARD_BEEP_IO;

    while(1)
    {
        BOARD_LED1_1;
        DELAY_MS(1000);
        BOARD_LED1_0;

        BOARD_LED2_1;
        DELAY_MS(1000);
        BOARD_LED2_0;

        BOARD_LED3_1;
        DELAY_MS(1000);
        BOARD_LED3_0;

        BOARD_BEEP;
        DELAY_MS(1000);
    }
}
