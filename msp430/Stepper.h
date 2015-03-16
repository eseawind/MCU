/**********************************************
 *
 *      Author  :       Shawn Guo
 *      Mail    :       iguoxiaopeng@gmail.com
 *      Date    :       2013/8/26
 *      Last    :
 *      Notes   :       Stepper Motor
 *      Tool    :       MSP430X2XX  / 两相四线步进电机
 *
 *----------------------------------------------------------
 *      使用的步进电机为 42HS40-1539
 *      42HS  -   边长42mm的正方形外形, 40 - 高度40mm,    15 - 最大1.5A工作电流
 *      接线  A+  A-  B+  B-
 *            黑  绿  红  蓝
 *      步距角: 1.8度(电动机转子齿轮数目: 100, 两相单拍, 步距角 = 360 / (齿轮数 * 相数 * 通电系数))
 *      通电系数:前后通电相数一致为1, 否则为2.
 *      so...两相单拍步距角1.8度，两相双拍步距角0.9度
 *
 *      步进电机转速: n = ((步距角 * 电脉冲频率) / (2*pi) ) * 60s = xxx转/分.
 *----------------------------------------------------------
 *      实现两相四线步进电机的四拍/八拍工作方式.
 *      四拍: A+B+    A-B+    A-B-    A+B-(正向)
 *            A+B+    A+B-    A-B-    A-B+(反向)
 *
 *      八拍:   1   2   3   4   5   6   7   8
 *      A+      1   1   0   0   0   0   0   1
 *      A-      0   0   0   1   1   1   0   0
 *      B+      0   1   1   1   0   0   0   0
 *      B-      0   0   0   0   0   1   1   1
 **********************************************/
#ifndef __STEPPER_H__
#define __STEPPER_H__

#define     STEPPER_IO      (P1DIR |= (BIT0 + BIT1 + BIT2 + BIT3 + BIT), P1OUT &= ~(BIT0 + BIT1 + BIT2 + BIT3))

#define     STEPPER_A0_1      (P1OUT |=  BIT0)
#define     STEPPER_A0_0      (P1OUT &= ~BIT0)
#define     STEPPER_A1_1      (P1OUT |=  BIT1)
#define     STEPPER_A1_0      (P1OUT &= ~BIT1)
#define     STEPPER_A_ENABLE  ()

#define     STEPPER_B0_1      (P1OUT |=  BIT2)
#define     STEPPER_B0_0      (P1OUT &= ~BIT2)
#define     STEPPER_B1_1      (P1OUT |=  BIT3)
#define     STEPPER_B1_0      (P1OUT &= ~BIT3)

#define     STEPPER_A_ENABLE    ()
#define     STEPPER_B_ENABLE    ()

/***************************************************************************//**
 * @brief
 * @param
 * @return none
 *******************************************************************************/
void Module_Init();

#endif /* __STEPPER_H__ */
