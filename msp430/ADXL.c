/**********************************************
 *
 *      Author  :       Shawn Guo
 *      Mail    :       iguoxiaopeng@gmail.com
 *      Date    :       2013/8/27
 *      Last    :
 *      Notes   :       ADXL345 加速度传感器模块
 *      Tool    :       MSP430X2XX  / ADXL345
 *
 *
 *      1.VCC   2.NC    3.NC    4.INT2  5.INT1  6.GND
 *      7.GND   8.CS    9.SOD   10.SDI 11.SCLK 12.VCC
 *      SPI配置: ADXL345为从机, SCLK空闲为高电平
 *               数据在下降沿更新,在上升沿捕获.
 *--------------------------------------------------------
 *
 **********************************************/


#include "ADXL.h"
#include "clock.h"
#include <math.h>
#include <msp430f2618.h>





unsigned char  ADXL_ACT_INACT_CTL = 0;   //ADXL检测活动与静止的配置数据
unsigned char  ADXL_INT_REG = 0x80;      //ADXL的中断设置
unsigned char  ADXL_INT_MAP_REG = 0;     //ADXL的中断映射
unsigned char  ADXL_DATA_FORM = 0x0b;    //数据格式

int ADXL_X_VALUE = 0;   //X加速度值
int ADXL_Y_VALUE = 0;   //Y加速度值
int ADXL_Z_VALUE = 0;   //Z加速度值

/***************************************************************************************************
 *                                        ADXL345定义
 ***************************************************************************************************/

//ADXL345初始化
void ADXL_INIT()
{
    unsigned char conf[]= {254,3,13};             //X、Y、Z的偏移量(254,2,13)

#ifndef SPI_SIM_H_
    IIC_INIT();
#else
    SPI_INIT();
#endif

    //config ADXL345
    ADXL_WRITE_BYTE(DATA_FORMAT,0x0B);    //正负16g，13位模式
    ADXL_WRITE_BYTE(POWER_CTL,0x08);      //测量模式
    ADXL_WRITE_BYTE(BW_RATE,0x03);        //正常供电，0.78Hz
    ADXL_WRITE_BYTE(INT_ENABLE,0x80);     //数据准备好中断
    ADXL_WRITE_STRING(OFSX,conf,3);       //X、Y、Z的偏移
}

//设置角度传感器的三个轴向的偏移量
//sel：OFSX、OFSY、OFSZ分别代表X、Y、Z三个轴
//data为-128~127,分辨率为:15.6mg/LSB
void ADXL_OFS_CONFIG(unsigned int sel, int data)
{
    ADXL_WRITE_BYTE(sel,data);
}

//设置敲击事件要检测的方向轴
//dir为对应的坐标轴:ADXL_X、ADXL_Y、ADXL_Z
//dat确定在第二次敲击检测之前检测到敲击，是否屏蔽第二次敲击事件
void ADXL_TAPDIR_CONFIG(unsigned char dir, unsigned char dat)
{
    if(dat)  dir |=BIT3;
    else     dir &=~BIT3;

    ADXL_WRITE_BYTE(TAP_AXES,dir);
}

//设置敲击事件的最小幅度和最大时间
//thresh为敲击的最小幅度,为零可能会出现异常，精度为：63.5mg/LSB,范围：0~255
//dur为敲击的最大时间，为零禁止单击/双击功能，精度为：625us/LSB,范围：0~255
void ADXL_TAP_CNOFIG(unsigned char thresh,unsigned char dur)
{
    ADXL_WRITE_BYTE(THRESH_TAP,thresh);
    ADXL_WRITE_BYTE(DUR,dur);
}

//设置第二次敲击的延迟和检测时间
//delay为第一次敲击到第二次敲击之间的延迟事件,为零禁用双击功能，精度为：1.25ms/LSB,范围：0~255
//time为第二次敲击的检测时间,为零禁用双击功能，精度为：1.25ms/LSB,范围：0~255
void ADXL_STAP_CONFIG(unsigned char delay, unsigned char time)
{
    ADXL_WRITE_BYTE(Latent,delay);
    ADXL_WRITE_BYTE(Window,time);
}

