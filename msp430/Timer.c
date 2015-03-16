/**********************************************
 *
 * 		Author	:   Shawn Guo
 *              Mail    :   iguoxiaopeng@gmail.com
 * 		Date	:   2013/7/26
 *              Last    :
 * 		Notes	:   Timer-16Bit
 * 		Tool    :   MSP430X2XX
 **********************************************/

/*****************************************************************************
* @file       Timer.c
* @addtogroup TIMER
* @{
******************************************************************************/
#include <msp430f2618.h>
#include "Timer.h"


/***************************************************************************//**
 * @brief   Initialize the TimerA sysyem - configure Timer_A clock, select the divider for
 *          clock, mode control.
 * @param
 * ------------------------------------------------------------------------------
 *          clk  =  0       TACLK
 *                  1       ACLK
 *                  2       SMCLK
 *                  3       INCLK
 * ------------------------------------------------------------------------------
 *          div  =  1       /1
 *                  2       /2
 *                  4       /4
 *                  8       /8
 * ------------------------------------------------------------------------------
 *          mod  =  0       Stop mode.
 *                  1       Up mode.          0  --->    TACCR0
 *                  2       Continuous mode.  0  --->    0XFFFFH
 *                  3       Up/down mode.     0  --->    TACCR0     --->    0x0000H
 *
 * @return none
 *******************************************************************************/
void TimerA_UpMode_Init(unsigned char clk, unsigned char div, unsigned int taccr0, unsigned int taccr1, unsigned int taccr2)
{
    TimerA_SetClk(clk, div);
    TimerA_Clear();
    TimerA_SetMod(1);
    TACCR0 = taccr0;
    TimerA_Enable();
    if(taccr1)
    {
        TACCR1 = taccr1;
        TimerA_R1_Enable();
    }
    if(taccr2)
    {
        TACCR2 = taccr2;
        TimerA_R2_Enable();
    }


}
void TimerA_ConMode_Init(unsigned char clk, unsigned char div, unsigned int taccr0, unsigned int taccr1, unsigned int taccr2)
{
    TimerA_SetClk(clk, div);
    TimerA_Clear();
    TimerA_SetMod(2);
    TimerA_Enable();
    if(taccr0)
    {
        TACCR0 = taccr0;
        TimerA_R0_Enable();
    }
    if(taccr1)
    {
        TACCR1 = taccr1;
        TimerA_R1_Enable();
    }
    if(taccr2)
    {
        TACCR2 = taccr2;
        TimerA_R2_Enable();
    }
}

void TimerA_UpDownMode_Init(unsigned char clk, unsigned char div, unsigned int taccr0, unsigned int taccr1, unsigned int taccr2)
{
    TimerA_SetClk(clk, div);
    TimerA_Clear();
    TimerA_SetMod(3);
    TACCR0 = taccr0;
    TimerA_Enable();
    if(taccr1)
    {
        TACCR1 = taccr1;
        TimerA_R1_Enable();
    }
    if(taccr2)
    {
        TACCR2 = taccr2;
        TimerA_R2_Enable();
    }
}

void TimerA_R_Init(unsigned char clk, unsigned char div, unsigned char mod,  int taccr0, unsigned int taccr1, unsigned int taccr2)
{
    TimerA_SetClk(clk, div);
    TimerA_Clear();
    TimerA_SetMod(mod);

    if(taccr0)
    {
        TACCR0 = taccr0;
        TimerA_R0_Enable();
    }
    if(taccr1)
    {
        TACCR1 = taccr1;
        TimerA_R1_Enable();
    }
    if(taccr2)
    {
        TACCR2 = taccr2;
        TimerA_R2_Enable();
    }
}



void TimerA_SetClk(unsigned char clk, unsigned char div)
{
    if(clk == 0)
        TACTL &= ~(0x300u);
    else if(clk == 1)
    {
        TACTL &= ~(0x200u);
        TACTL |= (0x100u);
    }
    else if(clk == 2)
    {
        TACTL |= (0x200u);
        TACTL &= ~(0x100u);
    }
    else
        TACTL |= (0x300u);

    if(div == 1)
    {
        TACTL &= ~(0x00c0u);
    }
    else if(div == 2)
    {
        TACTL &= ~(0x0080u);
        TACTL |= (0x0040u);
    }
    else if(div == 4)
    {
        TACTL |= (0x0080u);
        TACTL &= ~(0x0040u);
    }
    else
    {
        TACTL |= (0x00c0u);
    }
}

void TimerA_SetMod(unsigned char mod)
{
    if(mod == 0)
    {
        TACTL &= ~(0x0030u);
    }
    else if(mod == 1)
    {
        TACTL &= ~(0x0020u);
        TACTL |= (0x0010u);
    }
    else if(mod == 2)
    {
        TACTL |= (0x0020u);
        TACTL &= ~(0x0010u);
    }
    else
        TACTL |= (0x0030u);

}

void TimerA_Clear()
{
    TACTL |= TACLR;
}

void TimerA_Disable()
{
    TACTL &= ~TAIE;
}

void TimerA_Enable()
{
    TACTL |= TAIE;
    TACTL &= ~TAIFG;
}

void TimerA_Stop()
{
    TACTL &= ~(0x0030u);
}

