/**********************************************
 *
 *      Author  :       Shawn Guo
 *      Mail    :       iguoxiaopeng@gmail.com
 *      Date    :       2013/8/24
 *      Last    :
 *      Notes   :       MPU 6050陀螺仪模块
 *      Tool    :       MPU6050模块   MSP430X2XX
 **********************************************/

/*****************************************************************************
* @file       MPU6050.c
* @addtogroup MPU6050
* @{
******************************************************************************/
#include <msp430f2618.h>
#include <math.h>
#include "clock.h"
#include "MPU6050.h"


//加速度计采样值,采样后将数字值转为真正值且放大1000倍.
extern int MPU6050_AX = 0, MPU6050_AY = 0, MPU6050_AZ = 0;
//由加速度值计算出来的角度值.
//MPU6050_ANGLE_X表示加速度矢量在XZ平面上与X轴的夹角.
//MUP6050_ANGLE_Y表示加速度矢量在YZ平面上与Y轴的夹角.
extern int MPU6050_ANGLE_XZX = 0, MPU6050_ANGLE_YZY = 0;

//陀螺仪采样值,采样后将数据转为真正值且放大1000倍.
extern int MPU6050_GX = 0, MPU6050_GY = 0, MPU6050_GZ = 0;

//端口初始化
void MPU6050_Init()
{
    //初始化端口为I2C空闲状态
    MPU6050_SCL_DIR_OUT;
    MPU6050_SCL_1;

    MPU6050_SDA_DIR_OUT;
    MPU6050_SDA_1;

    //MPU6050 设备在上电后自动进入睡眠模式
    MPU6050_I2C_SendREG(PWR_MGMT_1, 0x00);	//解除休眠状态,时钟为内部8MHZ

    //DLPF is disabled 陀螺仪输出频率8KHZ, DLPF is Enabled, 陀螺仪输出频率1KHZ
    //采样频率 = 陀螺仪输出频率 / (1 + SMPLRT_DIV);
    //注意: 加速度计的输出频率 1KHZ,采样率不能大于1KHZ,否则多采同样的值.
    MPU6050_I2C_SendREG(SMPLRT_DIV, 0x07);  //陀螺仪采样率，典型值：0x07(125Hz)


    //不使能FSYNC,不使用外同步采样速率；DLPF_CFG[2~0],设置任意轴是否通过DLPF，
    //典型值：0x06(5Hz)低通滤波器带宽5Hz，
    //对加速度和陀螺仪都有效，输出频率为1kHz，决定SMPLRT_DIV的频率基准
    MPU6050_I2C_SendREG(CONFIG, 0x06);

    //不自测, 陀螺仪量程500度/s。
    MPU6050_I2C_SendREG(GYRO_CONFIG, 0x18);


    //不自测, 加速度计量程2G.
    MPU6050_I2C_SendREG(ACCEL_CONFIG, 0x00);
}

/************** X/Y/Z-Axis Acceleration 3轴加速度计*************************/
int MPU6050_AccelX()
{
    unsigned char data_h = 0, data_l = 0;

    MPU6050_I2C_ReadREG(ACCEL_XOUT_H, &data_h);
    MPU6050_I2C_ReadREG(ACCEL_XOUT_L, &data_l);

    MPU6050_AX = ((data_h) << 8) + data_l;

    MPU6050_AX = ACCEL_SENSI(MPU6050_AX);    //灵敏度转化, 转换后为真正的加速度值.

    return  MPU6050_AX;
}

int MPU6050_AccelY()
{
    unsigned char data_h = 0, data_l = 0;

    MPU6050_I2C_ReadREG(ACCEL_YOUT_H, &data_h);
    MPU6050_I2C_ReadREG(ACCEL_YOUT_L, &data_l);

    MPU6050_AY = (((int)data_h) << 8) + data_l;
    MPU6050_AY = ACCEL_SENSI(MPU6050_AY);

    return  MPU6050_AY;
}

int MPU6050_AccelZ()
{
    unsigned char data_h = 0, data_l = 0;

    MPU6050_I2C_ReadREG(ACCEL_ZOUT_H, &data_h);
    MPU6050_I2C_ReadREG(ACCEL_ZOUT_L, &data_l);

    MPU6050_AZ = (((int)data_h) << 8) + data_l;
    MPU6050_AZ = ACCEL_SENSI(MPU6050_AZ);

    return  MPU6050_AZ;
}

