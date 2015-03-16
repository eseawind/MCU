/**********************************************
 *
 * 		Author	:		Shawn Guo
 * 		Date	:		2013/5/10
 *      Last    :       2013/5/25
 * 		Tool    :	    MSP430G2553
 * 		Notes	:		LCD12864驱动
 *
 *      1. VSS  2. VDD  3. V0   4. CS   5.SID
 *      6. CLK  7. PSB  8. RST  9.A     10.K
 *
 *---------------------------------------
 *          CS ---> P1.7
 *          SDIN--> P1.6
 *          SCLK--> P2.5
 *          RST---> 悬空
 **********************************************/
#include <msp430g2553.h>
#include "LCD12864.h"

// ===============================精确延时宏定义=========================
#define CPU_F ((double)8000000)        // 这里的CPU_F需要自己配置，这里配置为1MHZ
#define DELAY_US(x) __delay_cycles((long)(CPU_F*(double)x/1000000.0))
#define DELAY_MS(x) __delay_cycles((long)(CPU_F*(double)x/1000.0))

void LCD_IO_set(void)    // I/O口配置
{
    P1DIR |= (BIT6 + BIT7);
    P2DIR |= (BIT5);

    LCD_SDIN_0;
    LCD_SCLK_0;
    LCD_CS_0;
}


void LCD_Write_Byte(unsigned char c)   // 写入一个字节数据
{
    unsigned char i;

    for(i = 0; i < 8; i++)
    {
        LCD_SCLK_0;  // 拉低SCLK，上升沿读取数据

        if(c & 0x80)
        {
            LCD_SDIN_1;
        }
        else
        {
            LCD_SDIN_0;
        }
        LCD_SCLK_1;
        c = c << 1;      //左移一位
    }
    LCD_SCLK_0;  // 拉低SCLK，上升沿读取数据
}

void LCD_Write_Data(unsigned char dat) // 写入一个字节数据
{
    LCD_CS_1;

    LCD_Write_Byte(0xfa);            //1111_1_RW(0)_RS(1)_0 , 写入数据
    LCD_Write_Byte(0xf0 & dat);      //
    LCD_Write_Byte(0xf0 & (dat << 4) );//再发送低四位(先执行<<)

    LCD_CS_0;
}

void LCD_Write_Com(unsigned char com) // 写入一个字节命令
{
    LCD_CS_1;
    LCD_Write_Byte(0xf8);            //11111_RW(0)_RS(0)_0 ,写入命令
    LCD_Write_Byte(0xf0 & com);      //高四位
    LCD_Write_Byte(0xf0 & (com << 4));//低四位(先执行<<)
    LCD_CS_0;
}

void LCD_Txt_Clear(void)      // LCD text RAM清零
{
    unsigned char i;

    LCD_Write_Com(0x30);      //8BitMCU,基本指令集合
    LCD_Write_Com(0x80);      //AC归起始位
    for(i = 0; i < 64; i++)
        LCD_Write_Data(0x20);       //在RAM中写满空格
}

void LCD_Bmp_Clear(void)      // LCD 图像 RAM清零
{
    unsigned char i,j;

    LCD_Write_Com(0x34);      //8Bit扩充指令集,即使是36H也要写两次
    LCD_Write_Com(0x36);      //绘图ON,基本指令集里面36H不能开绘图

    for(i=0; i<32; i++)          //该12864实际为256x32，详情请看使用手册
    {
        LCD_Write_Com(0x80 | i);      //先倀输入行位置
        LCD_Write_Com(0x80);          //再输入列位置
        for(j=0; j<32; j++)           //256/8=32 byte，即写入32个字节就可以将一行中的256个像素点均设置为0.
            LCD_Write_Data(0);
    } //循环32次，实现32行清零
}

void LCD_Init(void)      // LCD初始化
{
    LCD_IO_set();

    LCD_Write_Com(0x30);      //8位数据,基本指令集合操作
    LCD_Write_Com(0x03);      //AC归0,不改变DDRAM内容
    LCD_Write_Com(0x0C);      //显示ON,游标OFF,游标位反白OFF
    LCD_Write_Com(0x01);      //清屏,AC归0
    LCD_Write_Com(0x06);      //写入时,游标右移动
    DELAY_MS(5);
}

