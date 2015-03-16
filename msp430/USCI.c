/**********************************************
 *
 *      Author  :       Shawn Guo
 *      Mail    :       iguoxiaopeng@gmail.com
 *      Date    :       2013/8/12
 *      Last    :       2013/8/12
 *      Notes   :       USCI模块
 *      Tool    :       MSP430X2XX
 **********************************************/

/*****************************************************************************
* @file       USCI.c
* @addtogroup USCI
* @{
******************************************************************************/
#include <msp430f2618.h>
#include "clock.h"
#include "USCI.h"

void USCI_SPI_Init(unsigned char id, unsigned char clk_mode, unsigned char mode0, unsigned char mode1, unsigned int div)
{
    unsigned char ucxctl0 = 0;

    switch(clk_mode)
    {
    case 0:
        ucxctl0 &= ~(0x80u);
        ucxctl0 &= ~(0x40u);
        break;
    case 1:
        ucxctl0 &= ~(0x80u);
        ucxctl0 |=  (0x40u);
        break;
    case 2:
        ucxctl0 |=  (0x80u);
        ucxctl0 &= ~(0x40u);
        break;
    case 3:
        ucxctl0 |=  (0x80u);
        ucxctl0 |=  (0x40u);
        break;
    }


    switch(mode0)
    {
    case 0:
        ucxctl0 &= ~(0x20u);
        ucxctl0 &= ~(0x10u);
        ucxctl0 &= ~(0x08u);
        break;
    case 1:
        ucxctl0 &= ~(0x20u);
        ucxctl0 &= ~(0x10u);
        ucxctl0 |=  (0x08u);
        break;
    case 2:
        ucxctl0 &= ~(0x20u);
        ucxctl0 |=  (0x10u);
        ucxctl0 &= ~(0x08u);
        break;
    case 3:
        ucxctl0 &= ~(0x20u);
        ucxctl0 |=  (0x10u);
        ucxctl0 |=  (0x08u);
        break;
    case 4:
        ucxctl0 |=  (0x20u);
        ucxctl0 &= ~(0x10u);
        ucxctl0 &= ~(0x08u);
        break;
    case 5:
        ucxctl0 |=  (0x20u);
        ucxctl0 &= ~(0x10u);
        ucxctl0 |=  (0x08u);
        break;
    case 6:
        ucxctl0 |=  (0x20u);
        ucxctl0 |=  (0x10u);
        ucxctl0 &= ~(0x08u);
        break;
    case 7:
        ucxctl0 |=  (0x20u);
        ucxctl0 |=  (0x10u);
        ucxctl0 |=  (0x08u);
        break;
    }

    switch(mode1)
    {
    case 0:
        ucxctl0 &= ~(0x04u);
        ucxctl0 &= ~(0x02u);
        break;
    case 1:
        ucxctl0 &= ~(0x04u);
        ucxctl0 |=  (0x02u);
        break;
    case 2:
        ucxctl0 |=  (0x04u);
        ucxctl0 &= ~(0x02u);
        break;
    }

    ucxctl0 |= UCSYNC;  // 开启同步模式, 专门用于SPI

    //根据ID配置不同的寄存器
    switch(id)
    {
    case 0:
        UCA0CTL1 = UCSWRST;     //置位复位标志,开始配置寄存器

        UCA0CTL0 = ucxctl0;
        if(clk_mode < 4)        //时钟选择
        {
            UCA0CTL1 |= UCSSEL_2;
        }
        else UCA0CTL1 |= UCSSEL_1;

        if(div)
        {
            UCA0BR0 = (unsigned char)(div);
            div >>= 8;
            UCA0BR1 = (unsigned char)(div);
        }
        //IO配置  P3.0 CLK, P3.4 SIMO P3.5 SOMI
        //SOMI可能用不上(不管此引脚是否与从模块连接，此引脚的设置不能少，可产生作为传输结束的标志)
        P3SEL   |=  (BIT0 + BIT4 + BIT5);   //只需将三个引脚的两个SEL寄存器置1即可
        UCA0CTL1 &= ~UCSWRST;   //消除复位标志,完成配置
        break;
    case 1:
        UCA1CTL1 = UCSWRST;
        UCA1CTL0 = ucxctl0;
        if(clk_mode < 4)        //时钟选择
        {
            UCA1CTL1 |= UCSSEL_2;
        }
        else UCA1CTL1 |= UCSSEL_1;
        if(div)
        {
            UCA1BR0 = (unsigned char)(div);
            div >>= 8;
            UCA1BR1 = (unsigned char)(div);
        }
        //  A1SIMO P3.6         A1SOMI P3.7     A1CLK P5.0
        P3SEL   |=  (BIT6 + BIT7);   //只需将三个引脚的两个SEL寄存器置1即可
        P5SEL   |=  BIT0;

        UCA1CTL1 &= ~UCSWRST;   //消除复位标志,完成配置
        break;
    case 2:
        UCB0CTL1 = UCSWRST;
        UCB0CTL0 = ucxctl0;
        if(clk_mode < 4)        //时钟选择
        {
            UCB0CTL1 |= UCSSEL_2;
        }
        else UCB0CTL1 |= UCSSEL_1;
        if(div)
        {
            UCB0BR0 = (unsigned char)(div);
            div >>= 8;
            UCB0BR1 = (unsigned char)(div);
        }
        // B0SIMO P3.1         B0SOMI P3.2     B0CLK P3.3
        P3SEL   |=  (BIT1 + BIT2 + BIT3);   //只需将三个引脚的两个SEL寄存器置1即可

        UCB0CTL1 &= ~UCSWRST;   //消除复位标志,完成配置


        break;
    case 3:
        UCB1CTL1 = UCSWRST;
        UCB1CTL0 = ucxctl0;
        if(clk_mode < 4)        //时钟选择
        {
            UCB1CTL1 |= UCSSEL_2;
        }
        else UCB1CTL1 |= UCSSEL_1;
        if(div)
        {
            UCB1BR0 = (unsigned char)(div);
            div >>= 8;
            UCB1BR1 = (unsigned char)(div);
        }
        // B1SIMO P5.1         B1SOMI P5.2     B1CLK P5.3
        P5SEL   |=  (BIT1 + BIT2 + BIT3);   //只需将三个引脚的两个SEL寄存器置1即可

        UCB1CTL1 &= ~UCSWRST;   //消除复位标志,完成配置
        break;
    }
}

/***************************************************************************//**
 * @brief
 * @param
 * @return none
 *******************************************************************************/


/***************************************************************************//**
 * @}
 ******************************************************************************/