/***********************<math.h>   atan2(y,x)********************************
//对于任意不同时等于0的实参数x和y，
//atan2(y,x)所表达的意思是坐标原点为起点，
//指向(x,y)的射线在坐标平面上与x轴正方向之间的角的角度度
eg.
    temp = atan2(0,1)* 180 / 3.1415926;             0
    temp = atan2(1,1)* 180 / 3.1415926;             45
    temp = atan2(1,0)* 180 / 3.1415926;             90

    temp = atan2(1,-1) * 180 / 3.1415926;           135
    temp = atan2(0,-1)* 180 / 3.1415926;            180
    temp = atan2(-1,-1)* 180 / 3.1415926;           -135
    temp = atan2(-1,0)* 180 / 3.1415926;            -90
    temp = atan2(-1,1)* 180 / 3.1415926;            -45
**********************************************************************/
void MPU6050_Angle_XZX()
{
    // (atan2(Z,X) * 180 / pi);
    MPU6050_ANGLE_XZX = (int)(atan2(MPU6050_AZ, MPU6050_AX) * 573 / 10);
}

void MPU6050_Angle_YZY()
{
    // (atan2(Z,Y) * 180 / pi);
    MPU6050_ANGLE_YZY = (int)(atan2(MPU6050_AZ, MPU6050_AY) * 573 / 10);
}










/**************X/Y/Z-Axis Angular velocity 3轴陀螺仪******************/
int MPU6050_AnguX()
{
    unsigned char data_h = 0, data_l = 0;

    MPU6050_I2C_ReadREG(GYRO_XOUT_H, &data_h);
    MPU6050_I2C_ReadREG(GYRO_XOUT_L, &data_l);

    MPU6050_GX = (((int)data_h) << 8) + data_l + MPU6050_OFFSET_GX;
    MPU6050_GX = GYRO_SENSI(MPU6050_GX);

    return  MPU6050_GX;
}

int MPU6050_AnguY()
{
    unsigned char data_h = 0, data_l = 0;

    MPU6050_I2C_ReadREG(GYRO_YOUT_H, &data_h);
    MPU6050_I2C_ReadREG(GYRO_YOUT_L, &data_l);

    MPU6050_GY = (((int)data_h) << 8) + data_l + MPU6050_OFFSET_GY;
    MPU6050_GY = GYRO_SENSI(MPU6050_GY);

    return  MPU6050_GY;
}

int MPU6050_AnguZ()
{
    unsigned char data_h = 0, data_l = 0;

    MPU6050_I2C_ReadREG(GYRO_ZOUT_H, &data_h);
    MPU6050_I2C_ReadREG(GYRO_ZOUT_L, &data_l);

    MPU6050_GZ = (((int)data_h) << 8) + data_l + MPU6050_OFFSET_GZ;
    MPU6050_GZ = GYRO_SENSI(MPU6050_GZ);

    return  MPU6050_GZ;
}



unsigned char MPU6050_ReadID()
{
    unsigned char id = 0;

    MPU6050_I2C_ReadREG(WHO_AM_I, &id);

    return id;
}









/////////////////////////////////////////////I2C///////////////////////////////////



void MPU6050_I2C_Start()
{
    MPU6050_SDA_1;      //发送起始条件的数据信号,SDA置高电平，等待被拉低
    MPU6050_SCL_1;      //拉高时钟线
    DELAY_US(5);        //SDA高电平建立时间大于4.7us,延时

    MPU6050_SDA_0;      //SDA在SCL高电平时拉低，下降沿产生起始信号
    DELAY_US(5);        //起始条件锁定时间大于4μs
    MPU6050_SCL_0;      //SCL拉低后，SDA可以变化，准备发送或接收数据
}

void MPU6050_I2C_Stop()
{
    MPU6050_SDA_0;      //发送结束条件的数据信号，SDA拉低，等待被拉高
    MPU6050_SCL_1;
    DELAY_US(5);        //SDA低电平保持时间大于4μs
    MPU6050_SDA_1;      //SDA在SCL高电平时拉高，上升沿产生结束信号，I2C总线停止
    DELAY_US(5);        //延时
}

void MPU6050_I2C_Ask()     //发送应答信号,即低SDA
{
    //默认前面已经SCL = 0了
    MPU6050_SDA_0;                  //写应答信号
    MPU6050_SCL_1;                  //拉高时钟线
    DELAY_US(5);                      //延时
    MPU6050_SCL_0;                   //拉低时钟线,准备再传输数据
    DELAY_US(5);                    //延时
}

