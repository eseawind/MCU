/**********************************************
*
* 		Author	:		Shawn Guo
*       Mail    :       iguoxiaopeng@gmail.com
* 		Date	:       2013/10/27
*       Last    :       2013/11/3
* 		Notes	:       四驱车-移动站
*                       可以接收到基站的控制命令，并做出响应，返回必要的数据
* 		Tool    :       MSP430F2618 / NRF905 / 四驱车
**********************************************/



#include <msp430f2618.h>
#include "clock.h"
#include "Timer.h"
#include "USCI.h"
#include "NRF905.h"
#include "DS18B20.h"
#include "DHT11.h"
#include "drive.h"
#include "track.h"
#include "Board.h"

/************************ 基站命令 ******************************
*
*   1. 远程遥控(0x01):  远程遥控四驱车。
*   2. 自动循迹(0x02):  开启循迹模式
*   3. 自动避障(0x03):  开启超声波测距模式
*   4. 金属探测(0x04):  开启金属探测功能
*   5. 温度探测(0x05):  立即采样当前的温度并发送给基站
*
*
******************************************************************/
#define     CMD_CONTROL                 (0x01)
#define     CMD_TRACK                   (0x02)
#define     CMD_AVOIDANCE               (0x03)
#define     CMD_MENTAL                  (0x04)
#define     CMD_TEMPERATURE             (0x05)

#define     CMD_NUM                     (5)

//下面的命令为辅助命令
#define     CMD_RETURN                  (0xab)

/*************NRF905无线模块 全局变量*****************/
//四驱车无线地址 0xaa,0xbb,0xcc,0xdd
//地面站无线地址 0xdd,0xcc,0xbb,0xaa
unsigned char NRF905_RXDATA = 0; //默认一次接受一个字节的指令.
unsigned char NRF905_TXDATA[2] = {0}; //默认一次发送四个字节的指令.
unsigned char NRF905_REV_FLAG = 0; // 数据接收标志，接收到数据后置1。
/************* 超声波避障模块 ***********************/
int UTL_ANGLE = 0;
unsigned char UTL_DIR = 3;      // 0,1,2,3,4,5,6  七个方向测距, 初始默认居中
int utl_flag = 0;//小车前进摇头标志位，即前方无障碍物时，前进，但仍然摆动测距
unsigned char UTL_MODULE_START_FLAG = 0; //超声波模块启动.默认不启动，为0.
#define UTL_SPEED_MAIN          (50)

/************* 金属检测模块 **************************/
int RETURN_FLAG = 0; //返回标志，当接受到返回标志时，置为1
extern unsigned char JSTC_FLAG; //金属探测标志位,来自drive.c

