/***************************************************
 *         Author: Shawn Guo                       *
 *         Date  : 2013/2/14                       *
 *         Last  : 2013/2/14                       *
 *         Notes : 看门狗寄存器                   *
 *                P1 | JP1(0 1 ... A B)           *
 **************************************************/

#include <reg52.h>

sfr WDT_CONTR = 0XE1;      // STC单片机特有的看门狗寄存器，需自定义标号

sbit LED = P1^0;

void Delayms(unsigned int xms);

int main()
{
    // WDT_CONTR: - - EN_WDT        CLR_WDT       IDLE_WDT            PS2 PS1 PS0
    //                看门狗允许位  看门狗清零位  空闲模式是否计数位  溢出时间设定
    WDT_CONTR = 0X35; // 00 1 1 0 101， 启动看门狗，清零，设定溢出时间为2.0971s

    LED = 0;
    Delayms(500);
    LED = 1;

    while(1)
    {
        Delayms(1000);         // 改变间隔可以看到变化，参考值：1000,3000
        WDT_CONTR = 0X35;        // 每隔一段时间，需要喂狗一次(即寄存器计数清零)
    }
    return 0;
}

void Delayms(unsigned int xms)      // 毫秒级延时
{
    unsigned char i;

    while(xms--)            // --xms 与 xms-- 在汇编层次有差距
        for(i = 0; i < 110; i++)
            ;
}