void MPU6050_I2C_NoAsk()   //发送应答信号,即高SDA
{
    //默认前面已经SCL = 0了
    MPU6050_SDA_1;                  //写非应答信号
    MPU6050_SCL_1;                  //拉高时钟线
    DELAY_US(5);                      //延时
    MPU6050_SCL_0;                   //拉低时钟线,准备再传输数据
    DELAY_US(5);                    //延时
}

unsigned char MPU6050_I2C_ReadAsk() //接受应答
{
    unsigned char flag = 0;

    //跟在发射函数后面, 默认SCL仍是低电平.
    MPU6050_SDA_REN;
    MPU6050_SDA_DIR_IN;     //设为输入模式,上拉模式

    DELAY_US(3);            //等待应答
    MPU6050_SCL_1;                  //拉高时钟线,锁定应答信号
    DELAY_US(3);                    //延时
    if(!MPU6050_SDA_IN)              //读应答信号,若接收到应答引导0, 标志位置一
        flag = 1;

    MPU6050_SCL_0;                  //拉低时钟线,等待下一次数据传输

    MPU6050_SDA_DISREN;
    MPU6050_SDA_DIR_OUT;        //换回输出模式
    MPU6050_SDA_1;              //先置数据线1, 等下一次数据传输来临
    DELAY_US(5);

    return flag;
}

void MPU6050_I2C_Send(unsigned char dat)
{
    unsigned char i = 0;

    MPU6050_SCL_0;              //拉低时钟线

    for (i = 0; i < 8; i++)     //传输8-bits
    {
        if(dat & 0x80)  //高位为1
        {
            MPU6050_SDA_1;
        }
        else
            MPU6050_SDA_0;

        MPU6050_SCL_1;  /*置时钟线为高，SDA保持不变，通知被控器开始接收数据位*/
        DELAY_US(6);   /*保证时钟高电平周期大于4μs*/
        MPU6050_SCL_0;              //拉低时钟线
        dat <<= 1;
    }

    MPU6050_I2C_ReadAsk();
}

void MPU6050_I2C_Read(unsigned char *dat)
{
    unsigned char i = 0;

    *dat = 0;
    MPU6050_SCL_0;          //确保时钟沿为0
    MPU6050_SDA_REN;
    MPU6050_SDA_DIR_IN;     //SDA总线变为输入, 上拉电阻, 准备接受数据

    for (i = 0; i < 8; i++)     //传输8-bits
    {
        (*dat) <<= 1; // 腾出最低位，用于接受新的数据
        DELAY_US(5);         /*时钟低电平周期大于4.7μs，保证有足够时间改变SDA总线*/
        MPU6050_SCL_1;     /*置时钟线为高，保持SDA数据线上数据有效*/


        if(MPU6050_SDA_IN)
        {
            (*dat) |= 0x01;       //最低位置1
        }
        MPU6050_SCL_0;   /*置时钟线为低，准备接收数据位*/
    }
    //切换数据总线为输出
    MPU6050_SDA_DISREN;
    MPU6050_SDA_DIR_OUT;
    MPU6050_SDA_1;
}


void MPU6050_I2C_SendREG(unsigned char reg_address, unsigned char dat)
{
    MPU6050_I2C_Start();                    //起始信号
    MPU6050_I2C_Send(MPU6050_ADDRESS_W);   //发送设备地址+写信号
    MPU6050_I2C_Send(reg_address);          //内部寄存器地址，
    MPU6050_I2C_Send(dat);             //内部寄存器数据，
    MPU6050_I2C_Stop();                    //发送停止信号
}

void MPU6050_I2C_ReadREG(unsigned char reg_address, unsigned char *dat)
{
    MPU6050_I2C_Start();                    //起始信号
    MPU6050_I2C_Send(MPU6050_ADDRESS_W);    //发送设备地址+写信号
    MPU6050_I2C_Send(reg_address);          //发送存储单元地址，从0开始

    MPU6050_I2C_Start();                    //起始信号
    MPU6050_I2C_Send(MPU6050_ADDRESS_R);    //发送设备地址+读信号
    MPU6050_I2C_Read(dat);                  //读出寄存器数据
    MPU6050_I2C_NoAsk();                    //发送非应答信号，结束总线数据传输
    MPU6050_I2C_Stop();                     //停止信号
}

/***************************************************************************//**
 * @brief
 * @param
 * @return none
 *******************************************************************************/


/***************************************************************************//**
 * @}
 ******************************************************************************/
