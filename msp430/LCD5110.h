/**********************************************
 *
 * 		Author	:		Shawn Guo
 * 		Date	:		2013/8/14
 *      Last    :       2013/8/14
 *                      8/15   注意SPI的时序(必须第二个时钟沿才能改变数据),高位先发.
 * 		Tool    :	    MSP430
 * 		Notes	:		LCD5110驱动(水平寻址模式)
 *
 *      1.VCC   2.GND   3.SCE   4.RST
 *      5.D/C   6.SDIN  7.SCLK  8.LED+
 *      特点    1. MSB高位先发
 *              2. SDIN在SCLK上升沿采样
 *              3. D/C信号在第八个SCLK脉冲被读出
 *              4. 读完最后一位且SCE仍有效，则再下一个上升沿读取第二字节.
 *---------------------------------------------
 *
 *          SCE---> P7.5
 *          RST---> P7.4
 *          D/C---> P7.3
 *          SDIN--> P5.1
 *          SCLK--> P5.3
 *
 **********************************************/


#ifndef __LCD5110_H__            // 头文件保护策略
#define __LCD5110_H__

#define LCD5110_MONI_SPI        //使用模拟SPI端口

/**=======================LCD5110 I/O端口宏定义设置===========================*/

#define  LCD5110_CE_DIR         (P7DIR |= BIT5)
#define  LCD5110_CE_1           (P7OUT |= BIT5)
#define  LCD5110_CE_0           (P7OUT &=~BIT5)      // 低电平有效使能

#define  LCD5110_RST_DIR        (P7DIR |= BIT4)
#define  LCD5110_RST_1          (P7OUT |= BIT4)
#define  LCD5110_RST_0          (P7OUT &=~BIT4)     // 低电平有效

#define  LCD5110_DC_DIR         (P7DIR |= BIT3)
#define  LCD5110_DC_1           (P7OUT |= BIT3)
#define  LCD5110_DC_0           (P7OUT &=~BIT3)

#ifdef LCD5110_MONI_SPI

#define  LCD5110_SDIN_DIR       (P5DIR |= BIT1)     //IO端口模拟SPI
#define  LCD5110_SDIN_1         (P5OUT |= BIT1)
#define  LCD5110_SDIN_0         (P5OUT &=~BIT1)

#define  LCD5110_SCLK_DIR       (P5DIR |= BIT3)
#define  LCD5110_SCLK_1         (P5OUT |= BIT3)
#define  LCD5110_SCLK_0         (P5OUT &=~BIT3)

#else

//这里默认使用F2618的UCB1硬件SPI
#define  LCD5110_SDIN_IO        (P5SEL |= BIT1)
#define  LCD5110_SCLK_IO        (P5SEL |= BIT3)
#define  LCD5110_SCLK_FF        (CPU_FF)            //硬件SPI的时钟 单位:M

#endif
// SCLK频率范围0~4MHZ(实测8M速度可用)
//时钟周期:最小250ns


/** =======================LCD5110 功能函数===========================**/
void LCD5110_IO();    // I/O口配置
void LCD5110_Init();      // LCD5110初始化
void LCD5110_Clear(void);      // LCD5110 RAM清零

//设置LCD5110坐标函数
// X: 0-83      Y: 0-5
// 注意水平寻址与垂直寻址的区别
//显示字符串s.若S = 0, 则显示默认LCD5110_BUFFER. eg. LCD5110_Write_String(0,1,0);
void LCD5110_Write_String(unsigned char X,unsigned char Y,char *s); // 写入字符串
void LCD5110_Write_Char(unsigned char c);   // 写入ASCII字符

//将16位的long值转换为字符存储在LCD5110_BUFFER[]中.不考虑溢出
//共9字节，1字节符号位，8字节数值位
void LCD5110_Long2Char(long templong);  //16-bits


// 给LCD5110写入数据/命令。 当command = 0，即写入命令，命令内容为dat
//             当command = 1,即写入数据，数据内容为dat
void LCD5110_Write_Byte(unsigned char dat, unsigned char dc);   // 写入一字节数据/命令
void LCD5110_Set_XY(unsigned char X, unsigned char Y);  //设置位置离开(X,Y)
#endif