void main()
{
    WDTCTL = WDTPW + WDTHOLD;
    Clock_Init(0,0,0,CPU_FF); // 12M DCO

    //RF905 接受数据中断配置,接收到数据时P1.7触发中断
    P1DIR &= ~BIT7; //  设为输入
    P1IES &= ~BIT7; //  中断触发沿：上升沿；
    P1IE  |=  BIT7; //    开启 I/O中断
    P1IFG = 0;

    NRF905_Init();
    NRF905_Set_RevAddress(0xaa, 0xbb, 0xcc, 0xdd);//配置接收地址，四驱车的无线地址.
    NRF905_Set_SendAddress(0xdd, 0xcc, 0xbb, 0xaa);//配置发送地址: 地面站的无线地址.

    Drive_IO_Init(); //配置IO口
    Track_IO_Init(); //配置循迹IO
    DHT11_Init_IO(); //温湿度采样IO
    JSTC_IO_DIR;   //金属探测IO
    P2IFG = 0;
    BOARD_BEEP_IO;  //蜂鸣器
    BOARD_LED3_IO;  //LED灯

    _EINT(); //开总中断，在所有开启中断的模块都需要写中断函数(即使为空)

    NRF905_Set_Mode(NRF905_RCV);   //切换到接受模式，一直接受数据，收到数据后触发中断。

    while(1)    //主循环，等待用flag户按键控制
    {
        if(NRF905_REV_FLAG == 1) //如果收到数据,则执行下面的数据处理
        {
            //读取标志位清零
            NRF905_REV_FLAG = 0;
            //读取命令另执行响应的操作.
            NRF905_Read_Data(NRF905_RRP, &NRF905_RXDATA, 1);
            switch(NRF905_RXDATA) //译码
            {
            case CMD_CONTROL:
            {
                Drive_PWM_Init(); // 开启TimerA Up mode, 输出PWM频率1000HZ
                while(RETURN_FLAG == 0) //RETURN_FLAG
                {
                    if(NRF905_REV_FLAG) //接受到指令，进行相应的操作
                    {
                        NRF905_REV_FLAG = 0;
                        NRF905_Read_Data(NRF905_RRP, &NRF905_RXDATA, 1);
                        switch(NRF905_RXDATA)
                        {
                        case 0x10:
                            // 0,静止不动
                            MOTOR_LEFT_STOP;
                            MOTOR_RIGHT_STOP;
                            break;
                        case 0x11:
                            Drive_Motor_L(0, 50);
                            Drive_Motor_R(0, 50);
                            break;
                        case 0x12:
                            Drive_Motor_L(0, 100);
                            Drive_Motor_R(0, 50);
                            break;
                        case 0x13:
                            Drive_Motor_L(0, 50);
                            Drive_Motor_R(1, 50);
                            break;
                        case 0x14:
                            Drive_Motor_L(1, 100);
                            Drive_Motor_R(1, 50);
                            break;
                        case 0x15:
                            Drive_Motor_L(1, 50);
                            Drive_Motor_R(1, 50);
                            break;
                        case 0x16:
                            Drive_Motor_L(1, 50);
                            Drive_Motor_R(1, 100);
                            break;
                        case 0x17:
                            Drive_Motor_L(1, 50);
                            Drive_Motor_R(0, 50);
                            break;
                        case 0x18:
                            Drive_Motor_L(0, 50);
                            Drive_Motor_R(0, 100);
                            break;
                        case CMD_RETURN:
                            RETURN_FLAG = 1;
                            break;
                        }
                    }
                    DELAY_MS(100);
                }
                //返回主菜单.
                RETURN_FLAG = 0;
                MOTOR_LEFT_STOP;
                MOTOR_RIGHT_STOP;
                TimerA_Disable();
                TimerA_R0_Disable();
            }
            break;
            case CMD_TRACK:
            {
                //开始自动循迹
                Drive_PWM_Init(); // 开启TimerA Up mode, 输出PWM频率1000HZ
                while(NRF905_REV_FLAG == 0)//在没有接收到下一步指令前，将一直循环循迹操作
                    Track_Following2();
                //退出循环，则需要关闭循迹接口.这里关闭PWM输出
                MOTOR_LEFT_STOP;
                MOTOR_RIGHT_STOP;
                TimerA_Disable();
                TimerA_R0_Disable();
            }
            break;
            case CMD_AVOIDANCE:
            {
                //电机驱动pwm占用TimerA up mode, 1ms中断,
                //码盘测速占用P2.4 P2.5端口中断, TimerA 1ms中断
                //舵机占用P4.5(TB5) 比较模式, up mode 20ms中断.
                //超声波占用端口P7.4。 占用P4.1 捕获模式, 利用TimerA计时.
                Drive_PWM_Init();
                Drive_Servo_Init();//开启了TimerB1中断,若开_EINT(),则一定要写中断函数防止跑飞.
                Drive_UTL_Init();//超声波模块IO初始化,配以B1为捕获模式

                Drive_Motor_L(0, 50);//以停止姿态配置小车速度
                Drive_Motor_R(0, 50);

                Drive_Servo_Move(90);     //舵机归中

                UTL_MODULE_START_FLAG = 1; // 超声波模块开启
                while(NRF905_REV_FLAG == 0) //在没有接收到下一步指令前,一直进行超声波避障
                {
                    if(UTL_FLAG == 2)   //超声波测距结束，开始处理数据
                    {
                        Drive_UTL_DISTANCE();
                        UTL_START_TIME = 0; //计时器清零

                        //舵机根据现在的方向UTL_DIR确定下一步操作.
                        switch(UTL_DIR)
                        {
                        case 3:     //目前在中间方向
                            if(UTL_BUFFER < UTL_SAFE) //前进中遇到障碍，停止小车, 向左前转.
                            {
                                MOTOR_LEFT_STOP;
                                MOTOR_RIGHT_STOP;
                                UTL_DIR = 2;
                                Drive_Servo_Turn(90, 120);
                                utl_flag = 0;   //摆动标志位清零
                            }
                            else        //前方无障碍物，开始摆动
                            {
                                if(utl_flag == 0)   // 前方无障碍且开始左前测试一次.
                                {
                                    Drive_Servo_Turn(90, 135);
                                    utl_flag = 1;
                                }
                                else if(utl_flag == 1)  //左前无障碍且开始重新确定前方.
                                {
                                    Drive_Servo_Move(90);     //舵机归中
                                    utl_flag = 2;
                                }
                                else if(utl_flag == 2)  //右前
                                {
                                    Drive_Servo_Turn(90, 45);
                                    utl_flag = 3;
                                }
                                else if(utl_flag == 3)
                                {
                                    Drive_Servo_Move(90);     //舵机归中
                                    utl_flag = 0;
                                }
                            }
                            break;
                        case 2: //左前方向
                            if(UTL_BUFFER < UTL_SAFE) //左前方向遇到障碍, 向小左方向转.
                            {
                                Drive_Servo_Turn(120, 150);
                                UTL_DIR = 1;
                            }
                            else    //左前无障碍，则小车转30度前进.
                            {
                                MOTOR_LEFT_BACK;
                                MOTOR_RIGHT_GO;
                                DELAY_MS(220);
                                MOTOR_LEFT_GO;
                                MOTOR_RIGHT_GO;
                                Drive_Servo_Move(90);     //舵机归中
                                UTL_DIR = 3;
                            }
                            break;
                        case 1:
                            if(UTL_BUFFER < UTL_SAFE) //小左方向遇到障碍, 向大左方向转.
                            {
                                Drive_Servo_Turn(150, 180);
                                UTL_DIR = 0;
                            }
                            else    //小左无障碍，则小车转60度前进.
                            {
                                MOTOR_LEFT_BACK;
                                MOTOR_RIGHT_GO;
                                DELAY_MS(440);
                                MOTOR_LEFT_GO;
                                MOTOR_RIGHT_GO;
                                Drive_Servo_Move(90);     //舵机归中
                                UTL_DIR = 3;
                            }
                            break;
                        case 0: //大左方向
                            if(UTL_BUFFER < UTL_SAFE) // 左边都有障碍，右前测试
                            {
                                Drive_Servo_Turn(180, 60);
                                UTL_DIR = 4;
                            }
                            else    //大左方向无障碍，则小车左转90度前进.
                            {
                                MOTOR_LEFT_BACK;
                                MOTOR_RIGHT_GO;
                                DELAY_MS(660);
                                MOTOR_LEFT_GO;
                                MOTOR_RIGHT_GO;

                                Drive_Servo_Move(90);     //舵机归中
                                UTL_DIR = 3;
                            }
                            break;
                        case 4:
                            if(UTL_BUFFER < UTL_SAFE) //右前有障碍
                            {
                                Drive_Servo_Turn(60, 30);
                                UTL_DIR = 5;
                            }
                            else    //右前无障碍，右转30度
                            {
                                MOTOR_LEFT_GO;
                                MOTOR_RIGHT_BACK;
                                DELAY_MS(220);
                                MOTOR_LEFT_GO;
                                MOTOR_RIGHT_GO;

                                Drive_Servo_Move(90);     //舵机归中
                                UTL_DIR = 3;
                            }
                            break;
                        case 5:
                            if(UTL_BUFFER < UTL_SAFE)   //小右有障碍
                            {
                                Drive_Servo_Turn(30, 0);
                                UTL_DIR = 6;
                            }
                            else    //右转60度前进
                            {
                                MOTOR_LEFT_GO;
                                MOTOR_RIGHT_BACK;
                                DELAY_MS(440);
                                MOTOR_LEFT_GO;
                                MOTOR_RIGHT_GO;

                                Drive_Servo_Move(90);     //舵机归中
                                UTL_DIR = 3;
                            }
                            break;
                        case 6:
                            if(UTL_BUFFER < UTL_SAFE) //有障碍, 先后退, 等周围无障碍在原路转180度返回
                            {
                                MOTOR_LEFT_BACK;
                                MOTOR_RIGHT_BACK;
                                DELAY_MS(300);
                                UTL_DIR = 2;
                            }
                            else
                            {
                                MOTOR_LEFT_GO;
                                MOTOR_RIGHT_BACK;
                                DELAY_MS(700);
                                MOTOR_LEFT_GO;
                                MOTOR_RIGHT_GO;

                                Drive_Servo_Move(90);     //舵机归中
                                UTL_DIR = 3;
                            }
                            break;
                        }
                        UTL_FLAG = 0;
                    }

                }


                UTL_MODULE_START_FLAG = 0; // 超声波模块关闭，停止定时测距
                //接到返回指令，则消除模块的配置。
                //关闭小车驱动接口.这里关闭PWM输出
                MOTOR_LEFT_STOP;
                MOTOR_RIGHT_STOP;
                TimerA_Disable();
                TimerA_R0_Disable();
                //关闭舵机PWM输出
                TimerB_Disable();
                TimerB_R1_Disable();
            }
            break;
            case CMD_MENTAL:
            {
                while(NRF905_REV_FLAG == 0); //等待地面站发送配置信息
                NRF905_REV_FLAG = 0;
                NRF905_Read_Data(NRF905_RRP, &NRF905_RXDATA, 1);
                if(NRF905_RXDATA == 0x40) //关闭金属探测功能
                {
                    JSTC_DISABLE;

                }
                else if(NRF905_RXDATA == 0x41) //开启金属探测功能
                {
                    JSTC_ENABLE;
                }
                else ;
                break;
            }
            case CMD_TEMPERATURE:
                while(NRF905_REV_FLAG == 0) //在没有接收到下一步指令前，将一直循环采样温度并发送给地面站
                {
                    DS18B20_Temperatuer(); //读取温度,精确到0.1度，且默认放大10倍保持整数形式
                    //存放在DS18B20_TEMPERATURE 变量中.
                    DHT11_Sample();        //读取温湿度
                    //高位在前，低位在后，前两位为温度，后两位为湿度
                    NRF905_TXDATA[1] = (char)(DS18B20_TEMPERATURE);
                    NRF905_TXDATA[0] = (char)(DS18B20_TEMPERATURE >> 8);
                    //先发送DS18B20的数据
                    NRF905_Write_Data(NRF905_WTP, NRF905_TXDATA, 2);    //打包数据,准备发射
                    NRF905_Set_Mode(NRF905_SEND);   //切换到发送模式
                    DELAY_MS(20);

                    //再发送温湿度传感器的数据
                    NRF905_TXDATA[0] = DHT11_T_DATAH;
                    NRF905_TXDATA[1] = DHT11_RH_DATAH;
                    NRF905_Write_Data(NRF905_WTP, NRF905_TXDATA, 2);    //打包数据,准备发射
                    NRF905_Set_Mode(NRF905_SEND);   //切换到发送模式
                    DELAY_MS(10);
                    NRF905_REV_FLAG = 0; //刚才的一次中断是发送中断，不算数，故清零。

                    NRF905_Set_Mode(NRF905_RCV); //返回至监听状态.
                    DELAY_MS(300);
                }
                break;

            case CMD_RETURN:
                NRF905_Set_Mode(NRF905_RCV); //返回至监听状态.
                break;
            }
            NRF905_RXDATA = 0;
        }
    }
}

