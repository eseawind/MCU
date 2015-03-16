/**********************************************
 *
 *              Author	:       Shawn Guo
 *              Mail    :       iguoxiaopeng@gmail.com
 *              Date	:       2013/7/23
 *              Last    :       2013/8/9
 *              Notes	:       clock system
 *              Tool    :       MSP430x2xx
 **********************************************/
/*****************************************************************************
* @file       clock.c
* @addtogroup CLOCK
* @{
******************************************************************************/
#include <msp430f2618.h>
#include "clock.h"



/***************************************************************************//**
 * @brief  Initialize the Clock sysyem - configure MCLK,SMCLK,ACLK,DCO
 * @param
 *      parameter               source          div
 *-----------------------------------------------------------------------------
 *      mclk_config =   0       DCO             /1
 *                      1       DCO             /2
 *                      2       DCO             /4
 *                      3       DCO             /8
 *
 *                      4       XT2             /1
 *                      5       XT2             /2
 *                      6       XT2             /4
 *                      7       XT2             /8
 *
 *                      8       LFXT1CLK        /1
 *                      9       LFXT1CLK        /2
 *                      10      LFXT1CLK        /4
 *                      11      LFXT1CLK        /8
 *
 *                      12      VLO             /1
 *                      13      VLO             /2
 *                      14      VLO             /4
 *                      15      VLO             /8
 *
 *
 *------------------------------------------------------------------------------
 *    smclk_config =    0       DCO             /1
 *                      1       DCO             /2
 *                      2       DCO             /4
 *                      3       DCO             /8
 *
 *                      4       XT2             /1
 *                      5       XT2             /2
 *                      6       XT2             /4
 *                      7       XT2             /8
 *------------------------------------------------------------------------------
 *    aclk_config =     0       VLOCLK          /1
 *                      1       VLOCLK          /2
 *                      2       VLOCLK          /4
 *                      3       VLOCLK          /8
 *
 *                      4       LFXT1           /1
 *                      5       LFXT1           /2
 *                      6       LFXT1           /4
 *                      7       LFXT1           /8
 *------------------------------------------------------------------------------
 *
 *    dco_config  =     1       1M
 *                      8       8M
 *                      12      12M
 *                      16      16M
 *
 *------------------------------------------------------------------------------
 * @return none
 ******************************************************************************/
