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
 **********************************************/
#include <msp430g2553.h>
#include "PGA.h"

void PGA_IO_Init() // 初始化IO口
{
    P1DIR |= BIT3 + BIT4 + BIT5;       //设置为输出方向
    P1OUT &= ~(BIT3 + BIT4 + BIT5);     //输出为0
}

void PGA_Channel(int ch)  // 切换通道
{
    switch(ch)
    {
    case 0:
        CD4051_A0;
        CD4051_B0;
        CD4051_C0;
        break;
    case 1:
        CD4051_A1;
        CD4051_B0;
        CD4051_C0;
        break;
    case 2:
        CD4051_A0;
        CD4051_B1;
        CD4051_C0;
        break;
    case 3:
        CD4051_A1;
        CD4051_B1;
        CD4051_C0;
        break;
    case 4:
        CD4051_A0;
        CD4051_B0;
        CD4051_C1;
        break;
    case 5:
        CD4051_A1;
        CD4051_B0;
        CD4051_C1;
        break;
    case 6:
        CD4051_A0;
        CD4051_B1;
        CD4051_C1;
        break;
    case 7:
        CD4051_A1;
        CD4051_B1;
        CD4051_C1;
        break;
    default:
    }
}
