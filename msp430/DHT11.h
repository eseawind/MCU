/**********************************************
 *
 *      Author  :       Shawn Guo
 *      Mail    :       iguoxiaopeng@gmail.com
 *      Date    :       2013/8/23           基本功能实现.
 *      Last    :
 *      Notes   :       DHT11 温湿度传感器模块
 *      Tool    :       DHT11 , MSP430F2618
 *
 *-----------管脚说明---------------
 *  VCC         3.3V
 *  DATA        P4.2(当输入端口时需配置上拉电阻REN)
 *  NC          悬空
 *  GND         GND
 *
 *  数据格式:   8-bits 湿度整数 + 8-bits 湿度小数 + 8-bits 温度整数 + 8-bits温度小数 + 8-bits 校验和
 *
 *
 *  注:DHT11 对时钟要求特别高, 请注意时序.
 *
 *
 **********************************************/
#ifndef __DHT11_H__
#define __DHT11_H__

#define     DHT11_DATA_DIR_OUT  (P4DIR |=  BIT2)
#define     DHT11_DATA_DIR_IN   (P4DIR &= ~BIT2)
#define     DHT11_DATA_REN      (P4REN |=  BIT2, P4OUT |= BIT2) //上拉电阻
#define     DHT11_DATA_DISREN   (P4REN &= ~BIT2)

#define     DHT11_DATA_IN       (P4IN  &   BIT2)
#define     DHT11_DATA_OUT_0    (P4OUT &= ~BIT2)
#define     DHT11_DATA_OUT_1    (P4OUT |=  BIT2)


void DHT11_Init_IO();

//进行一次温湿度采样, 从开始总线，接收数据到关闭总线一系列过程
void DHT11_Sample();

//读取DATA总线上传输的数据
void DHT11_Read(unsigned char *dat);

void DHT11_Convert();   //将DHT11_T_DATAH以及DHT11_RH_DATAH转换为十进制.

extern unsigned char DHT11_T_DATAH, DHT11_T_DATAL, DHT11_RH_DATAH, DHT11_RH_DATAL, DHT11_CHECK_DATA, DHT11_CHECK;


#endif
