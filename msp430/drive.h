/**********************************************
 *
 * 		Author	:		Shawn Guo
 *              Mail    :       iguoxiaopeng@gmail.com
 * 		Date	:       2013/7/27
 *              Last    :       2013/8/3   确定f2618 IO端口及PWM配置
 *                              2013/8/14  优化代码,增强可读性
 *                              2013/8/18  金属探测功能
 *                              2013/8/    光源追踪功能
 *
 *
 * 		Notes	:       L293N for Smart Car
 * 		Tool    :       MSP430X2XX
 **********************************************/
#ifndef __DRIVE_H__
#define __DRIVE_H__

/**************************************************************
//电机驱动pwm占用TimerA up mode, 1ms中断,
//码盘测速占用P2.4 P2.5端口中断, TimerA 1ms中断
//舵机占用P4.5(TB5) 比较模式, up mode 20ms中断.
//超声波占用端口P7.4。 占用P4.1 捕获模式
***************************************************************/

/******************电机端口*******************************
*   左侧电机    P1.1    P1.2        PWM(P1.3 TA2)
*   右侧电机    P1.4    P1.5        PWM(P1.6 TA1)
*********************************************************/
#define INL_DIR (P1DIR |=  BIT1 + BIT2)
#define IN1_0   (P1OUT &= ~BIT1)    //直流减速电机1  输入端
#define IN1_1   (P1OUT |=  BIT1)
#define IN2_0   (P1OUT &= ~BIT2)
#define IN2_1   (P1OUT |=  BIT2)

#define IN5_0   (P1OUT &= ~BIT1)    //直流减速电机3   输入端
#define IN5_1   (P1OUT |=  BIT1)
#define IN6_0   (P1OUT &= ~BIT2)
#define IN6_1   (P1OUT |=  BIT2)

#define INR_DIR (P1DIR |=  BIT4 + BIT5)
#define IN3_0   (P1OUT &= ~BIT4)    //直流减速电机2   输入端
#define IN3_1   (P1OUT |=  BIT4)
#define IN4_0   (P1OUT &= ~BIT5)
#define IN4_1   (P1OUT |=  BIT5)

#define IN7_0   (P1OUT &= ~BIT4)    //直流减速电机4   输入端
#define IN7_1   (P1OUT |=  BIT4)
#define IN8_0   (P1OUT &= ~BIT5)
#define IN8_1   (P1OUT |=  BIT5)

#define ENL_IO  (P1DIR |=  BIT3, P1SEL |= BIT3) //输出,PWM第二功能打开
#define ENL_0   (P1OUT &= ~BIT3)    //左侧直流电机使能端   P1.3     TA2
#define ENL_1   (P1OUT |=  BIT3)

#define ENR_IO  (P1DIR |=  BIT6, P1SEL |= BIT6) //输出,PWM第二功能打开
#define ENR_0   (P1OUT &= ~BIT6)    //右侧直流电机使能端   P1.6     TA1
#define ENR_1   (P1OUT |=  BIT6)

/****************** 马达驱动基本操作 (前进后退需具体配置) **********************/
//切换前进后退的方向时需要改变。
#define MOTOR_LEFT_GO           (IN1_1, IN2_0)          //左侧电机前进
#define MOTOR_LEFT_BACK         (IN1_0, IN2_1)          //左侧电机后退
#define MOTOR_LEFT_STOP         (IN1_0, IN2_0)          //左侧电机停止

#define MOTOR_RIGHT_GO          (IN3_1, IN4_0)          //右侧电机前进
#define MOTOR_RIGHT_BACK        (IN3_0, IN4_1)          //右侧电机后退
#define MOTOR_RIGHT_STOP        (IN3_0, IN4_0)          //右侧电机停止

//电机驱动 PWM 配置, 无需配置定时器TimerA.
#define MOTOR_PWM_CLOCK     (TIMERA_F)            // PWM时钟 = TimerA Clock
#define MOTOR_PWM_F         (1000)                // 默认1000HZ
#define MOTOR_PWM_NUM       (MOTOR_PWM_CLOCK / MOTOR_PWM_F)   //给定的TACCR0值

