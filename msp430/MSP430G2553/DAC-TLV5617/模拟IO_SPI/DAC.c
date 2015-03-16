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
 *          CD  ---->P2.5
 *
 **********************************************/
#include <msp430g2553.h>
#include "DAC.h"

const double DAC_VDD = 3.3;// 电源VDD与基准源REF配置
const double DAC_REF = 1;

void DAC_Init()       //DAC器件初始化
{
    // DAC I/O口配置
    P2DIR |= BIT3 + BIT4 + BIT5; //P2.3 P2.4 P2.5口设置为输出方向
    P2OUT |= BIT3 + BIT4 + BIT5;
}

void DAC_Sent_Bytes(unsigned int dat)// 模拟SPI总线向TLV5617发送数据
{
    unsigned char i = 0;

    DAC_SDIN_1;
    DAC_SCLK_1;   //拉高时钟信号
    DAC_CE_0;//拉低使能信号，片选TLV5617,待数据发送完成后再拉高CE信号

    for(i = 0; i < 16; i++)  //发送16字节的数据,从最高位开始
    {
        if(dat & 0x8000) DAC_SDIN_0;
        else DAC_SDIN_1;

        DAC_SCLK_1;
        dat <<= 1;
        _NOP();
        DAC_SCLK_0;   //拉低SCLK，数据移入DAC的转移寄存器中
    }

    DAC_CE_1; //拉高使能信号，使得转移寄存器中的数据移入目标锁存
    DAC_SDIN_1;
    DAC_SCLK_1;   //拉高时钟信号
}

void DAC_PowerOff()   //进入掉电模式
{
    unsigned int dat = 0x2000;
    DAC_Sent_Bytes(dat);
}


void DAC_Sent_A(unsigned int dat, unsigned char mode)  //发送数据至OUTA端, 且用缓冲器的内容更新OUTB， mode = 1快速模式
{
    if(mode == 1) DAC_FAST_MODE(dat);
    dat |= 0x8000;
    dat &= ~0x1000;
    DAC_Sent_Bytes(dat);
}

void DAC_Sent_B(unsigned int dat, unsigned char mode)  //发送数据至OUTB端, mode = 1快速模式
{
    if(mode == 1) DAC_FAST_MODE(dat);
    dat &= ~0x8000;
    dat &= ~0x1000;
    DAC_Sent_Bytes(dat);
}
//发送dat1至OUTA端，发送dat2至outb端,mode = 1快速模式
void DAC_Sent_AB(unsigned int dat1, unsigned char mode1, unsigned int dat2,  unsigned char mode2)
{
    //第一步：为DAC_B写入数据至缓冲器
    if(mode2 == 1) DAC_FAST_MODE(dat2);
    dat2 &= ~0x8000;
    dat2 |= 0x1000;
    DAC_Sent_Bytes(dat2);

    if(mode1 == 1) DAC_FAST_MODE(dat1);
    dat1 |= 0x8000;
    dat1 &= ~0x1000;
    DAC_Sent_Bytes(dat1);
}

 // 将制定电压转换为DAC对应的代码dat，错误的v将返回0x0000；
 // 返回数据范围: 0x0000~ 0x0ffc,且5617仅取11 - 2这十位数据，最后两位无效
 // v = 2 * DAC_REF * (CODE / 1024); 最后CODE左移两位，以满足TLV5617A的数据格式
unsigned int DAC_V2Code(double v)
{
    unsigned int dat = 0;

    if(v >= 2 * DAC_REF || v < 0) return 0;

    dat = (int)(v * 512 / DAC_REF);
    dat <<= 2;

    return dat;
}


