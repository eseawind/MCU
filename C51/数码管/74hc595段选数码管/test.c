/***************************************************
 *         Author: Shawn Guo                       *
 *         Date  : 2013/2/19                       *
 *         Last  : 2013/2/19                       *
 *         Notes : 74hc595段选数码管               *
 *           JP2 | J12(A B ... H G)                *
 *           JP595短路帽                           *
 *         Question: 数码管显示不清晰              *
 **************************************************/

#include <reg52.h>
#include <intrins.h>   // _nop_();

// 74HC595 I/O
sbit MISO = P3^4;
sbit R_CLK = P3^5;    // 上升沿并出
sbit S_CLK = P3^6;   // 上升沿数据左移一位，加入新数据


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

unsigned char COUNT0 = 0;             // Timer0 中断计数器

void Delayms(unsigned int xms);
void Display();
void HC595_SendData(unsigned char SendVal);
void Time_Init(); //定时器初始配置

int main()
{
    Time_Init();

    while(1)
    {
        LED[0] = NUM_TABLE[COUNT0 % 10000 / 1000];
        LED[1] = NUM_TABLE[COUNT0 % 1000 / 100];
        LED[2] = NUM_TABLE[COUNT0 % 100 / 10];
        LED[3] = NUM_TABLE[COUNT0 % 10];

        Display();
    }

    return 0;
}



void Time_Init() //定时器初始配置
{
    //定时器工作方式配置
    //             TIME1           TIME0
    // TMOD = GATE C/T M1 M0   GATE C/T M1 M0
    TMOD = 0x01; // Time0 选择模式1  16位计数

    //预装初值
    TH0 = (65536 - 50000) / 256;          // 50ms
    TL0 = (65536 - 50000) % 256;

    //中断允许寄存器 IE配置 EA - ET2 ES ET1 EX1 ET0 EX0
    IE = 0x8A;   // EA = 1, ET1 = 1, ET0 = 1
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
    static unsigned char tem = 0;

    TH0 = (65536 - 50000) / 256;          // 50ms
    TL0 = (65536 - 50000) % 256;

    if(++tem >= 20){         // 1s
        tem = 0;
        COUNT0++;
    }
}

void Display()
{
    unsigned char i = 0;

    HC595_SendData(0xff);// 暂时全部高电平，数码管不亮

    for(i = 0; i < 4; i++){
        switch(i){  // 数码管位选  HC138
            case 0: HC138C = 0; HC138B = 0; HC138A = 0; break;
            case 1: HC138C = 0; HC138B = 0; HC138A = 1; break;
            case 2: HC138C = 0; HC138B = 1; HC138A = 0; break;
            case 3: HC138C = 0; HC138B = 1; HC138A = 1; break;
        }
        HC595_SendData(LED[i]);
        Delayms(3);
    }
    HC595_SendData(0xff);// 暂时全部高电平，数码管不亮
}

void HC595_SendData(unsigned char SendVal)
{
    unsigned char i;

    for(i = 0; i < 8; i++) // 发送8字节
    {
        if((SendVal << i) & 0x80 ) MISO = 1;   // 取一位数据
        else MISO = 0;

        S_CLK = 0;
        _nop_();	//产生方形波
        _nop_();
        S_CLK = 1;	// 上升沿，移位寄存器全部移位，MOSIO的数据进去移位寄存器最低位
    }

    R_CLK = 0;
    _nop_();	//产生方形波
    _nop_();
    R_CLK = 1; //片选HC595,上升沿，移位寄存器的数据移入数据寄存器中,可以被其他器件读取
}
