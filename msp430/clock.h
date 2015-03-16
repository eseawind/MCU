/**********************************************
 *
 *              Author	:       Shawn Guo
 *              Mail    :       iguoxiaopeng@gmail.com
 *              Date	:       2013/7/23
 *              Last    :       2013/8/9
 *              Notes	:       clock system
 *              Tool    :       MSP430x2xx
 *
 *              默认LFXT1低频(BCSCTL1 &= ~XTS),XT2为3~16M高频
 **********************************************/
#ifndef __CLOCK_H__
#define __CLOCK_H__

/*每次都必须配置的宏定义!!!*/
#define CPU_F       ((double)12000000)        // 这里的CPU_F需要自己配置，这里配置为1MHZ
#define CPU_FF      (12)                     //cpu时钟

#define TIMERA_DIV  (4)
#define TIMERA_F    (CPU_F / TIMERA_DIV)           //TimerA时钟

#define TIMERB_DIV  (4)
#define TIMERB_F    (CPU_F / TIMERB_DIV)           //TimerB时钟


#define DELAY_US(x) __delay_cycles((long)(CPU_F*(double)x/1000000.0))
#define DELAY_MS(x) __delay_cycles((long)(CPU_F*(double)x/1000.0))

extern void Clock_Init(unsigned char mclk_config, unsigned char smclk_config, unsigned char aclk_config, unsigned char dco_config);
// Clock_Init(0,0,0,1); // 1M DCO, 9K VLO ACLK.
// Clock_Init(4,4,0,1); //  12M XT2 MCLK, 12M XT2 SMCLK, VLOCLK ACLK, 1MDCO.
// Clock_Init(4,4,4,1); //  12M XT2 MCLK SMCLK, LFXT1 ACLK. 1MDCO
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
 *
 *      Clock_Init(4,4,0,CPU_FF); // 12M XT2 MCLK/SMCLK, 12M DCO ACLK
 *
 *
 ******************************************************************************/

extern void Clock_DCO_Set(unsigned char x,unsigned char y); // 输入参数为F2618 datasheet P42

extern void Clock_MCLK_Div(unsigned char Div);    //切换MCLK分频系数,不影响时钟源
extern void Clock_SMCLK_Div(unsigned char Div);   //切换SMCLK分频系数,不影响时钟源
extern void Clock_ACLK_Div(unsigned char Div);    //切换ACLK分频系数,不影响时钟源

extern void Clock_IE();      //Oscillator fault interrupt enable.
/*************************************************************/


#endif /* __CLOCK_H__ */
