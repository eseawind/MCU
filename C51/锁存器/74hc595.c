/***************************************************
 *         Author: Shawn Guo                       *
 *         Date  : 2013/2/12                       *
 *         Last  : 2013/2/12                       *
 *         Notes : 74HC595 串入并出锁存器          *
 *                 JP2 | JP1(A B ... A B)          *
 **************************************************/

#include <reg52.h>
#include <intrins.h>   // _nop_();

// 74HC595 I/O
sbit MISO = P3^4;
sbit R_CLK = P3^5;    // 上升沿并出
sbit S_CLK = P3^6;   // 上升沿数据左移一位，加入新数据

void Delayms(unsigned int xms);
void HC595_SendData(unsigned char SendVal);


int main()
{
    unsigned char led = 0xfe;

    while(1)
    {
        HC595_SendData(led);
        Delayms(500);
        led = (led << 1) | 0x01;
        if(led == 0xff) led = 0xfe;
    }

    return 0;
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

void Delayms(unsigned int xms)      // 毫秒级延时
{
    unsigned char i;

    while(--xms)            // --xms 与 xms-- 在汇编层次有差距
        for(i = 0; i < 110; i++)
            ;
}
