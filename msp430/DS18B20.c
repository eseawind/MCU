/**********************************************
 *
 *      Author  :       Shawn Guo
 *      Mail    :       iguoxiaopeng@gmail.com
 *      Date    :       2013/8/8
 *      Last    :
 *      Notes   :       DS18B20温度传感器驱动
 *      Tool    :       ds18b20     MSP430X2XX
 **********************************************/

/*****************************************************************************
* @file       DS18B20.c
* @addtogroup DS18B20
* @{
******************************************************************************/
#include <msp430f2618.h>
#include "clock.h"
#include "DS18B20.h"

extern unsigned char DS18B20_BUFFER[9] = {0};    //DS18B20缓存, 用作存放刚读取的数据，等待处理
unsigned int DS18B20_TEMPERATURE = 0;
unsigned char DS18B20_TEMP_FLAG = 0;    //温度标示, 0标示正数，1标示负数.

unsigned char DS18B20_Init()        // DS18B20初始化复位, 返回标志位(=1,成功; =0,初始化失败)
{
    unsigned char flag = 0;

    DS18B20_DIR1;   //数据线设为输出
    DS18B20_OUT1; // 数据线先置高电平1

    DS18B20_OUT0;   // 拉低电平,延时460~960us用作复位脉冲
    DELAY_US(700);   //延时460~960us
    DS18B20_OUT1;
    DELAY_US(50);  //延时15~60us,等待一个由DS18B20返回的低电平0

    DS18B20_DIR0;   //数据线改为输入, 准备接受DS18B20的存在信号
    if(DS18B20_IN == 0)
        flag = 1;       //detect ds18b20 success!
    else
        flag = 0;       //detect ds18b20 fail!

    DELAY_US(500);       //片选成功后仍要延时，从上一次拉高算起，至少480us.

    DS18B20_DIR1;
    DS18B20_OUT1;   //最后拉高结束

    return flag;
}

void DS18B20_Write_Byte(unsigned char dat)  //单字节写入
{
    unsigned char i;

    DS18B20_OUT1;

    for (i = 0; i < 8; i++)  // 每次1位数据
    {
        DS18B20_OUT0;    //先置低
        DELAY_US(10);       //延时10us, 15us内发送数据到总线
        if(dat & 0x01)    //从低位到高位发送数据
        {
            DS18B20_OUT1;
        }
        else
        {
            DS18B20_OUT0;
        }

        DELAY_US(45);   // 延时45us以上,写时序至少60us,至多120us
        DS18B20_OUT1;   // 数据线拉高1释放总线,等待下一位数据
        DELAY_US(1);    //连续写两位间的间隙要大于1us
        // 重复
        dat >>= 1;
    }
}


unsigned char DS18B20_Read_Byte()     //读取单字节
{
    unsigned char i = 0, dat = 0;

    DS18B20_OUT1;

    for(i = 0; i < 8; i++)
    {
        dat >>= 1;      // 右移一位，腾出最高位接受数据
        DS18B20_OUT0;   //拉低
        DELAY_US(2);
        DS18B20_OUT1;   //拉高，产生读信息
        DS18B20_DIR0;   //切换为输入
        DELAY_US(5);

        if(DS18B20_IN != 0)          //读取数据线数据
            dat |= 0x80;

        DELAY_US(30); //   延时30us
        DS18B20_DIR1;
    }

    return dat;
}


void DS18B20_Read_Str(unsigned char n)  //依次读取n个字节放入缓存数组DS18B20_BUFFER中, 要求n <= 8;
{
    unsigned char i = 0;
    for(i = 0; i < n; i++)
    {
        DS18B20_BUFFER[i] = DS18B20_Read_Byte();
    }
}


