/********************************************************
 *         Author: Shawn Guo                            *
 *         Date  : 2013/2/14                            *
 *         Last  : 2013/2/14                            *
 *         Notes : 利用ISP/IAP控制寄存器实现软件复位   *
 *                 P0 | J12(0 1 ... A B)                *
 ********************************************************/

#include <reg52.h>

sfr ISP_CONTR = 0XE7;

// 74HC138        数码管位选控制
sbit HC138A = P2^2;
sbit HC138B = P2^3;
sbit HC138C = P2^4;

// Nixie Tube(-)     低电平位选数码管， 高电平段选数码条， 因此数码表取自Tube(-) ，并取反
// a b ... g dp (数码管段选接口)
// 8 7 ... 2 1  (对应电平接口)
//                                  0      1       2      3     4      5      6      7       8     9      A      b      C      d      E      F
unsigned char code NUM_TABLE[] = {~0x03, ~0x9f, ~0x25, ~0x0d, ~0x99, ~0x49, ~0x41, ~0x1f, ~0x01, ~0x09, ~0x11, ~0xc1, ~0x63, ~0x85, ~0x61, ~0x71};
unsigned int COUNT0; //定时器中断响应计数器

void Delayms(unsigned int xms);
void Time_Init(); //定时器初始配置


int main()
{
    unsigned char i = 0;
    unsigned char led[8];
    unsigned char num = 0; //  秒数，最后显示在数码管上

    Time_Init();

    while(1)
    {
        //数码管段选设置
        led[0] = NUM_TABLE[num % 10000 / 1000]; //千位设置
        led[1] = NUM_TABLE[num % 1000 / 100]; //百位设置
        led[2] = NUM_TABLE[num % 100 / 10]; //十位设置
        led[3] = NUM_TABLE[num % 10]; //个位设置

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
            }
            Delayms(2);
        }

        if(COUNT0 >= 20) // 1s
        {
            COUNT0 = 0;
            num++;
            if(num == 10)   // 第10秒时进行操作
            {
                //ISP_CONTR: ISPEN                       SWBS                                              SWRST                                      - - WT2 WT1 WT0
                //        是否允许编程改变系统Flash区域  选择从AP用户程序去启动(0)，还是从ISP程序区启动(1) 高电平时产生软件系统复位，然后硬件自动清零      设定CPU等待最长时间
                ISP_CONTR = 0X20;  // 0 0 1 00 000  , 启动软件自动复位
            }
        }

    }

    return 0;
}

void Time_Init() //定时器初始配置
{
    //定时器工作方式配置
    //             TIME1           TIME0
    // TMOD = GATE C/T M1 M0   GATE C/T M1 M0
    TMOD = 0x01; //选择Timer0 模式1  16位计数

    //预装初值
    TH0 = (65536 - 50000) / 256;        // 5ms
    TL0 = (65536 - 50000) % 256;

    //中断允许寄存器 IE配置 EA - ET2 ES ET1 EX1 ET0 EX0
    IE = 0x82;   // EA = 1,ET0 = 1

    TR0 = 1;    //开启T0定时器
}

void Delayms(unsigned int xms)      // 毫秒级延时
{
    unsigned char i;

    while(xms--)            // --xms 与 xms-- 在汇编层次有差距,但是小心有一些Delayms(1)的语句，因此还是最好用xms--
        for(i = 0; i < 110; i++)
            ;
}

//定时器0 中断响应函数
void Timer0() interrupt 1
{
    TH0 = (65536 - 50000) / 256;        // 5ms
    TL0 = (65536 - 50000) % 256;

    COUNT0++;     // Timer0 中断响应计数器
}
