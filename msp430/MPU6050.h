/**********************************************
 *
 *      Author  :       Shawn Guo
 *      Mail    :       iguoxiaopeng@gmail.com
 *      Date    :       2013/8/24
 *      Last    :
 *      Notes   :       MPU 6050陀螺仪模块
 *      Tool    :       MPU6050模块   MSP430X2XX
 *
 *-------------------------------------------------
 *      VCC         3.3V
 *      GND         GND
 *      SCL         I2C通信时钟线        P7.0
 *      SDA         I2C通信数据线        P7.1
 *      XDA         外部I2C通信时钟线
 *      XCL         外部I2C通信数据线
 *      AD0         I2C地址选择          GND(B1101000)  / VCC(B1101001)
 *      INT         中断                  P1.0
 *
 *  注:加速度与陀螺仪的基本知识请看<加速度计和陀螺仪指南>
 *
 **********************************************/




#ifndef __MPU6050_H__
#define __MPU6050_H__

/*******************************************************************************
*                           模块全局变量
 *******************************************************************************/
//加速度计采样值,采样后将数字值转为真正值且放大1000倍.
extern int MPU6050_AX, MPU6050_AY, MPU6050_AZ;
//加速度计采样值正负标志位,AX-BIT0, AY-BIT1, AZ-BIT2, 为负时置一.
extern unsigned char MPU6050_ACCE_FLAG;
//由加速度值计算出来的角度值.
//MPU6050_ANGLE_X表示加速度矢量在XZ平面上与X轴的夹角.
//MUP6050_ANGLE_Y表示加速度矢量在YZ平面上与Y轴的夹角.
extern int MPU6050_ANGLE_XZX, MPU6050_ANGLE_YZY;

//陀螺仪采样值,采样后将数据转为真正值且放大1000倍.
extern int MPU6050_GX, MPU6050_GY, MPU6050_GZ;



/********************************************************************************
*                               硬件端口宏定义
 *******************************************************************************/

#define     MPU6050_SCL_DIR_OUT     (P7DIR |=  BIT0)
#define     MPU6050_SCL_1           (P7OUT |=  BIT0)
#define     MPU6050_SCL_0           (P7OUT &= ~BIT0)

#define     MPU6050_SDA_DIR_OUT     (P7DIR |=  BIT1)
#define     MPU6050_SDA_DIR_IN      (P7DIR &= ~BIT1)
#define     MPU6050_SDA_REN         (P7REN |=  BIT1, P7OUT |= BIT1) //上拉电阻
#define     MPU6050_SDA_DISREN      (P7REN &= ~BIT1)

#define     MPU6050_SDA_1           (P7OUT |=  BIT1)
#define     MPU6050_SDA_0           (P7OUT &= ~BIT1)
#define     MPU6050_SDA_IN          (P7IN  &   BIT1)


//陀螺仪偏差, 即静止时陀螺仪的示数。
// 保证陀螺仪静止时3轴示数为0.
#define     MPU6050_OFFSET_GX       (-17)
#define     MPU6050_OFFSET_GY       (-21)
#define     MPU6050_OFFSET_GZ       (-33)

/****************************************
 定义MPU6050内部地址
****************************************/
#define     MPU6050_ADDRESS_W         (0xD0)           //MPU6050模块的I2C地址、写110100 0 0
#define     MPU6050_ADDRESS_R         (0xD1)           //MPU6050模块的I2C地址、读110100 0 1


#define	    SMPLRT_DIV		0x19	//陀螺仪采样率，典型值：0x07(125Hz)
#define	    CONFIG			0x1A	//低通滤波频率，典型值：0x06(5Hz)
#define	    GYRO_CONFIG		0x1B	//陀螺仪自检及测量范围，典型值：0x18(不自检，满量程2000deg/s)
//陀螺仪量程     GYRO_CONFIG [BIT4 BIT3]         0   250deg/s    1   500deg/s    2   1000deg/s   3   2000deg/s
#define	    ACCEL_CONFIG	0x1C	//加速计自检、测量范围及高通滤波频率，典型值：0x01(不自检，2G，5Hz)
//加速度计量程    ACCEL_CONFIG[BIT4 BIT3]         0   2g          1   4g          2   8g          3   16g

#define     MOTION_DETECTION    0x1F    //运动检测阀值,加速度测量超过此值产生中断

