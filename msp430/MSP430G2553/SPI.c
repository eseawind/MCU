

void USCI_SPI_SET()
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
