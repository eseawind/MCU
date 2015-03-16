/***************************************************
 *         Author: Shawn Guo                       *
 *         Date  : 2013/2/12                       *
 *         Last  : 2013/2/12                       *
 *         Notes : Ralay                           *
 *          P1.4短路帽短接                         *
 **************************************************/

#include <reg52.h>

sbit RALAY = P1^4;

void Delayms(unsigned int xms);
void Ralay();

int main()
{
    while(1)
    {
        Ralay();
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

void Ralay()
{
    RALAY = 0;           // 低电平接通继电器
    Delayms(2000);
    RALAY = 1;
    Delayms(2000);
}

