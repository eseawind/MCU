/***************************************************
 *         Author: Shawn Guo                       *
 *         Date  : 2013/2/12                       *
 *         Last  : 2013/2/12                       *
 *         Notes : INT0 + INT1                     *
 *          P0 | JP12(0 1 ... H G)                 *
 *          P3 | JP5 (0 1 ... A B)                 *
 *            J1短路帽断开，避免红外遥控中断影响  *
 *  一个很简陋的外部中断计数器，甚至没有防抖功能  *
 *    后续: 加入防抖功能，真正实现外部中断计数器  *
 **************************************************/

#include <reg52.h>
#include <intrins.h>   // _nop_();

// 74HC138        数码管位选控制
sbit HC138A = P2^2;
sbit HC138B = P2^3;
sbit HC138C = P2^4;

// Nixie Tube(-)     低电平位选数码管， 高电平段选数码条， 因此数码表取自Tube(-) ，并取反
// a b ... g dp (数码管段选接口)
// 8 7 ... 2 1  (对应电平接口)
//                                  0      1       2      3     4      5      6      7       8     9      A      b      C      d      E      F
unsigned char code NUM_TABLE[] = {~0x03, ~0x9f, ~0x25, ~0x0d, ~0x99, ~0x49, ~0x41, ~0x1f, ~0x01, ~0x09, ~0x11, ~0xc1, ~0x63, ~0x85, ~0x61, ~0x71};
unsigned int COUNT0 = 0, COUNT1 = 0; //中断响应计数器

void Delayms(unsigned int xms);


int main()
{
    unsigned char i = 0;
    unsigned char led[8];

    P0 = 0xff;  // 初始配置(大可不必，一般无关位单片机加电后都自动置高电平)
    P1 = 0xff;
    P2 = 0xff;

    //外部中断0 配置
    //中断触发方式配置 IT0 = 0，低电平触发方式
    IT0=1;       //IT0 = 1，下降沿触发
    EX0=1; //开启外部中断0 中断允许



    //外部中断1 配置
    //中断触发方式配置 IT1 = 0，低电平触发方式
    IT1=1;        //IT1 = 1，下降沿触发
    EX1=1; //开启外部中断1 中断允许

    //开启全局中断允许位(中断基本前提)
    EA=1;

    while(1)
    {
        //数码管段选设置
        led[0] = NUM_TABLE[COUNT0 % 10000 / 1000]; //千位设置
        led[1] = NUM_TABLE[COUNT0 % 1000 / 100]; //百位设置
        led[2] = NUM_TABLE[COUNT0 % 100 / 10]; //十位设置
        led[3] = NUM_TABLE[COUNT0 % 10]; //个位设置

        led[4] = NUM_TABLE[COUNT1 % 10000 / 1000]; //千位设置
        led[5] = NUM_TABLE[COUNT1 % 1000 / 100]; //百位设置
        led[6] = NUM_TABLE[COUNT1 % 100 / 10]; //十位设置
        led[7] = NUM_TABLE[COUNT1 % 10]; //个位设置

        //数码管位选设置
        for(i = 0; i < 8; i++)
        {
            P0 = led[i];//段选信号
            switch(i) // 位选信号
            {
                case 0: HC138C = 0; HC138B = 0; HC138A = 0; break;
                case 1: HC138C = 0; HC138B = 0; HC138A = 1; break;
                case 2: HC138C = 0; HC138B = 1; HC138A = 0; break;
                case 3: HC138C = 0; HC138B = 1; HC138A = 1; break;
                case 4: HC138C = 1; HC138B = 0; HC138A = 0; break;
                case 5: HC138C = 1; HC138B = 0; HC138A = 1; break;
                case 6: HC138C = 1; HC138B = 1; HC138A = 0; break;
                case 7: HC138C = 1; HC138B = 1; HC138A = 1; break;
            }
            Delayms(2);
        }

    }

    return 0;
}

void Delayms(unsigned int xms)      // 毫秒级延时
{
    unsigned char i;

    while(--xms)            // --xms 与 xms-- 在汇编层次有差距
        for(i = 0; i < 110; i++)
            ;
}

// 外部中断0
void  Counter0() interrupt 0  using 1 //使用单片机内存4组工作寄存器中的1号
{

   EX0 = 0;  // 暂时屏蔽外部中断0

   COUNT0++;  //中断响应计数器

   EX0 = 1;  // 解除屏蔽
}

//外部中断1
void  Counter1() interrupt 2 using 2//使用单片机内存4组工作寄存器中的2号
{

   EX1 = 0; // 暂时屏蔽外部中断1

   COUNT1++;//中断响应计数器

   EX1 = 1;// 解除屏蔽
}
