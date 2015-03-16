/**********************************************
 *
 * 		Author	:		Shawn Guo
 * 		Date	:		2013/5/2
 *      Last    :       2013/5/2
 * 		Notes	:       Programmable Gain Amplifier
 * 		Tool    :	    MSP430G2553, LF353, CD4051
 *
 *************************************************
 *                  CD4051
 *              -----------------
 *        CH4   |1            16|   VDD
 *        CH6   |2            15|   CH2
 *    COM I/O   |3            14|   CH1
 *        CH7   |4            13|   CH0
 *        CH5   |5            12|   CH3
 *        INH   |6            11|   A   ---> P1.3
 *        VEE   |7            10|   B   ---> P1.4
 *        VSS   |8             9|   C   ---> P1.5
 *              -----------------
 *
 *
 *          KEY_A -----> P1.0
 *          KEY_B -----> P1.6
 *          KEY_C -----> P1.7
 *
 **********************************************/
#include <msp430g2553.h>
#include "PGA.h"
#include "LCD5110.h"

void Config_Clocks(); //配置时钟
void Config_Port(); //设置IO中断端口
char LCD_BUFFER[] = "Channel: CH0";

int CHANNEL = 0; // CD4051通道

int main()
{
    WDTCTL = WDTPW + WDTHOLD; // Stop watchdog timer

    Config_Clocks(); //配置时钟
    Config_Port();

    PGA_IO_Init();// 默认通道CH0


    LCD_IO_set();

    LCD_Init(); //初始化液晶
    LCD_Clear();
    LCD_Write_String(0,0,LCD_BUFFER);
    _EINT();

    while(1);
}


void Config_Clocks() //配置时钟
{
    BCSCTL1 = CALBC1_8MHZ; // Set range
    DCOCTL = CALDCO_8MHZ; // Set DCO step + modulation
    BCSCTL3 |= LFXT1S_0;
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

void Config_Port() //设置IO中断端口
{
    P1DIR &= ~BIT0; // 设为输入
    P1IES &= ~BIT0; //  中断触发沿：上升沿；
    P1IE |= BIT0; //    开启 I/O中断
    P1REN |= BIT0;  // 上下拉电阻使能
    P1OUT &= ~BIT0; //下拉电阻

    P1DIR &= ~BIT6; // 设为输入
    P1IES &= ~BIT6; //  中断触发沿：上升沿；
    P1IE |= BIT6; //    开启 I/O中断
    P1REN |= BIT6;  // 上下拉电阻使能
    P1OUT &= ~BIT6; //下拉电阻


    P1DIR &= ~BIT7; // 设为输入
    P1IES &= ~BIT7; //  中断触发沿：上升沿；
    P1IE |= BIT7; //    开启 I/O中断
    P1REN |= BIT7;  // 上下拉电阻使能
    P1OUT &= ~BIT7; //下拉电阻



}


//  I/O中断程序，键盘控制响应事件的发生
#pragma vector=PORT1_VECTOR
__interrupt void P1_ISR (void) // 按键控制PGA的通道切换
{
    if(P1IFG & BIT0) // P1.0中断，调节PGA的端口A
    {
        if(CD4051_A)
        {
            CD4051_A0;
            CHANNEl--;
        }
        else
        {
            CD4051_A1;
            CHANNEL++;
        }
    }
    else if(P1IFG & BIT6)
    {
        if(CD4051_B)
        {
            CD4051_B0;
            CHANNEL -= 2;
        }
        else
        {
            CD4051_B1;
            CHANNEL += 2;
        }

    }
    else if(P1IFG & BIT7)
    {
        if(CD4051_C)
        {
            CD4051_C0;
            CHANNEL -= 4;
        }
        else
        {
            CD4051_C1;
            CHANNEL += 4;
        }

    }
    LCD_Set_XY(66, 0);
    LCD_Write_Char((char)(CHANNEL + '0'));
    P1IFG = 0;         // P1端口的中断标志位清零，等待下一次中断
}
