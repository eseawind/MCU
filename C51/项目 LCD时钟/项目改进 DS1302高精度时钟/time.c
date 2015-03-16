/*******************************************************************
 *         Author: Shawn Guo                                       *
 *         Date  : 2013/2/15                                       *
 *         Last  : 2013/2/15                                       *
 *         Project:   LCD时钟                                      *
 *         Notes:   ds1302时钟驱动                                 *
 *                P3^0 | BEEP                                      *
 *******************************************************************/

#include <reg52.h>
#include <intrins.h> //_nop_();
#include "time.h"

sbit BEEP = P3^0;  // 蜂鸣器

sbit SCK = P3^6;	//时钟
sbit SDA = P3^4;	//数据
sbit RST = P3^5;    // DS1302复位, 高电平载入数据复位，低电平变为高阻态

//ds1302 寄存器
// 0x80 0x82 0x84 0x86 0x88 0x8a 0x8c 0x8e ; 读取寄存器时，地址最后一位要置高电平
//  秒   分   时   日    月   周  年 ， 静态RAM
code unsigned char W_ADDRESS[10]= {0x80,0x82,0x84,0x86,0x88,0x8a,0x8c,0xc0,0xc2,0xc4}; //秒分时日月周年，闹钟时，闹钟分，闹钟秒 最低位读写位
code unsigned char R_ADDRESS[10]= {0x81,0x83,0x85,0x87,0x89,0x8b,0x8d,0xc1,0xc3,0xc5};

extern char YEAR1, YEAR2, MONTH, DAY, WEEK, HOUR, MINUTE, SECOND;  // 年月日周时分秒
extern char ALARM_H , ALARM_M , ALARM_S;  // 闹铃变量, POS为 8,9,10说明是修改闹钟变量

extern void LCD_Display();        // 显示数据
extern void Delayms(unsigned int xms);

unsigned char TIME_DATA[10];// 秒分时日月周年，闹钟时，闹钟分，闹钟秒

void Time_Load()         // 载入时间数据
{
    unsigned char i, *p, tmp;

    p = R_ADDRESS; 	    //地址传递
    for(i = 0; i < 10; i++)		  //分10次读取  秒分时日月周年，闹钟时，闹钟分，闹钟秒 , 并将读取的BCD数据转换为10进制
    {
        TIME_DATA[i] = Read_Ds1302(*p);
        p++;
        tmp = TIME_DATA[i] / 16;
        TIME_DATA[i] = tmp * 10 + TIME_DATA[i] % 16;
    }

    SECOND = TIME_DATA[0];
    MINUTE = TIME_DATA[1];
    HOUR = TIME_DATA[2];
    DAY = TIME_DATA[3];
    MONTH = TIME_DATA[4];
    WEEK = TIME_DATA[5];
    YEAR2 = TIME_DATA[6];
    ALARM_H = TIME_DATA[7];
    ALARM_M = TIME_DATA[8];
    ALARM_S = TIME_DATA[9];

}

void Time_Save()         // 保存时间数据
{
    unsigned char i, *p, tmp;

    TIME_DATA[0] = SECOND;
    TIME_DATA[1] = MINUTE;
    TIME_DATA[2] = HOUR;
    TIME_DATA[3] = DAY;
    TIME_DATA[4] = MONTH;
    TIME_DATA[5] = WEEK;
    TIME_DATA[6] = YEAR2;
    TIME_DATA[7] = ALARM_H;
    TIME_DATA[8] = ALARM_M;
    TIME_DATA[9] = ALARM_S;


    for(i = 0; i < 10; i++) //将十进制数据转换为 BCD， DS1302中按BCD数据存储
    {
        tmp = TIME_DATA[i] / 10;
        TIME_DATA[i] = TIME_DATA[i] % 10 + tmp * 16;
    }

    Write_Ds1302(0x8E,0X00);   //写控制字，允许写操作

    p = W_ADDRESS;	//传地址
    for(i = 0; i < 10; i++)		//10次写入
    {
        Write_Ds1302(*p, TIME_DATA[i]);
        p++;
    }
    //         0X8E 的最高位为写保护标志
    Write_Ds1302(0x8E,0x80);  //写保护，不允许写
}

