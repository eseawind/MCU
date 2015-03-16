/**********************************************
 *
 *      Author  :       Shawn Guo
 *      Mail    :       iguoxiaopeng@gmail.com
 *      Date    :       2013/8/3
 *      Last    :
 *      Notes   :       DAC12
 *      Tool    :       MSP430X2XX
 **********************************************/

/*****************************************************************************
* @file       DAC12.c
* @addtogroup DAC12
* @{
******************************************************************************/
#include <msp430f2618.h>
#include "clock.h"
#include "DAC12.h"

void DAC12_Init()
{
    //DAC12_SetOutput(0);
    //DAC12_SetRef(0, 0);
    //DAC12_SetRes(0, 0);
    //DAC12_SetLsel(0, 0);
    DAC12_Calon(1);
    DAC12_SetIR(1, 1);
    DAC12_SetAMP(1, 5);
    //DAC12_SetDF(0, 0);
    //DAC12_Enabled(0);
    //DAC12_ENC(0);
    //DAC12_Group(0);

}


unsigned int DAC12_V2Int_121(unsigned long int v100)
{
    return ((v100 << 12) / VOLTAGE_REF);
}

unsigned int DAC12_V2Int_123(unsigned long int v100)
{
    return ((v100 << 12) / (VOLTAGE_REF * 3));
}

unsigned int DAC12_V2Int_81(unsigned long int v100)
{
    return ((v100 << 8) / VOLTAGE_REF);
}

unsigned int DAC12_V2Int_83(unsigned long int v100)
{
    return ((v100 << 12) / (VOLTAGE_REF * 3));
}

void DAC12_SetOutput(unsigned char ops)
{
    if(ops == 0)
    {
        DAC12_0CTL  &=  ~(0x8000u);//bit15
        DAC12_1CTL  &=  ~(0x8000u);
    }
    else
    {
        DAC12_0CTL  |=   (0x8000u);//bit15
        DAC12_1CTL  |=   (0x8000u);
    }
}

void DAC12_SetRef(unsigned char dac, unsigned char vref)
{
    if(dac == 0)
    {
        if(vref == 0)   //VREF+
        {
            DAC12_0CTL  &= ~(0x6000u);//bit14,bit13
        }
        else            //VeREF+
        {
            DAC12_0CTL  |=  (0x6000u);
        }
    }
    else
    {
        if(vref == 0)   //VREF+
        {
            DAC12_1CTL  &= ~(0x6000u);//bit14,bit13
        }
        else            //VeREF+
        {
            DAC12_1CTL  |=  (0x6000u);
        }
    }
}

void DAC12_SetRes(unsigned char dac, unsigned char bits)
{
    if(dac == 0)
    {
        if(bits == 0)   //12-bit
        {
            DAC12_0CTL  &= ~(0x1000u);//bit12
        }
        else            //8-bit
        {
            DAC12_0CTL  |=  (0x1000u);
        }
    }
    else
    {
        if(bits == 0)   //12-bit
        {
            DAC12_1CTL  &= ~(0x1000u);//bit12
        }
        else            //8-bit
        {
            DAC12_1CTL  |=  (0x1000u);
        }
    }
}

void DAC12_SetLsel(unsigned char dac, unsigned char lsel)
{
    if(dac == 0)
    {
        if(lsel == 0)
        {
            DAC12_0CTL &= ~(0x0800u);//bit11
            DAC12_0CTL &= ~(0x0400u);//bit10
        }
        else if(lsel == 1)
        {
            DAC12_0CTL &= ~(0x0800u);//bit11
            DAC12_0CTL |=  (0x0400u);//bit10
        }
        else if(lsel == 2)
        {
            DAC12_0CTL |=  (0x0800u);//bit11
            DAC12_0CTL &= ~(0x0400u);//bit10
        }
        else
        {
            DAC12_0CTL |=  (0x0800u);//bit11
            DAC12_0CTL |=  (0x0400u);//bit10
        }
    }
    else
    {
        if(lsel == 0)
        {
            DAC12_1CTL &= ~(0x0800u);//bit11
            DAC12_1CTL &= ~(0x0400u);//bit10
        }
        else if(lsel == 1)
        {
            DAC12_1CTL &= ~(0x0800u);//bit11
            DAC12_1CTL |=  (0x0400u);//bit10
        }
        else if(lsel == 2)
        {
            DAC12_1CTL |=  (0x0800u);//bit11
            DAC12_1CTL &= ~(0x0400u);//bit10
        }
        else
        {
            DAC12_1CTL |=  (0x0800u);//bit11
            DAC12_1CTL |=  (0x0400u);//bit10
        }
    }
}

void DAC12_Calon(unsigned char dac)
{
    if(dac == 0)
    {
        DAC12_0CTL |= (0x0200u);    //bit9
    }
    else
    {
        DAC12_1CTL |= (0x0200u);    //bit9
    }
}

void DAC12_SetIR(unsigned char dac, unsigned char range)
{
    if(dac == 0)
    {
        if(range == 0)  // 3x vref.
        {
            DAC12_0CTL &= ~(0x0100u);    //bit8
        }
        else            //  1x vref.
        {
            DAC12_0CTL |=  (0x0100u);    //bit8
        }
    }
    else
    {
        if(range == 0)  // 3x vref.
        {
            DAC12_1CTL &= ~(0x0100u);    //bit8
        }
        else            //  1x vref.
        {
            DAC12_1CTL |=  (0x0100u);    //bit8
        }
    }
}