void Clock_Init(unsigned char mclk_config, unsigned char smclk_config, unsigned char aclk_config, unsigned char dco_config)
{
    unsigned int i = 0;

    //MCLK 配置
    if(mclk_config < 4)     // DCO
    {
        //BCSCTL1 |= XT2OFF; // XT2OFF上电复位默认为1
        //BCSCTL2 |= SELM_0;

        if(mclk_config == 0)
            /*BCSCTL2 |= DIVM_0*/
            ;
        else if(mclk_config == 1) BCSCTL2 |= DIVM_1;
        else if(mclk_config == 2) BCSCTL2 |= DIVM_2;
        else BCSCTL2 |= DIVM_3;
    }
    else if(mclk_config < 8)    //XT2
    {
        /****
        1.打开XT2晶振
        2.延时等待晶振起振(如果没有延时, 晶振来不及起振, 置OFIFG为1且切换到DCO)
        3.选择XT2为时钟源
        注: 如果LFXT1没有连接,那么使用XT2照样会置OFIFG为1且无法置零.
        ****/
        BCSCTL1 &= ~XT2OFF; // XT2OFF默认为1
        BCSCTL3 |= XT2S_2; /** 默认设置: Mode 2 for XT2 : 2 - 16 MHz **/
        do
        {
            IFG1 &= ~OFIFG;                           // Clear OSCFault flag
            for (i = 0x47FF; i > 0; i--);             // Time for flag to set
        }
        while ((IFG1 & OFIFG));                   // OSCFault flag still set?
        BCSCTL2 |= SELM_2;

        if(mclk_config == 4)
            //BCSCTL2 |= DIVM_0
            ;
        else if(mclk_config == 5) BCSCTL2 |= DIVM_1;
        else if(mclk_config == 6) BCSCTL2 |= DIVM_2;
        else BCSCTL2 |= DIVM_3;
    }
    else if(mclk_config < 12)   //LFXT1CLK
    {
        BCSCTL3 &= ~(BIT5 + BIT4);  //开启32768HZ LFXT1
        do
        {
            IFG1 &= ~OFIFG;                           // Clear OSCFault flag
            for (i = 0x47FF; i > 0; i--);             // Time for flag to set
        }
        while ((IFG1 & OFIFG));                   // OSCFault flag still set?
        BCSCTL2 |= SELM_3;


        if(mclk_config == 8)
            /*BCSCTL2 |= DIVM_0*/
            ;
        else if(mclk_config == 9) BCSCTL2 |= DIVM_1;
        else if(mclk_config == 10) BCSCTL2 |= DIVM_2;
        else BCSCTL2 |= DIVM_3;
    }
    else    //VLOCLK
    {

        BCSCTL3 |= (BIT5);
        BCSCTL3 &= ~(BIT4);

        BCSCTL2 |= SELM_3;

        if(mclk_config == 12)
            /*BCSCTL2 |= DIVM_0*/
            ;
        else if(mclk_config == 13) BCSCTL2 |= DIVM_1;
        else if(mclk_config == 14) BCSCTL2 |= DIVM_2;
        else BCSCTL2 |= DIVM_3;
    }


    if(smclk_config < 4) //SMCLK 配置
    {
        //DCO
        BCSCTL2 &= ~SELS;
        if(smclk_config == 0)
            //BCSCTL2 |= DIVS_0
            ;
        else if(smclk_config == 1) BCSCTL2 |= DIVS_1;
        else if(smclk_config == 2) BCSCTL2 |= DIVS_2;
        else  BCSCTL2 |= DIVS_3;
    }
    else//XT2CLK if XT2 oscilator   present.    否则LFXT1CLK or  VLOCLK
    {
        if(mclk_config < 4 || mclk_config > 7)  //MCLK没有配置XT2,需另行配置、
        {
            BCSCTL1 &= ~XT2OFF; // XT2OFF默认为1
            BCSCTL3 |= XT2S_2; /** 默认设置: Mode 2 for XT2 : 2 - 16 MHz **/
            do
            {
                IFG1 &= ~OFIFG;                           // Clear OSCFault flag
                for (i = 0x47FF; i > 0; i--);             // Time for flag to set
            }
            while ((IFG1 & OFIFG));                   // OSCFault flag still set?
        }
        BCSCTL2 |= SELS;

        if(smclk_config == 4)
            //BCSCTL2 |= DIVS_0
            ;
        else if(smclk_config == 5) BCSCTL2 |= DIVS_1;
        else if(smclk_config == 6) BCSCTL2 |= DIVS_2;
        else  BCSCTL2 |= DIVS_3;
    }


    if(aclk_config < 4) //ACLK 配置
    {
        //VLOCLK
        if(mclk_config < 8) //MCLK已配置LFXT1, 则ACLK配置无效
            BCSCTL3 |= LFXT1S_2;

        if(aclk_config == 0)
            ;
        else if(aclk_config == 1) BCSCTL1 |= DIVA_1;
        else if(aclk_config == 2) BCSCTL1 |= DIVA_2;
        else BCSCTL1 |= DIVA_3;
    }
    else
    {
        //32768-Hz crystal on LFXT1
        if(mclk_config < 8) //MCLK已配置VLO, 则ACLK配置LFXT1无效
        {
            BCSCTL3 &= ~(BIT5 + BIT4);  //开启32768HZ LFXT1
            do
            {
                IFG1 &= ~OFIFG;                           // Clear OSCFault flag
                for (i = 0x47FF; i > 0; i--);             // Time for flag to set
            }
            while ((IFG1 & OFIFG));                   // OSCFault flag still set?
        }
        if(aclk_config == 0)
            ;
        else if(aclk_config == 1) BCSCTL1 |= DIVA_1;
        else if(aclk_config == 2) BCSCTL1 |= DIVA_2;
        else BCSCTL1 |= DIVA_3;
    }

    //DCO配置
    if(dco_config == 1)
    {
        BCSCTL1 =  CALBC1_1MHZ;    // Set range
        DCOCTL = CALDCO_1MHZ;      // Set DCO step + modulation
    }
    else if(dco_config == 8)
    {
        BCSCTL1 =  CALBC1_8MHZ;    // Set range
        DCOCTL = CALDCO_8MHZ;      // Set DCO step + modulation
    }
    else if(dco_config == 12)
    {
        BCSCTL1 =  CALBC1_12MHZ;    // Set range
        DCOCTL = CALDCO_12MHZ;      // Set DCO step + modulation
    }
    else if(dco_config == 16)
    {
        BCSCTL1 =  CALBC1_16MHZ;    // Set range
        DCOCTL = CALDCO_16MHZ;      // Set DCO step + modulation
    }
    else
    {
        BCSCTL1 =  CALBC1_1MHZ;    // Set range
        DCOCTL = CALDCO_1MHZ;      // Set DCO step + modulation
    }


    IFG1 &= ~OFIFG; // Clear OSCFault flag

}








