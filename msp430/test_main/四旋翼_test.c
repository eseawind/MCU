/**********************************************
 *
 * 		Author	:		Shawn Guo
 *      Mail    :       iguoxiaopeng@gmail.com
 * 		Date	:       2013/8/27
 *      Last    :       2013/8/27
 * 		Notes	:       四旋翼- kk飞控板测试
 * 		Tool    :       MSP430F2618/好盈20A PWM电调/朗宇2212 KV980电机
 *
 *
 *
 *
 *
 **********************************************/


#include <msp430f2618.h>
#include "clock.h"
#include "Plane_Drive.h"

int main()
{
    WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
    Clock_Init(0,0,0,CPU_FF); // 1M DCO

    Plane_Drive_Init_IO();

    while(1)
    {
        DELAY_MS(3000);
        Plane_Drive_Unlock();
        Plane_Drive_Accele();
        Plane_Drive_Lock();
    }

}