void TimerA_R0_Enable()
{
    TACCTL0 |= CCIE;
    TACCTL0 &= ~CCIFG;
}
void TimerA_R1_Enable()
{
    TACCTL1 |= CCIE;
    TACCTL1 &= ~CCIFG;
}
void TimerA_R2_Enable()
{
    TACCTL2 |= CCIE;
    TACCTL2 &= ~CCIFG;
}

void TimerA_R0_Disable()
{
    TACCTL0 &= ~CCIE;
}
void TimerA_R1_Disable()
{
    TACCTL1 &= ~CCIE;
}
void TimerA_R2_Disable()
{
    TACCTL2 &= ~CCIE;
}


/***************************************************************************//**
 * @brief   Timer_A3 捕获/比较模块配置
 * @param
 * ------------------------------------------------------------------------------
 *          ccis(捕获源) =     0           CCIxA管脚
 *                             1           CCIxB管脚
 *                             2            GND
 *                             3            VCC
 * ------------------------------------------------------------------------------
 *          cm(捕获触发沿)=     0       禁止捕获
 *                              1       上升沿
 *                              2       下降沿
 *                              3        上升沿/下降沿
 * ------------------------------------------------------------------------------
 *          scs(异步捕获/同步捕获)  =  0        异步
 *                                      1       同步
 * @return none
 *******************************************************************************/

void TimerA_CAP0_Init(unsigned char ccis, unsigned char cm, unsigned char scs)
{
    TACCTL0 |= CAP;
    TACCTL0 &= ~(COV);
    TimerA_R0_Enable();

    if(ccis == 0) TACCTL0 &= ~(0x3000u);
    else if(ccis == 1)
    {
        TACCTL0 &= ~(0x2000u);
        TACCTL0 |= (0x1000u);
    }
    else if(ccis == 2)
    {
        TACCTL0 |= (0x2000u);
        TACCTL0 &= ~(0x1000u);
    }
    else
        TACCTL0 |= (0x3000u);



    if(cm == 0) TACCTL0 &= ~(0xc000u);
    else if(cm == 1)
    {
        TACCTL0 &= ~(0x8000u);
        TACCTL0 |= (0x4000u);
    }
    else if(cm == 2)
    {
        TACCTL0 |= (0x8000u);
        TACCTL0 &= (0x4000u);
    }
    else
    {
        TACCTL0 |= (0xc000u);
    }

    if(scs == 0)    TACCTL0 &= ~(0x0800u);
    else TACCTL0 |= (0x0800u);

}

void TimerA_CAP1_Init(unsigned char ccis, unsigned char cm, unsigned char scs)
{
    TACCTL1 |= CAP;
    TACCTL1 &= ~(COV);
    TimerA_R1_Enable();

    if(ccis == 0) TACCTL1 &= ~(0x3000u);
    else if(ccis == 1)
    {
        TACCTL1 &= ~(0x2000u);
        TACCTL1 |= (0x1000u);
    }
    else if(ccis == 2)
    {
        TACCTL1 |= (0x2000u);
        TACCTL1 &= ~(0x1000u);
    }
    else
        TACCTL1 |= (0x3000u);



    if(cm == 0) TACCTL1 &= ~(0xc000u);
    else if(cm == 1)
    {
        TACCTL1 &= ~(0x8000u);
        TACCTL1 |= (0x4000u);
    }
    else if(cm == 2)
    {
        TACCTL1 |= (0x8000u);
        TACCTL1 &= (0x4000u);
    }
    else
    {
        TACCTL1 |= (0xc000u);
    }

    if(scs == 0)    TACCTL1 &= ~(0x0800u);
    else TACCTL1 |= (0x0800u);

}

void TimerA_CAP2_Init(unsigned char ccis, unsigned char cm, unsigned char scs)
{
    TACCTL2 |= CAP;
    TACCTL2 &= ~(COV);
    TimerA_R2_Enable();

    if(ccis == 0) TACCTL2 &= ~(0x3000u);
    else if(ccis == 1)
    {
        TACCTL2 &= ~(0x2000u);
        TACCTL2 |= (0x1000u);
    }
    else if(ccis == 2)
    {
        TACCTL2 |= (0x2000u);
        TACCTL2 &= ~(0x1000u);
    }
    else
        TACCTL2 |= (0x3000u);



    if(cm == 0) TACCTL2 &= ~(0xc000u);
    else if(cm == 1)
    {
        TACCTL2 &= ~(0x8000u);
        TACCTL2 |= (0x4000u);
    }
    else if(cm == 2)
    {
        TACCTL2 |= (0x8000u);
        TACCTL2 &= (0x4000u);
    }
    else
    {
        TACCTL2 |= (0xc000u);
    }

    if(scs == 0)    TACCTL2 &= ~(0x0800u);
    else TACCTL2 |= (0x0800u);

}

unsigned char TimerA_SCCI0()
{
    return (unsigned char)(TACCTL0 & SCCI);
}
unsigned char TimerA_SCCI1()
{
    return (unsigned char)(TACCTL1 & SCCI);
}
unsigned char TimerA_SCCI2()
{
    return (unsigned char)(TACCTL2 & SCCI);
}

