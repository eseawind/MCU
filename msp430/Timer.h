/**********************************************
 *
 * 		Author	:		Shawn Guo
 *      Mail    :       iguoxiaopeng@gmail.com
 * 		Date	:       2013/7/26
 *      Last    :
 * 		Notes	:       Timer-16Bit
 * 		Tool    :       MSP430X2XX
 **********************************************/
#ifndef __TIMER_H__
#define __TIMER_H__

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
 * ------------------------------------------------------------------------------
 *
 * @return none
 *******************************************************************************/

// TimeA 3种常用模式,对应 TIMERA1_VECTOR中断函数, 开启TAIE
// 若不需要则不用配置TACCR1,TACCR2,置0即可。
// 注意与TimerA_R_Init()的对比(TAIE的开启与否);
void TimerA_UpMode_Init(unsigned char clk, unsigned char div, unsigned int taccr0, unsigned int taccr1, unsigned int taccr2);
void TimerA_ConMode_Init(unsigned char clk, unsigned char div, unsigned int taccr0, unsigned int taccr1, unsigned int taccr2);
/***Up-Down
    TimerA计数至TACCR0, TACCR0 CCIFG中断;    计数至0, TAIFG中断.
 ****/
void TimerA_UpDownMode_Init(unsigned char clk, unsigned char div, unsigned int taccr0, unsigned int taccr1, unsigned int taccr2);

//其他定时器模式:R0,R1,R2
//中断函数: R0中断写在R0特有的中断函数中，其他写在通用TimerA中断函数中
//TimerA计数到TACCR0, TACCR0 CCIFG中断; TimerA计数到TACCR0 + 1, 立刻变为0,此时TAIFG中断.
//开启不同的中断以取得不同的效果,但不要同时开启.因此此函数不要将TAIE置为1.
void TimerA_R_Init(unsigned char clk, unsigned char div, unsigned char mod,  int taccr0, unsigned int taccr1, unsigned int taccr2);
//例如: TimerA_R_Init(2, 1, 1, 10000, 5000, 0);

//基本操作
void TimerA_SetClk(unsigned char clk, unsigned char div);
void TimerA_SetMod(unsigned char mod);
void TimerA_Clear();
void TimerA_Stop();
void TimerA_Enable();
void TimerA_Disable();

void TimerA_R0_Enable();
void TimerA_R1_Enable();
void TimerA_R2_Enable();

void TimerA_R0_Disable();
void TimerA_R1_Disable();
void TimerA_R2_Disable();

/**************中断函数实例 start***************
black hat
#pragma vector=TIMERA1_VECTOR			// g2553 TIMER0_A0_VECTOR
__interrupt void TimerA1(void)
{
    switch(TAIV)                //  TA中断源软件判断,R0有自己单独的中断向量
    {
    case 2:          // 中断源为R1
        break;
    case 4:          // 中断源为R2
        break;
    case 10:         // 中断源为Timer_A
        break;
    }
}

#pragma vector=TIMERA0_VECTOR
__interrupt void TimerA0(void)
{
}

***************中断函数实例 end*****************/






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
 * ------------------------------------------------------------------------------
 *          CCI(Timer_A捕获模块的输入信号电平(异步))
 *          SCCI(Timer_A捕获模块的输入信号电平(同步))
 * ------------------------------------------------------------------------------
 *
 * @return none
 *******************************************************************************/

//捕获模块 Capture
// 注意如果不用TAIE就不要打开TAIE,或者一个空的TAIE中断函数.否则打开TAIE却没有TAIE中断就会二次触发TACCR0 CCIFG.
void TimerA_CAP0_Init(unsigned char ccis, unsigned char cm, unsigned char scs);
void TimerA_CAP1_Init(unsigned char ccis, unsigned char cm, unsigned char scs);
void TimerA_CAP2_Init(unsigned char ccis, unsigned char cm, unsigned char scs);

unsigned char TimerA_SCCI0();
unsigned char TimerA_SCCI1();
unsigned char TimerA_SCCI2();
unsigned char TimerA_CCI0();
unsigned char TimerA_CCI1();
unsigned char TimerA_CCI2();
unsigned char TimerA_COV0();
unsigned char TimerA_COV1();
unsigned char TimerA_COV2();

/******************Timer_A Camputer模块IO配置示例***********************

void TimerA_CAP_IO()
{
    P1DIR &= ~(BIT1);       //设为输入
    P1SEL |= BIT1;          //打开第二功能，作为Camputer的输入.
}




********************************************************************/

