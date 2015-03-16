/********************************************************
 *         Author: Shawn Guo                            *
 *         Date  : 2013/2/15                            *
 *         Last  : 2013/2/15                            *
 *         Project:   LCD时钟                           *
 *            插入LCD1602， P1 | JP5(0 1 ... A B)       *
  * K1: 功能键 K5: 返回键 K2: 增加键 K6:减少键          *
 ********************************************************/

#include "LCD.h"
#include "key.h"
#include "time.h"

void Delayms(unsigned int xms);

int main()
{
    LCD_Init();
    Time_Init();

    while(1)
    {
        Key_Wait();
    }
    return 0;
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
