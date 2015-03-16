/**********************************************
 *
 *      Author  :       Shawn Guo
 *      Mail    :       iguoxiaopeng@gmail.com
 *      Date    :       2013/8/27
 *      Last    :
 *      Notes   :       ADXL345 加速度传感器模块
 *      Tool    :       MSP430X2XX  / ADXL345
 *
 *
 *      1.VCC   2.NC    3.NC    4.INT2  5.INT1  6.GND
 *      7.GND   8.CS    9.SOD   10.SDI 11.SCLK 12.VCC
 *      SPI配置: ADXL345为从机, SCLK空闲为高电平
 *               数据在下降沿更新,在上升沿捕获.
 *--------------------------------------------------------
 *
 **********************************************/

#ifndef __ADXL_H__
#define __ADXL_H__

/***************************************************************************************************
 *                                ADXL345接口说明
 * -----SPI-----
 * SCL/SCLK         ----> P4.3
 * SDA/SDI/SDIO     ----> P4.1
 * SOD/ALT ADDRESS  ----> P4.2
 * CS               ----> P3.7
 *
 * ----I2C----
 * SCL              ----> P7.0
 * SDA              ----> P7.1
 * ALT ADDRESS      ----> GND
 * CS               ----> VCC
****************************************************************************************************/
#define ADXL_GRAIN  980.0           //重力加速度值,扩大100倍
#define ADXL_PI     3.14            //PI值
//#define SPI_SIM_H_                //是否使用模拟SPI，若使用，则使用该宏定义,否则，使用I2C

//IO口定义
#ifndef SPI_SIM_H_
/***IO口设置***/
#define  IIC_SCL          (P7SEL &=~BIT0, P7DIR |= BIT0)
#define  IIC_SDA_OUT      (P7SEL &=~BIT1, P7DIR |= BIT1)
#define  IIC_SDA_IN       (P7SEL &=~BIT1, P7DIR &=~BIT1)
/****输入与输出****/
#define  IIC_SCL_1        (P7OUT |= BIT0)    //IIC_SCL out : 1
#define  IIC_SCL_0        (P7OUT &=~BIT0)   //IIC_SCL out : 0
#define  IIC_SDA_1        (P7OUT |= BIT1)    //IIC_SDA out : 1
#define  IIC_SDA_0        (P7OUT &=~BIT1)   //IIC_SDA out : 0
#define  IIC_SDA          (P7IN  &  BIT1)     //IIC_SDA in

#else
//模拟SPI的IO口设置
#define SPIB1_SIMO        P4SEL &=~BIT1; P4DIR |=BIT1
#define SPIB1_SOMI        P4SEL &=~BIT2; P4DIR &=~BIT2
#define SPIB1_CLK_OUT     P4SEL &=~BIT3; P4DIR |=BIT3
#define SPIB1_CLK_IN      P4SEL &=~BIT3; P4DIR &=~BIT3
#define IO_CFG            P3SEL &=~BIT7; P3DIR |=BIT7; P3OUT |=BIT7    //IO config

//SPI的输入与输出
#define SPIB1_SOMI_IN     (P4IN & BIT2)    //SOMI in
#define SPIB1_SIMO_1      P4OUT |=BIT1     //SIMO out : 1
#define SPIB1_SIMO_0      P4OUT &=~BIT1    //SIMO out : 0
#define SPIB1_CLK_0       P4OUT &=~BIT3    //CLK  out : 0
#define SPIB1_CLK_1       P4OUT |=BIT3     //CLK  out : 1
#define CS_0        	  P3OUT &=~BIT7   //CS :  0
#define CS_1       		  P3OUT |=BIT7    //CS :  1
#endif


/****内部变量定义****/

extern unsigned char  ADXL_ACT_INACT_CTL;   //ADXL检测活动与静止的配置数据
extern unsigned char  ADXL_INT_REG;      //ADXL的中断设置
extern unsigned char  ADXL_INT_MAP_REG;     //ADXL的中断映射
extern unsigned char  ADXL_DATA_FORM;    //数据格式

extern int ADXL_X_VALUE;   //X加速度值
extern int ADXL_Y_VALUE;   //Y加速度值
extern int ADXL_Z_VALUE;   //Z加速度值

