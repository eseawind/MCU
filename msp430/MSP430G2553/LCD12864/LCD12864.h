/**********************************************
 *
 * 		Author	:		Shawn Guo
 * 		Date	:		2013/5/10
 *      Last    :       2013/5/15
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


#ifndef __LCD12864__            // 头文件保护策略
#define __LCD12864__

// =======================LCD I/O端口宏定义设置===========================
#define  LCD_CS_1       (P1OUT |= BIT7)
#define  LCD_CS_0       (P1OUT &=~BIT7)      // 高电平使能有效

#define  LCD_SDIN_1     (P1OUT |= BIT6)
#define  LCD_SDIN_0     (P1OUT &=~BIT6)

#define  LCD_SCLK_1     (P2OUT |= BIT5)
#define  LCD_SCLK_0     (P2OUT &=~BIT5)
// =======================LCD 功能函数===========================
void LCD_IO_set(void);    // I/O口配置
void LCD_Write_Byte(unsigned char c);   // 写入一个字节
void LCD_Write_Data(unsigned char dat); // 写入一个字节数据
void LCD_Write_Com(unsigned char com); // 写入一个字节命令
void LCD_Init(void);      // LCD初始化
void LCD_Txt_Clear(void);      // LCD text
void LCD_Bmp_Clear(void);      // LCD 图像 RAM清零是
void LCD_Write_String(unsigned char raw, unsigned char col, char *str); // 写入字符串, col(0~7),row(0~3)
void LCD_Write_Graph(unsigned char *graph); //绘图（128*32 + 128*32），数组大小为16字节*64行
void LCD_Set_XY(unsigned char x, unsigned char y);  //设置位置离开(X,Y)
// x范围为0~127
// y范围为0~64
void LCD_Set_Graph_XY(unsigned char x, unsigned char y); // 设置图像的(X,Y),假定已经为图像模式
void LCD_Display_4Num(unsigned char x, unsigned char y, unsigned int num); //显示一个四位数字
void LCD_Display_4_2flout(unsigned char x, unsigned char y, double num); //显示一个四位double,二位精度

// 以(x,y)为圆心，画出X坐标轴，向右为正方向，注意x轴主干上下均有3行箭头
// 设定圆心时需要考虑箭头占的地方
// x:3~28, y: 0~15

void LCD_Graph_Xaxis(unsigned char x, unsigned char y, unsigned char length);
// 以(x,y)为圆心，画出Y坐标轴，向下为正方向，注意Y轴主干左右均有3行箭头
// 设定圆心时需要考虑箭头占的地方
// x:0~32, y: 1~14
// 以(x,y)为圆心，画出Y坐标轴，向下为正方向，注意Y轴主干左右均有3行箭头
// 设定圆心时需要考虑箭头占的地方
// x:0~32, y: 1~14, length: 0~63
void LCD_Graph_Yaxis(unsigned char x, unsigned char y, unsigned char length);

//一共可以打印64个点而互不影响，即按行打印点
void LCD_Point(unsigned char x, unsigned char y);  //在128*64上画出这个点，注意LCD12864的写入规律以避免覆盖




// 显示汉字，显示汉字的区域
// 行 0~3  列 0 ~ 8
void LCD_Display_HanZi(unsigned char x, unsigned char y, char *str); //打印汉字

extern const unsigned char HZ_TABLE[];
#endif