unsigned char TimerA_CCI0()
{
    return (unsigned char)(TACCTL0 & CCI);
}
unsigned char TimerA_CCI1()
{
    return (unsigned char)(TACCTL1 & CCI);
}
unsigned char TimerA_CCI2()
{
    return (unsigned char)(TACCTL2 & CCI);
}

unsigned char TimerA_COV0()
{
    return (unsigned char)(TACCTL0 & COV);
}
unsigned char TimerA_COV1()
{
    return (unsigned char)(TACCTL1 & COV);
}
unsigned char TimerA_COV2()
{
    return (unsigned char)(TACCTL2 & COV);
}


void TimerA_COM_Init(unsigned int taccr0,unsigned int taccr1, unsigned char outmode1, unsigned int taccr2, unsigned char outmode2)
{
    if(taccr0)      // 设taccr0 = 0表示不改变
        TACCR0 = taccr0;

    if(taccr1)
    {
        TACCTL1 &= ~(CAP);
        TACCR1 = taccr1;


        if(outmode1 == 1)
        {
            TACCTL1 &= ~(0x0080u);
            TACCTL1 &= ~(0x0040u);
            TACCTL1 |=  (0x0020u);
        }
        else if(outmode1 == 2)
        {
            TACCTL1 &= ~(0x0080u);
            TACCTL1 |=  (0x0040u);
            TACCTL1 &= ~(0x0020u);
        }
        else if(outmode1 == 3)
        {
            TACCTL1 &= ~(0x0080u);
            TACCTL1 |=  (0x0040u);
            TACCTL1 |=  (0x0020u);
        }
        else if(outmode1 == 4)
        {
            TACCTL1 |=  (0x0080u);
            TACCTL1 &= ~(0x0040u);
            TACCTL1 &= ~(0x0020u);
        }
        else if(outmode1 == 5)
        {
            TACCTL1 |=  (0x0080u);
            TACCTL1 &= ~(0x0040u);
            TACCTL1 |=  (0x0020u);
        }
        else if(outmode1 == 6)
        {
            TACCTL1 |=  (0x0080u);
            TACCTL1 |=  (0x0040u);
            TACCTL1 &= ~(0x0020u);
        }
        else
        {
            TACCTL1 |= (0x0080u);
            TACCTL1 |= (0x0040u);
            TACCTL1 |= (0x0020u);
        }
    }

    if(taccr2)
    {
        TACCTL2 &= ~(CAP);
        TACCR2 = taccr2;

        if(outmode2 == 1)
        {
            TACCTL2 &= ~(0x0080u);
            TACCTL2 &= ~(0x0040u);
            TACCTL2 |= (0x0020u);
        }
        else if(outmode2 == 2)
        {
            TACCTL2 &= ~(0x0080u);
            TACCTL2 |= (0x0040u);
            TACCTL2 &= ~(0x0020u);
        }
        else if(outmode2 == 3)
        {
            TACCTL2 &= ~(0x0080u);
            TACCTL2 |= (0x0040u);
            TACCTL2 |= (0x0020u);
        }
        else if(outmode2 == 4)
        {
            TACCTL2 |= (0x0080u);
            TACCTL2 &= ~(0x0040u);
            TACCTL2 &= ~(0x0020u);
        }
        else if(outmode2 == 5)
        {
            TACCTL2 |= (0x0080u);
            TACCTL2 &= ~(0x0040u);
            TACCTL2 |= (0x0020u);
        }
        else if(outmode2 == 6)
        {
            TACCTL2 |= (0x0080u);
            TACCTL2 |= (0x0040u);
            TACCTL2 &= ~(0x0020u);
        }
        else
        {
            TACCTL2 |= (0x0080u);
            TACCTL2 |= (0x0040u);
            TACCTL2 |= (0x0020u);
        }
    }



}




















void TimerB_UpMode_Init(unsigned char clk, unsigned char div, unsigned int tbccr0)
{
    TimerB_SetClk(clk, div);
    TimerB_Clear();
    TimerB_SetMod(1);
    TBCCR0 = tbccr0;
    TimerB_Enable();
}

void TimerB_ConMode_Init(unsigned char clk, unsigned char div)
{
    TimerB_SetClk(clk, div);
    TimerB_Clear();
    TimerB_SetMod(2);
    TimerB_Enable();
}

void TimerB_UpDownMode_Init(unsigned char clk, unsigned char div, unsigned int tbccr0)
{
    TimerB_SetClk(clk, div);
    TimerB_Clear();
    TimerB_SetMod(3);
    TBCCR0 = tbccr0;
    TimerB_Enable();
}

void TimerB_UpModeR0_Init(unsigned char clk, unsigned char div, unsigned int tbccr0)
{
    TimerB_SetClk(clk, div);
    TimerB_Clear();
    TimerB_SetMod(1);

    TBCCR0 = tbccr0;
    TimerB_R0_Enable();
}

void TimerB_UpDownModeR0_Init(unsigned char clk, unsigned char div, unsigned int tbccr0)
{
    TimerB_SetClk(clk, div);
    TimerB_Clear();
    TimerB_SetMod(3);
    TBCCR0 = tbccr0;
    TimerA_R0_Enable();
}

