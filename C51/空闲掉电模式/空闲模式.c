/***************************************************
 *         Author: Shawn Guo                       *
 *         Date  : 2013/2/14                       *
 *         Last  : 2013/2/14                       *
 *         Notes : 空闲模式(仅CPU休眠)            *
 *         P0 | J12(0 1 ... A B)                  *
 *         P3 | JP5(按钮触发外部中断)             *
 **************************************************/

#include <reg52.h>

// 74HC138        数码管位选控制
sbit HC138A = P2^2;
sbit HC138B = P2^3;
sbit HC138C = P2^4;

// Nixie Tube(-)     低电平位选数码管， 高电平段选数码条， 因此数码表取自Tube(-) ，并取反
// a b ... g dp (数码管段选接口)
// 8 7 ... 2 1  (对应电平接口)
//                                  0      1       2      3     4      5      6      7       8     9      A      b      C      d      E      F
unsigned char code NUM_TABLE[] = {~0x03, ~0x9f, ~0x25, ~0x0d, ~0x99, ~0x49, ~0x41, ~0x1f, ~0x01, ~0x09, ~0x11, ~0xc1, ~0x63, ~0x85, ~0x61, ~0x71};
unsigned int COUNT0;  //定时器中断响应计数器

void Delayms(unsigned int xms);
void Time_Init(); //定时器初始配置


int main()
{
    unsigned char i = 0, num = 0;
    unsigned char led[8];

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
                case 0: HC138C = 0; HC138B = 0; HC138A = 0; break;
                case 1: HC138C = 0; HC138B = 0; HC138A = 1; break;
                case 2: HC138C = 0; HC138B = 1; HC138A = 0; break;
                case 3: HC138C = 0; HC138B = 1; HC138A = 1; break;
            }
            Delayms(2);
        }
        if(COUNT0 >= 20)   // 1s
        {
            COUNT0 = 0;
            num++;
            if(num == 6)  // 在第六秒时操作
            {
                ET0 = 0;  // 屏蔽T0中断
                PCON = 0X02; // ... PD(掉电模式) IDL(空闲模式)
                //进去CPU休眠模式
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
    TMOD = 0x01; //选择Timer0模式1  16位计数

    //预装初值
    TH0 = (65536 - 50000) / 256;
    TL0 = (65536 - 50000) % 256;

    //中断允许寄存器 IE配置 EA - ET2 ES ET1 EX1 ET0 EX0
    IE = 0x87;   // 1 0 0 0 0 1 1 1; EA = 1, EX1 = 1, ET0 = 1, EX0 = 1
    TR0 = 1;    //开启T0定时器
}

void Delayms(unsigned int xms)      // 毫秒级延时
{
    unsigned char i;

    while(xms--)            // --xms 与 xms-- 在汇编层次有差距
        for(i = 0; i < 110; i++)
            ;
}

//定时器0 中断响应函数
void Timer0() interrupt 1             // 5ms
{
    TH0 = (65536 - 50000) / 256;
    TL0 = (65536 - 50000) % 256;

    COUNT0++;
}

void INT_0() interrupt 0
{
    //电源管理寄存器 PCON
    // SMOD                 SMOD0 LVDF P0F  GF1 GF0            PD        IDL
    // 串口通信波特率调节                   通用工作标志位     掉电模式  空闲模式
    PCON = 0;     // 掉电模式位消零
    ET0 = 1;      //恢复Timer0中断允许
}

void INT_1() interrupt 2
{
    PCON = 0;     // 掉电模式位消零
    ET0 = 1;      //恢复Timer0中断允许
}
