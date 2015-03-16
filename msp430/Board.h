/**********************************************
 *
 *      Author  :       Shawn Guo
 *      Mail    :       iguoxiaopeng@gmail.com
 *      Date    :       2013/8/18
 *      Last    :
 *      Notes   :       拓展版模块功能
 *      Tool    :       MSP-EXP430F2618拓展板
 **********************************************/
#ifndef __BOARD_H__
#define __BOARD_H__


/***************************************************************************//**
*       LED1        --->        P8.5
*       LED2        --->        P8.4
*       LED3        --->        P8.3
*       BEEP        --->        P8.2
*       KEY0        --->        P1.0
*       KEY1        --->        P1.1
*       KEY2        --->        P1.2
*       KEY3        --->        P1.3
*
*
 *******************************************************************************/


#define     BOARD_LED1_IO   (P8DIR |=  BIT5, P8OUT &= ~BIT5)
#define     BOARD_LED1_1    (P8OUT |=  BIT5)
#define     BOARD_LED1_0    (P8OUT &= ~BIT5)

#define     BOARD_LED2_IO   (P8DIR |=  BIT4, P8OUT &= ~BIT4)
#define     BOARD_LED2_1    (P8OUT |=  BIT4)
#define     BOARD_LED2_0    (P8OUT &= ~BIT4)

#define     BOARD_LED3_IO   (P8DIR |=  BIT3, P8OUT &= ~BIT3)
#define     BOARD_LED3_1    (P8OUT |=  BIT3)
#define     BOARD_LED3_0    (P8OUT &= ~BIT3)

#define     BOARD_BEEP_IO   (P8DIR |=  BIT2, P8OUT &= ~BIT2)
#define     BOARD_BEEP_1    (P8OUT |=  BIT2)
#define     BOARD_BEEP_0    (P8OUT &= ~BIT2)
#define     BOARD_BEEP      (BOARD_BEEP_1, DELAY_MS(200), BOARD_BEEP_0)


/***************************************************************************//**
 * @brief
 * @param
 * @return none
 *******************************************************************************/

#endif