//加速度计测量值
#define	    ACCEL_XOUT_H	0x3B
#define	    ACCEL_XOUT_L	0x3C
#define	    ACCEL_YOUT_H	0x3D
#define	    ACCEL_YOUT_L	0x3E
#define	    ACCEL_ZOUT_H	0x3F
#define	    ACCEL_ZOUT_L	0x40
//加速度测量值的灵敏度.采集得到的数据除以灵敏度得到单位为G的加速度.
//eg. ACCEL_SENSI_2G(MPU6050_AX)
//这里进行算法优化, 去除浮点运算, 且将加速度值放大1000倍.
//这里默认以2G的范围的灵敏度进行计算.
#define     ACCEL_SENSI(x)      (ACCEL_SENSI_2G(x))
#define     ACCEL_SENSI_2G(x)   (((long)(x) * 125) >> 11) // 16384LSB/G,表示当为1G加速度时，得到的数字值为16384.
#define     ACCEL_SENSI_4G(x)   (((long)(x) * 125) >> 10) // 8196 LSB/G
#define     ACCEL_SENSI_8G(x)   (((long)(x) * 125) >> 9) // 4096 LSB/G
#define     ACCEL_SENSI_16G(x)  (((long)(x) * 125) >> 8) // 2048 LSB/G

#define	    TEMP_OUT_H		0x41
#define	    TEMP_OUT_L		0x42

#define	    GYRO_XOUT_H		0x43
#define	    GYRO_XOUT_L		0x44
#define	    GYRO_YOUT_H		0x45
#define	    GYRO_YOUT_L		0x46
#define	    GYRO_ZOUT_H		0x47
#define	    GYRO_ZOUT_L		0x48
//陀螺仪测量值的灵敏度.采集得到的数据除以灵敏度得到真正的数据，xx度/s.
//eg.
#define     GYRO_SENSI(x)       (GYRO_SENSI_500(x))
#define     GYRO_SENSI_250(x)   ((long)(x) * 1000 / 131) // 131    LSB/deg/s, 表示当为1 deg/s时，得到的数字值为131.
#define     GYRO_SENSI_500(x)   ((long)(x) * 2000 / 131)    // 65.5   LSB/deg/s
#define     GYRO_SENSI_1000(x)  ((long)(x) * 4000 / 131)    // 32.8   LSB/deg/s
#define     GYRO_SENSI_2000(x)  ((long)(x) * 8000 / 131)    // 16.4   LSB/deg/s

#define	    PWR_MGMT_1		0x6B	//电源管理，典型值：0x00(正常启用)
#define	    WHO_AM_I	    0x75	//IIC地址寄存器(默认数值0x68，只读)





//端口初始化
void MPU6050_Init();

// X/Y/Z-Axis Acceleration
int MPU6050_AccelX ();
int MPU6050_AccelY ();
int MPU6050_AccelZ ();

// X/Y/Z-Axis Angular velocity
int MPU6050_AnguX ();
int MPU6050_AnguY ();
int MPU6050_AnguZ ();

//根据加速度的采样值MPU6050_AX, MPU6050_AY, MPU6050_AZ计算出角度存放在全局变量
//MPU6050_ANGLE_X, MPU6050_ANGLE_Y中.
//需要用到角度转换算法.
void MPU6050_Angle_XZX(); //加速度矢量在XZ平面上与X轴的角度
void MPU6050_Angle_YZY(); //加速度矢量再YZ平面上与Y轴的角度

unsigned char MPU6050_ReadID();

/***************I2C基本操作************************/
void MPU6050_I2C_Start();
void MPU6050_I2C_Stop();

void MPU6050_I2C_SendREG(unsigned char reg_address, unsigned char dat);
void MPU6050_I2C_ReadREG(unsigned char reg_address, unsigned char *dat);


void MPU6050_I2C_Send(unsigned char dat);
void MPU6050_I2C_Read(unsigned char *dat);

void MPU6050_I2C_Ask();     //发送应答信号,即低SDA
void MPU6050_I2C_NoAsk();   //发送非应答,即高SDA
unsigned char MPU6050_I2C_ReadAsk(); //接受应答, 返回1表示接受应答, 返回0表示没有接受到应答


/***************************************************************************//**
 * @brief
 * @param
 * @return none
 *******************************************************************************/
#endif /* __MPU6050_H__ */
