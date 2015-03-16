/**********************************************
*
* 		Author	:		Shawn Guo
*       Mail    :       iguoxiaopeng@gmail.com
* 		Date	:       2013/9/3
*       Last    :       2013/9/3
* 		Notes	:       CJMCU_TEST
* 		Tool    :       MSP430F2618 / CJMCU
*
*
**********************************************/


#include "CJMCU.h"
#include "clock.h"
#include "LCD5110.h"

#include <msp430f2618.h>

extern unsigned char CJMCU_RCV_FLAG;   //只有当该值为1时，才可读取数据。也可以延时一段时间，确保该值为1，然后读取数据

int yaw = 0, pitch = 0, roll = 0 ;    //三个俯仰角

void main()
{
    WDTCTL =WDTPW + WDTHOLD;

    Clock_Init(4,4,0,CPU_FF);
    CJMCU_INIT();
    LCD5110_Init();

    _EINT();         //必须开中断

    DELAY_MS(3000);


    CJMCU_Aline();  //自动校准
    LCD5110_Long2Char(CJMCU_YAW_OFFSET);
    LCD5110_Write_String(0,0,0);
    LCD5110_Long2Char(CJMCU_PITCH_OFFSET);
    LCD5110_Write_String(0,1,0);
    LCD5110_Long2Char(CJMCU_ROLL_OFFSET);
    LCD5110_Write_String(0,2,0);

    DELAY_MS(3000);


    while(1)
    {
        CJMCU_START(); //只读取一次。每次读取数据时都要调用该函数
        while(CJMCU_RCV_FLAG==0);
        CJMCU_GET_VALUE(&yaw, &pitch, &roll);

        LCD5110_Long2Char(yaw);
        LCD5110_Write_String(0,0,0);
        LCD5110_Long2Char(pitch);
        LCD5110_Write_String(0,1,0);
        LCD5110_Long2Char(roll);
        LCD5110_Write_String(0,2,0);

        DELAY_US(10);
    }                         // Enable USCI_A0 RX interrupt
}

