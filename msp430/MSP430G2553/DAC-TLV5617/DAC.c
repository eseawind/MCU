/**********************************************
 *
 * 		Author	:		Shawn Guo
 * 		Date	:		2013/4/24
 *      Last    :       2013/7/17
 * 		Tool    :	    MSP430G2553
 * 		Notes	:		TLV5617 DAC驱动
 *
 *      1.SDIN  2.SCLK  3.CS    4.OUTA
 *      5.AGND  6.REF   7.OUTB  8.VDD
 *---------------------------------------
 *          工作在SPI 3线模式
 *
 *          SDIN---->P1.2
 *          SCLK---->P1.4
 *          CE  ---->P2.0
 *      CE端口可以更改，需要更改头文件宏定义以及DAC_CE_IO()函数。
 *      7.17 注： 用作DDS时，在设置SPI中必须关闭SPI的中断
 **********************************************/
#include <msp430g2553.h>
#include "DAC.h"

const double DAC_VDD = 3.555;// 电源VDD与基准源REF配置
const double DAC_REF = 1.49;

void DAC_Sent_Bytes(unsigned int dat)// 通过SPI总线向TLV5617发送数据
{
    unsigned char  h8 = 0, l8 = 0;

    l8 = (unsigned char)dat;
    h8 = (unsigned char)(dat >> 8);

    DAC_CE_0;//拉低使能信号，片选TLV5617,待数据发送完成后再拉高CE信号

    IFG2 &= ~UCA0RXIFG;
    IFG2 &= ~UCA0TXIFG;
    UCA0TXBUF = h8;
    while(!(IFG2 & UCA0RXIFG));    //

    IFG2 &= ~UCA0RXIFG;
    IFG2 &= ~UCA0TXIFG;
    UCA0TXBUF = l8;
    while(!(IFG2 & UCA0RXIFG));
    IFG2 &= ~UCA0RXIFG;
    IFG2 &= ~UCA0TXIFG;

    DAC_CE_1; //拉高使能信号，使得转移寄存器中的数据移入目标锁存
/********************************************************************************
    执行说明（3线模式）：

a、将传输的串行数据DATA送给芯片内部的UCA0TXBUF (UCA0TXBUF = DATA；);

b、UCA0TXBUF中的数据立即送给芯片内部的发送移位寄存器，结束时IFG2 寄存器中的UCA0TXIFG发送中断标志会置位（只表明UCA0TXBUF中的数据全部移入了发送移位寄存器，UCA0TXBUF可以接收新的数据，但不表示从模块已完全接收采样）；

c、当发送移位寄存器中的数据全部移出，IFG2 寄存器中的UCA0RXIFG接收中断标志置位，（表明此次串行数据传输结束，可用于查询传输是否完成）

d、可继续重复上述执行

e、主模块的USCI接收从模块的数据，需要先写入数据到TXBUF中，因为接收和发送是同时发生的。

参考资料：To receive dat ainto the USCI in maste rmode,data must be written to UCxTXBUF because receive and transmit operations operate concurrently.

注意：当未使用发送USCIAB0TX_VECTOR和接收USCIAB0RX_VECTOR中断时，两个中断标志位UCA0TXIFG和UCA0RXIFG不会自动复位，需程序复位。

      当从模块无需或不会发送数据给主模块即单片机时，UCxSOMI可以不与从模块相连，可以空接悬浮，但必须让其正常工作在UCxSOMI功能。
********************************************************************************/
}

void DAC_PowerOff()   //进入掉电模式
{
    unsigned int dat = 0x2000;
    DAC_Sent_Bytes(dat);
}


void DAC_Sent_A(unsigned int dat, unsigned char mode)  //发送数据至OUTA端, 且用缓冲器的内容更新OUTB， mode = 1快速模式
{
    if(mode == 1) DAC_FAST_MODE(dat);
    dat |= 0x8000;
    dat &= ~0x1000; //这里采用默认以提高性能
    DAC_Sent_Bytes(dat);
}

void DAC_Sent_B(unsigned int dat, unsigned char mode)  //发送数据至OUTB端, mode = 1快速模式
{
    if(mode == 1) DAC_FAST_MODE(dat);
    dat &= ~0x8000;//一般dat都是从ROM取出的值，其值只有BIT11-BIT2有效，其他位为0
    dat &= ~0x1000;//这里采用默认以提高性能
    DAC_Sent_Bytes(dat);
}
//发送dat1至OUTA端，发送dat2至outb端,mode = 1快速模式
void DAC_Sent_AB(unsigned int dat1, unsigned char mode1, unsigned int dat2,  unsigned char mode2)
{
    //第一步：为DAC_B写入数据至缓冲器
    if(mode2 == 1) DAC_FAST_MODE(dat2);
    dat2 &= ~0x8000;//这里采用默认以提高性能
    dat2 |= 0x1000;
    DAC_Sent_Bytes(dat2);

    if(mode1 == 1) DAC_FAST_MODE(dat1);
    dat1 |= 0x8000;
    dat1 &= ~0x1000; //这里采用默认以提高性能
    DAC_Sent_Bytes(dat1);
}

 // 将制定电压转换为DAC对应的代码dat，错误的v将返回0x0000；
 // 返回数据范围: 0x0000~ 0x0ffc,且5617仅取11 - 2这十位数据，最后两位无效
 // v = 2 * DAC_REF * (CODE / 1024); 最后CODE左移两位，以满足TLV5617A的数据格式