void TimerB_SetCounterLength(unsigned char bits) //配置Timer_B的计数长度,默认(16-bit);
{
    if(bits == 12)
    {
        TBCTL &= ~0X1000u;       //BIT12
        TBCTL |=  0X0800u;       //BIT11
    }
    else if(bits == 10)
    {
        TBCTL |=  0X1000u;       //BIT12
        TBCTL &= ~0X0800u;       //BIT11
    }
    else if(bits == 8)
    {
        TBCTL |=  0X1000u;       //BIT12
        TBCTL |=  0X0800u;       //BIT11
    }
    else        // 默认 16-bits
    {
        TBCTL &= ~0X1000u;       //BIT12
        TBCTL &= ~0X0800u;       //BIT11
    }
}


void TimerB_SetClk(unsigned char clk, unsigned char div)
{
    if(clk == 0)
    {
        TACTL &= ~(0x0200u); //BIT9
        TBCTL &= ~(0x0100u);  //BIT8
    }
             else if(clk == 1)
    {
        TBCTL &= ~(0x200u); //BIT9
        TBCTL |=  (0x100u); //BIT8
    }
    else if(clk == 2)
    {
        TBCTL |=  (0x200u);
        TBCTL &= ~(0x100u);
    }
    else
        TBCTL |= (0x300u);

    if(div == 1)
    {
        TBCTL &= ~(0x00c0u);
    }
    else if(div == 2)
    {
        TBCTL &= ~(0x0080u);
        TBCTL |=  (0x0040u);
    }
    else if(div == 4)
    {
        TBCTL |=  (0x0080u); // BIT7
        TBCTL &= ~(0x0040u); // BIT6
    }
    else
    {
        TBCTL |= (0x00c0u);
    }
}


void TimerB_SetMod(unsigned char mod)
{
    if(mod == 0)
    {
        TBCTL &= ~(0x0030u);
    }
    else if(mod == 1)
    {
        TBCTL &= ~(0x0020u);    //BIT5
        TBCTL |= (0x0010u);     //BIT4
    }
    else if(mod == 2)
    {
        TBCTL |= (0x0020u);
        TBCTL &= ~(0x0010u);
    }
    else
        TBCTL |= (0x0030u);
}

void TimerB_Clear()
{
    TBCTL |= TBCLR;
}

void TimerB_Stop()
{
    TBCTL &= ~(0x0030u);    //Set the TimerB mode = 00, stop mode.
}

void TimerB_Enable()
{
    TBCTL |= TBIE;
    TBCTL &= ~TAIFG;
}

void TimerB_Disable()
{
    TBCTL &= ~TBIE;
    TBCTL &= ~TBIFG;
}

void TimerB_CCR0(unsigned int tbccr0)
{
    TBCCR0 = tbccr0;
    TBCCTL0 |= CCIE;
    TBCCTL0 &= ~CCIFG;
}

void TimerB_CCR1(unsigned int tbccr1)
{
    TBCCR1 = tbccr1;
    TBCCTL1 |= CCIE;
    TBCCTL1 &= ~CCIFG;
}
void TimerB_CCR2(unsigned int tbccr2)
{
    TBCCR2 = tbccr2;
    TBCCTL2 |= CCIE;
    TBCCTL2 &= ~CCIFG;
}
void TimerB_CCR3(unsigned int tbccr3)
{
    TBCCR3 = tbccr3;
    TBCCTL3 |= CCIE;
    TBCCTL3 &= ~CCIFG;
}
void TimerB_CCR4(unsigned int tbccr4)
{
    TBCCR4 = tbccr4;
    TBCCTL4 |= CCIE;
    TBCCTL4 &= ~CCIFG;
}
void TimerB_CCR5(unsigned int tbccr5)
{
    TBCCR5 = tbccr5;
    TBCCTL5 |= CCIE;
    TBCCTL5 &= ~CCIFG;
}
void TimerB_CCR6(unsigned int tbccr6)
{
    TBCCR6 = tbccr6;
    TBCCTL6 |= CCIE;
    TBCCTL6 &= ~CCIFG;
}

void TimerB_R0_Enable()
{
    TBCCTL0 |= CCIE;
    TBCCTL0 &= ~CCIFG;
}
void TimerB_R1_Enable()
{
    TBCCTL1 |= CCIE;
    TBCCTL1 &= ~CCIFG;
}
void TimerB_R2_Enable()
{
    TBCCTL2 |= CCIE;
    TBCCTL2 &= ~CCIFG;
}
void TimerB_R3_Enable()
{
    TBCCTL3 |= CCIE;
    TBCCTL3 &= ~CCIFG;
}
void TimerB_R4_Enable()
{
    TBCCTL4 |= CCIE;
    TBCCTL4 &= ~CCIFG;
}
void TimerB_R5_Enable()
{
    TBCCTL5 |= CCIE;
    TBCCTL5 &= ~CCIFG;
}
void TimerB_R6_Enable()
{
    TBCCTL6 |= CCIE;
    TBCCTL6 &= ~CCIFG;
}

void TimerB_R0_Disable()
{
    TBCCTL0 &= ~CCIE;
}
void TimerB_R1_Disable()
{
    TBCCTL1 &= ~CCIE;
}
void TimerB_R2_Disable()
{
    TBCCTL2 &= ~CCIE;
}
void TimerB_R3_Disable()
{
    TBCCTL3 &= ~CCIE;
}
void TimerB_R4_Disable()
{
    TBCCTL4 &= ~CCIE;
}
void TimerB_R5_Disable()
{
    TBCCTL5 &= ~CCIE;
}
void TimerB_R6_Disable()
{
    TBCCTL6 &= ~CCIE;
}


