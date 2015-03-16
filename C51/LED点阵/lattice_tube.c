/***************************************************
 *         Author: Shawn Guo                       *
 *         Date  : 2013/2/12                       *
 *         Last  : 2013/2/12                       *
 *         Notes : Lattice Tube                    *
 *              P0 | J12(0 1 ... h g)              *
 *              P2 | J20(0 1 ... 1 2)              *
 *              P1 | J19(0 1 ... 1 2)              *
 **************************************************/

#include <reg52.h>

// 行选通由J12控制，高电平选通
unsigned char code ROW_TABLE[] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};
// 列选通由J19, J20控制，低电平选通
unsigned char code COL_TABLE[] = {0xfe, 0xfd, 0xfb, 0xf7, 0xef, 0xdf, 0xbf, 0x7f};

unsigned char code NUM_TABLE[10][8] ={  // 8列数据，每次选通一列，由对应数据高电平选通行，形成动态扫描
{0x00, 0x00, 0x7e, 0x42, 0x42, 0x7e, 0x00, 0x00}, // 0
{0x00, 0x00, 0x00, 0xff, 0xff, 0x00 ,0x00, 0x00}, // 1
{0x00, 0x00, 0x9e, 0x92, 0x92, 0xf2, 0x00, 0x00}, // 2
{0x00, 0x00, 0x92, 0x92, 0x92, 0xfe, 0x00, 0x00}, // 3
{0x00, 0x00, 0xf0, 0x10, 0x10, 0xfe, 0x00, 0x00}, // 4
{0x00, 0x00, 0xf2, 0x92, 0x92, 0x9e, 0x00, 0x00}, // 5
{0x00, 0x00, 0xfe, 0x92, 0x92, 0x9e, 0x00, 0x00}, // 6
{0x00, 0x00, 0x80, 0x80, 0x80, 0xfe, 0x00, 0x00}, // 7
{0x00, 0x00, 0xfe, 0x92, 0x92, 0xfe, 0x00, 0x00}, // 8
{0x00, 0x00, 0xf2, 0x92, 0x92, 0xfe, 0x00, 0x00}  // 9
};

void Delayms(unsigned int xms);
void Display_Line();  // line display
void Display_Num();   // number display


int main()
{
    while(1)
    {
        Display_Line();
        Display_Num();
    }

    return 0;
}

void Display_Line()
{
    unsigned char row, col;

    P2 = 0;  // 红色所有列均置低电平，选通所有红色列
    for(row = 0; row < 8; row++){ // 循环3次, 行线滚动
        P0 = ROW_TABLE[row];   // 选通一行，置高电平点亮
        Delayms(200);
    }
    P2 = 0xff;

    P1 = 0;             // 绿色所有列低电平选通
    for(row = 0; row < 8; row++){ // 循环3次, 行线滚动
        P0 = ROW_TABLE[row];   // 选通一行，置高电平点亮
        Delayms(200);
    }
    P1 = 0xff;

    //纵条
    P0 = 0xff;  // 高电平选通所有行
    for(col = 0; col < 8; col++){
        P2 = COL_TABLE[col];   // 低电平选通一列
        Delayms(200);
    }
    P2 = 0xff;
    for(col = 0; col < 8; col++){
        P1 = COL_TABLE[col];
        Delayms(200);
    }
    P1 = 0xff;
    P0 = 0;
}

void Display_Num()
{
    unsigned char col = 0, i = 0, county = 0;
    unsigned int countx = 0;

    while(1)
    {
        if(++county >= 50) // 列扫描速度设置
        {
            county = 0;
            if(++col >= 8)  // 每隔一小段时间，显示下一列，实现动态扫描一个数字点阵
                col = 0;
        }
        if(++countx >= 20000) // 数字扫描速度设置
        {
            countx = 0;
            if(++i >= 10)
                break;
        }

        P2 = COL_TABLE[col]; // 低电平选通一列
        P0 = NUM_TABLE[i][col]; // 高电平选通给定的行
    }
}

void Delayms(unsigned int xms)      // 毫秒级延时
{
    unsigned char i;

    while(--xms)            // --xms 与 xms-- 在汇编层次有差距
        for(i = 0; i < 110; i++)
            ;
}



