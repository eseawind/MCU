
#include "CJMCU.h"
#include "clock.h"
#include <msp430f2618.h>

/******************外部变量定义*******************/
extern unsigned char CJMCU_RCV_FLAG = 0;                       //读取数据标志

//姿态角的静止偏差
// YAW_0 和 YAW_1为相邻两次读取的YAW角度, 根据偏差控制电调。
extern int CJMCU_YAW_OFFSET = 0, CJMCU_PITCH_OFFSET = 0, CJMCU_ROLL_OFFSET = 0, CJMCU_YAW_0 = 0, CJMCU_YAW_1 = 0;

/******************内部变量定义******************/
extern unsigned char CJMCU_FLAG=0;                    //接收有效数据标志
extern unsigned char CJMCU_BUF[CJMCU_BUF_LEN] = {0};        //接收缓存
extern unsigned int  CJMCU_LEN=0;                     //接收数据长度


//初始化UART通信
void CJMCU_INIT()
{
    UCA1CTL1 |= UCSWRST;

    UCA1CTL1 |= UCSSEL1;  //SMCLK : 12MHZ
    UCA1BR0 = CJMCU_BAUDRATE;         //fre:12MHZ   baud_rate:57600
    UCA1BR1 = 0;

    UCA1MCTL = 2 << 1;

    UCA1CTL1 &=~UCSWRST;

    UARTA0_RX;          //IO config
    UARTA0_TX;
}

//CJMCU配置
void CJMCU_CONFIG(const unsigned char *pdata)
{
    unsigned int i=0;

    for(i=0; pdata[i]!='\0'; i++)
        CJMCU_SEND(pdata[i]);
}


//开启CJMCU测量姿态
void CJMCU_START()
{
    CJMCU_LEN=0;
    CJMCU_RCV_FLAG=0;
    UC1IFG &=~ UCA1RXIFG;
    UC1IE |= UCA1RXIE;
    DELAY_MS(5);
}

//关闭CJMCU测量姿态
void CJMCU_STOP()
{
    UC1IE &=~UCA1RXIE;
    CJMCU_FLAG=0;
}

//计算数据
unsigned char CJMCU_TRANSLATE(const unsigned char *pdata, unsigned char i, int *data)
{
    int d=0;
    unsigned char flag=0;  //正负标志 0 : 正，1 : 负

    if(pdata[i] == '-')
    {
        flag=1;
        i++;
    }

    for(; CJMCU_BUF[i]!=',' && CJMCU_BUF[i]!='\r' ; i++)
        if(CJMCU_BUF[i]!='.')
            d=d*10 + CJMCU_BUF[i] - '0';

    if(flag)  d=-d;
    *data=d;

    return ++i;
}

//获取姿态角
//z 为CJMCU以y为转轴，与水平面的夹角，向上为正，向下为负
//x 为CJMCU以x为转轴，与水平面的夹角，向上为负，向下为正
//y 为CJMCU以竖直方向为转轴，
void CJMCU_GET_VALUE(int *x, int *y, int *z)
{
    unsigned int i=0;
    int x_v=0,y_v=0,z_v=0;

    i=CJMCU_TRANSLATE(CJMCU_BUF,i,&x_v);
    i=CJMCU_TRANSLATE(CJMCU_BUF,i,&y_v);
    CJMCU_TRANSLATE(CJMCU_BUF,i,&z_v);

    if(x) *x=x_v;
    if(y) *y=y_v;
    if(z) *z=z_v;
}

//UART发送数据
void CJMCU_SEND(const unsigned char data)
{
    while(UC1IFG & UCA1TXIFG == 0);
    UCA1TXBUF =data;
    DELAY_MS(1);
}


//使九轴姿态传感器静止, 读取数据十次, 取平均值作为静止偏差
void CJMCU_Aline()
{
    unsigned char i = 0;
    int yaw = 0, pitch = 0, roll = 0;
    long yaw_sum = 0, pitch_sum = 0, roll_sum = 0;

    for(i = 0; i < 16; i++)     //读取16组数据, 扔掉
    {
        CJMCU_START(); //只读取一次。每次读取数据时都要调用该函数
        DELAY_MS(20);
        //while(CJMCU_RCV_FLAG==0);
        CJMCU_GET_VALUE(&yaw, &pitch, &roll);
    }

    for(i = 0; i < CJMCU_ALINE_NUM; i++)
    {
        CJMCU_START(); //只读取一次。每次读取数据时都要调用该函数
        DELAY_MS(20);
        //while(CJMCU_RCV_FLAG==0);

        CJMCU_GET_VALUE(&yaw, &pitch, &roll);
        yaw_sum += yaw;
        pitch_sum += pitch;
        roll_sum += roll;
    }

    CJMCU_YAW_OFFSET =      (yaw_sum / CJMCU_ALINE_NUM);
    CJMCU_PITCH_OFFSET =    (pitch_sum / CJMCU_ALINE_NUM);
    CJMCU_ROLL_OFFSET =     (roll_sum / CJMCU_ALINE_NUM);
}



#pragma vector=USCIAB1RX_VECTOR
__interrupt void UCAB1RX_Interrupt()
{
    if(UCA1RXBUF == 61)  CJMCU_FLAG=1;
    else if(CJMCU_FLAG == 1)
    {
        if(UCA1RXBUF != '\n')
            CJMCU_BUF[CJMCU_LEN++]=UCA1RXBUF;
        else
        {
            CJMCU_STOP();
            CJMCU_RCV_FLAG = 1;
        }
    }

    UC1IFG &= ~UCA1RXIFG;
}
