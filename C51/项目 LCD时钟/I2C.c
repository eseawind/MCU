/***************************************************
 *         Author: Shawn Guo                       *
 *         Date  : 2013/2/13                       *
 *         Last  : 2013/2/13                       *
 *         Notes : I2C总线驱动                     *
 **************************************************/
#include<reg52.h>
#include <intrins.h>         // _nop_();
#include "I2C.h"

sbit SCL = P2^1;     //I2C  时钟线
sbit SDA = P2^0;     //I2C  数据线

bit ack;                 /*应答标志位*/

/*******************************************************************
                     起动总线函数
函数原型: void  Start_I2C();
功能:     启动I2C总线
********************************************************************/
void Start_I2C()
{
    SDA = 1;         //发送起始条件的数据信号,SDA置高电平，等待被拉低
    _nop_();

    SCL = 1;        // SCL置高电平
    _nop_();        //SDA高电平建立时间大于4.7us,延时
    _nop_();        // 此单片机为12MHZ, 一条简单语句1us
    _nop_();
    _nop_();
    _nop_();

    SDA = 0;         /*SDA在SCL高电平时拉低，下降沿产生起始信号*/
    _nop_();        /* 起始条件锁定时间大于4μs*/
    _nop_();
    _nop_();
    _nop_();
    _nop_();

    SCL = 0;       /*SCL拉低后，SDA可以变化，准备发送或接收数据 */
    _nop_();
    _nop_();
}


/*******************************************************************
                      结束总线函数
函数原型: void  Stop_I2C();
功能:     结束I2C总线
********************************************************************/
void Stop_I2C()
{
    SDA = 0;      /*发送结束条件的数据信号，SDA拉低，等待被拉高*/
    _nop_();       /*发送结束条件的时钟信号*/

    SCL = 1;      /*SDA低电平保持时间大于4μs*/
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();

    SDA = 1;  /*SDA在SCL高电平时拉高，上升沿产生结束信号，I2C总线停止*/
    _nop_();
    _nop_();
    _nop_();
    _nop_();
}


/*******************************************************************
                 字节数据发送函数
函数原型: void  Send_Byte(unsigned char c);
功能:     将一字节数据c发送出去,可以是地址,也可以是数据
            发完后等待应答,并对此状态位进行操作.(不应答或非应答都使ack=0)
返回：    发送数据正常，ack=1; ack=0表示被控器无应答或损坏。
********************************************************************/
void  Send_Byte(unsigned char  c)
{
    unsigned char  i;

    for(i = 0; i < 8; i++)  // 8 bit
    {
        if(( c << i) & 0x80) SDA = 1;   /*判断发送位*/
        else  SDA=0;

        _nop_();
        SCL = 1;  /*置时钟线为高，SDA保持不变，通知被控器开始接收数据位*/
        _nop_();
        _nop_();             /*保证时钟高电平周期大于4μs*/
        _nop_();
        _nop_();
        _nop_();
        SCL = 0;   //SCL拉低，准备将下一位数据送上SDA
    }

    _nop_();
    _nop_();

    SDA = 1;  /*8位发送完后释放数据线，准备接收应答位*/
    _nop_();
    _nop_();

    SCL = 1;   //应答：在主机SCL高电平时，探测从机的SDA是否为低电平
    _nop_();
    _nop_();
    _nop_();

    if(SDA == 1) ack = 0;
    else ack = 1;        /*判断是否接收到应答信号*/

    SCL = 0;     //SCL拉低，等待其他数据传输
    _nop_();
    _nop_();
}

