/**********************************************
 *
 *      Author  :       Shawn Guo
 *      Mail    :       iguoxiaopeng@gmail.com
 *      Date    :       2013/8/12
 *      Last    :
 *      Notes   :       NRF905无线收发器模块
 *      Tool    :       NRF905, MSP430F2618
 **********************************************/

/*****************************************************************************
* @file       NRF905.c
* @addtogroup NRF905
* @{
******************************************************************************/
#include <msp430f2618.h>
#include "clock.h"
#include "NRF905.h"
#include "USCI.h"


/***************************************************************************//**
 * @brief   RF-Configuration-Register(R/W)寄存器
 *******************************************************************************/
extern unsigned char NRF905_Config[10]=
{
    0x4c,                 //CH_NO,配置频段在430.0MHZ
    0x0c,                 //输出功率为10DB,不重发数据，正常模式
    0x44,                 //RX/TX地址宽度为4个字节
    0x02,0x02,            //数据的宽度为2个字节
    0x23,0xbb,0xcc,0xdd,  //默认接收数据地址0x23,0xbb,0xcc,0xdd,
    0x58                  //CRC允许，8位CRC校验，外部时钟信号不使能，16M晶振 0X58
    //打开时钟, 则可以为0x5c,0x5d,0x5e,0x5f（4M->2M->1M->500K）
};

/*******************************************************************************
 *                              接口函数
 ******************************************************************************/

void NRF905_Init()
{
    NRF905_MODE_IO_OUT; // MODE_IO init
    NRF905_TXEN_1;      // Standby & SPI mode.
    NRF905_CE_0;

    NRF905_STATE_IO_IN; // Start IO Init, AM,CD,DR

#ifdef NRF905_MONI_SPI
    NRF905_SPI_DIR;
    NRF905_SIMO_0;
    NRF905_CLK_0;
#else
    //SPI Init
    // 基本配置: USA0, 3-SPI,第二个时钟沿改变数据, SMCLK, MSB, 8-bit, 3-SPI主机模式
    // 保持1M的SCLK，禁止中断.
    USCI_SPI_Init(0, 2, 5, 1, CPU_FF);
#endif // NRF905_MONI_SPI

    //配置NRF905的工作模式
    //工作模式为模式控制字NRF905_Config[]中定义的所有模式
    //nRF的状态需要切换到：待机编程模式

    //NRF905_Set_Clock(NRF905_CLK_OUT_ON + NRF905_CLK_OUT_2M + NRF905_CLK_16M);

    //从00000000字节开始,写入10个字节命令数据, 配置寄存器RF-Configuration.
    NRF905_Write_Data(NRF905_WC, NRF905_Config, 10);
}

void NRF905_Set_Mode(unsigned char mode)       //选择NRF905的工作方式
{
    switch(mode)
    {
    case NRF905_WAIT:
        NRF905_CE_0;
        NRF905_MODE = NRF905_WAIT;
        break;

    case NRF905_RCV :
        NRF905_TXEN_0;
        NRF905_CE_1;
        NRF905_MODE = NRF905_RCV;
        break;

    case NRF905_SEND:
        NRF905_TXEN_1;
        NRF905_CE_1;
        NRF905_MODE = NRF905_SEND;
        break;
    }
    DELAY_US(650);  //器件模式切换所需时间
}


void NRF905_Write_Data(const unsigned char  cmd,unsigned char  *pdata, const unsigned char  len)
{
    unsigned char i = 0;
    unsigned char s = NRF905_MODE;  //记录原始状态

    if(NRF905_MODE != NRF905_WAIT)      //切换到Standby模式
        NRF905_Set_Mode(NRF905_WAIT);

    NRF905_CSN_0;       //SPI使能
    DELAY_US(10);
    NRF905_SPI_Send(cmd);

    for(i = 0; i < len; i++)
        NRF905_SPI_Send(pdata[i]);

    NRF905_CSN_1;

    NRF905_Set_Mode(s);
}

void NRF905_Read_Data(const unsigned char cmd, unsigned char *pdata, const unsigned char len)
{
    unsigned char i = 0;
    unsigned char s = NRF905_MODE;  //记录原始状态

    if(NRF905_MODE != NRF905_WAIT)      //切换到Standby模式
        NRF905_Set_Mode(NRF905_WAIT);

    NRF905_CSN_0;       //SPI使能
    DELAY_US(10);
    NRF905_SPI_Send(cmd);

    for(i = 0; i < len; i++)
        NRF905_SPI_Receive(pdata + i);

    NRF905_CSN_1;

    NRF905_Set_Mode(s);

}

/*******************************************************************************
 *                              内部函数
 ******************************************************************************/
void NRF905_Set_Power(unsigned char power)
{
    NRF905_Config[1] &= ~(BIT3 + BIT2);
    NRF905_Config[1] |= power;
}

