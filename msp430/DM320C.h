/**********************************************
 *
 *      Author  :       Shawn Guo
 *      Mail    :       iguoxiaopeng@gmail.com
 *      Date    :       2013/8/27
 *      Last    :
 *      Notes   :       DM320C步进电机驱动
 *      Tool    :       DM320C步进电机/两相四线步进电机
 *
 *-----------------------------------------------------------
 *
 *      PUL+    P1.0
 *      PUL-    GND
 *      DIR+    P1.2
 *      DIR-    GND
 *      ENA+    悬空
 *      ENA-    悬空
 *      ALM+    -
 *      ALM-    -
 *
 *      A+  A-  B+  B-
 *      黑  绿  红  蓝
 *
 *      PUL下降沿脉冲有效.
 *
 **********************************************/
#ifndef __DM320C_H__
#define __DM320C_H__

#include "clock.h"

#define     DM320C_PUL_DIR_OUT      (P1DIR |= BIT0)
#define     DM320C_PUL_0            (P1OUT &= ~BIT0)
#define     DM320C_PUL_1            (P1OUT |=  BIT0)

#define     DM320C_DIR_DIR_OUT      (P1DIR |= BIT2)
//DIR = 0, 顺时针转动;   DIR = 1, 逆时针转动;
#define     DM320C_DIR_0            (P1OUT &= ~BIT2,DELAY_US(5))    //DIR至少提前PUL下降沿5us以上.
#define     DM320C_DIR_1            (P1OUT |=  BIT2,DELAY_US(5))

//代表一个脉冲
#define     DM320C_PUL              (DM320C_PUL_0,DELAY_US(7),DM320C_PUL_1,DELAY_US(7)) //脉冲宽度不低于7us.

//DM320C_PLUSE_NUM表示多少脉冲一圈, 可以调节SW5,SW6调节;
#define     DM320C_PLUSE_NUM        (3200)
//DM320C_DELAY 决定步进电机的速度, 提高PLUSE_NUM可以会提高精度, 但相同延时会花费更多时间.
#define     DM320C_DELAY            (DELAY_MS(1))
/***************************************************************************//**
 * @brief
 * @param
 * @return none
 *******************************************************************************/
void DM320C_Init_IO();

#endif /* __DM320C_H__ */
