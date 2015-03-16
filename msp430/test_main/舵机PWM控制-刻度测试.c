#include "clock.h"
#include "Timer.h"
#include "drive.h"
#include <msp430f2618.h>

void main()
{
    WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT

    Clock_Init(0,0,0,CPU_FF); // 1M DCO
    Drive_Servo_Init();

    while(1)
    {
        SERVO_TXCCR = 2000;
        DELAY_MS(1000);

        SERVO_TXCCR = 4250;
        DELAY_MS(1000);

        SERVO_TXCCR = 6500;
        DELAY_MS(2000);

        // Drive_Servo_Turn(0, 180);
        // Drive_Servo_Turn(180, 0);
    /**
        Drive_Servo_Turn(0, 30);
        DELAY_MS(3000);
        Drive_Servo_Turn(30, 60);
        DELAY_MS(3000);
        Drive_Servo_Turn(60, 90);
        DELAY_MS(3000);
        Drive_Servo_Turn(90, 120);
        DELAY_MS(3000);
        Drive_Servo_Turn(120, 150);
        DELAY_MS(3000);
        Drive_Servo_Turn(150, 180);
        DELAY_MS(3000);
        Drive_Servo_Turn(180, 0);
        DELAY_MS(3000);
        **/
    }

}
