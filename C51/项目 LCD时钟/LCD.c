/********************************************************
 *         Author: Shawn Guo                            *
 *         Date  : 2013/2/15                            *
 *         Last  : 2013/2/15                            *
 *         Project:   LCD时钟                           *
 *         Notes :  LCD驱动                             *
 ********************************************************/


#include <reg52.h>
#include <intrins.h> //_nop_();
#include "LCD.h"
#include "I2C.h"

extern unsigned char DISPLAY1[];
extern unsigned char DISPLAY2[];

extern void Delayms(unsigned int xms);
extern void Time_Load();         // 载入时间数据
extern void Time_Save();         // 保存时间数据

// LCD1602 接口
sbit LCD_RS = P2^6;  //复位端, 高电平数据操作，低电平命令操作
sbit LCD_RW = P2^5;	 //写数据端，高电平读取，低电平写入
sbit LCD_EN = P2^7;	 //使能端，高电平时能读取，高脉冲写入
//  LCD1602 数据
unsigned char POS = 0;   // 功能键的位置，0为初始值
char YEAR1 = 20, YEAR2 = 13, MONTH = 2, DAY = 15, WEEK = 5, HOUR = 12, MINUTE = 00, SECOND = 00;  // 年月日周时分秒

bit LCD_IsBusy()   // 检测LCD是否繁忙
{
    bit result;

    LCD_RS = 0;       //读取状态字，状态字输出至D0~D7供CPU读取
    LCD_RW = 1;
    LCD_EN = 1;
    _nop_();
    _nop_();
    _nop_();
    _nop_();

    //状态字 STA7 STA6 STA5 STA4 STA3 STA2 STA1 STA0
    // STA7：读写操作使能，高电平表示繁忙，禁止读写操作；低电平表示空闲
    result = (bit)(P0 & 0x80);	 //读取STA7

    LCD_EN = 0; //结束使能片选

    return result;//返回侧忙结果
}

void LCD_W_Cmd(unsigned char cmd)// 写入指令数据到LCD
{

    while(LCD_IsBusy());   //等待LCD1602空闲

    LCD_RS = 0;
    LCD_RW = 0;   //写指令

    LCD_EN = 0;  //拉低，准备发射高脉冲送入命令
    _nop_();
    _nop_();

    P0 = cmd; //将8位指令通过P0口传给1602
    _nop_();
    _nop_(); //用于产生一个脉冲宽度
    _nop_();
    _nop_();
    LCD_EN = 1;   //高脉冲送入命令
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    LCD_EN = 0;
}

void LCD_Pos(unsigned char pos) //设定LCD数据指针
{
    //数据指针设置 80H + (0~27H, 40~67H)
    LCD_W_Cmd(pos | 0x80);
}

void LCD_W_Data(unsigned char dat)  //写入字符显示数据到LCD
{
    while(LCD_IsBusy());   //等待LCD1602空闲

    LCD_RS = 1;
    LCD_RW = 0;   //写数据操作

    LCD_EN = 0;   //拉低，准备发射高脉冲送入数据

    P0 = dat;  //将8位数据通过P0口传给1602
    _nop_();
    _nop_();   //用于产生一个脉冲宽度
    _nop_();
    _nop_();
    LCD_EN = 1;   //高脉冲送入数据至LCD
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    LCD_EN = 0;
}

void LCD_Init()    //LCD初始化设定
{
    unsigned char i = 0;

    //显示模式设置 0011 1000， 16*2显示，5*7点阵，8位数据接口
    LCD_W_Cmd(0x38);
    Delayms(1);

    //0000 1           D        C       B
    LCD_W_Cmd(0x0c);//显示on，光标off,  光标闪烁off
    Delayms(1);

    //0000 01          N                   S
    LCD_W_Cmd(0x06);//读写操作后指针加/减  写入字符后整屏是否左移
    Delayms(1);

    // 01H 数据指针清零，显示清零        02H 数据指针清零
    LCD_W_Cmd(0x01);     //清除LCD的显示内容
    Delayms(10);

    Time_Load(); // 载入时间数据
    //显示数据
    LCD_Display();
}