/***************************************************************************//**
 * @brief   配置比较模式并配合TACCR1/TACCR2的值(可以两路PWM输出),
 *          需提前配置定时器如何计数以确保TACCR0,需提前配置对应的IO打开第二功能。
 * @param
 *          taccr0 = 0          不改变TACCR0的值
 *                   x          TACCR0 = X;
 *          taccr1 = 0          不配置TACCR1
 *                   x          配置比较模块1，且配置TACCR1 = taccr1
 *          taccr2 = 0          不配置TACCR2
 *                   x          配置比较模块2，且配置TACCR2 = taccr2
 *
 *          outmode=    1       计数至TACCRx则置1
 *                      2       计数至TACCRx则取反，至TACCR0则置0
 *                      3       计数至TACCRx(1), TACCR0(0)
 *                      4       TACCRx(取反)
 *                      5       TACCRx(0)
 *                      6       TACCRx(取反),TACCR0(1)
 *                      7       TACCRx(0),TACCR0(1)
 * ------------------------------------------------------------------------------
 * @return none
 *******************************************************************************/
//比较模块  Compare
// 此函数前需提前定义定时器的模式，以确保TACCR0的计数方式。
// 常用的为增计数模式 TimerA_UpMode_Init(unsigned char clk, unsigned char div, unsigned int taccr0);
void TimerA_COM_Init(unsigned int taccr0,unsigned int taccr1, unsigned char outmode1, unsigned int taccr2, unsigned char outmode2);
/******************Timer_A Compare模块IO配置示例***********************
main函数实例：
    TimerA_COM_IO();
    TimerA_UpMode_Init(2, 1, 1000);
    TimerA_COM_Init(1000, 300, 7, 0, 0);

TimerA_COM_IO()
{
    P1DIR |= BIT1;
    P1SEL |= BIT1;
}
********************************************************************/



























/***************************************************************************//**
 * @brief   Initialize the TimerB sysyem - configure Timer_B clock, select the divider for
 *          clock, mode control.
 * @param
 * ------------------------------------------------------------------------------
 *          clk  =  0       TBCLK
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
 * ------------------------------------------------------------------------------
 *          bits =  16      Timer_B Counter length = 16-bits
 *                  12                               12
 *                  10
 *                  8
 *          @note   默认16bits Counter则不需要配置;
 *                  否则必须自行配置void TimerB_SetCounterLength(unsigned char bits);
 * ------------------------------------------------------------------------------
 * @return none
 *******************************************************************************/

// TimeB 3种常用模式,对应 TIMERB1_VECTOR中断函数, 开启TBIE
// 注意与TimerB_UpModeR0_Init()的对比(区别:开启哪个中断);
void TimerB_UpMode_Init(unsigned char clk, unsigned char div, unsigned int tbccr0);
void TimerB_ConMode_Init(unsigned char clk, unsigned char div);

//TimerB计数至TBCCR0, TBCCR0 CCIFG中断;    计数至0, TBIFG中断.
void TimerB_UpDownMode_Init(unsigned char clk, unsigned char div, unsigned int tbccr0);

//不开启TBIE的2种常用模式,因此需要额外配置TBCCRx CCIE. Up mode 和 Up-down默认开启TBCCR0 CCIE.
void TimerB_UpModeR0_Init(unsigned char clk, unsigned char div, unsigned int tbccr0);
void TimerB_UpDownModeR0_Init(unsigned char clk, unsigned char div, unsigned int tbccr0);
//可在以上基础上增加TBCCRx中断.如调用 TimerB_CCR1(),TimerB_CCR2()函数等


//基本操作
void TimerB_SetClk(unsigned char clk, unsigned char div);
void TimerB_SetMod(unsigned char mod);
void TimerB_Clear();
void TimerB_Stop();
void TimerB_Enable();
void TimerB_Disable();
void TimerB_SetCounterLength(unsigned char bits); //配置Timer_B的计数长度,默认(16-bit);

// 配置TBCCRx的值并使能.
void TimerB_CCR0(unsigned int tbccr0);
void TimerB_CCR1(unsigned int tbccr0);
void TimerB_CCR2(unsigned int tbccr0);
void TimerB_CCR3(unsigned int tbccr0);
void TimerB_CCR4(unsigned int tbccr0);
void TimerB_CCR5(unsigned int tbccr0);
void TimerB_CCR6(unsigned int tbccr0);

void TimerB_R0_Enable();
void TimerB_R1_Enable();
void TimerB_R2_Enable();
void TimerB_R3_Enable();
void TimerB_R4_Enable();
void TimerB_R5_Enable();
void TimerB_R6_Enable();

void TimerB_R0_Disable();
void TimerB_R1_Disable();
void TimerB_R2_Disable();
void TimerB_R3_Disable();
void TimerB_R4_Disable();
void TimerB_R5_Disable();
void TimerB_R6_Disable();