/*****寄存器定义******/
#define   DEVID            0x00   // R
#define   THRESH_TAP       0x1d   // R/W
#define   OFSX             0x1e   // R/W
#define   OFSY             0x1f   // R/W
#define   OFSZ             0x20   // R/W
#define   DUR              0x21   // R/W
#define   Latent           0x22   // R/W
#define   Window           0x23   // R/W
#define   THRESH_ACT       0x24   // R/W
#define   THRESH_INACT     0x25   // R/W
#define   TIME_INACT       0x26   // R/W
#define   ACT_INACT_CTL    0x27   // R/W
#define   THRESH_FF        0x28   // R/W
#define   TIME_FF          0x29   // R/W
#define   TAP_AXES         0x2a   // R/W
#define   ACT_TAP_STATUS   0x2b   // R
#define   BW_RATE          0x2c   // R/W
#define   POWER_CTL        0x2d   // R/W
#define   INT_ENABLE       0x2e   // R/W
#define   INT_MAP          0x2f   // R/W
#define   INT_SOURCE       0x30   // R
#define   DATA_FORMAT      0x31   // R
#define   DATAX0           0x32   // R
#define   DATAX1           0x33   // R
#define   DATAY0           0x34   // R
#define   DATAY1           0x35   // R
#define   DATAZ0           0x36   // R
#define   DATAZ1           0x37   // R
#define   FIFO_CTL         0x38   // R/W
#define   FIFO_STATUS      0x39   // R
/****三个轴向的偏移量***/
#define ADXL_OFS_X         OFSX   //X轴偏移
#define ADXL_OFS_Y         OFSY   //Y轴偏移
#define ADXL_OFS_Z         OFSZ   //Z轴偏移
/***三个轴定义***/
#define ADXL_X             (0x04)   //X轴
#define ADXL_Y             (0x02)   //Y轴
#define ADXL_Z             (0x01)   //Z轴
/***比较模式***/
#define ADXL_AC            (0)   //交流
#define ADXL_DC            (1)   //直流
/***状态选择***/
#define ADXL_TAP           (0)  //敲击
#define ADXL_ACT           (1)  //活动
#define ADXL_SLEEP         (2)  //是否睡眠
/***电源模式***/
#define ADXL_NORMAL       (0)  //正常
#define ADXL_LOW          (1)  //低功耗
/***工作模式***/
/****数据速率****/
#define ADXL_RATE_400     (0x0c)
#define ADXL_RATE_200     (0x0b)
#define ADXL_RATE_100     (0x0a)
#define ADXL_RATE_50      (0x09)
#define ADXL_RATE_25      (0x08)
#define ADXL_RATE_12_5    (0x07)
/***中断定义***/
/***中断有效***/
#define ADXL_VALID_H     (0)   //高电平有效
#define ADXL_VALID_L     (1)   //低电平有效
/***中断映射***/
#define ADXL_INT1        (0)   //INT1
#define ADXL_INT2        (1)   //INT2
/***中断源***/
#define DATA_READY       (0X80)  //数据准备好中断
#define SINGLE_TAP       (0x40)  //单击中断
#define DOUBLE_TAP       (0x20)  //双击中断
#define ACTIVITY         (0x10)  //活动检测中断
#define INACTIVITY       (0x08)  //静止检测中断
#define FREE_FALL        (0x04)  //自由落体中断
#define WATERMASK        (0x02)  //水印中断
#define OVERRUN          (0x01)  //溢出中断
/********测量范围*********/
#define ADXL_RANGE_2     (0)  //正负2g
#define ADXL_RANGE_4     (1)  //正负4g
#define ADXL_RANGE_8     (2)  //正负8g
#define ADXL_RANGE_16    (3)  //正负16g
//初始化ADXL345
void ADXL_INIT();

//设置角度传感器的三个轴向的偏移量
void ADXL_OFS_CONFIG(unsigned int sel,int data);

//设置敲击事件要检测的方向轴
void ADXL_TAPDIR_CONFIG(unsigned char dir, unsigned char dat);

//设置敲击事件的最小幅度和最大时间
void ADXL_TAP_CNOFIG(unsigned char thresh,unsigned char dur);

//设置第二次敲击的延迟和检测时间
void ADXL_STAP_CONFIG(unsigned char delay, unsigned char time);

//设置活动检测的阀值
void ADXL_ACT_CONFIG(unsigned char thresh, unsigned char dir, unsigned char mode);

//设置静止检测的阀值和时间
void ADXL_INACT_CONFIG(unsigned char thresh, unsigned char time, unsigned char dir, unsigned char mode);

//设置自由落体运动的加速度阀值和时间阀值
void ADXL_FF_CONFIG(unsigned char thresh, unsigned char time);

//读取敲击/活动事件的来源，或是系统当前的休眠状态
unsigned char ADXL_GET_STATE(unsigned char sel);

//设置工作模式和数据速率
void ADXL_MODE_CONFIG(unsigned char mode, unsigned char rate);

//设置测量范围
void ADXL_SET_RANGE(unsigned char range);

//设置中断的有效电平
void ADXL_INT_VALID(unsigned char dat);

//使能中断
void ADXL_E_INT(unsigned char intp);

//禁止中断
void ADXL_D_INT(unsigned char intp);

//设置中断对应的中断引脚
void ADXL_INT_MAP(unsigned char sel, unsigned char intp);

//返回中断标志位
unsigned char ADXL_GET_IFG();

//获取三个轴的加速度值
void ADXL_GET_VALUE(int *x, int *y, int *z);

//计算加速度值
int ADXL_TRANSFER(unsigned char high, unsigned char low);

//计算角度值
void ADXL_GET_ANGLE(int *x, int *y, int *z);

//向ADXL345的某一个寄存器中写入数据
void ADXL_WRITE_BYTE(unsigned char reg, unsigned char data);

//从ADXL345的某一个寄存器中读取数据
unsigned char ADXL_READ_BYTE(unsigned char reg);

//从ADXL345中读取一个字节流
void ADXL_READ_STRING(unsigned char reg, unsigned char *pdata, unsigned int len);

//向ADXL345中写入一个字节流
void ADXL_WRITE_STRING(unsigned char reg, unsigned char *pdata, unsigned int len);

//显示轴向加速度
void ADXL_DISPLAY(int data,const unsigned char x, const unsigned char y);












/*****************************************************************************************************
*                                        SPI、I2C接口说明
******************************************************************************************************/

/*****变量定义*****/
#ifndef SPI_SIM_H_
static unsigned char IIC_SLAVE_ADDRESS=0xA6;    //IIC从机地址

#else
#endif

/********SPI IO函数定义*********/
#ifndef SPI_SIM_H_
//初始化IIC通信
void IIC_INIT();

//开始IIC传送数据
void IIC_START();

//结束IIC传送数据
void IIC_STOP();

//IIC发送一个字节
void IIC_TX(unsigned char data);

//IIC接收一个字节
unsigned char IIC_RX();

//发送一个ACK
void IIC_SEND_ACK(unsigned char data);

#else
//初始化SPI通信
void SPI_INIT();

//模拟SPI发送
void SPI_SIM_SEND(unsigned char data);

//模拟SPI接收
unsigned char SPI_SIM_RCV();
#endif

#endif
