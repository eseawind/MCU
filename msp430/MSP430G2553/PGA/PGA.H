/**********************************************
 *
 * 		Author	:		Shawn Guo
 * 		Date	:		2013/5/2
 *      Last    :       2013/5/2
 * 		Notes	:       Programmable Gain Amplifier
 * 		Tool    :	    MSP430G2553, LF353, CD4051
 *
 *************************************************
 *                  CD4051
 *              -----------------
 *        CH4   |1            16|   VDD
 *        CH6   |2            15|   CH2
 *    COM I/O   |3            14|   CH1
 *        CH7   |4            13|   CH0
 *        CH5   |5            12|   CH3
 *        INH   |6            11|   A   ---> P1.3
 *        VEE   |7            10|   B   ---> P1.4
 *        VSS   |8             9|   C   ---> P1.5
 *              -----------------
 **********************************************/

#ifndef __PGA__
#define __PGA__

#define CD4051_A1   (P1OUT |= BIT3)
#define CD4051_A0   (P1OUT &= ~BIT3)
#define CD4051_A    (P1OUT & BIT3)

#define CD4051_B1   (P1OUT |= BIT4)
#define CD4051_B0   (P1OUT &= ~BIT4)
#define CD4051_B    (P1OUT & BIT4)

#define CD4051_C1   (P1OUT |= BIT5)
#define CD4051_C0   (P1OUT &= ~BIT5)
#define CD4051_C    (P1OUT & BIT5)

/*
#define CD4051_INH1   (P1OUT |= BIT6) // 不用时可以直接接地以省IO资源
#define CD4051_INH2   (P1OUT &= ~BIT6)
*/


void PGA_IO_Init(); // 初始化IO口
void PGA_Channel(int ch);  // 切换通道

#endif // __PGA__
