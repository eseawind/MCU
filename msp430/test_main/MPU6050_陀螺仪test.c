/**********************************************
 *
 * 		Author	:		Shawn Guo
 *      Mail    :       iguoxiaopeng@gmail.com
 * 		Date	:
 *      Last    :
 * 		Notes	:       MPU6050 加速度计部分test. 正常工作.
 * 		Tool    :
 **********************************************/



#include <msp430f2618.h>
#include "clock.h"
#include "LCD5110.h"
#include "MPU6050.h"
#include "math.h"


char LCD_MAIN_BUFFER1[] = "GX:";
char LCD_MAIN_BUFFER2[] = "GY:";
char LCD_MAIN_BUFFER3[] = "GZ:";

void main()
{
    WDTCTL = WDTPW + WDTHOLD;
    Clock_Init(0,0,0,CPU_FF); // 12M DCO

    LCD5110_Init();

    LCD5110_Write_String(0,0,LCD_MAIN_BUFFER1);
    LCD5110_Write_String(0,1,LCD_MAIN_BUFFER2);
    LCD5110_Write_String(0,2,LCD_MAIN_BUFFER3);


    MPU6050_Init();


    while(1)
    {
        MPU6050_AnguX();
        LCD5110_Long2Char(MPU6050_GX);
        LCD5110_Write_String(18,0,0);

        MPU6050_AnguY();
        LCD5110_Long2Char(MPU6050_GY);
        LCD5110_Write_String(18,1,0);

        MPU6050_AnguZ();
        LCD5110_Long2Char(MPU6050_GZ);
        LCD5110_Write_String(18,2,0);

        DELAY_MS(500);

    }

}