void TimerB_CAP0_Init(unsigned char ccis, unsigned char cm, unsigned char scs)
{
    TBCCTL0 |=  CAP;
    TBCCTL0 &= ~(COV);
    TBCCTL0 |=  CCIE;
    TBCCTL0 &= ~CCIFG;

    if(ccis == 0) TBCCTL0 &= ~(0x3000u);
    else if(ccis == 1)
    {
        TBCCTL0 &= ~(0x2000u);          //BIT13
        TBCCTL0 |=  (0x1000u);           //BIT12
    }
    else if(ccis == 2)
    {
        TBCCTL0 |=  (0x2000u);
        TBCCTL0 &= ~(0x1000u);
    }
    else
        TBCCTL0 |=  (0x3000u);



    if(cm == 0) TBCCTL0 &= ~(0xC000u);
    else if(cm == 1)
    {
        TBCCTL0 &= ~(0x8000u);  //BIT15
        TBCCTL0 |=  (0x4000u);  //BIT14
    }
    else if(cm == 2)
    {
        TBCCTL0 |= (0x8000u);
        TBCCTL0 &= (0x4000u);
    }
    else
    {
        TBCCTL0 |= (0xC000u);
    }

    if(scs == 0)    TBCCTL0 &= ~(0x0800u);
    else TBCCTL0 |= (0x0800u);
}

