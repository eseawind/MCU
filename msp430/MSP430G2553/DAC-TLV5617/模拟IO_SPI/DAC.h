/**********************************************
 *
 * 		Author	:		Shawn Guo
 * 		Date	:		2013/4/21
 *      Last    :       2013/4/21
 * 		Tool    :	    MSP430G2553
 * 		Notes	:		TLV5617 DAC驱动
 *
 *      1.SDIN  2.SCLK  3.CS    4.OUTA
 *      5.AGND  6.REF   7.OUTB  8.VDD
 *---------------------------------------
 *
 *          SDIN---->P2.3
 *          SCLK---->P2.4
 *          CE  ---->P2.5
 *---------------------------------
 *        掉电模式:             dat |= 0x2000;
 *        快速模式：            dat |= 0x4000;
 *        慢速模式(默认)：      dat &= ~0x4000;
 *
 *
 **********************************************/

#ifndef __LCD5110__            // 头文件保护策略
#define __LCD5110__

// =======================DAC I/O端口宏定义设置===========================
#define  DAC_CE_1       (P2OUT |= BIT5)
#define  DAC_CE_0       (P2OUT &=~BIT5)      // 低电平有效使能

#define  DAC_SDIN_1         (P2OUT |= BIT3)
#define  DAC_SDIN_0         (P2OUT &=~BIT3)   // 传输数据高位在前

#define  DAC_SCLK_1         (P2OUT |= BIT4)
#define  DAC_SCLK_0         (P2OUT &=~BIT4)


//===========================
#define  DAC_FAST_MODE(DAT) (DAT |= 0x4000)     //快速模式

#define  DAC_SLOW_MODE(DAT) (DAT &= ~0X4000)    //慢速模式

#define  DAC_POWEROFF(DAT)  (DAT |= 0x2000)     //掉电模式

// ==============================DAC函数====================
void DAC_Init();       //DAC器件初始化
void DAC_Sent_Bytes(unsigned int dat);// 模拟SPI总线向TLV5617发送数据

//发送数据至OUTA端, 且用缓冲器的内容更新OUTB， mode = 1快速模式
void DAC_Sent_A(unsigned int dat, unsigned char mode);

void DAC_Sent_B(unsigned int dat, unsigned char mode);  //发送数据至OUTB端, mode = 1快速模式

//发送dat1至OUTA端，发送dat2至outb端,mode = 1快速模式
void DAC_Sent_AB(unsigned int dat1, unsigned char mode1, unsigned int dat2,  unsigned char mode2);

void DAC_PowerOff();   //进入掉电模式

 // 将制定电压转换为DAC对应的代码dat，错误的v将返回0x0000；
 // 返回数据范围: 0x0000~ 0x0ffc,且5617仅取11 - 2这十位数据，最后两位无效
 // v = 2 * DAC_REF * (CODE / 1024); 最后CODE左移两位，以满足TLV5617A的数据格式
unsigned int DAC_V2Code(double v);

#endif