// 写入字符串, col(0~7),row(0~3)
void LCD_Write_String(unsigned char raw, unsigned char col, char *str)
{
    LCD_Set_XY(col , raw);

    while( *str != '\0' )
    {
        LCD_Write_Data(*str++);
    }
}

void LCD_Write_Graph(unsigned char *graph) //绘图（128*32 + 128*32），数组大小为16字节*64行
{
    unsigned int x = 0;
    unsigned char i, j;

    LCD_Write_Com(0x34);      //启动8Bit扩充指令集, 即使是36H也要写两次
    LCD_Write_Com(0x36);      //绘图ON,因为基本指令集里面36H不能开绘图

    for(i=0; i<32; i++)          //12864实际为256x32，每次循环处理一行256位
    {
        LCD_Write_Com(0x80 | i);      //先写入行位置
        LCD_Write_Com(0x80);      //再写入倀列位置
        for(j=0; j<32; j++)    //256/8=32 byte，写入32个字节0即实现了一行256个像素点的清零
        {
            //列位置每行自动增加
            LCD_Write_Data(graph[x++]);
        }
    }
}

void LCD_Set_XY(unsigned char x, unsigned char y)  //设置位置离开(X,Y)
{
    unsigned char Address ;
    switch( y )
    {
    case 0 :
        Address = 0x80 + x;
        break;
    case 1 :
        Address = 0x90 + x;
        break;
    case 2 :
        Address = 0x88 + x;
        break;
    case 3 :
        Address = 0x98 + x;
        break;
    default :
        break;
    }
    LCD_Write_Com(Address);
}

// x范围为0~31
// y范围为0~15
void LCD_Set_Graph_XY(unsigned char x, unsigned char y) // 设置图像的(X,Y),假定已经为图像模式
{
    //先写入行的位置
    LCD_Write_Com(0x80 | x);
    //再写入列位置
    LCD_Write_Com(0x80 | y);
}

void LCD_Display_4Num(unsigned char x, unsigned char y, unsigned int num) //显示一个四位数字
{
    unsigned char num_1, num_2, num_3, num_4;

    LCD_Set_XY(x, y);

    num_4 = (num / 1000) + 0x30;
    num_3 = ((num % 1000) / 100) + 0x30;
    num_2 = (((num % 1000) % 100) / 10) + 0x30;
    num_1 = (((num % 1000) % 100) % 10) + 0x30;

    LCD_Write_Data(num_4);
    LCD_Write_Data(num_3);
    LCD_Write_Data(num_2);
    LCD_Write_Data(num_1);
}

void LCD_Display_4_2flout(unsigned char x, unsigned char y, double dou) //显示一个四位double,二位精度
{
    unsigned char num_1, num_2, num_3, num_4, num_5, num_6;
    unsigned long int num = (long int)(dou * 100);

    LCD_Set_XY(x, y);

    num_6 = (num / 100000) + 0x30;
    num %= 100000;
    num_5 = (num / 10000) + 0x30;
    num %= 10000;
    num_4 = (num / 1000) + 0x30;
    num %= 1000;
    num_3 = (num / 100) + 0x30;
    num %= 100;
    num_2 = (num / 10) + 0x30;
    num %= 10;
    num_1 = num + 0x30;


    LCD_Write_Data(num_6);
    LCD_Write_Data(num_5);
    LCD_Write_Data(num_4);
    LCD_Write_Data(num_3);
    LCD_Write_Data('.');
    LCD_Write_Data(num_2);
    LCD_Write_Data(num_1);
}

const unsigned char HZ_TABLE[]=
{
    0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,      //第一行汉字位置
    0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,      //第二行汉字位置
    0x88,0x89,0x8a,0x8b,0x8c,0x8d,0x8e,0x8f,      //第三行汉字位置
    0x98,0x99,0x9a,0x9b,0x9c,0x9d,0x9e,0x9f,      //第四行汉字位置
};

