/***************************************************
 *         Author: Shawn Guo                       *
 *         Date  : 2013/2/12                       *
 *         Last  : 2013/2/12                       *
 *         Notes : 74HC165 并入串出锁存器          *
 *                 JP1 | P2(A B ... P2^7 P2^6)     *
 *                 JP5 | JP6(A B ... A B)          *
 *                 JP165短路帽短接                 *
 **************************************************/

#include <reg52.h>
#include <intrins.h>   // _nop_();


sbit RELAY = P1^4;

//74HC165 I/O
sbit    CLK    = P3^6;	   //串行时钟，若为上升沿且Shift， 则将最高位移出且移入INDAT对应的电平
sbit    SH_LD  = P1^6;    //Shift/Load 低电平载入数据，高电平为等待移位
sbit    INDAT = P1^7;    //数据通过P1.7脚移进单片机内处理


// Button
unsigned char bdata Key;  //bdata用于将变量定义在可位寻址片内数据存储区，允许位与字节混合访问。
sbit    K0 = Key^0;	 //位定义
sbit    K1 = Key^1;	 //位定义
sbit    K2 = Key^2;	 //位定义
sbit    K3 = Key^3;	 //位定义
sbit    K4 = Key^4;
sbit    K5 = Key^5;
sbit    K6 = Key^6;
sbit    K7 = Key^7;

void Delayms(unsigned int xms);
unsigned char Receive_HC165();


int main()
{
    unsigned char M0 = 0, K0J = 0;

    while(1)
    {
        P2 = 0xff;// 暂时熄灭所有LED灯
        RELAY = 1;  // 暂时关闭继电器
        Key = Receive_HC165(); // 接受新数据

        // KEY处理

        //实现脉冲输入,每次只有当K0被按下并松开之时会有脉冲反应
        if(K0 & K0J) M0 = ~M0;
        K0J = ~K0;

        if(M0)
        {
            RELAY = 0;    //实现脉冲输入
            P2 = 0x7f;
        }

        if(~K1) P2 = 0xfd;
        if(~K2) P2 = 0Xfb;
        if(~K3) P2 = 0Xf7;
        if(~K4) P2 = 0xef;
        if(~K5) P2 = 0xdf;
        if(!K6) P2 = 0xbf;
        if(~K7) P2 = 0x7f;
    }

    return 0;
}

unsigned char Receive_HC165()
{
    unsigned char i, dat;

    SH_LD = 0;  //低电平信号载入锁存器
    _nop_();    // 短暂延时，产生一定宽度的脉冲
    SH_LD = 1;  // 高电平，等待移位串出数据
    _nop_();

    dat = 0;
    for(i = 0; i < 8; i++)
    {
        dat = dat << 1; // 腾出最低位，准备接受新数据
        if(INDAT == 1) dat = dat + 1; //接受新数据

        CLK = 0;
        _nop_();
        CLK = 1;   //产生上升沿时钟信号，移位串出新数据值INDAT
    }

    return dat;

}

void Delayms(unsigned int xms)      // 毫秒级延时
{
    unsigned char i;

    while(--xms)            // --xms 与 xms-- 在汇编层次有差距
        for(i = 0; i < 110; i++)
            ;
}
