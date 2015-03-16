/********************************************************
 *         Author: Shawn Guo                            *
 *         Date  : 2013/2/14                            *
 *         Last  : 2013/2/14                            *
 *         Project:   LCD时钟                           *
 *         Notes :  独立键盘驱动                        *
 *                  P1 | JP5(0 1 ... K1 K2)               *
  * K1: 功能键 K5: 返回键 K2: 增加键 K6:减少键 K3:闹铃键*
 ********************************************************/

#include <reg52.h>
#include "key.h"


extern void Delayms(unsigned int xms);
extern void LCD_Op1();     // 功能键操作
extern void LCD_Op2();     // 增加键操作
extern void LCD_Display();        // 显示数据
extern void LCD_Op3();     // 闹铃键操作
extern void LCD_Op5();     // 返回键操作
extern void LCD_Op6();     // 减少键操作
extern unsigned char POS;   // 功能键的位置，0为初始值

// button
sbit K1 = P1^0;     // 功能键
sbit K2 = P1^1;     // 增加键
sbit K3 = P1^2;     // 闹铃设置键
sbit K4 = P1^3;
sbit K5 = P1^4;     // 返回键
sbit K6 = P1^5;     // 减少键
sbit K7 = P1^6;
sbit K8 = P1^7;

void Key_Wait()          // 等待按键
{
    P1 = 0xff;           // 置高电平
    if(P1 != 0xff)
    {
        Delayms(5);   // 延时操作，防抖
        if(P1 != 0xff)   // 确实有键被按下
        {
            Key_Test();
        }
    }
}

void Key_Test()          // 检测是哪个按键，并进行相应的操作
{
    if(P1 == 0xfe)   // 1111 1110, K1被按下，功能键
    {
        if(POS >= 8)  // 闹钟键转功能键
        {
            LCD_Display();
            POS = 0;
        }
        TR0 = 0;     // 暂时停止时钟，检测按键
        while(!K1);  // 等待松键
        LCD_Op1();   //相应操作
        return;
    }
    else if(P1 == 0xfd)   // 1111 1101, K2被按下，增加键
    {
        if(POS == 0) // 没有选择功能键或闹钟键，而按下增加键，则按键无效
            return;
        while(!K2);  // 等待松键
        LCD_Op2();   //相应操作
        return;
    }
    else if(P1 == 0xfb) // 1111 1011, K3被按下，闹铃设置键
    {
        TR0 = 0;     // 暂时停止时钟，检测按键
        while(!K3);  // 等待松键
        LCD_Op3();   //相应操作
        return;
    }
    else if(P1 == 0xef)   // 1110 1111, K5被按下，返回键
    {
        if(POS == 0) // 没有选择功能键或闹钟键，而按下返回键，则按键无效
            return;
        while(!K5);  // 等待松键
        LCD_Op5();   //相应操作
        return;
    }
    else if(P1 == 0xdf)   // 1101 1111, K6被按下，减少键
    {
        if(POS == 0) // 没有选择功能键或闹钟键，而按下减少键，则按键无效
            return;
        while(!K6);  // 等待松键
        LCD_Op6();   //相应操作
        return;
    }
}