/*******************************************************************
                 字节数据接收函数
函数原型: unsigned char   Receive_Byte();
功能:        用来接收从器件传来的数据
            并判断总线错误(不发应答信号)
            发完后请用应答函数应答从机。
返回：       接受到的一字节数据
********************************************************************/
unsigned char   Receive_Byte()
{
    unsigned char  dat;
    unsigned char  i;

    dat = 0;
    SDA = 1;                       /*置数据线为输入方式*/
    for(i = 0; i < 8; i++)
    {
        _nop_();
        SCL = 0;  /*置时钟线为低，准备接收数据位*/
        _nop_();
        _nop_();   /*时钟低电平周期大于4.7μs，保证有足够时间改变SDA总线*/
        _nop_();
        _nop_();
        _nop_();

        SCL = 1;    /*置时钟线为高，保持SDA数据线上数据有效*/
        _nop_();
        _nop_();

        dat = dat << 1;   // 腾出最低位，用于接受新的数据
        if(SDA == 1)  dat = dat + 1;  /*读数据位,接收的数据位放入dat中 */
        _nop_();
        _nop_();
    }

    SCL = 0;
    _nop_();
    _nop_();

    return dat;
}

/********************************************************************
                     应答子函数
函数原型:  void Answer_I2C(bit flag);
功能:      主控器进行应答信号(可以是应答或非应答信号，由位参数flag决定)
                flag = 0, 应答信号；flag = 1， 非应答信号
********************************************************************/
void Answer_I2C(bit flag)
{

    if(flag == 0) SDA = 0; //flag = 0,  SDA拉低产生应答信号
    else SDA = 1;
    _nop_();
    _nop_();
    _nop_();

    SCL = 1;         //置SCL高电平，等待接受应答信号
    _nop_();
    _nop_();         /*时钟低电平周期大于4μs*/
    _nop_();
    _nop_();
    _nop_();

    SCL = 0; // SCL拉低，等待SDA上其他数据
    _nop_();
    _nop_();
}


//----------------------------------------------AT2402的功能函数

/*******************************************************************
                    向有子地址器件发送多字节数据函数
函数原型: bit Send_Str(unsigned char sla, unsigned char suba, unsigned char *s, unsigned char no);
功能:     从启动总线到发送地址，子地址,数据，结束总线的全过程
          从器件地址sla，子地址suba，发送内容是s指向的内容，发送no个字节。
返回：     如果返回1表示操作成功，否则操作有误。
注意：    使用前必须已结束总线。
********************************************************************/
bit Send_Str(unsigned char sla, unsigned char suba, unsigned char *s, unsigned char no)
{
    unsigned char i;

    Start_I2C();               /*启动总线*/

    Send_Byte(sla);             /*发送器件地址*/
    if(ack == 0) return 0;

    Send_Byte(suba);            /*发送器件子地址*/
    if(ack == 0) return 0;

    for(i = 0; i < no; i++)
    {
        Send_Byte(*s);            /*发送数据*/
        if(ack == 0) return 0;
        s++;
    }

    Stop_I2C();                /*结束总线*/

    return 1;
}

/*******************************************************************
                    向有子地址器件读取多字节数据函数
函数原型: bit Receive_Str(unsigned char sla,unsigned char suba,unsigned char *s,unsigned char no);
功能:     从启动总线到发送地址，子地址,读数据，结束总线的全过程,从器件
          地址sla，子地址suba，读出的内容放入s指向的存储区，读no个字节。
           如果返回1表示操作成功，否则操作有误。
注意：    使用前必须已结束总线。
********************************************************************/
bit Receive_Str(unsigned char sla,unsigned char suba,unsigned char *s,unsigned char no)
{
    unsigned char i;

    Start_I2C();                  /*启动总线*/

    Send_Byte(sla);                /*发送器件地址*/
    if(ack == 0)return 0;

    Send_Byte(suba);               /*发送器件子地址*/
    if(ack == 0)return 0;

    Start_I2C();                 /*已经片选器件成功，现在重新启动总线*/

    Send_Byte( sla + 1);    // 发送控制字节，最后一位改为高电平，表示读取
    if(ack == 0)return 0;


    for(i = 0; i < no - 1; i++)
    {
        *s = Receive_Byte();               /*读取数据*/
        Answer_I2C(0);                /*发送就答位*/
        s++;
    }
    *s = Receive_Byte();


    Answer_I2C(1);                   /*发送非应位，表示接收数据结束*/

    Stop_I2C();                   /*结束总线*/

    return 1;
}
