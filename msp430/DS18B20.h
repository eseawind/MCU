/**********************************************
 *
 *      Author  :       Shawn Guo
 *      Mail    :       iguoxiaopeng@gmail.com
 *      Date    :       2013/8/8
 *      Last    :
 *      Notes   :       DS18B20温度传感器驱动
 *                      单总线必须要接上拉电阻，
 *                      保证总线空置时将一直处于高电平状态
 *      Tool    :       ds18b20     MSP430X2XX
 **********************************************/
#ifndef __DS18B20_H__
#define __DS18B20_H__


#define DS18B20_DIR0    (P7DIR &= ~BIT1)
#define DS18B20_DIR1    (P7DIR |=  BIT1)

#define DS18B20_IN      (P7IN & BIT1)
#define DS18B20_OUT0    (P7OUT &= ~BIT1)
#define DS18B20_OUT1    (P7OUT |=  BIT1)

unsigned char DS18B20_Init();// DS18B20初始化复位, 返回标志位(=1,成功; =0,初始化失败)

void DS18B20_Write_Byte(unsigned char dat);  //单字节写入
unsigned char DS18B20_Read_Byte();     //读取单字节
void DS18B20_Read_Str(unsigned char n);  //依次读取n个字节放入缓存数组DS18B20_BUFFER中, 要求n <= 8;

// 读取温度至温度缓存数组TEMP_BUFF中,需要PTR提前指向温度缓存，最后的数据在TEMPERATURE中
void DS18B20_Get_Temp();

// DS18B20_Get_Temp0_1()函数获得的温度精确度为0.1度! 将温度放大10倍已达到更精确的精度.
void DS18B20_Get_Temp0_1();

void DS18B20_Config();  //重新配置报警限定值和分辨率
void DS18B20_Read_ID();//读取器件 id, 写在DS18B20_BUFFER缓存的前八位.
void DS18B20_Temperatuer(); // DS18B20 温度处理:转换温度，读取温度至温度缓存DS18B20_TEMPERATURE中

extern unsigned char DS18B20_BUFFER[9];
extern unsigned int DS18B20_TEMPERATURE;
extern unsigned char DS18B20_TEMP_FLAG;    //温度标示, 0标示正数，1标示负数.
#endif /* __DS18B20_H__ */