void DS18B20_Get_Temp()// 读取温度至温度缓存数组TEMP_BUFF中,需要PTR提前指向温度缓存，最后的数据在TEMPERATURE中
{
    DS18B20_Read_Str(2);    //读出温度计RAM里的第0和第1号单元, 里面为转换的温度值. 0为低字节，1为高字节

    if(DS18B20_BUFFER[1] & 0xFC)    //0到-55度， 符号位都为1,则0号单元前面都为1111 11xx以上
    {
        DS18B20_TEMP_FLAG = 1;      //负数标志.
        DS18B20_TEMPERATURE = ((DS18B20_BUFFER[1] << 8) | DS18B20_BUFFER[0]);
        DS18B20_TEMPERATURE = (~DS18B20_TEMPERATURE) + 1;
        DS18B20_TEMPERATURE >>= 4;     // DS18B20_TEMPERATURE = DS18B20_TEMPERATURE * 0.0625;    0.0625 = 2 ^ (-4);
    }
    else
    {
        DS18B20_TEMP_FLAG = 0;      //正数标志.
        DS18B20_TEMPERATURE = ((DS18B20_BUFFER[1] << 8) | DS18B20_BUFFER[0]);
        DS18B20_TEMPERATURE >>= 4;
    }
}

void DS18B20_Get_Temp0_1()
{
    DS18B20_Read_Str(2);    //读出温度计RAM里的第0和第1号单元, 里面为转换的温度值. 0为低字节，1为高字节

    if(DS18B20_BUFFER[1] & 0xFC)    //0到-55度， 符号位都为1,则0号单元前面都为1111 11xx以上
    {
        DS18B20_TEMP_FLAG = 1;      //负数标志.
        DS18B20_TEMPERATURE = ((DS18B20_BUFFER[1] << 8) | DS18B20_BUFFER[0]);
        DS18B20_TEMPERATURE = (~DS18B20_TEMPERATURE) + 1;
        DS18B20_TEMPERATURE = (DS18B20_TEMPERATURE * 25 / 40);
    }
    else
    {
        DS18B20_TEMP_FLAG = 0;      //正数标志.
        DS18B20_TEMPERATURE = ((DS18B20_BUFFER[1] << 8) | DS18B20_BUFFER[0]);
        DS18B20_TEMPERATURE = (DS18B20_TEMPERATURE * 25 / 40);
    }
}
void DS18B20_Config()  //重新配置报警限定值和分辨率
{
    DS18B20_Init();
    DS18B20_Write_Byte(0xcc);  // 0XCC, 跳过ROM，忽略64位ROM地址，直接发送指令(适用于一个从机工作)
    DS18B20_Write_Byte(0x4e);  // 0X4E, 写暂存器。在内部RAM中的2,3字节写入上、下限温度数据
    DS18B20_Write_Byte(0x19);  //上限
    DS18B20_Write_Byte(0x1a);  //下限
    DS18B20_Write_Byte(0x7f);  //set 12 bit (0.125)

    DS18B20_Init();
    DS18B20_Write_Byte(0xcc);  //skip rom
    DS18B20_Write_Byte(0x48);  // 0X48，复制暂存器。将RAM中的2,3字节TH,TL的内容复制到EEPROOM中的TH,TL中以保存设定置

    DS18B20_Init();
    DS18B20_Write_Byte(0xcc);  //skip rom
    DS18B20_Write_Byte(0xb8);  // 0XB8，重调EEPROM，将EEPROM中的TH,TL的内容到RAM的TH,TL中
}

void DS18B20_Read_ID() //读取器件 id, 写在DS18B20_BUFFER缓存的前八位.
{
    DS18B20_Init();
    DS18B20_Write_Byte(0x33);  // 0X33, read rom.读DS18B20 ROM中的编码(64位地址)
    DS18B20_Read_Str(8);     // 连续读8个字节，共64位
}

void DS18B20_Temperatuer() // DS18B20 温度处理:转换温度，读取温度至温度缓存DS18B20_TEMPERATURE中
{
    DS18B20_Init();
    DS18B20_Write_Byte(0xcc);   //skip rom
    DS18B20_Write_Byte(0x44);   //0X44, 温度转换指令， 启动DS18B20进行温度转换,数据存入内部9字节的RAM中

    DS18B20_Init();
    DS18B20_Write_Byte(0xcc);   //skip rom
    DS18B20_Write_Byte(0xbe);   //0XBE, 读暂存器

    DS18B20_Get_Temp0_1();
}


/***************************************************************************//**
 * @brief
 * @param
 * @return none
 *******************************************************************************/


/***************************************************************************//**
 * @}
 ******************************************************************************/
