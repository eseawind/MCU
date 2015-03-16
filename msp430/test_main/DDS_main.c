/**********************************************
 *
 * 		Author	:		Shawn Guo
 *      Mail    :       iguoxiaopeng@gmail.com
 * 		Date	:       2013/8/4
 *      Last    :
 * 		Notes	:       DDS示例程序
 * 		Tool    :       MSP430FX2XX DAC12   TimerA
 **********************************************/

#include <msp430f2618.h>
#include "clock.h"
#include "Timer.h"
#include "DAC12.h"
#include "dds_data.h"

int wave_i = 0;
const int WAVE_N = 4096;
int STEP = 320;    // DDS步进量
const int TIME_NUM = 155;  //定时器时间，即多久给DAC12发送一个数据. 
/*********DDS
TIMERA_CLOCK    STEP  TIME_NUM    WAVE_F
    16M         4     155         100HZ
    16M         8     155         200HZ
    16M         16    155        400HZ
    16M         80    155         1970HZ

    8M          4     155         50HZ 
    8M          16    155        200HZ 
    8M          80    155         990HZ

    1M          32     155        50HZ
    1M          160    155        247HZ
    1M          320    155        493HZ

*************/


int main()
{
    WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT

    Clock_Init(0,0,0,1); // 1M DCO
    
    ADC12CTL0 = REFON;
    
    //DAC12_Init
    DAC12_Calon(0);
    DAC12_SetIR(0, 1);
    DAC12_SetAMP(0, 7);
    
    //Timer_A
    TimerA_R_Init(2, 1, 1, TIME_NUM, 0, 0); 

    _EINT();
    while(1);
    //__bis_SR_register(LPM0_bits + GIE);       // Enter LPM0
}


#pragma vector=TIMERA0_VECTOR
__interrupt void TimerA0(void)
{
    DAC12_0DAT = WAVE_ROM[wave_i];
    wave_i += STEP;
    if(wave_i >= WAVE_N)
    {
        wave_i = 0;
    }
}