unsigned int DAC_V2Code(double v)
{
    unsigned int dat = 0;

    if(v >= 2 * DAC_REF || v < 0) return 0;

    dat = (int)(v * 512 / DAC_REF);
    dat <<= 2;

    return dat;
}

void DAC_SPI_SET()           // 为DAC配置SPI总线,可以修改寄存器相应位，但时序不能变！
{
    UCA0CTL1 = UCSWRST; // UCSWRST置位，等待配置USCI寄存器，最后清除UCSWRST使得USCI可操作

    UCA0CTL0 |= UCMSB + UCMST + UCMODE_0+ UCSYNC;
/*************************  UCA0CTL0    ***************************************
    UCCKPH UCCKPL：设置SPI的CLK的时序即有效边沿
        UCCKPL控制CLK开始和结束的电平：1-高电平开始，0-低电平开始；
        UCCKPH控制CLK的有效边沿即触发串行数据从移位寄存器中移出的边沿：1-CLK脉冲的右边，0- CLK脉冲的左边。
            CLK时序选择建议：为保证从模块采样串行数据的正确性，应选择与从模块CLK有效边沿的相反边沿，先稳定串行数据，再采样。
            还应注意芯片的SPI的CLK中有多少上升沿和下降沿，确保数据都能被从模块采样，因为只有在传输串行数据时CLK才有脉冲，否则保持为0或1。
    UCMSB: 0 LSM first  1 MSB first设置串行数据高位优先或低位优先
    UC7BIT: 选择长度; 0 8-bit data, 1 7-bit data
    UCMST: 0: 从模式   1:主模式
    UCMODEx:    00  3-PIN SPI
                01 4-pin SPI with UCxSTE active high: slave enabled when UCxSTE = 1
                10 4-pin SPI with UCxSTE active low: slave enabled when UCxSTE = 0
                11 I2C mode
    UCSYNC: Synchronous mode enable
            0 Asynchronous mode
            1 Synchronous mode        // SPI是同步的
****************************************************************/

    UCA0CTL1 |= UCSSEL_2;// 设置UCA0CLK 源，可达到芯片主系统频率：SMCLK=16MHZ
/*************************  UCA0CTL1    ***************************************
    UCSSELx   USCI clock source select.
    主模式下为选择BRCLK source clock. 从模式下从UCxCLK输入时钟信号，无视UCSSELx.
    00  NA      01  ACLK       10   SMCLK      11   SMCLK

    UCSWRST     软件复位使能
    0 Disabled. USCI reset released for operation.
    1 复位使能. USCI 复位！
****************************************************************/

    UCA0BR0 =0;
    UCA0BR1 =0; //0分频 //两个8位寄存器设置分频率：UCA0BR1 为高8位，
/*************************  UCA0BRx    ***************************************
UCBRx 时钟分频设置，将BRCLK分频最后得到USCI的时钟
 分频因子(UCxxBR0 + UCxxBR1 × 256)
****************************************************************/

//配置UCA0的I/O口：
//P1.1----->UCA0SOMI
//P1.2----->UCA0SIMO     (不管此引脚是否与从模块连接，此引脚的设置不能少，可产生作为传输结束的标志)；
//P1.4----->UCA0CLK     只需将三个引脚的两个SEL寄存器置1
    P1SEL  = BIT1 + BIT2 + BIT4;
    P1SEL2 = BIT1 + BIT2 + BIT4;
    P1DIR |= BIT2 + BIT4;

/*************************  USCI端口配置   *********************
UCBRx 时钟分频设置，将BRCLK分频最后得到USCI的时钟
 分频因子(UCxxBR0 + UCxxBR1 × 256)
****************************************************************/

    UCA0CTL1 &= ~UCSWRST;      // 关闭“软件复位”使能，USCI恢复工作

    //IE2 |= UCA0RXIE;

    //IE2 |= UCA0TXIE;

    // Enable USCI0 RX interrupt, Enable USCI0 TX interrupt,不轻易使用

/*************************  中断配置  *********************
UCA0RXIE    接受数据中断使能
UCA0TXIE    发送数据中断使能
****************************************************************/

}

void DAC_CE_IO()      //配置DAC的使能IO端口
{
    P2DIR |= BIT0;
    P2OUT |= BIT0;
}

void DAC_Init()
{
    DAC_SPI_SET();
    DAC_CE_IO();
}
