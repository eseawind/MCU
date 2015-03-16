/***************************************************
 *         Author: Shawn Guo                       *
 *         Date  : 2013/2/13                       *
 *         Last  : 2013/2/13                       *
 *         Notes : EEPROM  AT24C02                 *
 *         P0 | J12(0 1 ... A B)                   *
 *         P1 | JP5(0 1 ... 0 1)                   *																			   *
 *  K1: Save   K2: Load  K3:Add  K4: Subtracting   *
 **************************************************/

#include "I2C.h"
#include<reg52.h>

#define AT24C02 0xa0  //只有一个器件，器件地址3个均接地，因此这个AT24C02 地址 1010 0000

// 74HC138        数码管位选控制
sbit HC138A = P2^2;
sbit HC138B = P2^3;
sbit HC138C = P2^4;

// Nixie Tube(-)     低电平位选数码管， 高电平段选数码条， 因此数码表取自Tube(-) ，并取反
// a b ... g dp (数码管段选接口)
// 8 7 ... 2 1  (对应电平接口)
//                                  0      1       2      3     4      5      6      7       8     9      A      b      C      d      E      F
unsigned char code NUM_TABLE[] = {~0x03, ~0x9f, ~0x25, ~0x0d, ~0x99, ~0x49, ~0x41, ~0x1f, ~0x01, ~0x09, ~0x11, ~0xc1, ~0x63, ~0x85, ~0x61, ~0x71};
unsigned char COUNT1;  //计数器，放在定时器中断中处理，有效的防抖操作


//Button
sbit K1 = P1^0;	 //Save data
sbit K2 = P1^1;	 // Load data
sbit K3 = P1^2;	 // add data
sbit K4 = P1^3;   //subtracting data

void Delayms(unsigned int xms);
void Time_Init(); //定时器初始配置


int main()
{
    unsigned char i = 0;
    unsigned char led[8];

    Time_Init();

    while(1)
    {
        //不停的键盘检测
        if(K1 == 0)    //保存COUNT1至EEPROM，COUNT1为char型，1字节
        {
            Send_Str(AT24C02, 0, &COUNT1, 1);
        }

        if(K2 == 0)// EEPROM数据读取
        {
            Receive_Str(AT24C02, 0, &COUNT1, 1);    // 从AT24C02的第0个存储单元处开始连续读取8个字节；
            //注: 考虑到页地址上卷现象，一次最多读取8个字节

        }



        //数码管段选设置
        led[0] = NUM_TABLE[COUNT1 % 10000 / 1000]; //千位设置
        led[1] = NUM_TABLE[COUNT1 % 1000 / 100]; //百位设置
        led[2] = NUM_TABLE[COUNT1 % 100 / 10]; //十位设置
        led[3] = NUM_TABLE[COUNT1 % 10]; //个位设置

        //数码管位选设置
        for(i = 0; i < 4; i++)
        {
            P0 = led[i];//段选信号
            switch(i) // 位选信号
            {
            case 0:
                HC138C = 0;
                HC138B = 0;
                HC138A = 0;
                break;
            case 1:
                HC138C = 0;
                HC138B = 0;
                HC138A = 1;
                break;
            case 2:
                HC138C = 0;
                HC138B = 1;
                HC138A = 0;
                break;
            case 3:
                HC138C = 0;
                HC138B = 1;
                HC138A = 1;
                break;
            case 4:
                HC138C = 1;
                HC138B = 0;
                HC138A = 0;
                break;
            case 5:
                HC138C = 1;
                HC138B = 0;
                HC138A = 1;
                break;
            case 6:
                HC138C = 1;
                HC138B = 1;
                HC138A = 0;
                break;
            case 7:
                HC138C = 1;
                HC138B = 1;
                HC138A = 1;
                break;
            }
            Delayms(2);
        }

    }

    return 0;
}

void Time_Init() //定时器初始配置
{
    //定时器工作方式配置
    //             TIME1           TIME0
    // TMOD = GATE C/T M1 M0   GATE C/T M1 M0
    TMOD = 0x11; //选择模式1  16位计数

    //预装初值
    TH1 = 0xfe;
    TL1 = 0x33;

    //中断允许寄存器 IE配置 EA - ET2 ES ET1 EX1 ET0 EX0
    IE = 0x8A;   // EA = 1, ET1 = 1, ET0 = 1
    TR1 = 1;   //Start Timer1
}

void Delayms(unsigned int xms)      // 毫秒级延时
{
    unsigned char i;

    while(--xms)            // --xms 与 xms-- 在汇编层次有差距
        for(i = 0; i < 110; i++)
            ;
}

//定时器1 中断响应函数
void Timer1() interrupt 3
{
    static unsigned char te = 0;
    TH1 = 0xfe;               // 0.5ms
    TL1 = 0x33;
    if(++te >= 200)
    {
        te = 0;
        if(!K3) COUNT1++;                   // 100ms检测一次键盘，有效的防抖操作
        if(!K4 && COUNT1 != 0) COUNT1--;
    }
}