//PWM递增最小刻度, 仅用于函数Drive_Morto_Speed_LL();,这里默认为0.5%速度递增.
#define MOTOR_MINI_PWM_SCALE    (MOTOR_PWM_NUM / 200)

#define MOTOR_PWM_TXCCRL    (TACCR2)            //左侧电机PWM信号对应的PWM比较器
#define MOTOR_PWM_TXCCRR    (TACCR1)


/***************************************************************************//**
 * @brief   Initialize the Drive for smart car.
 * @param
 *          mode = 0, forward
 *          mode = 1, backward
 *          mode = 2, stop (don't care about the parament of 'unsigned int speed')
 *
 *          speed ( 40 < speed < 100 )
 *          if speed == 0, it means do nothing about speed and just keep the old speed.
 * @return none
 *******************************************************************************/

//开启TimerA0,TimerB1中断. 如果又开了_EINT(), 请务必加入中断函数, 即使为空, 否则函数将中断跑飞.
void Drive_Init(unsigned char mode, unsigned int speed);
void Drive_IO_Init();   //配置电机左右侧的信号控制端口以及PWM端口
void Drive_PWM_Init();
void Drive_Motor_L(unsigned char mode, unsigned int speed);
void Drive_Motor_R(unsigned char mode, unsigned int speed);

void Drive_Morto_Speed_LL();    //将左电机的速度增加一个最小刻度,最小刻度可以编程设置.
void Drive_Morto_Speed_L_();    //将左电机速度减小一个刻度.
void Drive_Morto_Speed_RR();
void Drive_Morto_Speed_R_();

/***************************************************************************//**
 * @brief   控制小车转圈
 * @param
 *              mode = 0, 原地左转 (45 * angle)度;
 *              mode = 1, 原地右转 (45 * angle)度;
 *
 * @return none
 *******************************************************************************/
void Drive_Around(unsigned char mode, unsigned char angle);















/******************光电码盘端口*******************************
*   左侧码盘    P2.0        端口中断
*   右侧码盘    P2.1        端口中断
**************************************************************/
#define SPEED_BITL  (BIT0)
#define SPEED_BITR  (BIT1)
#define SPEED_PXDIR (P2DIR)
#define SPEED_PXREN (P2REN)
#define SPEED_PXOUT (P2OUT)
#define SPEED_PXIE  (P2IE)
#define SPEED_PXIES (P2IES)
#define SPEED_PXIFG (P2IFG)
#define SPEED_PXIN  (P2IN)

#define SPEED_DISABLE   (SPEED_PXIE &= ~(SPEED_BITL + SPEED_BITR))


#define SPEED_OUT1  (SPEED_PXIN & SPEED_BITL)
#define SPEED_OUT2  (SPEED_PXIN & SPEED_BITR)

#define SPEED_MAPAN     (20)                  //码盘一圈的空隙个数, 空隙为低电平
#define SPEED_TIRE_R    (33)                  //小车轮胎半径 单位：mm
#define SPEED_GRITH     (103)                 // 每个码盘代表的距离. 单位毫米，扩大10倍.
/***************************************************************************//**
 * @brief  测速模块
 * @param
 *
 * @return 一段时间内走过的路程,单位mm, 根据时间计算出速度
 *******************************************************************************/
void Drive_Speed_IO();//输入,上拉电阻,下降沿触发中断
unsigned int Drive_Speed_Journey_L();
unsigned int Drive_Speed_Journey_R();

//码盘计数器,每经过一个码盘空隙, 下降沿中断使得SPEED_COUNT_x ++;
extern int SPEED_COUNT_L, SPEED_COUNT_R, SPEED_BUF_L, SPEED_BUF_R;
//速度显示标志位, 当速度计算出来, 置一表示可以显示.
extern unsigned char SPEED_DISPLAY_FLAG;

