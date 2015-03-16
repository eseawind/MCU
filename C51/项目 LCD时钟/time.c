/********************************************************
 *         Author: Shawn Guo                            *
 *         Date  : 2013/2/15                            *
 *         Last  : 2013/2/15                            *
 *         Project:   LCD时钟                           *
 *         Notes:   时钟驱动，将数据保存在AT24C02中    *
 ********************************************************/

#include <reg52.h>
#include <intrins.h> //_nop_();
#include "time.h"
#include "I2C.h"

#define AT24C02 0xa0  //只有一个器件，器件地址3个均接地，因此这个AT24C02 地址 1010 0000

extern char YEAR1, YEAR2, MONTH, DAY, WEEK, HOUR, MINUTE, SECOND;  // 年月日周时分秒
extern void LCD_Display();        // 显示数据
extern void Delayms(unsigned int xms);

void Time_Load()         // 载入时间数据
{
    Receive_Str(AT24C02, 0, &YEAR1, 1);
    Receive_Str(AT24C02, 1, &YEAR2, 1);
    Receive_Str(AT24C02, 2, &MONTH, 1);
    Receive_Str(AT24C02, 3, &DAY, 1);
    Receive_Str(AT24C02, 4, &WEEK, 1);
    Receive_Str(AT24C02, 5, &HOUR, 1);
    Receive_Str(AT24C02, 6, &MINUTE, 1);
    Receive_Str(AT24C02, 7, &SECOND, 1);
}

void Time_Save()         // 保存时间数据
{
    Send_Str(AT24C02, 0, &YEAR1, 1);       // 要有足够的间隔时间去等待AT24C02接受数据
    Delayms(2);
    Send_Str(AT24C02, 1, &YEAR2, 1);
    Delayms(2);
    Send_Str(AT24C02, 2, &MONTH, 1);
    Delayms(2);
    Send_Str(AT24C02, 3, &DAY, 1);
    Delayms(2);
    Send_Str(AT24C02, 4, &WEEK, 1);
    Delayms(2);
    Send_Str(AT24C02, 5, &HOUR, 1);
    Delayms(2);
    Send_Str(AT24C02, 6, &MINUTE, 1);
    Delayms(2);
    Send_Str(AT24C02, 7, &SECOND, 1);
    Delayms(2);
}

void Time_Init() //定时器初始配置
{
    //定时器工作方式配置
    //             TIME1           TIME0
    // TMOD = GATE C/T M1 M0   GATE C/T M1 M0
    TMOD = 0x01; //选择Timer0 模式1  16位定时

    //预装初值
    TH0 = (65536 - 50000) / 256;             // 50ms定时
    TL0 = (65536 - 50000) % 256;

    //中断允许寄存器 IE配置 EA - ET2 ES ET1 EX1 ET0 EX0
    IE = 0x82;   // EA = 1, ET0 = 1
    TR0 = 1;    //开启T0定时器
}

void Timer0() interrupt 1
{
    static unsigned char num = 0;

    TH0 = (65536 - 50000) / 256;             // 50ms定时
    TL0 = (65536 - 50000) % 256;

    if(++num >= 20)          // 1s
    {
        num = 0;
        if(++SECOND >= 60)
        {
            SECOND = 0;
            if(++MINUTE >= 60)
            {
                MINUTE = 0;
                if(++HOUR >= 24)
                {
                    HOUR = 0;
                    if(++DAY >= 32)
                    {
                        DAY = 1;
                        if(++MONTH >= 13)
                        {
                            MONTH = 1;
                            if(++YEAR1 >= 100)
                            {
                                YEAR1 = 0;
                                YEAR2++;
                                Send_Str(AT24C02, 1, &YEAR2, 1);
                            }
                            Send_Str(AT24C02, 0, &YEAR1, 1);

                        }
                        Send_Str(AT24C02, 2, &MONTH, 1);
                    }
                    if(++WEEK >= 8)
                        WEEK = 1;
                    Send_Str(AT24C02, 4, &WEEK, 1);
                    Send_Str(AT24C02, 3, &DAY, 1);
                }
                Send_Str(AT24C02, 5, &HOUR, 1);
            }
            Send_Str(AT24C02, 6, &MINUTE, 1);
        }
        Send_Str(AT24C02, 7, &SECOND, 1);
        LCD_Display();
    }

}