/*DCOCTL 寄存器*/
/********************************************************************************
* bit7     bit6     bit5     bit4     bit3     bit2     bit1     bit0
* DCO.2    CCO.1    DCO.0    MOD.4    MOD.3    MOD.2    MOD.1    MOD.0
*********************************************************************************
* 当RSELx选择以后，
* DCO.0——DCO.2定义8种频率之一，可分段调节DCOCLK频率，相邻两种频率相差10%。
* 而频率由注入直流发生器的电流定义。
----------------------------------------------------------------
* MOD.O——MOD.4定义在32个DCO周期中插入的fdco+l周期个数，而在余下的DCO周期
* 中为fDco周期，控制切换DCO和DCO+1选择的两种频率。如果DCO常数为7，表示已
* 经选择最高颂率，此时不能利用MOD.O-MOD.4进行频率调整。
* Not useable when DCOx = 7.
*********************************************************************************/



/*BCSCTL1 寄存器*/

/**********************************************************************************
*
* bit7      bit6      bit5      bit4     bit3     bit2    bit1     bit0
*
* XT2OFF    XTS       DIVA.1    DIVA.0   Rse1.3   Rse1.2  Rse1.1   Rse1.0
***********************************************************************************
*XT2OFF控制 XT2 振荡器的开启与关闭。
*XT2OFF=0，XT2振荡器开启；
*XT2OFF=1，XT2振疡器关闭(默认XT2关闭)。
----------------------------------------------------------------
*XTS控制 LFXTl 工作模武，选择需结合实际晶体振荡器连接情况。
*XTS=0，LFXTl工作在低频模式 (默认低频模式)；
*XTS=1，LFXTl工作在高频模式(必须连接有相应高频时钟源)。       // MSP430G2XX系列不支持XTS=1
----------------------------------------------------------------
*DIVA.0，DIVA.l控制ACLK分频。
*0  不分频（默认不分频）；
*1  2分频；
*2  4分频；
*3  8分频。
----------------------------------------------------------------
*Rse1.0，Rsel.l，Rse1.2,Rse1.3四位控制某个内部电阻以决定16种不同的标称频率。
*Rse1=0， 选择最低的标称频率；
*Rse1=15，选择最高的标称频率；
RSEL1.3 is ignored when DCOR = 1
***********************************************************************************/

/*BCSCTL2 寄存器*/

/***********************************************************************************
* bit7     bit6     bit5     bit4     bit3     bit2      bit1      bit0
*SELM.1   SELM.0   DIVM.1   DIVM.0    SELS     DIVS.1    DIVS.0    DCOR
************************************************************************************
*SELM.1，SELM.0选择 MCLK 时钟源。
*0  时钟源为 DCOLCK（默认时钟源）；
*1  时钟源为 DCOCLK ;
*2  时钟源为 XT2CLK when XT2 oscillator present on-chip. LFXT1CLK or VLOCLK when XT2 oscillator not presenton-chip.；
*3  时钟源为 LFXT1CLK or VLOCLK 。
----------------------------------------------------------------
*DIVM.1，DlVM.0选择 MCLK 分频。
*0  1分频;
*1  2分频；
*2  4分频；
*3  8分频。
----------------------------------------------------------------
*   SELS Select SMCLK. This bit selects the SMCLK source.
*   0 DCOCLK
*   1 XT2CLK when XT2 oscillator present. LFXT1CLK or VLOCLK when XT2 oscillator not present.
----------------------------------------------------------------
*DIVS.1，DIVS.0选择 SMCLK 分频。
*0  1分频;
*1  2分频；
*2  4分频；
*3  8分频。
----------------------------------------------------------------
*   DCOR , DCO resistor select.
*   Not available in all devices. See the device-specific data sheet.
*
*   0 Internal resistor
*   1 External resistor
*
************************************************************************************/



