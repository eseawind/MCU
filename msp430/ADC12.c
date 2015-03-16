/**********************************************
*
* 		Author	:		Shawn Guo
*      Mail    :       iguoxiaopeng@gmail.com
* 		Date	:       2013/8/10
*      Last    :
* 		Notes	:       ADC12
* 		Tool    :       ADC12 for MSP430X2XX
**********************************************/

/*****************************************************************************
* @file       ADC12.c
* @addtogroup ADC12
* @{
******************************************************************************/
#include <msp430f2618.h>
#include "ADC12.h"

void ADC12_Init(unsigned char clk, unsigned char div, unsigned char mode, unsigned char shtime)
{

    ADC12_Set_CLK(clk, div);
    ADC12_Set_Mode(mode);
    switch(shtime)  //ADC12 Sample-and-hold time.
    {
    case 0:
        ADC12CTL0 &= ~(0xff00u);
        break;
    case 1:
        ADC12CTL0 &= ~(0xee00u);
        ADC12CTL0 |=  (0x1100u);
        break;
    case 2:
        ADC12CTL0 &= ~(0xdd00u);
        ADC12CTL0 |=  (0x2200u);
        break;
    case 3:
        ADC12CTL0 &= ~(0xcc00u);
        ADC12CTL0 |=  (0x3300u);
        break;
    case 4:
        ADC12CTL0 &= ~(0xbb00u);
        ADC12CTL0 |=  (0x4400u);
        break;
    case 5:
        ADC12CTL0 &= ~(0xaa00u);
        ADC12CTL0 |=  (0x5500u);
        break;
    case 6:
        ADC12CTL0 &= ~(0x9900u);
        ADC12CTL0 |=  (0x6600u);
        break;
    case 7:
        ADC12CTL0 &= ~(0x8800u);
        ADC12CTL0 |=  (0x7700u);
        break;
    case 8:
        ADC12CTL0 &= ~(0x7700u);
        ADC12CTL0 |=  (0x8800u);
        break;
    case 9:
        ADC12CTL0 &= ~(0x6600u);
        ADC12CTL0 |=  (0x9900u);
        break;
    case 10:
        ADC12CTL0 &= ~(0x5500u);
        ADC12CTL0 |=  (0xaa00u);
        break;
    case 11:
        ADC12CTL0 &= ~(0x4400u);
        ADC12CTL0 |=  (0xbb00u);
        break;
    case 12:
        ADC12CTL0 &= ~(0x3300u);
        ADC12CTL0 |=  (0xcc00u);
        break;
    case 13:
        ADC12CTL0 &= ~(0x2200u);
        ADC12CTL0 |=  (0xdd00u);
        break;
    case 14:
        ADC12CTL0 &= ~(0x1100u);
        ADC12CTL0 |=  (0xee00u);
        break;
    case 15:
        ADC12CTL0 |=  (0xff00u);
        break;
    }

    ADC12CTL0 |= ADC12ON;   //开启ADC12
}

void ADC12_Set_VREF2_5() // ADC12 Reference generator voltage = 2.5V
{
    ADC12CTL0 |=  REFON;
    ADC12CTL0 |=  REF2_5V;
}

void ADC12_Set_VREF1_5() // ADC12 REF = 1.5V
{
    ADC12CTL0 |=  REFON;
    ADC12CTL0 &= ~REF2_5V;
}

