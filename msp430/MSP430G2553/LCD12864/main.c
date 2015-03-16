
#include <msp430g2553.h>
#include "LCD12864.h"

void main()
{
	int i = 0;

    WDTCTL = WDTPW + WDTHOLD;

    LCD_Init();
    LCD_Txt_Clear();
    LCD_Bmp_Clear();

    LCD_Write_Com(0x34);      //8Bit扩充指令集,即使是36H也要写两次
    LCD_Write_Com(0x36);      //绘图ON,基本指令集里面36H不能开绘图

    LCD_Graph_Yaxis(0,0,63);
    LCD_Graph_Xaxis(3, 0, 8);
    LCD_Point(100, 4);
    LCD_Point(100, 5);
    LCD_Point(99, 6);
    for(i = 1; i < 40; i++)
    	LCD_Point(99 - i , i + 6);


}