/*BCSCTL3 寄存器*/

/************************************************************************************
* bit7      bit6    bit5     bit4     bit3    bit2      bit1     bit0
* XT2S1     XT2S0   LFXT1S1  LFXT1S0  XCAP1   XCAP0     XT2OF    LFXT1OF
************************************************************************************
*   XT2Sx   XT2 range select. These bits select the frequency range for XT2.
* XT2S1和XT2S0（2553不支持）
----------------------------------------------------------------
*   LFXT1S1和LFXT1S0选择LFXT1的范围
*   Low-frequency clock select and LFXT1 range select.
    These bits select between LFXT1 and VLO when XTS =0,
    and select the frequency range for LFXT1 when XTS = 1.(MSP430G2XXX 不支持外部高速晶振)
    When XTS = 0:
    00 32768-Hz crystal on LFXT1
    01 Reserved
    10 VLOCLK (Reserved in MSP430F21x1 devices)
    11 Digital external clock source
----------------------------------------------------------------
*   XCAP1和XCAP0选择LFXT1的匹配电容
    Oscillator capacitor selection.
    These bits select the effective capacitance seen by the LFXT1 crystal
    when XTS = 0. If XTS = 1 or if LFXT1Sx = 11 XCAPx should be 00.（外部高速晶振需要20~30P的电容，需要外接）
* 00  1pf
* 01  6pf
* 10  10pf
* 11  12.5pf
************************************************************************************/








/***************************************************************************//**
 * @brief  Clock_DCO_Set          对时钟DCOCLK进行配置
 * @param  传入的形参为x和y，其值参考 datsheet DCO频率表
 * @return none
 *******************************************************************************/
void Clock_DCO_Set(unsigned char x,unsigned char y) // 输入参数为F2618 datasheet P42
{
    unsigned char temp=( x << 4) + y; //将x,y整合成1个值作为switch的case值

    DCOCTL &= ~(0xFF);      //DCOCTL , BCSCTL1 全部清零
    BCSCTL1 &= ~(0xFF);

    switch(temp)
    {
    case 0x00:
    {
        DCOCTL  &=~( DCO0 + DCO1 + DCO2);
        BCSCTL1 &=~( RSEL0 + RSEL1 + RSEL2 + RSEL3);
        break;
    }
    case 0x03:
    {
        DCOCTL  |= ( DCO0 + DCO1 );
        BCSCTL1 &=~( RSEL0 + RSEL1 + RSEL2 + RSEL3);
        break;
    }
    case 0x13:
    {
        DCOCTL  |= ( DCO0 + DCO1 );
        BCSCTL1 |= ( RSEL0 );
        break;
    }
    case 0x23:
    {
        DCOCTL  |= ( DCO0 + DCO1 );
        BCSCTL1 |= ( RSEL1 );
        break;
    }
    case 0x33:
    {
        DCOCTL  |=  ( DCO0 + DCO1  );
        BCSCTL1 |= ( RSEL0 + RSEL1 );
        break;
    }
    case 0x43:
    {
        DCOCTL  |= ( DCO0 + DCO1  );
        BCSCTL1 |= ( RSEL2);
        break;
    }
    case 0x53:
    {
        DCOCTL  |= ( DCO0 + DCO1  );
        BCSCTL1 |= ( RSEL0 + RSEL2 );
        break;
    }
    case 0x63:
    {
        DCOCTL  |= ( DCO0 + DCO1  );
        BCSCTL1 |= ( RSEL1 + RSEL2 );
        break;
    }
    case 0x73:
    {
        DCOCTL  |= ( DCO0 + DCO1  );
        BCSCTL1 |= ( RSEL0 + RSEL1 + RSEL2 );
        break;
    }
    case 0x83:
    {
        DCOCTL  |= ( DCO0 + DCO1  );
        BCSCTL1 |= ( RSEL3);
        break;
    }
    case 0x93:
    {
        DCOCTL  |= ( DCO0 + DCO1  );
        BCSCTL1 |= ( RSEL0+ RSEL3);
        break;
    }
    case 0xA3:
    {
        DCOCTL  |= ( DCO0 + DCO1  );
        BCSCTL1 |= ( RSEL1 + RSEL3);
        break;
    }
    case 0xB3:
    {
        DCOCTL  |= ( DCO0 + DCO1 );
        BCSCTL1 |= ( RSEL0 + RSEL1 + RSEL3);
        break;
    }
    case 0xC3:
    {
        DCOCTL  |= ( DCO0 + DCO1 );
        BCSCTL1 |= ( RSEL2 + RSEL3);
        break;
    }
    case 0xD3:
    {
        DCOCTL  |= ( DCO0 + DCO1 );
        DCOCTL  |= ( MOD4 + MOD3 + MOD2 + MOD1 + MOD0 );//微调DCOCLK
        BCSCTL1 |= ( RSEL0 + RSEL2 + RSEL3);
        break;
    }
    case 0xE3:
    {
        DCOCTL  |= ( DCO0 + DCO1  );
        BCSCTL1 |= ( RSEL1 + RSEL2 + RSEL3);
        break;
    }
    case 0xF3:
    {
        DCOCTL  |= ( DCO0 + DCO1  );
        BCSCTL1 |= ( RSEL0 + RSEL1 + RSEL2 + RSEL3);
        break;
    }
    case 0xF7:
    {
        DCOCTL  |= ( DCO0 + DCO1 + DCO2 );
        BCSCTL1 |= ( RSEL0 + RSEL1 + RSEL2 + RSEL3);
        break;
    }
    default:
    {
        DCOCTL  |= ( DCO0 + DCO1 + DCO2 );
        BCSCTL1 |= ( RSEL0 + RSEL1 + RSEL2 + RSEL3);
    }
    }
}