//设置活动检测的阀值
//thresh为活动检测的最大阀值,为零会工作异常，精度为：62.5mg/LSB
//dir选择要检测哪一个方向的加速度:ADXL_X、ADXL_Y、ADXL_Z,可以取或
//mode选择比较模式(交流或直流比较)：ADXL_AC、ADXL_DC
void ADXL_ACT_CONFIG(unsigned char thresh, unsigned char dir, unsigned char mode)
{
    ADXL_WRITE_BYTE(THRESH_ACT,thresh);

    ADXL_ACT_INACT_CTL &=~(BIT4 + BIT5 + BIT6);
    ADXL_ACT_INACT_CTL |=dir<<4;

    if(mode == ADXL_AC)
        ADXL_ACT_INACT_CTL &=~BIT7;
    else
        ADXL_ACT_INACT_CTL |=BIT7;

    ADXL_WRITE_BYTE(ACT_INACT_CTL,ADXL_ACT_INACT_CTL);
}

//设置静止检测的阀值和时间
//thresh为静止检测的阀值,为零会工作异常,精度为：62.5mg/LSB
//time设置可容忍的加速度的时间范围，为零会工作异常，精度为：1s/LSB
//dir选择要检测哪一个方向的加速度:ADXL_X、ADXL_Y、ADXL_Z,可以取或
//mode选择比较模式(交流或直流比较)：ADXL_AC、ADXL_DC
void ADXL_INACT_CONFIG(unsigned char thresh, unsigned char time, unsigned char dir, unsigned char mode)
{
    ADXL_WRITE_BYTE(THRESH_INACT,thresh);
    ADXL_WRITE_BYTE(TIME_INACT,time);

    ADXL_ACT_INACT_CTL &=~(BIT0 + BIT1 + BIT2);
    ADXL_ACT_INACT_CTL |=dir;

    if(mode == ADXL_AC)
        ADXL_ACT_INACT_CTL &=~BIT3;
    else
        ADXL_ACT_INACT_CTL |=BIT3;

    ADXL_WRITE_BYTE(ACT_INACT_CTL,ADXL_ACT_INACT_CTL);
}

//设置自由落体运动的加速度阀值和时间阀值
//thrsh为加速度阀值，为零会异常，精度为：63.5mg/LSB
//time为时间阀值，为零会异常，精度为：5ms/LSB
void ADXL_FF_CONFIG(unsigned char thresh, unsigned char time)
{
    ADXL_WRITE_BYTE(THRESH_FF,thresh);
    ADXL_WRITE_BYTE(TIME_FF,time);
}

//读取敲击/活动事件的来源，或是系统当前的休眠状态
//sel为：ADXL_TAP、ADXL_ACT、ADXL_SLEEP
//返回值：4-->x  2-->y  1--->z
unsigned char ADXL_GET_STATE(unsigned char sel)
{
    unsigned char data=0;

    data=ADXL_READ_BYTE(ACT_TAP_STATUS);

    if(sel == ADXL_TAP)
        return (data & 0x07);
    else if(sel == ADXL_ACT)
        return (data & 0x70);
    else
        return data & 0x08;
}

//设置工作模式和数据速率
//mode为电源模式:ADXL_NORMAL、ADXL_LOW
//rate为数据传送的速率：　ADXL_RATE_400 ~ ADXL_RATE_12_5
void ADXL_MODE_CONFIG(unsigned char mode, unsigned char rate)
{
    unsigned char data=0;

    data =mode | rate;
    ADXL_WRITE_BYTE(BW_RATE,data);
}

//设置测量范围
void ADXL_SET_RANGE(unsigned char range)
{
    ADXL_DATA_FORM &=~(BIT0 + BIT1);
    ADXL_DATA_FORM |=range;

    ADXL_WRITE_BYTE(DATA_FORMAT,ADXL_DATA_FORM);
}

//设置中断的有效电平
void ADXL_INT_VALID(unsigned char dat)
{
    if(dat == ADXL_VALID_H)
        ADXL_DATA_FORM &=~BIT5;
    else
        ADXL_DATA_FORM |=BIT5;

    ADXL_WRITE_BYTE(DATA_FORMAT,ADXL_DATA_FORM);
}

//使能中断
void ADXL_E_INT(unsigned char intp)
{
    ADXL_INT_REG |=intp;

    ADXL_WRITE_BYTE(INT_ENABLE,ADXL_INT_REG);
}

//禁止中断
void ADXL_D_INT(unsigned char intp)
{
    ADXL_INT_REG &=~intp;

    ADXL_WRITE_BYTE(INT_ENABLE,ADXL_INT_REG);
}

//设置中断对应的中断引脚
void ADXL_INT_MAP(unsigned char sel, unsigned char intp)
{
    if(sel == ADXL_INT1)
        ADXL_INT_MAP_REG &=~intp;
    else
        ADXL_INT_MAP_REG |=intp;

    ADXL_WRITE_BYTE(ADXL_INT_MAP_REG,sel);
}

