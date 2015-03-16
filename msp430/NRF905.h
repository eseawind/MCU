/**********************************************
 *
 *      Author  :       Shawn Guo
 *      Mail    :       iguoxiaopeng@gmail.com
 *      Date    :       2013/8/12
 *      Last    :       2013/8/15
 *      Notes   :       NRF905无线收发器模块
 *      Tool    :       NRF905, MSP430F2618
 **********************************************/
#ifndef __NRF905_H__
#define __NRF905_H__

//#define NRF905_MONI_SPI     //模拟SPI功能

/*******************************************************************************
 *                          nRF905的IO配置说明
 *      VCC     --->    VCC_3.3V
 *      PWR_UP  --->    VCC_3.3V
 *      CE      --->    P8.1        发射/接受使能,高电平有效
 *      UCK     --->                时钟输出
 *      AM      --->    P8.0        地址匹配
 *      MISO    --->    P3.5
 *      SCK     --->    P3.0        UCA0CLK
 *      CSN     --->    P7.2        SPI使能,低电平有效
 *      MOSI    --->    P3.4
 *      DR      --->    P1.7        接受/发射数据完成,等待处理(中断)
 *      CD      --->    P7.7        载波检测
 *      TXEN    --->    P7.6        TX/RX模式选择
 *
 ******************************************************************************/

#define  NRF905_CE_DIR           (P8DIR |=     BIT1)
#define  NRF905_CE_0             (P8OUT &=    ~BIT1)        //高电平有效
#define  NRF905_CE_1             (P8OUT |=     BIT1)

//状态IO配置    //配置CD、AM、DR为单片机输入
#define  NRF905_STATE_IO_IN      (P7DIR &= ~BIT7, P1DIR &= ~BIT7, P8DIR &= ~BIT0)
#define  NRF905_CD_IN     		 (P7IN & BIT7)    //get data from CD
#define  NRF905_AM_IN     		 (P8IN & BIT0)    //get data from AM   若要用中断，则AM为上升沿触发
#define  NRF905_DR_IN     		 (P1IN & BIT7)    //get data from DR

#define  NRF905_TXEN_DIR         (P7DIR |=     BIT6)    //高电平有效
#define  NRF905_TXEN_0           (P7OUT &=    ~BIT6)
#define  NRF905_TXEN_1           (P7OUT |=     BIT6)

#define  NRF905_CSN_DIR        (P7DIR |=     BIT2)  //低电平有效
#define  NRF905_CSN_1          (P7OUT |=     BIT2)
#define  NRF905_CSN_0          (P7OUT &=    ~BIT2)

//工作模式IO配置 TXEN, CE, CSN为输出
#define  NRF905_MODE_IO_OUT      (NRF905_TXEN_DIR,NRF905_CE_DIR,NRF905_CSN_DIR)


#ifdef  NRF905_MONI_SPI

//3-模拟SPI-IO端口配置
#define NRF905_SPI_DIR  		        (P3DIR |=  (BIT4 + BIT0), P3DIR &= ~BIT5)

#define NRF905_SIMO_0  		            (P3OUT &= ~BIT4)
#define NRF905_SIMO_1  		            (P3OUT |=  BIT4)
#define NRF905_SOMI  		            (P3IN  &   BIT5)
#define NRF905_CLK_0   		            (P3OUT &= ~BIT0)
#define NRF905_CLK_1   		            (P3OUT |=  BIT0)
#else

// MSP430F2618硬件USCI A0,默认1M的数据
#define NRF905_SIMO     (P3SEL |= BIT4)
#define NRF905_SOMI     (P3SEL |= BIT5)
#define NRF905_CLK      (P3SEL |= BIT0)

#endif

/*******************************************************************************
 *                              工作模式配置
 *      PWR_UP      CE      TX_EN       MODE
 *      0           X       X           掉电      &       SPI编程
 *      1           0       X           Standby   &       SPI编程
 *      1           1       0           RX
 *      1           1       1           TX
 *******************************************************************************/
