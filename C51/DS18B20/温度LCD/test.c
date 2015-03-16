/********************************************************
 *         Author: Shawn Guo                            *
 *         Date  : 2013/2/18                            *
 *         Last  : 2013/2/18                            *
 *         Note:   DS18B20温度传感器 LCD显示            *
 ********************************************************/

#include <reg52.H>
#include <intrins.h>       // _nop_();
#include "temp.h"

// LCD1602 接口
sbit LCD_RS = P2^6;  //复位端, 高电平数据操作，低电平命令操作
sbit LCD_RW = P2^5;	 //写数据端，高电平读取，低电平写入
sbit LCD_EN = P2^7;	 //使能端，高电平时能读取，高脉冲写入
//  LCD1602 数据
unsigned char code DISPLAY1[] = {"   Temperature  "}; // 16 bytes

extern unsigned int  idata TEMPERATURE;
extern unsigned char idata TIM;  // Time1 中断响应计时器

extern void GetTemp();  //每间隔一段时间，读取一次温度
void Delayms(unsigned int xms);
void Time_Init();         // 配置定时器Timer1, 一定时间读取一次温度

bit LCD_IsBusy();   // 检测LCD是否繁忙
void LCD_W_Cmd(unsigned char cmd);// 写入指令数据到LCD
void LCD_Pos(unsigned char pos); //设定LCD数据指针
void LCD_W_Data(unsigned char dat);  //写入字符显示数据到LCD
void LCD_Init();    //LCD初始化设定


int main()
{
    unsigned char i = 0;
    unsigned char a = 0, b = 0;

    Time_Init();

    LCD_Init();
    i = 0;
    LCD_Pos(0x00);
    while(DISPLAY1[i] != '\0')
        LCD_W_Data(DISPLAY1[i++]);

    while(1)
    {
        GetTemp();        // 固定时间间隔，读取一次温度数据
        a = TEMPERATURE % 100 / 10 + 0x30;
        b = TEMPERATURE % 10 + 0x30;

        LCD_Pos(0x47);
        LCD_W_Data(a);
        LCD_W_Data(b);
    }

    return 0;
}


void Time_Init()
{
    //定时器工作方式配置
    //             TIME1           TIME0
    // TMOD = GATE C/T M1 M0   GATE C/T M1 M0
    TMOD |= 0x10;  //Timer1选择模式1  16位计时

    TH1 = 0xD8;    //10
    TL1 = 0xF0;

    //中断允许寄存器 IE配置 EA - ET2 ES ET1 EX1 ET0 EX0
    IE = 0x88;   // EA = 1, ET1 = 1

    TR1  = 1;
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

void Timer1() interrupt 3
{
    TH1 = 0xD8;    //10
    TL1 = 0xF0;

    TIM++;         // Timer1中断响应计时器

}


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
    Delayms(1);
}

