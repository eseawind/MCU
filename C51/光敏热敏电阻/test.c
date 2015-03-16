/***************************************************
 *         Author: Shawn Guo                       *
 *         Date  : 2013/2/14                       *
 *         Last  : 2013/2/14                       *
 *         Notes :  光敏电阻，热敏电阻            *
 *         J6 | J4(RG NTC ... 2 3)                 *
 *         P0 | J12(0 1 ... A B)                   *
 **************************************************/

#include<reg52.h>
#include "I2C.h"

#define  PCF8591 0x90    //PCF8591 地址 1001 A1 A2 A3 R/W

// 74HC138        数码管位选控制
sbit HC138A = P2^2;
sbit HC138B = P2^3;
sbit HC138C = P2^4;

// Nixie Tube(-)     低电平位选数码管， 高电平段选数码条， 因此数码表取自Tube(-) ，并取反
// a b ... g dp (数码管段选接口)
// 8 7 ... 2 1  (对应电平接口)
//                                  0      1       2      3     4      5      6      7       8     9      A      b      C      d      E      F
unsigned char code NUM_TABLE[] = {~0x03, ~0x9f, ~0x25, ~0x0d, ~0x99, ~0x49, ~0x41, ~0x1f, ~0x01, ~0x09, ~0x11, ~0xc1, ~0x63, ~0x85, ~0x61, ~0x71};
extern bit ack;                 /*应答标志位*/
void Delayms(unsigned int xms);
unsigned char ADC_Receive_Byte(unsigned char sla); // AD转换，从地址sla处读取数据，sla = PCF8591
bit ADC_Send_Byte(unsigned char sla, unsigned char c); //AD转换，向地址sla发送地址
bit DAC_Conversion(unsigned char sla, unsigned char c, unsigned char Val); //DA转换，发送数字信号Val


int main()
{
    unsigned char i = 0;
    unsigned char led[8];
    int digital0, digital1;

    while(1)
    {
        // A/D转换，选择模拟输入AIN2信道(RG光敏电阻电压)，采样并转化为数字
        ADC_Send_Byte(PCF8591, 0x42); //发送控制字节
        //控制字节 0 D6 D5 D4 0 D2 D1 D0
        //D0 D1:A/D通道编号 D2:自动增量允许位 D3：特征位，固定为0; D4 D5:模拟量输入方式选择位
        // D6：模拟输出允许位，A/D转换设为1，D/A转换设为0
        digital0 = ADC_Receive_Byte(PCF8591);//接受AD转换后的数据

        // A/D转换，选择模拟输入AIN3信道(NTC热敏电阻的电压信号)，采样并转化为数字
        ADC_Send_Byte(PCF8591, 0x43); //发送控制字节
        digital1 = ADC_Receive_Byte(PCF8591);//接受AD转换后的数据

        //将采样获得的数字信号显示在数码管上，通关旋转电位器，改变电压，可以改变数字
        //数码管段选设置
        led[0] = NUM_TABLE[digital0 % 10000 / 1000]; //千位设置
        led[1] = NUM_TABLE[digital0 % 1000 / 100]; //百位设置
        led[2] = NUM_TABLE[digital0 % 100 / 10]; //十位设置
        led[3] = NUM_TABLE[digital0 % 10]; //个位设置

        led[4] = NUM_TABLE[digital1 % 10000 / 1000]; //千位设置
        led[5] = NUM_TABLE[digital1 % 1000 / 100]; //百位设置
        led[6] = NUM_TABLE[digital1 % 100 / 10]; //十位设置
        led[7] = NUM_TABLE[digital1 % 10]; //个位设置

        //数码管位选设置
        for(i = 0; i < 8; i++)
        {
            P0 = led[i];//段选信号
            switch(i) // 位选信号
            {
            case 0:
                HC138C = 0;
                HC138B = 0;
                HC138A = 0;
                break;
            case 1:
                HC138C = 0;
                HC138B = 0;
                HC138A = 1;
                break;
            case 2:
                HC138C = 0;
                HC138B = 1;
                HC138A = 0;
                break;
            case 3:
                HC138C = 0;
                HC138B = 1;
                HC138A = 1;
                break;
            case 4:
                HC138C = 1;
                HC138B = 0;
                HC138A = 0;
                break;
            case 5:
                HC138C = 1;
                HC138B = 0;
                HC138A = 1;
                break;
            case 6:
                HC138C = 1;
                HC138B = 1;
                HC138A = 0;
                break;
            case 7:
                HC138C = 1;
                HC138B = 1;
                HC138A = 1;
                break;
            }
            Delayms(2);
        }
    }

    return 0;
}

/*******************************************************************
DAC 变换, 转化函数
*******************************************************************/
bit DAC_Conversion(unsigned char sla, unsigned char c, unsigned char Val)
{
    Start_I2C();              //启动总线

    Send_Byte(sla);            //发送器件地址
    if(ack == 0)return 0;

    Send_Byte(c);              //发送控制字节
    if(ack == 0)return 0;

    Send_Byte(Val);            //发送DAC的数值
    if(ack == 0)return 0;

    Stop_I2C();               //结束总线

    return 1;
}

/*******************************************************************
ADC发送字节[命令]数据函数
*******************************************************************/
bit ADC_Send_Byte(unsigned char sla, unsigned char c)
{
    Start_I2C();              //启动总线

    Send_Byte(sla);            //发送器件地址 1001 000 0 ，写入操作
    if(ack == 0)return 0;

    Send_Byte(c);              //发送数据
    if(ack == 0)return 0;

    Stop_I2C();               //结束总线

    return 1;
}

/*******************************************************************
ADC读字节数据函数
*******************************************************************/
unsigned char ADC_Receive_Byte(unsigned char sla)
{
    unsigned char dat;

    Start_I2C();          //启动总线

    Send_Byte(sla + 1);      //发送器件地址 1001 000 1 ，片选PCF8591， 最低位1表示读取
    if(ack == 0) return 0;

    dat = Receive_Byte();          //读取数据0

    Answer_I2C(1);           //发送非就答位

    Stop_I2C();           //结束总线

    return dat;
}

void Delayms(unsigned int xms)      // 毫秒级延时
{
    unsigned char i;

    while(--xms)            // --xms 与 xms-- 在汇编层次有差距
        for(i = 0; i < 110; i++)
            ;
}
