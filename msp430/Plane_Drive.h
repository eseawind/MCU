/**********************************************
 *
 *      Author  :       Shawn Guo
 *      Mail    :       iguoxiaopeng@gmail.com
 *      Date    :       2013/8/27
 *      Last    :       2013/9/4
 *      Notes   :       四旋翼驱动模块
 *      Tool    :       MSP430X2XX  / 好盈天行者20A电调    / 朗宇2212 KV980无刷电机
 *
 *------------------------------------------------------------------------------
 *      好盈20A电调: 20ms的频率, 1ms脉宽代表0%油门, 2ms脉宽代表100%油门.
 *                  为了防止伤人，电调默认启动不转，　需要0%油门等待3s，再慢慢拉高油门.
 *
 *      使用TimerB的R1~R4四路PWM信号输出控制电调.
 *
 *      十字模式:                   1
 *                                2    3
 *                                  4
 *      TBCCR1  --> 电调1
 *      TBCCR2  --> 电调2
 *      TBCCR3  --> 电调3
 *      TBCCR4  --> 电调4
 *
 *
 *      PWM信号说明:        20ms的周期, PWM信号从1ms~2ms变化.
 *                          1ms -> 0%       2ms -> 100%
 *      3M时钟, 60000为20ms，1ms (3000), 2ms(6000)
 *
 *
 **********************************************/


#ifndef __PLANE_DRIVE_H__
#define __PLANE_DRIVE_H__

//定时器选择为Up mode, PLANE_TXCCR0是根据TimerB_F / 50计算出来的, 要保证不能溢出
//使用外部12M晶振, TimerB /4分频, PLANE_TXCCR0 = 60000;
#define     PLANE_TXCCR0            (60000)

#define     PLANE_LOW_PWM           (3000)
#define     PLANE_HIGH_PWM          (6000)
#define     PLANE_MID_PWM           (PLANE_LOW_PWM + (PLANE_HIGH_PWM - PLANE_LOW_PWM) / 2)
#define     PLANE_PRECENT_PWM       ((PLANE_HIGH_PWM - PLANE_LOW_PWM) / 100)

//电调PWM调速 x : 0~100%
#define     PLANE_DT1(x)            (TBCCR1 = PLANE_LOW_PWM + (x) * PLANE_PRECENT_PWM)
#define     PLANE_DT2(x)            (TBCCR2 = PLANE_LOW_PWM + (x) * PLANE_PRECENT_PWM)
#define     PLANE_DT3(x)            (TBCCR3 = PLANE_LOW_PWM + (x) * PLANE_PRECENT_PWM)
#define     PLANE_DT4(x)            (TBCCR4 = PLANE_LOW_PWM + (x) * PLANE_PRECENT_PWM)

#define     PLANE_DT_CCR1(x)        (TBCCR1 = (x))
#define     PLANE_DT_CCR2(x)        (TBCCR2 = (x))
#define     PLANE_DT_CCR3(x)        (TBCCR3 = (x))
#define     PLANE_DT_CCR4(x)        (TBCCR4 = (x))


//电调锁定标志, 置0表示电调未锁定, 置1表示已锁定.
extern char PLANE_DT_LOCK;

//开启4路PWM信号IO, 输出默认的4路PWM信号, 解锁飞控
void Plane_Drive_Init_IO();

//油门行程设定.
void Plane_Drive_Set_Gas();

//解锁飞控板
void Plane_Drive_Unlock();

//锁定飞控板
void Plane_Drive_Lock();

//油门从0%遍历到100%,并返回. 每次变换10%, 保持1s.
void Plane_Drive_Accele();

//最短的延迟, 只保证平稳起飞, 不包括自稳
void Plane_Auto_Launch();

#endif /* __PLANE_DRIVE_H__ */


/*********************************************************************
电机启动例程:

int main()
{
    WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT

    Clock_Init(4,4,0,CPU_FF); // 12M XT2 MCLK/SMCLK, 12M DCO ACLK

    DELAY_MS(300);   //延时50ms等待飞控板初始化.多延时以保证电池安装完毕以及飞控系统初始化
    Plane_Drive_Init_IO();
    DELAY_MS(1000); // 延时1s保证系统初始化和自检查时间，延时过短会出现 绿绿绿绿LED灯闪烁.
    PLANE_BREAKOFF; //掰杆后启动电机，电机有延迟性，不可低于2s
    DELAY_MS(500);
    PLANE_CENTER;
    DELAY_MS(3000);
    PLANE_BREAKOFF;

    while(1)
    {
        //Plane_Drive_Accele();
    }
}


**********************************************************************/



/***************************************************************************//**
 * @brief
 * @param
 * @return none
 *******************************************************************************/
