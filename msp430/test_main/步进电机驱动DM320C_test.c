/**********************************************
 *
 * 		Author	:		Shawn Guo
 *      Mail    :       iguoxiaopeng@gmail.com
 * 		Date	:       2013/8/27
 *      Last    :       2013/8/27
 * 		Notes	:       两相四线步进电机驱动 DM320C test
 * 		Tool    :
 **********************************************/



#include <msp430f2618.h>
#include "clock.h"
#include "DM320C.h"

int i = 0;

void main()
{
    WDTCTL = WDTPW + WDTHOLD;
    Clock_Init(0,0,0,CPU_FF); // DCO clock

    DM320C_Init_IO();
    while(1)
    {
        DM320C_DIR_0;
        for(i = 0; i < DM320C_PLUSE_NUM; i++)
        {
            DM320C_PUL;
            DM320C_DELAY;
        }
        DELAY_MS(1000);

        DM320C_DIR_1;
        for(i = 0; i < DM320C_PLUSE_NUM; i++)
        {
            DM320C_PUL;
            DM320C_DELAY;
        }
        DELAY_MS(1000);
    }
}
