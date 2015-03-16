/**********************************************
 *
 * 		Author	:		Shawn Guo
 *      Mail    :       iguoxiaopeng@gmail.com
 * 		Date	:       2013/8/27
 *      Last    :       2013/8/27
 * 		Notes	:       四旋翼-无刷直流电机测试
 * 		Tool    :       MSP430F2618/好盈20A PWM电调/朗宇2212 KV980电机
 **********************************************/


#include <msp430f2618.h>
#include "clock.h"
#include "Timer.h"

int main()
{
    WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
    Clock_Init(0,0,0,CPU_FF); // 1M DCO

    P1DIR |= BIT2;
    P1SEL |= BIT2;

    //1M时钟, R0 = 20000,50HZ,
    TimerA_UpMode_Init(2, 1, 20000, 0, 0);
    TimerA_COM_Init(20000, 1000, 7, 0, 0);    //1ms对应油门0%, 2ms对应油门100%
    DELAY_MS(3000);
    while(1)
    {
        if(TACCR1 < 2000)
        {
          TACCR1 += 100;
        }
        else
        {
          TACCR1 = 1000;
        }
        DELAY_MS(1000);
    }

}
