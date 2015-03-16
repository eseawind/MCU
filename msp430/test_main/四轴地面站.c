/*********************************************************
 *
 * 		Author	:		Shawn Guo
 *      Mail    :       iguoxiaopeng@gmail.com
 * 		Date	:       2013/9/4
 *      Last    :       2013/9/4
 * 		Notes	:       四旋翼地面站
 *                      单片机按键对应响应的功能,
 *                      发射信号给四轴,四轴接受信号以后, 执行响应操作.
 *
 * 		Tool    :       MSP430X2XX  / NRF905
 ********************************************************/
#include <msp430f2618.h>
#include "clock.h"
#include "LCD5110.h"
#include "USCI.h"
#include "NRF905.h"


/************************地面站命令******************************
*   1. 自动起飞(0x01):  起飞四旋翼, 将自动进入悬停模式, 保持自稳.
*   2. 失控降落(0x02):  调整四旋翼进入失控模式, 自动降落.
*   3. 强制关机(0x03):  立刻关闭四旋翼电机, 并强制死循环.
*
*
*
******************************************************************/
#define     CODE_AUTO_LAUNCH         (0x01)
#define     CODE_AUTO_LAND           (0x02)
#define     CODE_POWEROFF            (0x03)




/*************NRF905无线模块 全局变量*****************/

//RXADDRESS: 飞行器无线地址 0xaa,0xbb,0xcc,0xdd
//TXADDRESS: 地面站无线地址 0xdd,0xcc,0xbb,0xaa
unsigned char NRF905_MAIN_BUFFER[10];
unsigned char NRF905_TXDATA[2] = {0x00, 0x00};
//发送标志位，当标志位值1时，发送NRF905_TXDATA.
unsigned char NRF905_MAIN_SEND_FLAG = 0;


unsigned char MAIN_FLAG = 0;

int main()
{
    WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT

    Clock_Init(4,4,0,CPU_FF); // 12M DCO

    //开启按键中断, 按键后对应端口被拉低.
    P1DIR &= ~(BIT0 + BIT1 + BIT2 + BIT3);
    P1REN |=  (BIT0 + BIT1 + BIT2 + BIT3);
    P1OUT |=  (BIT0 + BIT1 + BIT2 + BIT3);
    P1SEL &= ~(BIT0 + BIT1 + BIT2 + BIT3);  //下降沿触发中断
    P1IE  |=  (BIT0 + BIT1 + BIT2 + BIT3);
    P1IFG &= ~(BIT0 + BIT1 + BIT2 + BIT3);

    LCD5110_Init();

    NRF905_Init();
    NRF905_Set_RevAddress(0xaa, 0xbb, 0xcc, 0xdd);//配置空间站的接受地址.
    NRF905_Set_SendAddress(0xdd, 0xcc, 0xbb, 0xaa);//配置发送地址: 地面站

    NRF905_Write_Data(NRF905_WTP, NRF905_TXDATA, 2);    //打包数据,准备发射
    NRF905_Set_Mode(NRF905_SEND);   //切换到发送模式

    _EINT();

    while(1)
    {
        if(NRF905_MAIN_SEND_FLAG)   //发送数据
        {
            NRF905_MAIN_SEND_FLAG = 0;
            NRF905_Write_Data(NRF905_WTP, NRF905_TXDATA, 2);    //打包数据,准备发射
            NRF905_Set_Mode(NRF905_SEND);   //切换到发送模式

            switch(NRF905_MAIN_SEND_FLAG)
            {
            case 1:
                LCD5110_Write_String(0,0,"CODE_AUTO_LAUNCH");
                break;
            case 2:
                LCD5110_Write_String(0,0,"CODE_AUTO_LAND");
                break;
            case 3:
                LCD5110_Write_String(0,0,"CODE_POWEROFF");
                break;
            }
        }
        DELAY_MS(1000);
    }
}

#pragma vector=PORT1_VECTOR
__interrupt void P1_ISR (void)
{
    DELAY_MS(5);//去抖延时

    if((P1IN & BIT0) == 0)        // KEY0
    {
        NRF905_TXDATA[0] = CODE_AUTO_LAUNCH;
        NRF905_MAIN_SEND_FLAG = 1;

    }
    else if((P1IN & BIT1) == 0)   // KEY1
    {
        NRF905_TXDATA[0] = CODE_AUTO_LAND;
        NRF905_MAIN_SEND_FLAG = 2;
    }
    else if((P1IN & BIT2) == 0)   // KEY2
    {
        NRF905_TXDATA[0] = CODE_POWEROFF;
        NRF905_MAIN_SEND_FLAG = 3;
    }
    else if((P1IN & BIT3) == 0)   // KEY3 , 功能同KEY2
    {
        NRF905_TXDATA[0] = CODE_POWEROFF;
        NRF905_MAIN_SEND_FLAG = 3;
    }

    P1IFG = 0;
}