/******************测速中断示例********************
#pragma vector=PORT2_VECTOR
__interrupt void P2_ISR (void)
{
    if(P2IFG & SPEED_BITL)
    {
        SPEED_COUNT_L++;
    }
    if(P2IFG & SPEED_BITR)
    {
        SPEED_COUNT_R++;
    }

    // ....something else.
    P2IFG = 0;
}

定一段时间后, 将COUNT_X的值存在BUF_X中,
并调用Drive_Speed_Journey_L()和Drive_Speed_Journey_R()得到这段时间行驶的距离
除以时间即可得到速度
//注意:定时要保证这段时间int(16-bits)类型的COUNT不会溢出.




#pragma vector=TIMERA0_VECTOR
__interrupt void TimerA0(void)  //5s中断显示,显示速度.
{
    static int i = 0;

    if(++i >= 1000) //5s
    {
        i = 0;
        SPEED_DISPLAY_FLAG = 1; //显示SPEED
        SPEED_BUF_L = SPEED_COUNT_L;
        SPEED_BUF_R = SPEED_COUNT_R;
        SPEED_COUNT_L = 0;
        SPEED_COUNT_R = 0;
    }
}


    while(1)
    {
        if(SPEED_DISPLAY_FLAG == 1)
        {
            SPEED_DISPLAY_FLAG = 0;

            SPEED_L = Drive_Speed_Journey_L();
            SPEED_R = Drive_Speed_Journey_R();
            LCD5110_Long2char(SPEED_L);
            LCD5110_Write_String(36, 0, 0);
            LCD5110_Long2char(SPEED_R);
            LCD5110_Write_String(36, 1, 0);
        }
    }

*************************************************************************************/







/******************舵机端口*******************************
*   舵机脉冲信号      PWM(TB5)    P4.5
**********************************************************/
#define SERVO_PWM_CLOCK     (TIMERB_F)
#define SERVO_PWM_F         (50)                                //舵机频率 50HZ, 20ms
#define SERVO_PWM_NUM       (SERVO_PWM_CLOCK / SERVO_PWM_F)     //PWM对应的定时器值

#define SERVO_IO    (P4DIR |= BIT5, P4SEL |= BIT5)      //舵机信号输入端 TB5, 开启PWM功能

#define SERVO_MAX   6500                                //定时器可以定义的最大值, 2.5ms
#define SERVO_MIN   2000                                //最小值, 0.5ms
#define SERVO_MEDIUM    (4250)                          //中间值

#define SERVO_DELAY             ((DELAY_US(200)))           //舵机最小刻度延时,和舵机刻度一起决定舵机转动的速度和卡顿情况
#define SERVO_FULL_SCALE        (SERVO_MAX - SERVO_MIN)                 //舵机的最小刻度, SERVO_SCALE代表舵机180度满量程。
#define SERVO_MEDIUM_SCALE      (SERVO_FULL_SCALE / 2)               //中间刻度

#define SERVO_TXCCR             (TBCCR5)            //控制高电平的定时器
/***************************************************************************//**
* @brief  舵机控制
* @param
*        scale   (0 ~ SERVO_FULL_SCALE)        方位示意图       MEDIUM_SCALE
*                                                 FULL_SCALE                         0
*
*
*        angle(0~180);从angle1缓慢转动到angle2, 速度由 SERVO_DELAY 控制.
*
*
* @return
*******************************************************************************/
void Drive_Servo_Init();    //TimerB 20msPWM方波, TB5控制高电平宽度
void Drive_Servo_Move(unsigned int angle);    //直接移动到指定角度
void Drive_Servo_Turn(unsigned int angle1, unsigned int angle2);  //慢慢从angle1转动到angle2，速度由SERVO_FULL_SCALE 、SERVO_DELAY决定

/*********************************************************************************
#pragma vector=TIMERB1_VECTOR           //舵机TimerB1中断, 20ms中断一次.
__interrupt void TimerB1(void)
{
    switch(TBIV)                //  TB中断源软件判断,R0有自己单独的中断向量
    {
    case 2:          // 中断源为R1, 超声波测距中断端口

        break;
    case 4:          // 中断源为R2
        break;
    case 6:             // R3
        break;
    case 8:             // R4
        break;
    case 10:             // R5
        break;
    case 12:             // R6
        break;
    case 14:         // 中断源为Timer_B
        break;
    }

}
****************************************************************************/


