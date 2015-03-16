/***************************************************
 *         Author: Shawn Guo                       *
 *         Date  : 2013/2/12                       *
 *         Last  : 2013/2/12                       *
 *         Notes : Nixie Tube(+)                   *
 *           P2 | JP3                              *
 **************************************************/

#include <reg52.h>

// Nixie Tube(+)
// a b ... g dp (数码管段选接口)
// 8 7 ... 2 1  (对应电平接口)
//                                 0      1    2     3     4      5    6      7     8    9     A      b     C     d     E    F
unsigned char code NUM_TABLE[] = {0x03, 0x9f, 0x25, 0x0d, 0x99, 0x49, 0x41, 0x1f, 0x01, 0x09, 0x11, 0xc1, 0x63, 0x85, 0x61, 0x71};


void Delayms(unsigned int xms);

int main()
{
    unsigned char i = 0;

    while(1)
    {
        for(i = 0; i < 16; i++)
        {
            P2 = NUM_TABLE[i];
            Delayms(1000);
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