void TimerB_CAP1_Init(unsigned char ccis, unsigned char cm, unsigned char scs)
{
    TBCCTL1 |=  CAP;
    TBCCTL1 &= ~(COV);
    TBCCTL1 |=  CCIE;
    TBCCTL1 &= ~CCIFG;

    if(ccis == 0) TBCCTL1 &= ~(0x3000u);
    else if(ccis == 1)
    {
        TBCCTL1 &= ~(0x2000u);          //BIT13
        TBCCTL1 |=  (0x1000u);           //BIT12
    }
    else if(ccis == 2)
    {
        TBCCTL1 |=  (0x2000u);
        TBCCTL1 &= ~(0x1000u);
    }
    else
        TBCCTL1 |=  (0x3000u);



    if(cm == 0) TBCCTL1 &= ~(0xC000u);
    else if(cm == 1)
    {
        TBCCTL1 &= ~(0x8000u);  //BIT15
        TBCCTL1 |=  (0x4000u);  //BIT14
    }
    else if(cm == 2)
    {
        TBCCTL1 |= (0x8000u);
        TBCCTL1 &= (0x4000u);
    }
    else
    {
        TBCCTL1 |= (0xC000u);
    }

    if(scs == 0)    TBCCTL1 &= ~(0x0800u);
    else TBCCTL1 |= (0x0800u);
}
void TimerB_CAP2_Init(unsigned char ccis, unsigned char cm, unsigned char scs)
{
    TBCCTL2 |=  CAP;
    TBCCTL2 &= ~(COV);
    TBCCTL2 |=  CCIE;
    TBCCTL2 &= ~CCIFG;

    if(ccis == 0) TBCCTL2 &= ~(0x3000u);
    else if(ccis == 1)
    {
        TBCCTL2 &= ~(0x2000u);          //BIT13
        TBCCTL2 |=  (0x1000u);           //BIT12
    }
    else if(ccis == 2)
    {
        TBCCTL2 |=  (0x2000u);
        TBCCTL2 &= ~(0x1000u);
    }
    else
        TBCCTL2 |=  (0x3000u);



    if(cm == 0) TBCCTL2 &= ~(0xC000u);
    else if(cm == 1)
    {
        TBCCTL2 &= ~(0x8000u);  //BIT15
        TBCCTL2 |=  (0x4000u);  //BIT14
    }
    else if(cm == 2)
    {
        TBCCTL2 |= (0x8000u);
        TBCCTL2 &= (0x4000u);
    }
    else
    {
        TBCCTL2 |= (0xC000u);
    }

    if(scs == 0)    TBCCTL2 &= ~(0x0800u);
    else TBCCTL2 |= (0x0800u);
}
void TimerB_CAP3_Init(unsigned char ccis, unsigned char cm, unsigned char scs)
{
    TBCCTL3 |=  CAP;
    TBCCTL3 &= ~(COV);
    TBCCTL3 |=  CCIE;
    TBCCTL3 &= ~CCIFG;

    if(ccis == 0) TBCCTL3 &= ~(0x3000u);
    else if(ccis == 1)
    {
        TBCCTL3 &= ~(0x2000u);          //BIT13
        TBCCTL3 |=  (0x1000u);           //BIT12
    }
    else if(ccis == 2)
    {
        TBCCTL3 |=  (0x2000u);
        TBCCTL3 &= ~(0x1000u);
    }
    else
        TBCCTL3 |=  (0x3000u);



    if(cm == 0) TBCCTL3 &= ~(0xC000u);
    else if(cm == 1)
    {
        TBCCTL3 &= ~(0x8000u);  //BIT15
        TBCCTL3 |=  (0x4000u);  //BIT14
    }
    else if(cm == 2)
    {
        TBCCTL3 |= (0x8000u);
        TBCCTL3 &= (0x4000u);
    }
    else
    {
        TBCCTL3 |= (0xC000u);
    }

    if(scs == 0)    TBCCTL3 &= ~(0x0800u);
    else TBCCTL3 |= (0x0800u);
}
void TimerB_CAP4_Init(unsigned char ccis, unsigned char cm, unsigned char scs)
{
    TBCCTL4 |=  CAP;
    TBCCTL4 &= ~(COV);
    TBCCTL4 |=  CCIE;
    TBCCTL4 &= ~CCIFG;

    if(ccis == 0) TBCCTL4 &= ~(0x3000u);
    else if(ccis == 1)
    {
        TBCCTL4 &= ~(0x2000u);          //BIT13
        TBCCTL4 |=  (0x1000u);           //BIT12
    }
    else if(ccis == 2)
    {
        TBCCTL4 |=  (0x2000u);
        TBCCTL4 &= ~(0x1000u);
    }
    else
        TBCCTL4 |=  (0x3000u);



    if(cm == 0) TBCCTL4 &= ~(0xC000u);
    else if(cm == 1)
    {
        TBCCTL4 &= ~(0x8000u);  //BIT15
        TBCCTL4 |=  (0x4000u);  //BIT14
    }
    else if(cm == 2)
    {
        TBCCTL4 |= (0x8000u);
        TBCCTL4 &= (0x4000u);
    }
    else
    {
        TBCCTL4 |= (0xC000u);
    }

    if(scs == 0)    TBCCTL4 &= ~(0x0800u);
    else TBCCTL4 |= (0x0800u);
}
void TimerB_CAP5_Init(unsigned char ccis, unsigned char cm, unsigned char scs)
{
    TBCCTL5 |=  CAP;
    TBCCTL5 &= ~(COV);
    TBCCTL5 |=  CCIE;
    TBCCTL5 &= ~CCIFG;

    if(ccis == 0) TBCCTL5 &= ~(0x3000u);
    else if(ccis == 1)
    {
        TBCCTL5 &= ~(0x2000u);          //BIT13
        TBCCTL5 |=  (0x1000u);           //BIT12
    }
    else if(ccis == 2)
    {
        TBCCTL5 |=  (0x2000u);
        TBCCTL5 &= ~(0x1000u);
    }
    else
        TBCCTL5 |=  (0x3000u);



    if(cm == 0) TBCCTL5 &= ~(0xC000u);
    else if(cm == 1)
    {
        TBCCTL5 &= ~(0x8000u);  //BIT15
        TBCCTL5 |=  (0x4000u);  //BIT14
    }
    else if(cm == 2)
    {
        TBCCTL5 |= (0x8000u);
        TBCCTL5 &= (0x4000u);
    }
    else
    {
        TBCCTL5 |= (0xC000u);
    }

    if(scs == 0)    TBCCTL5 &= ~(0x0800u);
    else TBCCTL5 |= (0x0800u);
}
void TimerB_CAP6_Init(unsigned char ccis, unsigned char cm, unsigned char scs)
{
    TBCCTL6 |=  CAP;
    TBCCTL6 &= ~(COV);
    TBCCTL6 |=  CCIE;
    TBCCTL6 &= ~CCIFG;

    if(ccis == 0) TBCCTL6 &= ~(0x3000u);
    else if(ccis == 1)
    {
        TBCCTL6 &= ~(0x2000u);          //BIT13
        TBCCTL6 |=  (0x1000u);           //BIT12
    }
    else if(ccis == 2)
    {
        TBCCTL6 |=  (0x2000u);
        TBCCTL6 &= ~(0x1000u);
    }
    else
        TBCCTL6 |=  (0x3000u);



    if(cm == 0) TBCCTL6 &= ~(0xC000u);
    else if(cm == 1)
    {
        TBCCTL6 &= ~(0x8000u);  //BIT15
        TBCCTL6 |=  (0x4000u);  //BIT14
    }
    else if(cm == 2)
    {
        TBCCTL6 |= (0x8000u);
        TBCCTL6 &= (0x4000u);
    }
    else
    {
        TBCCTL6 |= (0xC000u);
    }

    if(scs == 0)    TBCCTL6 &= ~(0x0800u);
    else TBCCTL6 |= (0x0800u);
}


unsigned char TimerB_CCI0()
{
    return (unsigned char)(TBCCTL0 & CCI);
}
unsigned char TimerB_CCI1()
{
    return (unsigned char)(TBCCTL1 & CCI);
}
unsigned char TimerB_CCI2()
{
    return (unsigned char)(TBCCTL2 & CCI);
}
unsigned char TimerB_CCI3()
{
    return (unsigned char)(TBCCTL3 & CCI);
}
unsigned char TimerB_CCI4()
{
    return (unsigned char)(TBCCTL4 & CCI);
}
unsigned char TimerB_CCI5()
{
    return (unsigned char)(TBCCTL5 & CCI);
}
unsigned char TimerB_CCI6()
{
    return (unsigned char)(TBCCTL6 & CCI);
}

