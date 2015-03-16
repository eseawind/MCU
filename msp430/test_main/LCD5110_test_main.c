/**********************************************
 *
 * 		Author	:		Shawn Guo
 *      Mail    :       iguoxiaopeng@gmail.com
 * 		Date	:       2013/8/14
 *      Last    :       2013/8/14
 * 		Notes	:       lcd5110_test
 * 		Tool    :       MSP430F2618
 **********************************************/
#include <msp430f2618.h>
#include "clock.h"
#include "LCD5110.h"

void main()
{
    char LCD_BUFFER1[]="  jupengfei";
    char LCD_BUFFER2[]="  jupengfei";
    char LCD_BUFFER3[]="  jupengfei";
    char LCD_BUFFER4[]="  jupengfei";

    WDTCTL = WDTPW + WDTHOLD;
    Clock_Init(0,0,0,1); // 1M DCO

    LCD5110_Init();
    LCD5110_Write_String(0,0, LCD_BUFFER1);
    LCD5110_Write_String(0,1, LCD_BUFFER2);
    LCD5110_Write_String(0,2, LCD_BUFFER3);
    LCD5110_Write_String(0,3, LCD_BUFFER4);
    LCD5110_Write_String(0,3, "@");

    while(1);
}