/**************中断函数实例 start***************

#pragma vector=TIMERB1_VECTOR			// g2553 TIMER0_A0_VECTOR
__interrupt void TimerB1(void)
{
    switch(TBIV)                //  TB中断源软件判断,R0有自己单独的中断向量
    {
    case 2:          // 中断源为R1
        break;
    case 4:          // 中断源为R2
        break;
    case 6:             // R3
        break;
    case 8:             // R4
        break;
    case 10:             // R5
        break;
    case 12:             // R6
        break;
    case 14:         // 中断源为Timer_B
        break;
    }
}

#pragma vector=TIMERB0_VECTOR
__interrupt void TimerB0(void)
{
}

***************中断函数实例 end*****************/

/***************************************************************************//**
 * @brief   Timer_B7 捕获/比较模块配置
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
 * ------------------------------------------------------------------------------
 *          CCI(Timer_B捕获模块的输入信号电平(异步))
 * ------------------------------------------------------------------------------
 *
 * @return none
 *******************************************************************************/

//捕获模块 Capture
// 注意如果不用TBIE就不要打开TBIE,或者一个空的TBIE中断函数.否则打开TBIE却没有TBIE中断就会二次触发TBCCR0 CCIFG.
void TimerB_CAP0_Init(unsigned char ccis, unsigned char cm, unsigned char scs);
void TimerB_CAP1_Init(unsigned char ccis, unsigned char cm, unsigned char scs);
void TimerB_CAP2_Init(unsigned char ccis, unsigned char cm, unsigned char scs);
void TimerB_CAP3_Init(unsigned char ccis, unsigned char cm, unsigned char scs);
void TimerB_CAP4_Init(unsigned char ccis, unsigned char cm, unsigned char scs);
void TimerB_CAP5_Init(unsigned char ccis, unsigned char cm, unsigned char scs);
void TimerB_CAP6_Init(unsigned char ccis, unsigned char cm, unsigned char scs);

unsigned char TimerB_CCI0();
unsigned char TimerB_CCI1();
unsigned char TimerB_CCI2();
unsigned char TimerB_CCI3();
unsigned char TimerB_CCI4();
unsigned char TimerB_CCI5();
unsigned char TimerB_CCI6();

unsigned char TimerB_COV0();
unsigned char TimerB_COV1();
unsigned char TimerB_COV2();
unsigned char TimerB_COV3();
unsigned char TimerB_COV4();
unsigned char TimerB_COV5();
unsigned char TimerB_COV6();

/******************Timer_B Camputer模块IO配置示例***********************

void TimerB_CAP_IO()
{
    P1DIR &= ~(BIT1);       //设为输入
    P1SEL |= BIT1;          //打开第二功能，作为Camputer的输入.
}




********************************************************************/



/***************************************************************************//**
 * @brief   配置比较模式.
 *          需提前配置定时器如何计数以确保TBCCR0,需提前配置对应的IO打开第二功能。
 * @param
 *          tbccr0 = 0          不改变TBCCR0的值
 *                   x          TBCCR0 = X;
 *
 *          outmode=    1       计数至TACCRx则置1
 *                      2       计数至TACCRx则取反，至TACCR0则置0
 *                      3       计数至TACCRx(1), TACCR0(0)
 *                      4       TACCRx(取反)
 *                      5       TACCRx(0)
 *                      6       TACCRx(取反),TACCR0(1)
 *                      7       TACCRx(0),TACCR0(1)
 *
 *          tbclgrp =   0       7通道独立
 *                      1       0,(1,2),(3,4),(5,6)
 *                      2       0,(1,2,3),(4,5,6)
 *                      3       (0,1,2,3,4,5,6)
 * ------------------------------------------------------------------------------
 * @return none
 *******************************************************************************/
//比较模块  Compare
// 此函数前需提前定义定时器的模式，以确保TBCCR0的计数方式。
// 常用的为增计数模式 TimerB_UpMode_Init(unsigned char clk, unsigned char div, unsigned int taccr0);
void TimerB_COM_Init(unsigned int tbccr0, unsigned char tbclgrp);

void TimerB_COM1(unsigned int tbccr1, char outmode);
void TimerB_COM2(unsigned int tbccr2, char outmode);
void TimerB_COM3(unsigned int tbccr3, char outmode);
void TimerB_COM4(unsigned int tbccr4, char outmode);
void TimerB_COM5(unsigned int tbccr5, char outmode);
void TimerB_COM6(unsigned int tbccr6, char outmode);
/******************Timer_A Compare模块IO配置示例***********************
main函数实例：
    TimerA_COM_IO()
    TimerA_UpMode_Init(2, 1, 1000);
    TimerA_COM_Init(1000, 300, 7, 0, 0);

********************************************************************/


#endif /* __TIMER_H__ */