#define  NRF905_OFF      (0)    //掉电(这里PWR_UP直接连接VCC_3.3V,因此掉电模式不可选)
#define  NRF905_WAIT     (1)    //Standby待机
#define  NRF905_RCV      (2)    //接受
#define  NRF905_SEND     (3)    //发射

static unsigned char NRF905_MODE = NRF905_WAIT;    //默认Standby模式

/*******************************************************************************
 *                              NRF905-频段表
 *******************************************************************************/
// CH_NO   #0[7:0]
static const unsigned char NRF905_FRE_TABLE[]=
{
    0X4C,0X6B,0X6C,0X7B,   // 433.X MHZ ,   HFREQ_PLL = 0, CH_NO[8] = 0
    0X56,0X75,0X76,0X7D,   // 868.X MHZ ,       1           0
    0X1F,0X20,0X9F         // 902.X             1           1
};

// 宏定义枚举
#define NRF905_430_0M    (0x00)   //430.0M
#define NRF905_433_1M    (0x01)   //433.1M
#define NRF905_433_2M    (0x02)   //433.2M
#define NRF905_434_7M    (0x03)   //434.7M
#define NRF905_868_0M    (0x04)   //862.0M
#define NRF905_868_2M    (0x05)   //868.2M
#define NRF905_868_4M    (0x06)   //868.4M
#define NRF905_868_8M    (0x07)   //869.8M
#define NRF905_902_2M    (0x08)   //902.2M
#define NRF905_902_4M    (0x09)   //902.4M
#define NRF905_902_8M    (0x0a)   //927.8M

/*******************************************************************************
 *                              nRF905的控制字
 ******************************************************************************/
// SPI发送命令字
#define  NRF905_WC   0X00    //写配置寄存器       0000XXXX    (XXXX为写操作的开始字节)
#define  NRF905_RC   0X10    //读配置寄存器       0001XXXX    (XXXX为读操作的开始字节)
#define  NRF905_WTP  0X20    //写发送数据         00100000    (写TX有效数据)
#define  NRF905_RTP  0X21    //读发送的数据       00100001    (读TX有效数据)
#define  NRF905_WTA  0X22    //写目标方的地址
#define  NRF905_RTA  0X23    //读目标方的地址
#define  NRF905_RRP  0X24    //读接收到的数据

//寄存器   RF-Configuration-Register

//PA_PWR[1:0]     #1  BIT3 BIT2
#define  NRF905_POWER_10_    	  	(0x00)     //输出功率: -10dB
#define  NRF905_POWER_2_      	    (0x04)     //输出功率：-2dB
#define  NRF905_POWER_6        	    (0x08)     //输出功率：6dB
#define  NRF905_POWER_10      	    (0x0C)     //输出功率：10dB
//长度配置  RW_AWF #2[2:0]      TX_AWF[6:4]
#define  NRF905_RA_1        	(0x01)     //接收地址长度：1Byte
#define  NRF905_RA_4        	(0x04)     //接收地址长度：4Byte
#define  NRF905_TA_1            (0x10)     //发送地址长度：1Byte
#define  NRF905_TA_4            (0x40)     //发送地址长度：1Byte
// RX_PWR #3[5:0]   TX_PWR #4[5:0]
#define  NRF905_RD_1        	(0x01)     //接收数据长度：1Byte
#define  NRF905_RD_2        	(0x02)     //接收数据长度：2Byte
#define  NRF905_RD_32       	(0x80)     //接收数据长度：32Byte
#define  NRF905_TD_1        	(0x01)     //发送数据长度：1Byte
#define  NRF905_TD_2        	(0x02)     //发送数据长度：2Byte
#define  NRF905_TD_32       	(0x80)     //发送数据长度：32Byte

//工作模式配置    RX_RED_PWR #1[4]      AUTO_BETRAN #1[5]
#define  NRF905_MODE0   	    (0x10)     //节能模式
#define  NRF905_MODE1   	    (0x00)     //正常模式
#define  NRF905_REPEAT1         (0x20)     //重复发送
#define  NRF905_REPEAT0   	 	(0x00)     //单次发送

