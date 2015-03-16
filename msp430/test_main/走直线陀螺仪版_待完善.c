/**********************************************
 *
 * 		Author	:		Shawn Guo
 *      Mail    :       iguoxiaopeng@gmail.com
 * 		Date	:       2013/8/18
 *      Last    :       2013/8/18
 * 		Notes	:       走直线, 没有绝对的校准只靠记录码盘数量是走不出绝对的直线, 只能走出近似的直线.
 *                      绝对的直线必须加入校正功能. 此时应该考虑陀螺仪/电子指南/磁传感器进行绝对方向定位.
 * 		Tool    :       MSP-EXP430F2618 / 智能小车  / 测速模块
 **********************************************/

#include <msp430f2618.h>
#include "clock.h"
#include "LCD5110.h"
#include "Timer.h"
#include "drive.h"



int main()
{
    WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT

    Clock_Init(0,0,0,CPU_FF);

    LCD5110_Init();
}