unsigned char TimerB_COV0()
{
    return (unsigned char)(TBCCTL0 & COV);
}
unsigned char TimerB_COV1()
{
    return (unsigned char)(TBCCTL1 & COV);
}
unsigned char TimerB_COV2()
{
    return (unsigned char)(TBCCTL2 & COV);
}
unsigned char TimerB_COV3()
{
    return (unsigned char)(TBCCTL3 & COV);
}
unsigned char TimerB_COV4()
{
    return (unsigned char)(TBCCTL4 & COV);
}
unsigned char TimerB_COV5()
{
    return (unsigned char)(TBCCTL5 & COV);
}
unsigned char TimerB_COV6()
{
    return (unsigned char)(TBCCTL6 & COV);
}


void TimerB_COM_Init(unsigned int tbccr0, unsigned char tbclgrp)
{
    if(tbccr0)
        TBCCR0 = tbccr0;
    if(tbclgrp == 0)
    {
        TBCTL &= ~(0x4000u);//bit14
        TBCTL &= ~(0x2000u);//bit13
    }
    else if(tbclgrp == 1)
    {
        TBCTL &= ~(0x4000u);//bit14
        TBCTL |=  (0x2000u);//bit13
    }
    else if(tbclgrp == 2)
    {
        TBCTL |=  (0x4000u);//bit14
        TBCTL &= ~(0x2000u);//bit13
    }
    else
    {
        TBCTL |=  (0x4000u);//bit14
        TBCTL |=  (0x2000u);//bit13
    }
}