/******************超声波端口*******************************
*  超声波控制端口      P7.0
*  超声波接受端口      P4.1   (TB1)捕获高电平脉冲宽度
************************************************************/
//超声波控制端口   ultrasonic
#define ULT_TRIG1  (P7OUT |=  BIT0)
#define ULT_TRIG0  (P7OUT &= ~BIT0)

#define ULT_ECHO        (P4IN & BIT1)
#define UTL_IO_INIT     (P7DIR |= BIT0, P7OUT &= ~BIT0, P4DIR &= ~BIT1, P4SEL |= BIT1)

#define UTL_OVERFLOW_TIMER  (SERVO_PWM_NUM)   //定时器溢出一次多少值, 这里与舵机共用TimerB
#define UTL_START       (ULT_TRIG1, DELAY_US(30), ULT_TRIG0)    //开启一次超声波测距
#define UTL_SAFE        (35)                //默认的超声波安全距离

void Drive_UTL_Init();  //超声波模块IO初始化

//根据UTL_START_TIME, UTL_END_TIME, UTL_TIME_OVERFLOW和时钟频率，返回距离值 cm.
//距离值存放在UTL_BUFFER中
void Drive_UTL_DISTANCE();

//void Drive_UTL_Avoid(); //超声波避障算法,控制舵机从0~180测距,舵机转向无障碍的地点.


//记录超声波模块返回的脉冲时间
// UTL_FLAG = 0  超声波空闲，可以开始计时
// UTL_FLAG = 1  超声波正在计时, 若中断来临表示计时完成
// UTL_FLAG = 2  超声波计时完成, 等待处理
extern unsigned int UTL_START_TIME, UTL_END_TIME, UTL_TIME_OVERFLOW, UTL_BUFFER, UTL_FLAG;
// UTL_LLEFT~~~~UTL_RRIGHT
extern unsigned int UTL_LLEFT, UTL_LEFT, UTL_FRONT, UTL_RIGHT, UTL_RRIGHT;


/******************金属探测端口*******************************
*   接近开关IO          P2.4        上拉电阻, 下降沿中断
*
*
*   注意:调试模式下，由于电压不够5V，接近开关不会正常工作。
************************************************************/
#define     JSTC_IO_DIR         (P2DIR &= ~BIT4, P2REN |= BIT4, P2OUT |= BIT4)
#define     JSTC_ENABLE         (P2IE |= BIT4, P2IES |= BIT4, P2IFG &= ~BIT4)
#define     JSTC_DISABLE        (P2IE &= ~BIT4, P2IFG &= ~BIT4)
#define     JSTC_IFG            ((P2IFG & BIT4))

//JSTC_FALG = 0, 没有探测到金属
//JSTC_FLAG = 1, 探测到金属
extern unsigned char JSTC_FLAG;


/*****************金属探测中断示例********************

#pragma vector=PORT2_VECTOR
__interrupt void P2_ISR (void)
{

    if(JSTC_IFG)        //金属探测中断处理
    {
    }
    // ....something else.
    P2IFG = 0;
}

*************************************************/


/******************红外端口*******************************
*  红外端口L             P2.2
*  红外端口R             P2.3
*  上拉电阻,下降沿中断
************************************************************/

#define INFRARED_L  (P2IN & BIT2)
#define INFRARED_R  (P2IN & BIT3)
#define INFRARED_IO (P2DIR &= ~(BIT2 + BIT3), P2REN |= (BIT2 + BIT3), P2OUT |= (BIT2 + BIT3), P2IE |= (BIT2 + BIT3), P2IES &= ~(BIT2 + BIT3),P2IFG &= ~(BIT2+BIT3)




#endif /* __DRIVE_H__ */
