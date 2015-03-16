/**********************************************
 *
 * 		Author	:		Shawn Guo
 * 		Date	:		2013/4/16
 *      Last    :       2013/4/16
 * 		Notes	:       LED流水灯
 * 		Tool    :	    MSP430G2553
 *             P1----> LED
 *
 **********************************************/
#include <msp430g2553.h>

// ===============================精确延时宏定义=========================
#define CPU_F ((double)1000000)        // 这里的CPU_F需要自己配置，这里配置为1MHZ
#define DELAY_US(x) __delay_cycles((long)(CPU_F*(double)x/1000000.0))
#define DELAY_MS(x) __delay_cycles((long)(CPU_F*(double)x/1000.0))

void main()
{
    WDTCTL = WDTPW + WDTHOLD; // Stop watchdog timer

    P1DIR |= 0XFF;     // 设P1端全部为输出
    P1OUT = 0XFE;       // P1端口输出 1111_1110
    while(1)
    {
        DELAY_MS(1000);
        P1OUT = (P1OUT << 1) + 1;
        if(P1OUT == 0xff) P1OUT = 0XFE;
    }
}

