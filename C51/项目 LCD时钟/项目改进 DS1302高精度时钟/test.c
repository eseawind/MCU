/********************************************************
 *         Author: Shawn Guo                            *
 *         Date  : 2013/2/15                            *
 *         Last  : 2013/2/15                            *
 *         Project:   LCD高精度时钟                     *
 *            插入LCD1602， P1 | JP5(0 1 ... A B)       *
 * K1: 功能键 K5: 返回键 K2: 增加键 K6:减少键 K3:闹铃键*
 *               P3^0 | BEEP                            *
 ********************************************************/

#include "LCD.h"
#include "key.h"
#include "time.h"

extern char YEAR1, YEAR2, MONTH, DAY, WEEK, HOUR, MINUTE, SECOND;

void Delayms(unsigned int xms);

int main()
{
    // Time_Save(); // 第一次使用时，初始化ds1302内存
    Time_Init();
    LCD_Init();


    while(1)
        Key_Wait();

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