// 显示汉字，显示汉字的区域
// 行 0~3  列 0 ~ 8
void LCD_Display_HanZi(unsigned char row, unsigned char col, char *str) //打印汉字
{
    //再写入列位置
    //LCD_Write_Com(0x30);      //8BitMCU,基本指令集合
    LCD_Write_Com(HZ_TABLE[ 8 * row + col]);      //起始位置
    while(*str != '\0')      //判断字符串是否显示完毕
    {
        LCD_Write_Data(*str);      //一个汉字要写两次，因为汉字对一个的ASCII码为双CHAR
        str++;
        LCD_Write_Data(*str);
        str++;
    }
}

// 以(x,y)为圆心，画出X坐标轴，向右为正方向，注意x轴主干上下均有3行箭头
// 设定圆心时需要考虑箭头占的地方
// x:3~28, y: 0~15
void LCD_Graph_Xaxis(unsigned char x, unsigned char y, unsigned char length)
{
    int i = 0;

    LCD_Set_Graph_XY(x, y);

    for(; i < length; i++)
    {
        LCD_Write_Data(0xff);
        LCD_Write_Data(0xff);
    }

    //LCD_Graph_Xaxis(25,9,7); //x轴主干
    //x轴箭头绘制
    LCD_Set_Graph_XY(x - 1, y + length - 1);      //三行上箭头
    LCD_Write_Data(0x00);
    LCD_Write_Data(0x02);
    LCD_Set_Graph_XY(x - 2, y + length - 1);
    LCD_Write_Data(0x00);
    LCD_Write_Data(0x04);
    LCD_Set_Graph_XY(x - 3, y + length - 1);
    LCD_Write_Data(0x00);
    LCD_Write_Data(0x08);

    LCD_Set_Graph_XY(x + 1, y + length - 1); //三行下箭头
    LCD_Write_Data(0x00);
    LCD_Write_Data(0x02);
    LCD_Set_Graph_XY(x + 2, y + length - 1);
    LCD_Write_Data(0x00);
    LCD_Write_Data(0x04);
    LCD_Set_Graph_XY(x + 3, y + length - 1);
    LCD_Write_Data(0x00);
    LCD_Write_Data(0x08);
}

// 以(x,y)为圆心，画出Y坐标轴，向下为正方向，注意Y轴主干左右均有3行箭头
// 设定圆心时需要考虑箭头占的地方
// x:0~32, y: 1~14, length: 0~63
void LCD_Graph_Yaxis(unsigned char x, unsigned char y, unsigned char length)
{
    int i = 0, temp = 0;

    if(y < 8)  //先显示上面的128*32，从x到31行画线
    {
        if(x + length > 31)  //需要上下两个半屏，在下面的屏幕中显示箭头
        {
            temp = length - (32 - x);
            for(i = 0; i < temp - 3; i++)
            {
                LCD_Set_Graph_XY(i, y + 8);
                LCD_Write_Data(0x10);
                LCD_Write_Data(0x00);
            }
            length = 32 - x;

            //箭头
            LCD_Set_Graph_XY(temp - 3, y + 8);
            LCD_Write_Data(0x92);
            LCD_Write_Data(0x00);
            LCD_Set_Graph_XY(temp - 2, y + 8);
            LCD_Write_Data(0x54);
            LCD_Write_Data(0x00);
            LCD_Set_Graph_XY(temp - 1, y + 8);
            LCD_Write_Data(0x38);
            LCD_Write_Data(0x00);
            LCD_Set_Graph_XY(temp, y + 8);
            LCD_Write_Data(0x10);
            LCD_Write_Data(0x00);

            for(i = 0; i < length; i++)  //显示上面的屏幕
            {
                LCD_Set_Graph_XY(x + i, y);
                LCD_Write_Data(0x10);
                LCD_Write_Data(0x00);
            }
        }
        else  //只需要上面一个屏幕
        {
            for(i = 0; i < length - 3; i++)  //显示上面的屏幕
            {
                LCD_Set_Graph_XY(x + i, y);
                LCD_Write_Data(0x10);
                LCD_Write_Data(0x00);
            }
            LCD_Set_Graph_XY(x + length - 3, y);
            LCD_Write_Data(0x92);
            LCD_Write_Data(0x00);
            LCD_Set_Graph_XY(x + length - 2, y);
            LCD_Write_Data(0x54);
            LCD_Write_Data(0x00);
            LCD_Set_Graph_XY(x + length - 1, y);
            LCD_Write_Data(0x38);
            LCD_Write_Data(0x00);
            LCD_Set_Graph_XY(x + length, y);
            LCD_Write_Data(0x10);
            LCD_Write_Data(0x00);
        }
    }
    else //仅在下面的屏幕中显示坐标系
    {
        for(i = 0; i < length - 3; i++)
        {
            LCD_Set_Graph_XY(x + i, y);
            LCD_Write_Data(0x10);
            LCD_Write_Data(0x00);
        }

        //箭头
        LCD_Set_Graph_XY(x + length - 3, y);
        LCD_Write_Data(0x92);
        LCD_Write_Data(0x00);
        LCD_Set_Graph_XY(x + length - 2, y);
        LCD_Write_Data(0x54);
        LCD_Write_Data(0x00);
        LCD_Set_Graph_XY(x + length - 1, y);
        LCD_Write_Data(0x38);
        LCD_Write_Data(0x00);
        LCD_Set_Graph_XY(x + length, y);
        LCD_Write_Data(0x10);
        LCD_Write_Data(0x00);
    }
}


