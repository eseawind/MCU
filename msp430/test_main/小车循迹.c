/**********************************************
 *
 * 		Author	:		Shawn Guo
 *      Mail    :       iguoxiaopeng@gmail.com
 * 		Date	:
 *      Last    :
 * 		Notes	:
 * 		Tool    :
 **********************************************/

#include <msp430f2618.h>
#include "clock.h"
#include "drive.h"
#include "track.h"

int main()
{
    WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT

    Clock_Init(0,0,0,CPU_FF); // DCO
    Drive_IO_Init();
    Drive_PWM_Init();

    Track_IO_Init();
    while(1)
        Track_Following();
}
