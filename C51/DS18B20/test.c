/********************************************************
 *         Author: Shawn Guo                            *
 *         Date  : 2013/2/18                            *
 *         Last  : 2013/2/18                            *
 *         Note:   DS18B20温度传感器                    *
 *                P0 | J12(0 1 ... A B)                 *
 ********************************************************/

#include <reg52.H>
#include <intrins.h>       // _nop_();
#include "temp.h"

// 74HC138        数码管位选控制
sbit HC138A = P2^2;
sbit HC138B = P2^3;
sbit HC138C = P2^4;
// Nixie Tube(-)     低电平位选数码管， 高电平段选数码条， 因此数码表取自Tube(-) ，并取反
// a b ... g dp (数码管段选接口)
// 8 7 ... 2 1  (对应电平接口)
//                                  0      1       2      3     4      5      6      7       8     9      A      b      C      d      E      F
unsigned char code NUM_TABLE[] = {~0x03, ~0x9f, ~0x25, ~0x0d, ~0x99, ~0x49, ~0x41, ~0x1f, ~0x01, ~0x09, ~0x11, ~0xc1, ~0x63, ~0x85, ~0x61, ~0x71};
unsigned char LED[7];

extern unsigned int  idata TEMPERATURE;
extern unsigned char idata TIM;  // Time1 中断响应计时器

extern void GetTemp();  //每间隔一段时间，读取一次温度
void Delayms(unsigned int xms);
void Time_Init();         // 配置定时器Timer1, 一定时间读取一次温度

int main()
{
    unsigned char i;

    Time_Init();

    while(1)
    {
        GetTemp();        // 固定时间间隔，读取一次温度数据

        // 数码管段选信号配置
        LED[0] = NUM_TABLE[TEMPERATURE % 10000 / 1000];
        LED[1] = NUM_TABLE[TEMPERATURE % 1000 / 100];
        LED[2] = NUM_TABLE[TEMPERATURE % 100 / 10];	 //十位
        LED[3] = NUM_TABLE[TEMPERATURE % 10];    //个位

        for(i = 0; i < 4; i++)
        {
            P0 = LED[i] ;

            switch(i)
            {
            case 0:
                HC138A=0;
                HC138B=0;
                HC138C=0;
                break;
            case 1:
                HC138A=1;
                HC138B=0;
                HC138C=0;
                break;
            case 2:
                HC138A=0;
                HC138B=1;
                HC138C=0;
                break;
            case 3:
                HC138A=1;
                HC138B=1;
                HC138C=0;
                break;
            }
            Delayms(2);
        }
        P0 = 0;
    }

    return 0;
}


void Time_Init()
{
    //定时器工作方式配置
    //             TIME1           TIME0
    // TMOD = GATE C/T M1 M0   GATE C/T M1 M0
    TMOD |= 0x10;  //Timer1选择模式1  16位计时

    TH1 = 0xD8;    //10
    TL1 = 0xF0;

    //中断允许寄存器 IE配置 EA - ET2 ES ET1 EX1 ET0 EX0
    IE = 0x88;   // EA = 1, ET1 = 1

    TR1  = 1;
}

void Delayms(unsigned int xms)
{
    unsigned char i;

    while(xms--)
    {
        for(i = 0; i < 110; i++)
            ;
    }
}

void Timer1() interrupt 3
{
    TH1 = 0xD8;    //10
    TL1 = 0xF0;

    TIM++;         // Timer1中断响应计时器

}