#pragma vector=PORT1_VECTOR
__interrupt void P1_ISR (void)
{
    if(P1IFG & BIT7)
    {
        NRF905_REV_FLAG = 1; //中断内仅置标志位，读取工作在主函数中进行
    }

    P1IFG = 0;
}

//金属探测中断入口
//码盘测速中断入口
#pragma vector=PORT2_VECTOR
__interrupt void P2_ISR (void)
{
    if(JSTC_IFG)        //金属探测中断处理
    {
        //JSTC_FLAG = 1; //无需置标志位了，直接在中断中执行响应操作
        BOARD_LED3_1;
        BOARD_BEEP;
        BOARD_LED3_0;
        //JSTC_FLAG = 0;
    }
    // ....something else.
    P2IFG = 0;
}


//TimerA up mode , 1000HZ, R1/R2用于驱动小车，输出PWM信号
//超声波定时测距.
#pragma vector=TIMERA0_VECTOR
__interrupt void TimerA0(void)      // 1ms中断.1000HZ
{
    static unsigned int i = 0;

    if(UTL_MODULE_START_FLAG == 1) //超声波模块启动的话,则一段时间进行一次测距
    {
        i++; //自动计数
        if(i % 300 == 0)  //测距一次
        {
            if(UTL_FLAG == 0)    //若超声波空闲，则测距.
            {
                UTL_START;  //给超声波设备发送控制指令，等待超声波反射
                i = 0;
            }
        }
    }

}

//20ms中断
//超声波测距/舵机PWM信号输出
#pragma vector=TIMERB1_VECTOR
__interrupt void TimerB1(void)
{
    switch(TBIV)                //  TB中断源软件判断,R0有自己单独的中断向量
    {
    case 2:          // 中断源为R1      //捕获上升下降沿.
        if(UTL_FLAG == 0)     //若已开启超声波计
        {
            UTL_FLAG = 1;     //正在计时中...
            UTL_START_TIME = TBR;
            UTL_TIME_OVERFLOW = 0;
        }
        else if(UTL_FLAG == 1)   //计时过程中再次中断，则说明计时结束
        {
            UTL_FLAG = 2;        // 关闭计时，等待处理数据
            UTL_END_TIME = TBR;
        }
        else
            ;
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
        if(UTL_FLAG == 1)  //如果正在超声波计时，则记录此后的中断溢出次数
            UTL_TIME_OVERFLOW++;
        break;
    }
}