//一共可以打印64个点而互不影响，即按行打印点
// x: 0~127   y: 0~63
void LCD_Point(unsigned char x, unsigned char y)  //在128*64上画出这个点，注意LCD12864的写入规律以避免覆盖
{
    unsigned char row = 0, col = 0, dat = 0;

    if(y > 31)  //下面的LCD12864*32
    {
        col = x / 16 + 8;
        row = y - 32;
        dat = x % 16;
    }
    else  //上面的LCD128*32
    {
        col = x / 16;
        row = y;
        dat = x % 16;
    }
    LCD_Set_Graph_XY(row , col);
    switch(dat)
    {
    case 0:
        LCD_Write_Data(0x80);
        LCD_Write_Data(0x00);
        break;
    case 1:
        LCD_Write_Data(0x40);
        LCD_Write_Data(0x00);
        break;
    case 2:
        LCD_Write_Data(0x20);
        LCD_Write_Data(0x00);
        break;
    case 3:
        LCD_Write_Data(0x10);
        LCD_Write_Data(0x00);
        break;
    case 4:
        LCD_Write_Data(0x08);
        LCD_Write_Data(0x00);
        break;
    case 5:
        LCD_Write_Data(0x04);
        LCD_Write_Data(0x00);
        break;
    case 6:
        LCD_Write_Data(0x02);
        LCD_Write_Data(0x00);
        break;
    case 7:
        LCD_Write_Data(0x01);
        LCD_Write_Data(0x00);
        break;
    case 8:
        LCD_Write_Data(0x00);
        LCD_Write_Data(0x80);
        break;
    case 9:
        LCD_Write_Data(0x00);
        LCD_Write_Data(0x40);
        break;
    case 10:
        LCD_Write_Data(0x00);
        LCD_Write_Data(0x20);
        break;
    case 11:
        LCD_Write_Data(0x00);
        LCD_Write_Data(0x10);
        break;
    case 12:
        LCD_Write_Data(0x00);
        LCD_Write_Data(0x08);
        break;
    case 13:
        LCD_Write_Data(0x00);
        LCD_Write_Data(0x04);
        break;
    case 14:
        LCD_Write_Data(0x00);
        LCD_Write_Data(0x02);
        break;
    case 15:
        LCD_Write_Data(0x00);
        LCD_Write_Data(0x01);
        break;
    }
}