void DAC12_SetAMP(unsigned char dac, unsigned char amp)
{
    if(dac == 0)
    {
        switch(amp)
        {
        case 0:
            DAC12_0CTL &= ~(0x0080u);   //bit7
            DAC12_0CTL &= ~(0x0040u);   //bit6
            DAC12_0CTL &= ~(0x0020u);   //bit5
            break;
        case 1:
            DAC12_0CTL &= ~(0x0080u);
            DAC12_0CTL &= ~(0x0040u);
            DAC12_0CTL |=  (0x0020u);
            break;
        case 2:
            DAC12_0CTL &= ~(0x0080u);
            DAC12_0CTL |=  (0x0040u);
            DAC12_0CTL &= ~(0x0020u);
            break;
        case 3:
            DAC12_0CTL &= ~(0x0080u);
            DAC12_0CTL |=  (0x0040u);
            DAC12_0CTL |=  (0x0020u);
            break;
        case 4:
            DAC12_0CTL |=  (0x0080u);
            DAC12_0CTL &= ~(0x0040u);
            DAC12_0CTL &= ~(0x0020u);
            break;
        case 5:
            DAC12_0CTL |=  (0x0080u);
            DAC12_0CTL &= ~(0x0040u);
            DAC12_0CTL |=  (0x0020u);
            break;
        case 6:
            DAC12_0CTL |=  (0x0080u);
            DAC12_0CTL |=  (0x0040u);
            DAC12_0CTL &= ~(0x0020u);
            break;
        case 7:
            DAC12_0CTL |=  (0x0080u);
            DAC12_0CTL |=  (0x0040u);
            DAC12_0CTL |=  (0x0020u);
            break;
        }
    }
    else
    {
        switch(amp)
        {
        case 0:
            DAC12_1CTL &= ~(0x0080u);   //bit7
            DAC12_1CTL &= ~(0x0040u);   //bit6
            DAC12_1CTL &= ~(0x0020u);   //bit5
            break;
        case 1:
            DAC12_1CTL &= ~(0x0080u);
            DAC12_1CTL &= ~(0x0040u);
            DAC12_1CTL |=  (0x0020u);
            break;
        case 2:
            DAC12_1CTL &= ~(0x0080u);
            DAC12_1CTL |=  (0x0040u);
            DAC12_1CTL &= ~(0x0020u);
            break;
        case 3:
            DAC12_1CTL &= ~(0x0080u);
            DAC12_1CTL |=  (0x0040u);
            DAC12_1CTL |=  (0x0020u);
            break;
        case 4:
            DAC12_1CTL |=  (0x0080u);
            DAC12_1CTL &= ~(0x0040u);
            DAC12_1CTL &= ~(0x0020u);
            break;
        case 5:
            DAC12_1CTL |=  (0x0080u);
            DAC12_1CTL &= ~(0x0040u);
            DAC12_1CTL |=  (0x0020u);
            break;
        case 6:
            DAC12_1CTL |=  (0x0080u);
            DAC12_1CTL |=  (0x0040u);
            DAC12_1CTL &= ~(0x0020u);
            break;
        case 7:
            DAC12_1CTL |=  (0x0080u);
            DAC12_1CTL |=  (0x0040u);
            DAC12_1CTL |=  (0x0020u);
            break;
        }
    }
}

void DAC12_SetDF(unsigned char dac, unsigned char df)
{
    if(dac == 0)
    {
        if(df == 0)
        {
            DAC12_0CTL &= ~(0x0010u);    //bit4
        }
        else
            DAC12_0CTL |=  (0x0010u);
    }
    else
    {
        if(df == 0)
        {
            DAC12_1CTL &= ~(0x0010u);    //bit4
        }
        else
            DAC12_1CTL |=  (0x0010u);
    }
}

void DAC12_Enabled(unsigned char dac)
{
    if(dac == 0)
    {
        DAC12_0CTL |=  DAC12IE;
        DAC12_0CTL &= ~DAC12IFG;
    }
    else
    {
        DAC12_1CTL |=  DAC12IE;
        DAC12_1CTL &= ~DAC12IFG;
    }
}
void DAC12_Disabled(unsigned char dac)
{
    if(dac == 0)
    {
        DAC12_0CTL &= ~DAC12IE;
    }
    else
    {
        DAC12_1CTL &= ~DAC12IE;
    }
}

void DAC12_ENC(unsigned char dac)
{
    if(dac == 0)
    {
        DAC12_0CTL |= DAC12ENC;
    }
    else
    {
        DAC12_1CTL |= DAC12ENC;
    }
}
void DAC12_Group(unsigned char group)
{
    if(group == 0)
    {
        DAC12_0CTL &= ~DAC12GRP;
    }
    else
    {
        DAC12_0CTL |=  DAC12GRP;
    }
}



/***************************************************************************//**
 * @}
 ******************************************************************************/

/***************************************************************************//**
 * @brief
 * @param
 * @return none
 *******************************************************************************/