void ADC12_Set_Address(unsigned char address)  // ADC12 Conversion Start address.
{
    switch(address)
    {
    case 0:
        ADC12CTL1 &= ~(0xf000u);
        break;
    case 1:
        ADC12CTL1 &= ~(0xe000u);
        ADC12CTL1 |=  (0x1000u);
        break;
    case 2:
        ADC12CTL1 &= ~(0xd000u);
        ADC12CTL1 |=  (0x2000u);
        break;
    case 3:
        ADC12CTL1 &= ~(0xc000u);
        ADC12CTL1 |=  (0x3000u);
        break;
    case 4:
        ADC12CTL1 &= ~(0xb000u);
        ADC12CTL1 |=  (0x4000u);
        break;
    case 5:
        ADC12CTL1 &= ~(0xa000u);
        ADC12CTL1 |=  (0x5000u);
        break;
    case 6:
        ADC12CTL1 &= ~(0x9000u);
        ADC12CTL1 |=  (0x6000u);
        break;
    case 7:
        ADC12CTL1 &= ~(0x8000u);
        ADC12CTL1 |=  (0x7000u);
        break;
    case 8:
        ADC12CTL1 &= ~(0x7000u);
        ADC12CTL1 |=  (0x8000u);
        break;
    case 9:
        ADC12CTL1 &= ~(0x6000u);
        ADC12CTL1 |=  (0x9000u);
        break;
    case 10:
        ADC12CTL1 &= ~(0x5000u);
        ADC12CTL1 |=  (0xa000u);
        break;
    case 11:
        ADC12CTL1 &= ~(0x4000u);
        ADC12CTL1 |=  (0xb000u);
        break;
    case 12:
        ADC12CTL1 &= ~(0x3000u);
        ADC12CTL1 |=  (0xc000u);
        break;
    case 13:
        ADC12CTL1 &= ~(0x2000u);
        ADC12CTL1 |=  (0xd000u);
        break;
    case 14:
        ADC12CTL1 &= ~(0x1000u);
        ADC12CTL1 |=  (0xe000u);
        break;
    case 15:
        ADC12CTL1 |=  (0xf000u);
        break;
    }

}

void ADC12_Sample_Source(unsigned char shs, unsigned char shp)  // Sample-and-hold source select
{
    switch(shs)
    {
    case 0:
        ADC12CTL1 &= ~(0x0c00u);
        break;
    case 1:
        ADC12CTL1 &= ~(0x0800u);
        ADC12CTL1 |=  (0x0400u);
        break;
    case 2:
        ADC12CTL1 |=  (0x0800u);
        ADC12CTL1 &= ~(0x0400u);
        break;
    case 3:
        ADC12CTL1 |=  (0x0800u);
        ADC12CTL1 |=  (0x0400u);
        break;
    }

    if(shp)
    {
        ADC12CTL1 |= SHP;
    }
}

void ADC12_Set_CLK(unsigned char clk, unsigned char div)
{
    switch(clk)
    {
    case 0:
        ADC12CTL1 &= ~(0x0018u);
        break;
    case 1:
        ADC12CTL1 &= ~(0x0010u);
        ADC12CTL1 |=  (0x0008u);
        break;
    case 2:
        ADC12CTL1 &= ~(0x0008u);
        ADC12CTL1 |=  (0x0010u);
        break;
    case 3:
        ADC12CTL1 |=  (0x0018u);
        break;
    }
    switch(div)
    {
    case 1:
        ADC12CTL1 &= ~(0x00e0u);
        break;
    case 2:
        ADC12CTL1 &= ~(0x00c0u);
        ADC12CTL1 |=  (0x0020u);
        break;
    case 3:
        ADC12CTL1 &= ~(0x00a0u);
        ADC12CTL1 |=  (0x0040u);
        break;
    case 4:
        ADC12CTL1 &= ~(0x0080u);
        ADC12CTL1 |=  (0x0060u);
        break;
    case 5:
        ADC12CTL1 &= ~(0x0060u);
        ADC12CTL1 |=  (0x0080u);
        break;
    case 6:
        ADC12CTL1 &= ~(0x0040u);
        ADC12CTL1 |=  (0x00a0u);
        break;
    case 7:
        ADC12CTL1 &= ~(0x0020u);
        ADC12CTL1 |=  (0x00c0u);
        break;
    case 8:
        ADC12CTL1 |=  (0x00e0u);
        break;

    }
}

void ADC12_Set_Mode(unsigned char mode)
{
    switch(mode)
    {
    case 0:
        ADC12CTL1 &= ~(0x0006);
        ADC12CTL0 &= ~MSC;
        break;
    case 1:
        ADC12CTL1 &= ~(0x0004);
        ADC12CTL1 |=  (0x0002);
        ADC12CTL0 |= MSC;
        break;
    case 2:
        ADC12CTL1 &= ~(0x0002);
        ADC12CTL1 |=  (0x0004);
        ADC12CTL0 |= MSC;           //Multiple sample and conversion.
        break;
    case 3:
        ADC12CTL1 |=  (0x0006);
        ADC12CTL0 |= MSC;
        break;
    }
}