void Alarm_Load()         // 载入闹钟数据
{
    unsigned char i, *p, tmp;

    p = R_ADDRESS + 7; 	    //地址传递
    for(i = 7; i < 10; i++)		  //分3次读取 闹钟时，闹钟分，闹钟秒 , 并将读取的BCD数据转换为10进制
    {
        TIME_DATA[i] = Read_Ds1302(*p);
        p++;
        tmp = TIME_DATA[i] / 16;
        TIME_DATA[i] = tmp * 10 + TIME_DATA[i] % 16;
    }
    ALARM_H = TIME_DATA[7];
    ALARM_M = TIME_DATA[8];
    ALARM_S = TIME_DATA[9];
}
void Alarm_Save()         // 保存闹钟数据
{
    unsigned char i, *p, tmp;

    TIME_DATA[7] = ALARM_H;
    TIME_DATA[8] = ALARM_M;
    TIME_DATA[9] = ALARM_S;


    for(i = 7; i < 10; i++) //将十进制数据转换为 BCD， DS1302中按BCD数据存储
    {
        tmp = TIME_DATA[i] / 10;
        TIME_DATA[i] = TIME_DATA[i] % 10 + tmp * 16;
    }

    Write_Ds1302(0x8E,0X00);   //写控制字，允许写操作

    p = W_ADDRESS + 7;	//传地址
    for(i = 7; i < 10; i++)		//10次写入
    {
        Write_Ds1302(*p, TIME_DATA[i]);
        p++;
    }
    //         0X8E 的最高位为写保护标志
    Write_Ds1302(0x8E,0x80);  //写保护，不允许写
}

void Time_Init() //定时器初始配置, DS1302初始设置,定时器每隔一段时间读取一次数据
{
    //定时器工作方式配置
    //             TIME1           TIME0
    // TMOD = GATE C/T M1 M0   GATE C/T M1 M0
    TMOD = 0x01; //选择Timer0 模式1  16位定时

    //预装初值
    TH0 = (65536 - 50000) / 256;             // 50ms定时
    TL0 = (65536 - 50000) % 256;

    //中断允许寄存器 IE配置 EA - ET2 ES ET1 EX1 ET0 EX0
    IE = 0x82;   // EA = 1, ET0 = 1
    TR0 = 1;    //开启T0定时器
}

void Time_Alarm()        // 闹钟检测，如果为闹钟时间，则设定报警程序，任意按键后取消报警
{
    TR0 = 0;    //暂时关闭定时器，即停止LCD改变
    P1 = 0xff;
    while(P1 == 0XFF)   //等待按键响应闹铃
    {
        BEEP = 0;   //低电平接通蜂鸣器，音频信号通过电磁线圈，产生磁场驱动膜片发声
        Delayms(5);
        BEEP = 1;
        Delayms(5);
    }
    TR0 = 1;
}

void Timer0() interrupt 1
{
    static unsigned char num = 0;

    TH0 = (65536 - 50000) / 256;             // 50ms定时
    TL0 = (65536 - 50000) % 256;

    if(++num >= 20)          // 1s , 每秒从DS1302中读取一次数据
    {
        num = 0;
        Time_Load();
        LCD_Display();
        if(SECOND == ALARM_S)
            if(MINUTE == ALARM_M)
                if(HOUR == ALARM_H)
                    Time_Alarm();
    }

}


//控制字节从最低位开始输出
void Write_Ds1302_Byte(unsigned char temp)
{
    unsigned char i;

    for (i = 0; i < 8; i++)     	//循环8次 写入数据
    {
        SCK = 0;
        SDA = temp & 0x01;     //每次传输低字节
        temp >>= 1;  		//右移一位
        SCK = 1;            //上升沿，写入数据
    }
}

//写入一个数据至相应地址
void Write_Ds1302( unsigned char address, unsigned char dat)
{
    RST = 0;
    _nop_();
    SCK = 0;
    _nop_();
    RST = 1;
    _nop_();                    //启动

    Write_Ds1302_Byte(address);	//发送地址

    Write_Ds1302_Byte(dat);		//发送数据

    RST = 0;  		            //恢复
}

//从相应地址读取数据并返回
unsigned char Read_Ds1302 (unsigned char address)  // 单字节读操作需要16个时钟，前8个时钟的上升沿输入地址，后8个时钟的下降沿输出数据
{
    unsigned char i,temp = 0x00;

    RST = 0;
    _nop_();
    _nop_();

    SCK = 0;
    _nop_();
    _nop_();

    RST = 1;
    _nop_();
    _nop_();   //启动

    Write_Ds1302_Byte(address);

    for (i = 0; i < 8; i++) 		//循环8次 读取数据
    {
        if(SDA) temp |= 0x80;			//每次接受低字节

        SCK = 0;         //SCK拉低，等待SDA数据变化
        temp >>= 1;			//右移一位
        _nop_();
        _nop_();
        _nop_();
        SCK = 1;         //SCK高电平，SDA数据保持不变，等待接受
    }

    RST = 0;
    _nop_();	          	//以下为DS1302复位的稳定时间
    _nop_();
    RST = 0;
    SCK = 0;
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    SCK = 1;
    _nop_();
    _nop_();
    SDA = 0;
    _nop_();
    _nop_();
    SDA = 1;
    _nop_();
    _nop_();

    return temp;			//返回
}
