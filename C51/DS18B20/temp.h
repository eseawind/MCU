/********************************************************
 *         Author: Shawn Guo                            *
 *         Date  : 2013/2/18                            *
 *         Last  : 2013/2/18                            *
 *         Note:   DS18B20温度传感器 驱动               *
 ********************************************************/

void Init_DS18B20();        // DS18B20初始化
void Write_Byte (unsigned char idata dat);  //单字节写入
unsigned char Read_Byte();     //读取单字节
void Read_Str(unsigned char idata n);  //依次读取n个字节放入从p开始的内存中,p必须是以配置好的ID缓存或温度缓存
unsigned char CRC(unsigned char n);      // CRC校验温度缓存中的n位数据,并返回结果
void Get_Temp();             // 读取温度至温度缓存数组TEMP_BUFF中,需要P提前指向温度缓存，最后的数据在TEMPERATURE中
void Config_DS18B20();  //重新配置报警限定值和分辨率
void Read_ID(); //读取器件 id，需要p提前设置位ID缓存
void Temperatuer_Result(); // DS18B20 温度处理:读ID，配置温度计，转换温度，读取温度至温度缓存
void GetTemp();  //每间隔一段时间，读取一次温度
void Delay_Temp(unsigned char idata xus);