//选择内存单元ADC12MCTL0~ADC12MCTL15,选择参考，选择通道
void ADC12_MCTL(unsigned char mctl, unsigned char svref, unsigned char channel)
{
    unsigned char mctlx = 0;
    switch(svref)
    {
    case 0:
        mctlx &= ~(0x70u);
        break;
    case 1:
        mctlx &= ~(0x60u);
        mctlx |=  (0x10u);
        break;
    case 2:
        mctlx &= ~(0x50u);
        mctlx |=  (0x20u);
        break;
    case 3:
        mctlx &= ~(0x40u);
        mctlx |=  (0x30u);
        break;
    case 4:
        mctlx &= ~(0x30u);
        mctlx |=  (0x40u);
        break;
    case 5:
        mctlx &= ~(0x20u);
        mctlx |=  (0x50u);
        break;
    case 6:
        mctlx &= ~(0x10u);
        mctlx |=  (0x60u);
        break;
    case 7:
        mctlx |=  (0x70u);
        break;
    }

    switch(channel)
    {
    case 0:
        mctlx &= ~(0x0fu);
        break;
    case 1:
        mctlx &= ~(0x0eu);
        mctlx |=  (0x01u);
        break;
    case 2:
        mctlx &= ~(0x0du);
        mctlx |=  (0x02u);
        break;
    case 3:
        mctlx &= ~(0x0cu);
        mctlx |=  (0x03u);
        break;
    case 4:
        mctlx &= ~(0x0bu);
        mctlx |=  (0x04u);
        break;
    case 5:
        mctlx &= ~(0x0au);
        mctlx |=  (0x05u);
        break;
    case 6:
        mctlx &= ~(0x09u);
        mctlx |=  (0x06u);
        break;
    case 7:
        mctlx &= ~(0x08u);
        mctlx |=  (0x07u);
        break;
    case 8:
        mctlx &= ~(0x07u);
        mctlx |=  (0x08u);
        break;
    case 9:
        mctlx &= ~(0x06u);
        mctlx |=  (0x09u);
        break;
    case 10:
        mctlx &= ~(0x05u);
        mctlx |=  (0x0au);
        break;
    case 11:
        mctlx &= ~(0x04u);
        mctlx |=  (0x0bu);
        break;
    case 12:
        mctlx &= ~(0x03u);
        mctlx |=  (0x0cu);
        break;
    case 13:
        mctlx &= ~(0x02u);
        mctlx |=  (0x0du);
        break;
    case 14:
        mctlx &= ~(0x01u);
        mctlx |=  (0x0eu);
        break;
    case 15:
        mctlx |=  (0x0fu);
        break;
    }

    switch(mctl)
    {
    case 0:
        ADC12MCTL0 = mctlx;
        break;
    case 1:
        ADC12MCTL1 = mctlx;
        break;
    case 2:
        ADC12MCTL2 = mctlx;
        break;
    case 3:
        ADC12MCTL3 = mctlx;
        break;
    case 4:
        ADC12MCTL4 = mctlx;
        break;
    case 5:
        ADC12MCTL5 = mctlx;
        break;
    case 6:
        ADC12MCTL6 = mctlx;
        break;
    case 7:
        ADC12MCTL7 = mctlx;
        break;
    case 8:
        ADC12MCTL8 = mctlx;
        break;
    case 9:
        ADC12MCTL9 = mctlx;
        break;
    case 10:
        ADC12MCTL10 = mctlx;
        break;
    case 11:
        ADC12MCTL11 = mctlx;
        break;
    case 12:
        ADC12MCTL12 = mctlx;
        break;
    case 13:
        ADC12MCTL13 = mctlx;
        break;
    case 14:
        ADC12MCTL14 = mctlx;
        break;
    case 15:
        ADC12MCTL15 = mctlx;
        break;

    }


}

//使能ADC12中15个中断源的某个中断
void ADC12_Enable(unsigned int adc12ie)
{
    ADC12IE  =   adc12ie;
    ADC12IFG &= ~adc12ie;
}

unsigned int ADC12_T(unsigned int vtemp)  //输入ADC12温度传感器采样出来的毫伏级电压，转化为温度的100倍显示出来.
{
    return ((vtemp - 986) * 200 / 7);
}


/***************************************************************************//**
 * @brief
 * @param
 * @return none
 *******************************************************************************/


/***************************************************************************//**
 * @}
 ******************************************************************************/
