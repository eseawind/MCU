/**********************************************
 *
 * 		Author	:		Shawn Guo
 *      Mail    :       iguoxiaopeng@gmail.com
 * 		Date	:		2013/5/4
 *      Last    :       2013/5/4
 * 		Notes	:       PWM方波输出
 * 		Tool    :	    MSP430G2553
 *
 *      P1.2 ---> TA0.0
 *      Timer_A (DCO CLK)
 *
 *      P2.2 KEY_A 控制频率，100us步进
 *      P2.3 KEY_D 控制占空比，1%步进
 **********************************************/
#include <msp430g2553.h>
#include "PWM.h"



#define CALBC1_M CALBC1_1MHZ
#define CALDCO_M CALDCO_1MHZ


// ===============================精确延时宏定义=========================
#define CPU_F ((double)1000000)        // 这里的CPU_F需要自己配置，这里配置为1MHZ
#define DELAY_US(x) __delay_cycles((long)(CPU_F*(double)x/1000000.0))
#define DELAY_MS(x) __delay_cycles((long)(CPU_F*(double)x/1000.0))


extern double PWM_F; // 频率默认100HZ
extern double PWM_DC; // 占空比默认50%
extern long int PWM_CLK; // 1M DCO CLK

void Config_Clocks(); //配置时钟
void Config_Timer0(); //设置定时器
void Config_Port();   // 中断端口配置

void main(void)
{
    WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
    Config_Clocks();
    Config_Timer0();
    Config_Port();

    PWM_Init();            //PWM配置
    PWM_CLK = 1000000;
    PWM_Set(PWM_F, PWM_DC);

    _EINT();
    while(1);


}

void Config_Clocks() //配置时钟，该时钟将决定 定时器 溢出的时间
{
    BCSCTL1 = (CALBC1_M); // Set range
    DCOCTL = (CALDCO_M); // Set DCO step + modulation
    BCSCTL3 |= LFXT1S_2;  /* Mode 2 for LFXT1 : VLO */

    IFG1 &= ~OFIFG; // Clear OSCFault flag

// BCSCTL2, Basic Clock System Control Register 2

    // SELMx , Select MCLK. These bits select the MCLK source.
    // 00 DCOCLK    01 DCOCLK  11 LFXT1CLK or VLOCLK
    // 10 XT2CLK when XT2 oscillator present on-chip.
    //LFXT1CLK or VLOCLK when XT2 oscillator not presenton-chip.

    //SELS , Select SMCLK. This bit selects the SMCLK source.
    // 0 DCOCLK
    //1 XT2CLK when XT2 oscillator present.
    //LFXT1CLK or VLOCLK when XT2 oscillator not present

    //DIVMx , Divider for MCLK
    // 00 /1	01 /2	10 /4	11 /8
    //DIVSx , Divider for SMCLK
    //00 /1		01 /2	10 /4	11 /8

    //DCOR , DCO resistor select. Not available in all devices. See the device-specific data sheet.
    //0 Internal resistor
    //1 External resistor

    //    MCLK=DC0       不分频
    BCSCTL2 |= SELM_0 + DIVM_0 + DIVS_0; // SMCLK = DCO
}

void Config_Timer0() //设置定时器
{
    //TACTL,Timer_A Control Register
    //TASSELx, Timer_A clock source select
    //00 TACLK    01 ACLK    	10 SMCLK

    //MCx, Mode control.
    //00 Stop mode: the timer is halted.
    //01 Up mode: the timer counts up to TACCR0.
    //10 Continuous mode: the timer counts up to 0FFFFh.
    //11 Up/down mode: the timer counts up to TACCR0 then down to 0000h.

    //TAIE ,Timer_A interrupt enable.
    //0 Interrupt disabled
    //1 Interrupt enabled

    //TACLR ,Timer_A clear.

    //IDx , 7-6 Input divider.
    //00 /1           01 /2       10 /4         11 /8

    //      SMCLK
    TACTL = TASSEL_2 + MC_1 + TACLR + ID_0; //TA基准时钟为SMCLK，且SMCLK与MCLK频率相同
}

void Config_Port()   // 中断端口配置
{
    P2DIR &= ~BIT2; //  P1.0设为输入
    P2IES &= ~BIT2; //  P1.0中断触发沿：上升沿；
    P2IE |= BIT2; //    开启P1.0 I/O中断
    P2REN |= BIT2;
    P2OUT &= ~BIT2;

    P2DIR &= ~BIT3; //  P1.0设为输入
    P2IES &= ~BIT3; //  P1.0中断触发沿：上升沿；
    P2IE |= BIT3; //    开启P1.1 I/O中断
    P2REN |= BIT3;
    P2OUT &= ~BIT3;
}

//  I/O中断程序，键盘控制响应事件的发生
#pragma vector=PORT2_VECTOR
__interrupt void P2_ISR (void)
{
    if(P2IFG & BIT2) //键盘控制频率步进量,100us步进
    {
        PWM_F += 10;
        if(PWM_F > 1000) PWM_F = 100;
        DELAY_MS(300);
    }
    else if(P2IFG & BIT3) //键盘控制占空比，1%步进
    {
        PWM_DC += 0.01;
        if(PWM_DC > 1.0) PWM_DC = 0;
        DELAY_MS(300);
    }
    PWM_Set(PWM_F, PWM_DC);
    P2IFG = 0;         // P1端口的中断标志位清零，等待下一次中断
}