//返回中断标志位
// 用于确定是哪一种情况发生了
unsigned char ADXL_GET_IFG()
{
    return ADXL_READ_BYTE(INT_SOURCE);
}

//获取三个轴的加速度值
void ADXL_GET_VALUE(int *x, int *y, int *z)
{
    unsigned char buf[6];
    unsigned int i=0;

    for(i=0; i<5; i++)
    {
        ADXL_READ_STRING(DATAX0,buf,6);
        if(buf[0]==255 && buf[1]==255 && buf[2]==255 && buf[3]==255 && buf[4]==255 && buf[5]==255) continue;
        break;
    }

    ADXL_X_VALUE=ADXL_TRANSFER(buf[1],buf[0]);
    ADXL_Y_VALUE=ADXL_TRANSFER(buf[3],buf[2]);
    ADXL_Z_VALUE=ADXL_TRANSFER(buf[5],buf[4]);

    if(ADXL_Y_VALUE >0)  ADXL_Y_VALUE -=15;

    if(x) *x=ADXL_X_VALUE;
    if(y) *y=ADXL_Y_VALUE;
    if(z) *z=ADXL_Z_VALUE;
}

//计算加速度值
int ADXL_TRANSFER(unsigned char high, unsigned char low)
{
    int num=0;

    if(high & 0x80)
    {
        low =~low + 1;
        high =low==0? ~high+1 : ~high;
        num= (int)((-(low + (high<<8)))*3.822);
    }
    else
        num =(int)((low + (high<<8))*3.822);

    return num;
}

//计算角速度值
//x为x轴水平时，板子与竖直方向的夹角(正向)
//y为y轴水平时，板子与水平方向的夹角(正向)
//z为z轴水平时，y轴与水平方向的夹角(正向)
void ADXL_GET_ANGLE(int *x, int *y, int *z)
{
    (*x) = (int)(atan2(-1.0 * ADXL_Y_VALUE, ADXL_Z_VALUE) / ADXL_PI * 180);
    if(*x < 0)   (*x) += 360;

    (*y) = (int)(atan2(-1.0 * ADXL_X_VALUE, ADXL_Z_VALUE) / ADXL_PI * 180);
    if(*y < 0)   (*y) += 360;

    (*z) = (int)(atan2(-1.0 * ADXL_Y_VALUE, ADXL_X_VALUE) / ADXL_PI * 180);
    if(*z < 0)   (*z) += 360;
}



/*******************底层IO接口操作************************/
//从ADXL345中读取一个字节流
//reg为寄存器的地址
//*pdata指向存储数据的缓存
//len为缓存的大小
void ADXL_READ_STRING(unsigned char reg, unsigned char *pdata, unsigned int len)
{
    unsigned int i=0;

#ifndef SPI_SIM_H_
    IIC_START();
    IIC_TX(IIC_SLAVE_ADDRESS & 0xfe);
    IIC_TX(reg);
    IIC_START();
    IIC_TX(IIC_SLAVE_ADDRESS | 0x01);

    for(i=0; i<len; i++)
    {
        DELAY_US(30);
        pdata[i]=IIC_RX();
        if(i==len-1)
            IIC_SEND_ACK(1);    //NOACK
        else
            IIC_SEND_ACK(0);    //ACK
    }
    IIC_STOP();

#else
    reg |=0x80;
    if(len>1) reg |=0x40;
    CS_0;

    SPI_SIM_SEND(reg);
    for(i=0; i<len; i++)
        pdata[i]=SPI_SIM_RCV();

    CS_1;
#endif

}

//向ADXL345中写入一个字节流
//reg为寄存器的地址
//pdata指向存储读取数据的缓冲区
//len为要读取的数据的个数
void ADXL_WRITE_STRING(unsigned char reg, unsigned char *pdata, unsigned int len)
{
    unsigned int i=0;

#ifndef SPI_SIM_H_
    IIC_START();
    IIC_TX(IIC_SLAVE_ADDRESS & 0xfe);
    IIC_TX(reg);

    for(i=0; i<len; i++)
        IIC_TX(pdata[i]);
    IIC_STOP();

#else
    reg &=~0x80;
    if(len>1)  reg |=0x40;
    CS_0;

    SPI_SIM_SEND(reg);
    for(i=0; i<len; i++)
        SPI_SIM_SEND(pdata[i]);

    CS_1;
#endif
}

