/**********************************************
 *
 * 		Author	:		Shawn Guo
 * 		Date	:		2013/8/14
 *      Last    :       2013/8/14
 * 		Tool    :	    MSP430
 * 		Notes	:		LCD5110驱动
 *
 *      1.VCC   2.GND   3.SCE   4.RST
 *      5.D/C   6.SDIN  7.SCLK  8.LED+
 *---------------------------------------------
 *
 *          SCE---> P1.0
 *          RST---> P1.1
 *          D/C---> P1.2
 *          SDIN--> P1.3
 *          SCLK--> P1.4
 *
 **********************************************/


#ifndef __LCD5110_H__            // 头文件保护策略
#define __LCD5110_H__

/**=======================LCD5110 I/O端口宏定义设置===========================*/

#define  LCD5110_CE_DIR     (P1DIR |= BIT0)
#define  LCD5110_CE_1       (P1OUT |= BIT0)
#define  LCD5110_CE_0       (P1OUT &=~BIT0)      // 低电平有效使能

#define  LCD5110_RST_DIR    (P1DIR |= BIT1)
#define  LCD5110_RST_1      (P1OUT |= BIT1)
#define  LCD5110_RST_0      (P1OUT &=~BIT1)     // 低电平有效

#define  LCD5110_DC_DIR     (P1DIR |= BIT2)
#define  LCD5110_DC_1       (P1OUT |= BIT2)
#define  LCD5110_DC_0       (P1OUT &=~BIT2)

#define  LCD5110_SDIN_DIR       (P1DIR |= BIT3)
#define  LCD5110_SDIN_1         (P1OUT |= BIT3)
#define  LCD5110_SDIN_0         (P1OUT &=~BIT3)

#define  LCD5110_SCLK_DIR       (P1DIR |= BIT4)
#define  LCD5110_SCLK_1         (P1OUT |= BIT4)      // 注意：这里可能作者搞错了，写成了P2OUT ^_^#
#define  LCD5110_SCLK_0         (P1OUT &=~BIT4)        // SCLK频率范围0~4MHZ
//时钟周期:最小250ns
// ===============================精确延时宏定义=========================
#define CPU_F ((double)1000000)        // 这里的CPU_F需要自己配置，这里配置为1MHZ
#define DELAY_US(x) __delay_cycles((long)(CPU_F*(double)x/1000000.0))
#define DELAY_MS(x) __delay_cycles((long)(CPU_F*(double)x/1000.0))

// =======================LCD5110 功能函数===========================
void LCD5110_IO();    // I/O口配置
void LCD5110_Init();      // LCD5110初始化
void LCD5110_Clear(void);      // LCD5110 RAM清零
void LCD5110_Write_String(unsigned char X,unsigned char Y,char *s); // 写入字符串
void LCD5110_Write_Char(unsigned char c);   // 写入ASCII字符


// 给LCD5110写入数据/命令。 当command = 0，即写入命令，命令内容为dat
//             当command = 1,即写入数据，数据内容为dat
void LCD5110_Write_Byte(unsigned char dat, unsigned char dc);   // 写入一字节数据/命令

void LCD5110_Set_XY(unsigned char X, unsigned char Y);  //设置位置离开(X,Y)

#endif
