/**********************************************
 *
 *      Author  :       Shawn Guo
 *      Mail    :       iguoxiaopeng@gmail.com
 *      Date    :       2013/8/23           基本功能实现.
 *      Last    :
 *      Notes   :       DHT11 温湿度传感器模块
 *      Tool    :       DHT11 , MSP430F2618
 **********************************************/

/*****************************************************************************
* @file       DHT11.c
* @addtogroup DHT11
* @{
******************************************************************************/
#include <msp430f2618.h>
#include "clock.h"
#include "DHT11.h"

unsigned char DHT11_T_DATAH = 0, DHT11_T_DATAL = 0, DHT11_RH_DATAH = 0, DHT11_RH_DATAL = 0, DHT11_CHECK_DATA = 0, DHT11_CHECK = 0;

void DHT11_Init_IO()
{
    //默认初始化为输出
    DHT11_DATA_DIR_OUT;
    DHT11_DATA_OUT_1;
}

//进行一次温湿度采样, 从开始总线，接收数据到关闭总线一系列过程
void DHT11_Sample()
{
    //主机拉低18ms以上

    DHT11_DATA_OUT_0;
    DELAY_MS(18);
    DHT11_DATA_OUT_1;

    //总线由上拉电阻拉高 主机延时20us~40us
    DELAY_US(20);

    //主机设为输入 判断从机响应信号
    DHT11_DATA_DIR_IN;
    DHT11_DATA_REN;

    //判断从机是否有低电平响应信号 如不响应则跳出，响应则向下运行
    if(!(DHT11_DATA_IN))
    {
        //接收到低电平, 说明接收到从机的响应信号.

        while((!DHT11_DATA_IN));    //接收40us的低电平响应
        while((DHT11_DATA_IN)); //接收40us的高电平响应

        //开始数据接收.
        DHT11_Read(&DHT11_RH_DATAH);
        DHT11_Read(&DHT11_RH_DATAL);
        DHT11_Read(&DHT11_T_DATAH);
        DHT11_Read(&DHT11_T_DATAL);
        DHT11_Read(&DHT11_CHECK_DATA);

        //拉高总线以结束
        DHT11_DATA_DIR_OUT;
        DHT11_DATA_DISREN;
        DHT11_DATA_OUT_1;

        //数据校验
        DHT11_CHECK = DHT11_T_DATAH + DHT11_T_DATAL + DHT11_RH_DATAH + DHT11_RH_DATAL;
        if(DHT11_CHECK != DHT11_CHECK_DATA) //数据校验失败, 数据位清零
        {
            DHT11_T_DATAH = 0;
            DHT11_T_DATAL = 0;
            DHT11_RH_DATAH = 0;
            DHT11_RH_DATAL = 0;
            DHT11_CHECK_DATA = 0;
        }

    }
}

//读取DATA总线上传输的数据
void DHT11_Read(unsigned char *dat)
{
    unsigned char i = 0;

    (*dat) = 0;
    //接收8字节的数据, 每字节的数据都是以低电平开始，高电平结束。
    //根据高电平的持续时间判断1/0
    //低电平持续时间,12-14us
    //高电平数据0(26-28us), 数据1(116-118us)
    for(i = 0; i < 8; i++)
    {
        (*dat) <<= 1;
        while((!DHT11_DATA_IN));    //等待低电平结束
        //延时30us, 30us保证大于数据0的持续时间, 此时在下一位的低电平;
        //而又保证小于数据1的持续时间, 此时仍在高电平.
        DELAY_US(40);

        if(DHT11_DATA_IN)
            (*dat)++;

        while((DHT11_DATA_IN));    //等待数据1的高电平结束,若为数据0则会直接跳过不影响

        //判断数据位是0还是1
        // 如果高电平高过预定0高电平值则数据位为 1
    }
}

/***************************************************************************//**
 * @}
 ******************************************************************************/