void TimerB_COM1(unsigned int tbccr1, char outmode)
{
    TBCCTL1 &= ~(CAP);
    TBCCR1 = tbccr1;


    if(outmode == 1)
    {
        TBCCTL1 &= ~(0x0080u);
        TBCCTL1 &= ~(0x0040u);
        TBCCTL1 |=  (0x0020u);
    }
    else if(outmode == 2)
    {
        TBCCTL1 &= ~(0x0080u);
        TBCCTL1 |=  (0x0040u);
        TBCCTL1 &= ~(0x0020u);
    }
    else if(outmode == 3)
    {
        TBCCTL1 &= ~(0x0080u);
        TBCCTL1 |=  (0x0040u);
        TBCCTL1 |=  (0x0020u);
    }
    else if(outmode == 4)
    {
        TBCCTL1 |=  (0x0080u);
        TBCCTL1 &= ~(0x0040u);
        TBCCTL1 &= ~(0x0020u);
    }
    else if(outmode == 5)
    {
        TBCCTL1 |=  (0x0080u);
        TBCCTL1 &= ~(0x0040u);
        TBCCTL1 |=  (0x0020u);
    }
    else if(outmode == 6)
    {
        TBCCTL1 |=  (0x0080u);
        TBCCTL1 |=  (0x0040u);
        TBCCTL1 &= ~(0x0020u);
    }
    else
    {
        TBCCTL1 |= (0x0080u);
        TBCCTL1 |= (0x0040u);
        TBCCTL1 |= (0x0020u);
    }

}
void TimerB_COM2(unsigned int tbccr2, char outmode)
{
    TBCCTL2 &= ~(CAP);
    TBCCR2 = tbccr2;


    if(outmode == 1)
    {
        TBCCTL2 &= ~(0x0080u);
        TBCCTL2 &= ~(0x0040u);
        TBCCTL2 |=  (0x0020u);
    }
    else if(outmode == 2)
    {
        TBCCTL2 &= ~(0x0080u);
        TBCCTL2 |=  (0x0040u);
        TBCCTL2 &= ~(0x0020u);
    }
    else if(outmode == 3)
    {
        TBCCTL2 &= ~(0x0080u);
        TBCCTL2 |=  (0x0040u);
        TBCCTL2 |=  (0x0020u);
    }
    else if(outmode == 4)
    {
        TBCCTL2 |=  (0x0080u);
        TBCCTL2 &= ~(0x0040u);
        TBCCTL2 &= ~(0x0020u);
    }
    else if(outmode == 5)
    {
        TBCCTL2 |=  (0x0080u);
        TBCCTL2 &= ~(0x0040u);
        TBCCTL2 |=  (0x0020u);
    }
    else if(outmode == 6)
    {
        TBCCTL2 |=  (0x0080u);
        TBCCTL2 |=  (0x0040u);
        TBCCTL2 &= ~(0x0020u);
    }
    else
    {
        TBCCTL2 |= (0x0080u);
        TBCCTL2 |= (0x0040u);
        TBCCTL2 |= (0x0020u);
    }

}
void TimerB_COM3(unsigned int tbccr3, char outmode)
{
    TBCCTL3 &= ~(CAP);
    TBCCR3 = tbccr3;


    if(outmode == 1)
    {
        TBCCTL3 &= ~(0x0080u);
        TBCCTL3 &= ~(0x0040u);
        TBCCTL3 |=  (0x0020u);
    }
    else if(outmode == 2)
    {
        TBCCTL3 &= ~(0x0080u);
        TBCCTL3 |=  (0x0040u);
        TBCCTL3 &= ~(0x0020u);
    }
    else if(outmode == 3)
    {
        TBCCTL3 &= ~(0x0080u);
        TBCCTL3 |=  (0x0040u);
        TBCCTL3 |=  (0x0020u);
    }
    else if(outmode == 4)
    {
        TBCCTL3 |=  (0x0080u);
        TBCCTL3 &= ~(0x0040u);
        TBCCTL3 &= ~(0x0020u);
    }
    else if(outmode == 5)
    {
        TBCCTL3 |=  (0x0080u);
        TBCCTL3 &= ~(0x0040u);
        TBCCTL3 |=  (0x0020u);
    }
    else if(outmode == 6)
    {
        TBCCTL3 |=  (0x0080u);
        TBCCTL3 |=  (0x0040u);
        TBCCTL3 &= ~(0x0020u);
    }
    else
    {
        TBCCTL3 |= (0x0080u);
        TBCCTL3 |= (0x0040u);
        TBCCTL3 |= (0x0020u);
    }

}
void TimerB_COM4(unsigned int tbccr4, char outmode)
{
    TBCCTL4 &= ~(CAP);
    TBCCR4 = tbccr4;


    if(outmode == 1)
    {
        TBCCTL4 &= ~(0x0080u);
        TBCCTL4 &= ~(0x0040u);
        TBCCTL4 |=  (0x0020u);
    }
    else if(outmode == 2)
    {
        TBCCTL4 &= ~(0x0080u);
        TBCCTL4 |=  (0x0040u);
        TBCCTL4 &= ~(0x0020u);
    }
    else if(outmode == 3)
    {
        TBCCTL4 &= ~(0x0080u);
        TBCCTL4 |=  (0x0040u);
        TBCCTL4 |=  (0x0020u);
    }
    else if(outmode == 4)
    {
        TBCCTL4 |=  (0x0080u);
        TBCCTL4 &= ~(0x0040u);
        TBCCTL4 &= ~(0x0020u);
    }
    else if(outmode == 5)
    {
        TBCCTL4 |=  (0x0080u);
        TBCCTL4 &= ~(0x0040u);
        TBCCTL4 |=  (0x0020u);
    }
    else if(outmode == 6)
    {
        TBCCTL4 |=  (0x0080u);
        TBCCTL4 |=  (0x0040u);
        TBCCTL4 &= ~(0x0020u);
    }
    else
    {
        TBCCTL4 |= (0x0080u);
        TBCCTL4 |= (0x0040u);
        TBCCTL4 |= (0x0020u);
    }

}
void TimerB_COM5(unsigned int tbccr5, char outmode)
{
    TBCCTL5 &= ~(CAP);
    TBCCR5 = tbccr5;


    if(outmode == 1)
    {
        TBCCTL5 &= ~(0x0080u);
        TBCCTL5 &= ~(0x0040u);
        TBCCTL5 |=  (0x0020u);
    }
    else if(outmode == 2)
    {
        TBCCTL5 &= ~(0x0080u);
        TBCCTL5 |=  (0x0040u);
        TBCCTL5 &= ~(0x0020u);
    }
    else if(outmode == 3)
    {
        TBCCTL5 &= ~(0x0080u);
        TBCCTL5 |=  (0x0040u);
        TBCCTL5 |=  (0x0020u);
    }
    else if(outmode == 4)
    {
        TBCCTL5 |=  (0x0080u);
        TBCCTL5 &= ~(0x0040u);
        TBCCTL5 &= ~(0x0020u);
    }
    else if(outmode == 5)
    {
        TBCCTL5 |=  (0x0080u);
        TBCCTL5 &= ~(0x0040u);
        TBCCTL5 |=  (0x0020u);
    }
    else if(outmode == 6)
    {
        TBCCTL5 |=  (0x0080u);
        TBCCTL5 |=  (0x0040u);
        TBCCTL5 &= ~(0x0020u);
    }
    else
    {
        TBCCTL5 |= (0x0080u);
        TBCCTL5 |= (0x0040u);
        TBCCTL5 |= (0x0020u);
    }

}
void TimerB_COM6(unsigned int tbccr6, char outmode)
{
    TBCCTL6 &= ~(CAP);
    TBCCR6 = tbccr6;


    if(outmode == 1)
    {
        TBCCTL6 &= ~(0x0080u);
        TBCCTL6 &= ~(0x0040u);
        TBCCTL6 |=  (0x0020u);
    }
    else if(outmode == 2)
    {
        TBCCTL6 &= ~(0x0080u);
        TBCCTL6 |=  (0x0040u);
        TBCCTL6 &= ~(0x0020u);
    }
    else if(outmode == 3)
    {
        TBCCTL6 &= ~(0x0080u);
        TBCCTL6 |=  (0x0040u);
        TBCCTL6 |=  (0x0020u);
    }
    else if(outmode == 4)
    {
        TBCCTL6 |=  (0x0080u);
        TBCCTL6 &= ~(0x0040u);
        TBCCTL6 &= ~(0x0020u);
    }
    else if(outmode == 5)
    {
        TBCCTL6 |=  (0x0080u);
        TBCCTL6 &= ~(0x0040u);
        TBCCTL6 |=  (0x0020u);
    }
    else if(outmode == 6)
    {
        TBCCTL6 |=  (0x0080u);
        TBCCTL6 |=  (0x0040u);
        TBCCTL6 &= ~(0x0020u);
    }
    else
    {
        TBCCTL6 |= (0x0080u);
        TBCCTL6 |= (0x0040u);
        TBCCTL6 |= (0x0020u);
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
