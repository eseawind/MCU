/**********************************************
 *
 * 		Author	:		Shawn Guo
 *      Mail    :       iguoxiaopeng@gmail.com
 * 		Date	:       2013/8/20
 *      Last    :
 * 		Notes	:       智能小车の耗电模式, 用光电池的剩余电量
 * 		Tool    :
 **********************************************/

#include <msp430f2618.h>
#include "clock.h"
#include "drive.h"

int main()
{
    WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT

    Clock_Init(0,0,0,CPU_FF);

    Drive_IO_Init();
    Drive_PWM_Init();       //电机启动

    Drive_Motor_L(0, 100);
    Drive_Motor_R(0, 100);

    while(1)
    {
    }

}
