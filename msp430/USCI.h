/**********************************************
 *
 *      Author  :       Shawn Guo
 *      Mail    :       iguoxiaopeng@gmail.com
 *      Date    :       2013/8/12
 *      Last    :       2013/8/12
 *      Notes   :       USCI模块
 *      Tool    :       MSP430X2XX
 *
 *----------------------------------------------
 *  注意:由于SPI是双全工工作,所以当接受数据时,主机应该发送空数据以保证可以接受数据.
 **********************************************/
#ifndef __USCI_H__
#define __USCI_H__

#define USCI_A0TX_ENABLE      (IE2 |= UCA0TXIE)
#define USCI_A0RX_ENABLE      (IE2 |= UCA0RXIE)
#define USCI_B0TX_ENABLE      (IE2 |= UCB0TXIE)
#define USCI_B0RX_ENABLE      (IE2 |= UCB0RXIE)

#define USCI_A1TX_ENABLE      (UC1IE |= UCA1TXIE)
#define USCI_A1RX_ENABLE      (UC1IE |= UCA1RXIE)
#define USCI_B1TX_ENABLE      (UC1IE |= UCB1TXIE)
#define USCI_B1RX_ENABLE      (UC1IE |= UCB1RXIE)

/***************************************************************************//**
 * @brief   Initialize the USCI system.
 * @param
 *
 *-------------------------------------------------------------------------------
 *          id  =       0               USCI_A0
 *                      1               USCI_A1
 *                      2               USCI_B0
 *                      3               USCI_B1
 *-------------------------------------------------------------------------------
 *          clk_mode    0               UCCKPH=0,UCCKPL=0   SMCLK
 *                      1                      0        1   SMCLK
 *                      2                      1        0   SMCLK
 *                      3                      1        1   SMCLK
 *          clk_mode    4               UCCKPH=0,UCCKPL=0   ACLK
 *                      5                      0        1   ACLK
 *                      6                      1        0   ACLK
 *                      7                      1        1   ACLK
 *          UCCKPH = 0(第一个时钟沿改变，第二个时钟沿捕获)
 *          UCCKPH = 1(第二个时钟沿改变，第一个时钟沿捕获)
 *          UCCKPL = 0(空闲状态低电平)   1(空闲状态高电平)
 *          UCCKPH  --->    CPHA            UCCKPL  --->    CPOL    对应的别名
 *-------------------------------------------------------------------------------
 *          mode0       0             MSB=0,BIT=8,  Slave mode
 *                      1               0       8   Master
 *                      2               0       7   S
 *                      3               0       7   M
 *                      4               1       8   S
 *                      5               1       8   m
 *                      6               1       7   S
 *                      7               1       7   m
 *-------------------------------------------------------------------------------
 *          mode1       0               3-SPI
 *                      1               4-SPI, HIGH UCXCTE
 *                      2               4-SPI, LOW  UCXCTE
 *
 *-------------------------------------------------------------------------------
 *          div         分频系数, 16位
 *-------------------------------------------------------------------------------
 *-------------------------------------------------------------------------------
 *-------------------------------------------------------------------------------
 *-------------------------------------------------------------------------------
 *-------------------------------------------------------------------------------
 *
 *
 * @return none
 *******************************************************************************/
// 默认不开中断, 需另行配置        IE |= UCA0TXIE      等等
void USCI_SPI_Init(unsigned char id, unsigned char clk_mode, unsigned char mode0, unsigned char mode1, unsigned int div);

//eg. 默认模式 高位先发8位三线主机SPI
//USCI_SPI_Init(3, 2, 5, 1, 0);
#endif /* __USCI_H__ */
