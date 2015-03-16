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

    Clock_Init(0,0,0,CPU_FF); // 1M DCO
    Drive_IO_Init();
    Drive_PWM_Init();
    //Track_IO_Init();        //开启循迹
    LIGHT_IO_Init();          //开启超声波

    Drive_Motor_L(0, 50);
    Drive_Motor_R(0, 50);

    while(1)
    {
        LIGHT_Following2();
    }
}