//配置接受地址
void NRF905_Set_RevAddress(unsigned char add1, unsigned char add2, unsigned char add3, unsigned char add4)
{
    NRF905_Config[5] = add1;
    NRF905_Config[6] = add2;
    NRF905_Config[7] = add3;
    NRF905_Config[8] = add4;
    NRF905_Write_Data(NRF905_WC | 0X05, &NRF905_Config[5], 4);
    //NRF905_Write_Data(NRF905_WC, NRF905_Config, 10);
}

void NRF905_Set_SendAddress(unsigned char add0, unsigned char add1, unsigned char add2, unsigned char add3)
{
    unsigned char address[4];

    address[0] = add0;
    address[1] = add1;
    address[2] = add2;
    address[3] = add3;

    NRF905_Write_Data(NRF905_WTA, address, 4);
}



void NRF905_Set_Channel(unsigned char channel)    //配置频段
{

    NRF905_Config[0]  = 0;
    NRF905_Config[1]  &= ~(BIT0 + BIT1);
    switch(channel)
    {
    case NRF905_430_0M:
    case NRF905_433_1M:
    case NRF905_433_2M:
    case NRF905_434_7M:
        NRF905_Config[0] = NRF905_FRE_TABLE[channel];
        break;

    case NRF905_868_0M:
    case NRF905_868_2M:
    case NRF905_868_4M:
    case NRF905_868_8M:
        NRF905_Config[0] = NRF905_FRE_TABLE[channel];
        NRF905_Config[1] |= (BIT1);     //HFREQ_PLL = 1;
        break;

    case NRF905_902_2M:
    case NRF905_902_4M:
    case NRF905_902_8M:
        NRF905_Config[0] = NRF905_FRE_TABLE[channel];
        NRF905_Config[1] |= (BIT1 + BIT0);     //HFREQ_PLL = 1;
        break;
    }
}

void NRF905_Address_LEN(unsigned char len)     //配置地址长度
{
    NRF905_Config[2] = len;
}

void NRF905_Data_LEN(unsigned char rx, unsigned char tx)       //配置数据长度
{
    NRF905_Config[3] = rx;
    NRF905_Config[4] = tx;
}

void NRF905_Set_Clock(unsigned char clk)   //配置时钟
{
    NRF905_Config[9] &= ~(0x3f);
    NRF905_Config[9] |= clk;
}

void NRF905_Set_CRC(unsigned char crc)    //配置CRC校验
{
    NRF905_Config[9] &= ~(0xc0);
    NRF905_Config[9] |= crc;
}

void NRF905_Set_Pattern(unsigned char mode)   //配置工作模式
{
    NRF905_Config[1] &= ~(0x30);
    NRF905_Config[1] |= mode;
}




/*******************************************************************************
 *                              SPI配置
 ******************************************************************************/
void NRF905_SPI_Send(unsigned char dat)    //SPI发送数据
{
#ifdef  NRF905_MONI_SPI
    unsigned char i = 0;
    for (i = 0; i < 8; i++)
    {
        if (dat & 0x80)	//总是发送最高位
        {
            NRF905_SIMO_1;
        }
        else
        {
            NRF905_SIMO_0;
        }
        NRF905_CLK_1;

        dat <<= 1;
        NRF905_CLK_0;
    }
#else
    while(!(IFG2 & UCA0TXIFG)); //等待空闲
    UCA0TXBUF = dat;
    while(!(IFG2 & UCA0RXIFG)); //等待发送完成,发送完成会置UCA0RXIFG;
    IFG2 &= ~UCA0RXIFG;

#endif // NRF905_MONI_SPI
}


void NRF905_SPI_Receive(unsigned char* dat)  //SPI接收数据至USA0RXBUF
{

#ifdef  NRF905_MONI_SPI
    unsigned char i = 0;
    for (i = 0; i < 8; i++)
    {
        (*dat) = (*dat) << 1;
        NRF905_CLK_1;

        if (NRF905_SOMI)	//读取最高位，保存至最末尾，通过左移位完成整个字节
        {
            (*dat)++;   //最末位置一
        }

        NRF905_CLK_0;
    }
#else
    UCA0TXBUF = 0X00;       //由于SPI是双全工工作,所以当接受数据时,主机应该发送空数据以保证可以接受数据.
    while(!(IFG2 & UCA0RXIFG)); //TX发送完成则代表RX接受完成
    *dat = UCA0RXBUF;           //取数据将自动清零UCA0RXBUF;
#endif // NRF905_MONI_SPI

}

/***************************************************************************//**
 * @brief
 * @param
 * @return none
 *******************************************************************************/


/***************************************************************************//**
 * @}
 ******************************************************************************/