//XOF #9[5:3], UP_CLK_EN #9[2], UP_CLK_FREQ #9[1:0]
#define  NRF905_CLK_OUT_OFF     (0x00)     //输出时钟UCLK关闭
#define  NRF905_CLK_OUT_ON      (0x04)     //输出时钟UCLK打开
#define  NRF905_CLK_OUT_4M      (0x00)     //输出频率：4M
#define  NRF905_CLK_OUT_2M      (0x01)     //输出频率：2M
#define  NRF905_CLK_OUT_1M      (0x02)     //输出频率：1M
#define  NRF905_CLK_OUT_500K    (0x03)     //输出频率：500K
#define  NRF905_CLK_4M          (0x00)         //输入频率：4M
#define  NRF905_CLK_8M          (0x08)         //输入频率：8M
#define  NRF905_CLK_12M         (0x10)         //输入频率：12M
#define  NRF905_CLK_16M         (0x18)         //输入频率：16M
#define  NRF905_CLK_20M         (0x28)         //输入频率：20M
//  CRC_MODE #9[7]  CRC_EN  #9[6]
#define  NRF905_CRC_ENABLE      (0x40)     //CRC校验允许
#define  NRF905_CRC_DISABLE     (0x00)     //CRC校验禁止
#define  NRF905_CRC_8           (0x00)     //CRC校验为8位
#define  NRF905_CRC_16          (0x80)     //CRC校验为16位

/***************************************************************************//**
 * @brief   Initialize the IO, SPI, and Configurate Register of NRF905.
 *
 * @param   NONE
 * @return
 *******************************************************************************/

void NRF905_Init();
void NRF905_Set_Mode(unsigned char mode);       //选择NRF905的工作方式
//cmd为写入的命令
//*pdata指向要写入的数据
//len为要写入数据的长度
//NRF905的状态需要切换到：待机编程模式
void NRF905_Write_Data(const unsigned char  cmd, unsigned char  *pdata, const unsigned char  len);
void NRF905_Read_Data(const unsigned char cmd, unsigned char *pdata, const unsigned char len);
void NRF905_SPI_Send(unsigned char dat);        //SPI发送数据
void NRF905_SPI_Receive(unsigned char* dat);    //SPI接收数据

//配置接受/发射地址,本配置立即生效.
void NRF905_Set_RevAddress(unsigned char add1, unsigned char add2, unsigned char add3, unsigned char add4);
void NRF905_Set_SendAddress(unsigned char add1, unsigned char add2, unsigned char add3, unsigned char add4);

/***************************************************************************//**
 * @brief   内部基础函数, 修改的是NRF905_Config[]里面的值
 *          更改后需要调用 NRF905_Write_Data(NRF905_WC, NRF905_Config, 10);    更新。
 *******************************************************************************/
void NRF905_Set_Power(unsigned char power);  //配置功率NRF905_POWER_X
void NRF905_Set_Channel(unsigned char channel);//配置频段
void NRF905_Address_LEN(unsigned char len);//配置地址长度    NRF905_Address_LEN(NRF905_RA_1 + NRF905_TA_4);

//配置数据长度    NRF905_Data_LEN(NRF905_RD_1, NRF905_TD_32);
void NRF905_Data_LEN(unsigned char rx, unsigned char tx);

//NRF905_Set_Clock(NRF905_CLK_OUT_ON + NRF905_CLK_OUT_4M + NRF905_CLK_16M);
void NRF905_Set_Clock(unsigned char clk);   //配置时钟

// NRF905_Set_CRC(NRF905_CRC_ENABLE + NRF905_CRC_16);
void NRF905_Set_CRC(unsigned char crc);    //配置CRC校验

// NRF905_REPEAT0 + NRF905_MODE0
void NRF905_Set_Pattern(unsigned char pattern);   //配置工作模式
extern unsigned char NRF905_Config[10];

#endif /* __MODULE_H__ */
