/********************************************************
 *         Author: Shawn Guo                            *
 *         Date  : 2013/2/15                            *
 *         Last  : 2013/2/15                            *
 *         Project:   LCD时钟                           *
 *         Notes:   时钟驱动                            *
 ********************************************************/

void Time_Load();         // 载入时间数据
void Time_Save();         // 保存时间数据
void Alarm_Load();         // 载入闹钟数据
void Alarm_Save();         // 保存闹钟数据
void Time_Init(); //定时器初始配置
void Time_Alarm();        // 闹钟检测，如果为闹钟时间，则设定报警程序，任意按键后取消报警
void Write_Ds1302_Byte(unsigned char temp);
void Write_Ds1302( unsigned char address, unsigned char dat);
unsigned char Read_Ds1302 (unsigned char address);
