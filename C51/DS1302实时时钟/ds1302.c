/*******************************************************************************
*  标题:  试验数码管显示时钟                                                   *
*																			   *
* 通过本例程了解 DS1302时钟芯片的基本原理和使用 ,理解并掌握DS1302时钟芯片	   *
* 驱动程序的编写以及实现数字字符在数码管中的显示。							   *
* 注意：JP1302跳线冒要短接。                   	                               *
* 请学员认真消化本例程，懂DS1302在C语言中的操作                       	       *
********************************************************************************/
#include<reg52.h> //包含头文件，一般情况不需要改动，头文件包含特殊功能寄存器的定义
#include <intrins.h>

sbit SCK=P3^6;	//时钟
sbit SDA=P3^4;	//数据
sbit RST = P3^5;// DS1302复位

sbit LS138A=P2^2;
sbit LS138B=P2^3;
sbit LS138C=P2^4;

bit ReadRTC_Flag;//定义读DS1302标志

unsigned char l_tmpdate[7]= {0,0,12,15,5,3,8}; //秒分时日月周年08-05-15 12:00:00
unsigned char l_tmpdisplay[8];

code unsigned char write_rtc_address[7]= {0x80,0x82,0x84,0x86,0x88,0x8a,0x8c}; //秒分时日月周年 最低位读写位
code unsigned char read_rtc_address[7]= {0x81,0x83,0x85,0x87,0x89,0x8b,0x8d};



code unsigned char table[]= {0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x40};
//共阴数码管 0-9  '-' '熄灭‘表

/******************************************************************/
/*                    函数声明                                    */
/******************************************************************/
void Write_Ds1302_byte(unsigned char temp);
void Write_Ds1302( unsigned char address,unsigned char dat );
unsigned char Read_Ds1302 ( unsigned char address );
void Read_RTC(void);//read RTC
void Set_RTC(void); //set RTC
void InitTIMER0(void);//inital timer0
/******************************************************************/
/*                   主函数                                       */
/******************************************************************/
void main(void)
{
    InitTIMER0();          //初始化定时器0
    Set_RTC();             //写入时钟值，如果使用备用电池时候，不需要没每次上电写入，此程序应该屏蔽

    while(1)
    {
        if(ReadRTC_Flag)
        {
            ReadRTC_Flag=0;
            Read_RTC();

            l_tmpdisplay[0]=l_tmpdate[2]/16;			//数据的转换，因我们采用数码管0~9的显示,将数据分开
            l_tmpdisplay[1]=l_tmpdate[2]&0x0f;
            l_tmpdisplay[2]=10;					    	//加入"-"
            l_tmpdisplay[3]=l_tmpdate[1]/16;
            l_tmpdisplay[4]=l_tmpdate[1]&0x0f;
            l_tmpdisplay[5]=10;
            l_tmpdisplay[6]=l_tmpdate[0]/16;
            l_tmpdisplay[7]=l_tmpdate[0]&0x0f;

        }
    }
}
/******************************************************************/
/*                   定时器0初始化                                */
/******************************************************************/
void InitTIMER0(void)
{
    TMOD|=0x01;//定时器设置 16位
    TH0=0xef;//初始化值
    TL0=0xf0;
    ET0=1;
    TR0=1;
    EA=1;
}


/******************************************************************/
/*                   写一个字节                                   */
/******************************************************************/
void Write_Ds1302_Byte(unsigned  char temp)
{
    unsigned char i;
    for (i=0; i<8; i++)     	//循环8次 写入数据
    {
        SCK=0;
        SDA=temp&0x01;     //每次传输低字节
        temp>>=1;  		//右移一位
        SCK=1;
    }
}
/******************************************************************/
/*                  写入DS1302                                    */
/******************************************************************/
void Write_Ds1302( unsigned char address,unsigned char dat )
{
    RST=0;
    _nop_();
    SCK=0;
    _nop_();
    RST=1;
    _nop_();                    //启动
    Write_Ds1302_Byte(address);	//发送地址
    Write_Ds1302_Byte(dat);		//发送数据
    RST=0;  		            //恢复
}
/******************************************************************/
/*                   读出DS1302数据                               */
/******************************************************************/
unsigned char Read_Ds1302 ( unsigned char address )
{
    unsigned char i,temp=0x00;
    RST=0;
    _nop_();
    _nop_();
    SCK=0;
    _nop_();
    _nop_();
    RST=1;
    _nop_();
    _nop_();
    Write_Ds1302_Byte(address);
    for (i=0; i<8; i++) 		//循环8次 读取数据
    {
        if(SDA)
            temp|=0x80;			//每次传输低字节
        SCK=0;
        temp>>=1;			//右移一位
        _nop_();
        _nop_();
        _nop_();
        SCK=1;
    }
    RST=0;
    _nop_();	          	//以下为DS1302复位的稳定时间
    _nop_();
    RST=0;
    SCK=0;
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    SCK=1;
    _nop_();
    _nop_();
    SDA=0;
    _nop_();
    _nop_();
    SDA=1;
    _nop_();
    _nop_();
    return (temp);			//返回
}
/******************************************************************/
/*                   读时钟数据                                   */
/******************************************************************/
void Read_RTC(void)	        //读取 日历
{
    unsigned char i,*p;
    p=read_rtc_address; 	    //地址传递
    for(i=0; i<7; i++)		  //分7次读取 秒分时日月周年
    {
        l_tmpdate[i]=Read_Ds1302(*p);
        p++;
    }
}
/******************************************************************/
/*                  设定时钟数据                                  */
/******************************************************************/
void Set_RTC(void)		    //设定 日历
{
    unsigned char i,*p,tmp;
    for(i=0; i<7; i++)      //BCD处理
    {
        tmp=l_tmpdate[i]/10;
        l_tmpdate[i]=l_tmpdate[i]%10;
        l_tmpdate[i]=l_tmpdate[i]+tmp*16;
    }
    Write_Ds1302(0x8E,0X00);

    p=write_rtc_address;	//传地址
    for(i=0; i<7; i++)		//7次写入 秒分时日月周年
    {
        Write_Ds1302(*p,l_tmpdate[i]);
        p++;
    }
    Write_Ds1302(0x8E,0x80);
}
/******************************************************************/
/*                   定时器中断函数                               */
/******************************************************************/
void tim(void) interrupt 1 using 1
//中断，用于数码管扫描
{

    static unsigned char i,num;
    TH0=0xf5;
    TL0=0xe0;

    P0=table[l_tmpdisplay[i]];		//查表法得到要显示数字的数码段

    switch(i)
    {
    case 0:
        LS138A=0;
        LS138B=0;
        LS138C=0;
        break;
    case 1:
        LS138A=1;
        LS138B=0;
        LS138C=0;
        break;
    case 2:
        LS138A=0;
        LS138B=1;
        LS138C=0;
        break;
    case 3:
        LS138A=1;
        LS138B=1;
        LS138C=0;
        break;
    case 4:
        LS138A=0;
        LS138B=0;
        LS138C=1;
        break;
    case 5:
        LS138A=1;
        LS138B=0;
        LS138C=1;
        break;
    case 6:
        LS138A=0;
        LS138B=1;
        LS138C=1;
        break;
    case 7:
        LS138A=1;
        LS138B=1;
        LS138C=1;
        break;

    }
    i++;
    if(i==8)
    {
        i=0;
        num++;
        if(10==num)       //隔段时间读取1302的数据。时间间隔可以调整
        {
            ReadRTC_Flag=1; //使用标志位判断
            num=0;
        }
    }
}


