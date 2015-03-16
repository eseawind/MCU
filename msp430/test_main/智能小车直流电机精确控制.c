/**********************************************
 *
 * 		Author	:		Shawn Guo
 *      Mail    :       iguoxiaopeng@gmail.com
 * 		Date	:       2013/8/18
 *      Last    :       2013/8/18
 * 		Notes	:
 *
 * 		Tool    :       MSP-EXP430F2618 / 智能小车
 **********************************************/

#include <msp430f2618.h>
#include "clock.h"
#include "LCD5110.h"
#include "Timer.h"
#include "drive.h"
#include "track.h"

unsigned char LCD_BUFFER1[] = "Speed:";

extern int SPEED_COUNT_L, SPEED_COUNT_R, SPEED_BUF_L, SPEED_BUF_R;
extern unsigned char SPEED_DISPLAY_FLAG;
int SPEED_L = 0, SPEED_R = 0;
long int SPEED_SUM_L = 0, SPEED_SUM_R = 0;
int speed = 50;

int main()
{
    WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT

    Clock_Init(0,0,0,CPU_FF);

    Drive_IO_Init();
    Drive_PWM_Init();

    Drive_Motor_L(0, speed);
    Drive_Motor_R(0, speed);

    while(1)
    {
        MOTOR_LEFT_STOP;
        MOTOR_RIGHT_STOP;
        DELAY_MS(100);
        MOTOR_LEFT_BACK;
        MOTOR_RIGHT_BACK;
        DELAY_MS(10);
    }
}