void LCD_Op1()     // 功能键操作
{
    //0000 1           D        C       B
    LCD_W_Cmd(0x0f);//显示on，光标on,  光标闪烁on
    Delayms(1);

    if(++POS >= 8)
        POS = 1;
    //移动光标至相应位置
    switch(POS)
    {
    case 1:
        LCD_Pos(0x04);
        break;       // 年
    case 2:
        LCD_Pos(0x07);
        break;       // 月
    case 3:
        LCD_Pos(0x0a);
        break;       // 日
    case 4:
        LCD_Pos(0x0e);
        break;       // 周
    case 5:
        LCD_Pos(0x42);
        break;       // 时
    case 6:
        LCD_Pos(0x45);
        break;       // 分
    case 7:
        LCD_Pos(0x48);
        break;       // 秒
    }
}
void LCD_Op2()     // 增加键操作,人为操作，因此不考虑进位
{
    switch(POS)
    {
    case 1:  // 年份增加
        if(++YEAR2 >= 100)
        {
            YEAR2 = 0;
            YEAR1++;
            LCD_Set(0x01, YEAR1);
        }
        LCD_Set(0x03, YEAR2);
        break;
    case 2:   // 月份增加
        if(++MONTH >= 13)
            MONTH = 1;
        LCD_Set(0x06, MONTH);
        break;
    case 3:   // 日份增加
        if(++DAY >= 32)
            DAY = 1;
        LCD_Set(0x09, DAY);
        break;
    case 4:   // 周份增加
        if(++WEEK >= 8)
            WEEK = 1;
        LCD_Set_Week(WEEK);
        break;
    case 5:   // 时钟增加
        if(++HOUR >= 24)
            HOUR = 0;
        LCD_Set(0x41, HOUR);
        break;
    case 6:  // 分钟增加
        if(++MINUTE >= 60)
            MINUTE = 0;
        LCD_Set(0x44, MINUTE);
        break;
    case 7: // 秒针增加
        if(++SECOND >= 60)
            SECOND = 0;
        LCD_Set(0x47, SECOND);
        break;
    }
    LCD_W_Cmd(0x10); //需要重新调整光标
}
void LCD_Op5()     // 返回键操作
{
    POS = 0;

    //0000 1           D        C       B
    LCD_W_Cmd(0x0c);//显示on，光标off,  光标闪烁off
    Delayms(1);
    Time_Save();
    TR0 = 1;    // 重新开始时钟
}
void LCD_Op6()     // 减少键操作
{
    switch(POS)
    {
    case 1:  // 年份增加
        if(--YEAR2 < 0)
        {
            YEAR2 = 99;
            YEAR1--;
            LCD_Set(0x01, YEAR1);
        }
        LCD_Set(0x03, YEAR2);
        break;
    case 2:   // 月份增加
        if(--MONTH < 1)
            MONTH = 12;
        LCD_Set(0x06, MONTH);
        break;
    case 3:   // 日份增加
        if(--DAY < 1)
            DAY = 31;
        LCD_Set(0x09, DAY);
        break;
    case 4:   // 周份增加
        if(--WEEK < 1)
            WEEK = 7;
        LCD_Set_Week(WEEK);
        break;
    case 5:   // 时钟增加
        if(--HOUR < 0)
            HOUR = 23;
        LCD_Set(0x41, HOUR);
        break;
    case 6:  // 分钟增加
        if(--MINUTE < 0)
            MINUTE = 59;
        LCD_Set(0x44, MINUTE);
        break;
    case 7: // 秒针增加
        if(--SECOND < 0)
            SECOND = 59;
        LCD_Set(0x47, SECOND);
        break;
    }
    LCD_W_Cmd(0x10); //需要重新调整光标
}

void LCD_Set(unsigned char pos, unsigned char dat) // 在给定位置显示数据,数据必须为0~99之间
{
    unsigned char i = 0, tem = 0;

    LCD_Pos(pos);

    tem = dat % 10 + 0x30;   // 转化为个位ASCII码
    dat = dat / 10 + 0x30;   // 转化为十位ASCII码
    LCD_W_Data(dat);  // 显示十位
    LCD_W_Data(tem);  // 显示个位
}

void LCD_Set_Week(unsigned char week)  // 在固定位置设置星期
{
    LCD_Pos(0x0c);  // 设置地址

    switch(week)  // 设置数据
    {
    case 1:
        LCD_W_Data('M');
        LCD_W_Data('o');
        LCD_W_Data('n');
        break; // Monday
    case 2:
        LCD_W_Data('T');
        LCD_W_Data('u');
        LCD_W_Data('e');
        break; // Tuesday
    case 3:
        LCD_W_Data('W');
        LCD_W_Data('e');
        LCD_W_Data('d');
        break; // Wednesday
    case 4:
        LCD_W_Data('T');
        LCD_W_Data('h');
        LCD_W_Data('u');
        break; // Thursday
    case 5:
        LCD_W_Data('F');
        LCD_W_Data('r');
        LCD_W_Data('i');
        break; // Friday
    case 6:
        LCD_W_Data('S');
        LCD_W_Data('a');
        LCD_W_Data('t');
        break; // Saturday
    case 7:
        LCD_W_Data('S');
        LCD_W_Data('u');
        LCD_W_Data('n');
        break; // Sunday
    }
}

void LCD_Display()        // 显示数据
{
    LCD_Set(0x01, YEAR1);
    LCD_Set(0x03, YEAR2);
    LCD_W_Data('-');
    LCD_Set(0x06, MONTH);
    LCD_W_Data('-');
    LCD_Set(0x09, DAY);
    LCD_Set_Week(WEEK);

    LCD_Set(0x41, HOUR);
    LCD_W_Data(':');
    LCD_Set(0x44, MINUTE);
    LCD_W_Data(':');
    LCD_Set(0x47, SECOND);
}