//向ADXL345写入一个字节
//reg为寄存器的地址
//data为要写入的数据
void ADXL_WRITE_BYTE(unsigned char reg, unsigned char data)
{

#ifndef SPI_SIM_H_
    IIC_START();
    IIC_TX(IIC_SLAVE_ADDRESS & 0xfe);   //slave address
    IIC_TX(reg);                        //register address
    IIC_TX(data);                       //data
    IIC_STOP();

#else
    reg &=~0x80;
    CS_0;

    SPI_SIM_SEND(reg);
    SPI_SIM_SEND(data);

    CS_1;
#endif

    DELAY_US(10);
}

//从ADXL345读取一个字节
//reg为寄存器的地址
unsigned char ADXL_READ_BYTE(unsigned char reg)
{
    unsigned char data=0;

#ifndef SPI_SIM_H_
    IIC_START();
    IIC_TX(IIC_SLAVE_ADDRESS & 0xfe);   //slave address
    IIC_TX(reg);                        //register address
    IIC_START();
    IIC_TX(IIC_SLAVE_ADDRESS | 0x01);
    data=IIC_RX();
    IIC_SEND_ACK(1);
    IIC_STOP();

#else
    reg |=0x80;
    CS_0;

    SPI_SIM_SEND(reg);
    data=SPI_SIM_RCV();

    CS_1;
#endif

    DELAY_US(10);
    return data;
}

/***************************************************************************************************
 *                                         SPI、IIC定义
 ***************************************************************************************************/
#ifndef SPI_SIM_H_
//模拟IIC初始化
void IIC_INIT()
{
    IIC_SCL;
    IIC_SDA_OUT;
}

//开始IIC传送数据
void IIC_START()
{
    IIC_SDA_OUT;
    DELAY_US(3);

    IIC_SCL_1;
    IIC_SDA_1;

    DELAY_US(5);
    IIC_SDA_0;
    DELAY_US(5);
    IIC_SCL_0;
}

//结束IIC传送数据
void IIC_STOP()
{
    IIC_SDA_OUT;
    DELAY_US(3);

    IIC_SCL_1;
    IIC_SDA_0;

    DELAY_US(5);
    IIC_SDA_1;
    DELAY_US(5);
}

//IIC发送一个字节
void IIC_TX(unsigned char data)
{
    unsigned int i=0;

    IIC_SDA_OUT;
    DELAY_US(3);
    for(i=0; i<8; i++)
    {
        if(data & 0x80)
            IIC_SDA_1;
        else
            IIC_SDA_0;

        IIC_SCL_1;
        DELAY_US(5);
        IIC_SCL_0;
        DELAY_US(5);
        data <<=1;
    }

    IIC_SDA_IN;      //Receive ACK
    IIC_SCL_1;
    DELAY_US(5);
    IIC_SCL_0;
    DELAY_US(5);
}

//IIC接收一个字节
unsigned char IIC_RX()
{
    unsigned int i=0;
    unsigned char data=0;

    IIC_SDA_IN;
    DELAY_US(3);
    for(i=0; i<8; i++)
    {
        data <<=1;
        IIC_SCL_1;

        if(IIC_SDA)
            data |=0x01;
        DELAY_US(5);
        IIC_SCL_0;
        DELAY_US(5);
    }
    return data;
}

//发送ACK
void IIC_SEND_ACK(unsigned char data)
{
    IIC_SDA_OUT;    //Send ACK
    DELAY_US(3);

    if(data)
        IIC_SDA_1;
    else
        IIC_SDA_0;

    IIC_SCL_1;
    DELAY_US(5);
    IIC_SCL_0;
    DELAY_US(5);
}

#else

//模拟SPI初始化
void SPI_INIT()
{
    SPIB1_SIMO;
    SPIB1_SOMI;
    SPIB1_CLK_OUT;
    IO_CFG;
}

//模拟SPI发送
void SPI_SIM_SEND(unsigned char data)
{
    unsigned char i=0;

    for(i=0; i<8; i++)
    {
        if(data & 0x80)
            SPIB1_SIMO_1;
        else
            SPIB1_SIMO_0;

        SPIB1_CLK_0;
        data =data<<1;
        SPIB1_CLK_1;
    }

    DELAY_US(20);
}

//模拟SPI接收
unsigned char SPI_SIM_RCV()
{
    unsigned char i=0;
    unsigned char data=0;

    for(i=0; i<8; i++)
    {
        data =data<<1;
        SPIB1_CLK_0;

        if(SPIB1_SOMI_IN)
            data |=BIT0;
        else
            data &=~BIT0;

        SPIB1_CLK_1;
    }

    DELAY_US(20);
    return data;
}
#endif
