/***************************************************
 *         Author: Shawn Guo                       *
 *         Date  : 2013/2/12                       *
 *         Last  : 2013/2/12                       *
 *         Notes : Nixie Tube * 8(-)               *
 *           P0 | J12                              *
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


void Delayms(unsigned int xms);
void Display();


int main()
{
    while(1)
    {
        Display();
    }

    return 0;
}


void Delayms(unsigned int xms)      // 毫秒级延时
{
    unsigned char i;

    while(xms--)            // --xms 与 xms-- 在汇编层次有差距,但是小心有一些Delayms(1)的语句，因此还是最好用xms--
        for(i = 0; i < 110; i++)
            ;
}

void Display()
{
    unsigned char i = 0;

    P2 = 0xff;   // 暂时全部高电平，数码管不亮
    for(i = 0; i < 8; i++){
        switch(i){  // 数码管位选  HC138
            case 0: HC138C = 0; HC138B = 0; HC138A = 0; break;
            case 1: HC138C = 0; HC138B = 0; HC138A = 1; break;
            case 2: HC138C = 0; HC138B = 1; HC138A = 0; break;
            case 3: HC138C = 0; HC138B = 1; HC138A = 1; break;
            case 4: HC138C = 1; HC138B = 0; HC138A = 0; break;
            case 5: HC138C = 1; HC138B = 0; HC138A = 1; break;
            case 6: HC138C = 1; HC138B = 1; HC138A = 0; break;
            case 7: HC138C = 1; HC138B = 1; HC138A = 1; break;
        }
        P0 = NUM_TABLE[i];
        Delayms(2);// 改变延时，可以观察三种情况
        //1. 延时太短，位选信号已换，段选信号残留
        //2. 延时恰当，实现动态扫描
        //3. 延时太长，动态扫描间隙太大
    }
}