void Clock_MCLK_Div(unsigned char Div)
{
    switch(Div)                            //1分频
    {
    case 0x01:
        BCSCTL2 &=~(DIVM1 + DIVM0);
        break;
    case 0x02:                         //2分频
        BCSCTL2 &=~(DIVM1 + DIVM0);
        BCSCTL2 |=DIVM0;
        break;
    case 0x04:                         //4分频
        BCSCTL2 &=~(DIVM1 + DIVM0);
        BCSCTL2 |=DIVM1;
        break;
    case 0x08:                         //8分频
        BCSCTL2 |=(DIVM1 + DIVM0);
        break;
    default :                           //默认不分频
        BCSCTL2 &=~(DIVM1 + DIVM0);
    }
}

/********************************************************************

* 函数名    :  SMCLK_Div
* 函数功能  :   对时钟MCLK进行配置
* 函数形参  :   传入的形参为Div，对时钟源DCOCLK进行Div分频
* 函数返回值 :   无
********************************************************************/

void Clock_SMCLK_Div(unsigned char Div)
{
    switch(Div)
    {
    case 0x01:                         //1分频
        BCSCTL2 &=~(DIVS_3);
        break;
    case 0x02:                         //2分频
        BCSCTL2 &=~(DIVS_3);
        BCSCTL2 |=(DIVS_1);
        break;
    case 0x04:                         //4分频
        BCSCTL2 &=~(DIVS_3);
        BCSCTL2 |=(DIVS_2);
        break;
    case 0x08:                         //8分频
        BCSCTL2 |=(DIVS_3);
        break;
    default :                           //默认不分频
        BCSCTL2 &=~(DIVS_3);
    }
}



/********************************************************************
* 函数名    :  ACLK_Div
* 函数功能  :   对时钟MCLK进行配置
* 函数形参  :   传入的形参为Div，对时钟源LFXT1CLK进行Div分频
* 函数返回值 :   无
********************************************************************/

void Clock_ACLK_Div(unsigned char Div)
{
    switch(Div)
    {
    case 0x01:                         //1分频
        BCSCTL1 &=~(DIVA_3);
        break;
    case 0x02:                         //2分频
        BCSCTL1 &=~(DIVA_3);
        BCSCTL1 |=(DIVA_1);
        break;
    case 0x04:                         //4分频
        BCSCTL1 &=~(DIVA_3);
        BCSCTL1 |=(DIVA_2);
        break;
    case 0x08:                         //8分频
        BCSCTL1 |=(DIVA_3);
        break;
    default :                           //默认不分频
        BCSCTL1 &=~(DIVA_3);
    }
}

extern void Clock_IE()      //Oscillator fault interrupt enable.
{
    IFG1 &= ~OFIFG;
    IE1 |= OFIE;
}

/***************************************************************************//**
 * @}
 ******************************************************************************/
