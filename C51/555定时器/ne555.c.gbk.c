/***************************************************
 *         Author: Shawn Guo                       *
 *         Date  : 2013/2/14                       *
 *         Last  : 2013/2/14                       *
 *         Notes : 555定时器(硬件定时器)           *
 * 定时分类：软件定时，硬件定时，可编程硬件定时   *
 *     LCD1602, J11短路帽短接                      *
 *     NE555处的阀值由电位器控制，因此旋转电位器  *
 *     可看到NE555的频率不同                       *
 *     NE555的输出为P3^5(T1中断位)                 *
 **************************************************/

#include<reg52.h>
#include <intrins.h> //_nop_();

// LCD1602 接口
sbit LCD_RS = P2^6;  //复位端, 高电平数据操作，低电平命令操作
sbit LCD_RW = P2^5;	 //写数据端，高电平读取，低电平写入
sbit LCD_EN = P2^7;	 //使能端，高电平时能读取，高脉冲写入
//  LCD1602 数据
unsigned char code DISPLAY1[] = {"    CYMOMETER   "}; // 16 bytes
unsigned char code DISPLAY2[] = {"FREQ:        Hz "};
unsigned char LCD[8];

void Delayms(unsigned int xms);
void Delay_4nop();  // 4.34us
bit LCD_IsBusy();   // 检测LCD是否繁忙
void LCD_W_Cmd(unsigned char cmd);// 写入指令数据到LCD
void LCD_Pos(unsigned char pos); //设定LCD数据指针
void LCD_W_Data(unsigned char dat);  //写入字符显示数据到LCD
void LCD_Init();    //LCD初始化设定
void Display(unsigned int Val);  //  固定地方显示此值
void Time_Init();   //定时器初始化

bit SEC_FLAG = 0;// 秒标志位， 用T0计时，每一秒置SEC_FLAG为1,同时停止定时器等待处理
unsigned int COUNT = 0;  // TIME1中断响应计时器，用于计算频率之用
char code reserve[3]_at_ 0x3b; //保留0x3b开始的3个字节

int main()
{
    unsigned char i;
    unsigned char hdata, ldata;   // 用于记录每秒定时器0的TH0与TL0走了多少
    unsigned int frq_num;

    P3 = 0xff; // ???

    LCD_Init();

    LCD_Pos(0x00); //设置显示位置为第一行
    for (i = 0; i < 16; i++)
        LCD_W_Data(DISPLAY1[i]);

    LCD_Pos(0x40); //设置显示位置为第二行
    for (i = 0; i < 16; i++)
        LCD_W_Data(DISPLAY2[i]);

    Time_Init(); //定时器初始化，准备计算频率

    while (1)
    {
        if (SEC_FLAG)   //秒标志位为1时
        {
            hdata = TH1; //取计数值
            ldata = TL1;

            // 计算频率
            frq_num = ((COUNT *65535 + hdata * 256 + ldata) * 108 / 100);

            //
            TH1 = 0;
            TL1 = 0;

            SEC_FLAG = 0;
            COUNT = 0;

            TR1 = 1;   //重新开始
            TR0 = 1;
        }

        Display(frq_num);  // 显示频率数值
    }
    return 0;
}


void Time_Init()   //定时器初始化
{
    // 0101 0001 ;定时器1工作在16位计数方式，记录NE555的脉冲次数; 定时器0工作在16位定时方式，作为标志时间用来计算NE555的频率
    TMOD = 0x51;

    TH0 = 0x4c; //50ms定时
    TL0 = 0x00;

    TH1 = 0x00; //计数初值
    TL1 = 0x00;

    ET0 = 1; //使能TIMER0中断
    ET1 = 1; //使能TIMER1中断
    EA = 1; //允许中断

    PT1 = 1; //定义TIMER1中断优先

    TR0 = 1;
    TR1 = 1;
}

void Timer0() interrupt 1
{
    static unsigned char msec = 0;   // 中断响应计时器

    TH0 = 0x4c; //50ms定时
    TL0 = 0x00;

    msec++;
    if (msec == 20)       //50*20=1 S
    {
        TR0 = 0; //关闭TIMER0
        TR1 = 0; //关闭TIMER1
        msec = 0;

        SEC_FLAG = 1; //置秒标记位
    }
}

void Timer1() interrupt 3         // NE555的输出为P3^5(T1中断位)
{
    COUNT++;   // 中断响应计时器，用于计算频率之用
}


void Display(unsigned int Val)   //  固定地方显示此值
{
    unsigned char i;

    for (i = 0; i <= 4; i++)//数据转换,将1个int值转换为倒着的字符串数据
    {
        LCD[i] = Val % 10 + 0x30;  //每次都取个位转换
        Val = Val / 10;
    }
    LCD[5] = Val + 0x30;   // 此乃最高位

    for (i = 5; i > 0; i--)
    {
        if (LCD[i] == 0x30)//高位为"0"则不显示
            LCD[i] = 0x20;
        else
            break;
    }

    LCD_Pos(0x46); //显示实际频率值
    for (i = 5; i != 0xff; i--)
        LCD_W_Data(LCD[i]);
}

bit LCD_IsBusy()   // 检测LCD是否繁忙
{
    bit result;

    LCD_RS = 0;       //读取状态字，状态字输出至D0~D7供CPU读取
    LCD_RW = 1;
    LCD_EN = 1;
    _nop_();
    _nop_();
    _nop_();
    _nop_();

    //状态字 STA7 STA6 STA5 STA4 STA3 STA2 STA1 STA0
    // STA7：读写操作使能，高电平表示繁忙，禁止读写操作；低电平表示空闲
    result = (bit)(P0 & 0x80);	 //读取STA7

    LCD_EN = 0; //结束使能片选

    return result;//返回侧忙结果
}

void LCD_W_Cmd(unsigned char cmd)// 写入指令数据到LCD
{

    while(LCD_IsBusy());   //等待LCD1602空闲

    LCD_RS = 0;
    LCD_RW = 0;   //写指令

    LCD_EN = 0;  //拉低，准备发射高脉冲送入命令
    _nop_();
    _nop_();

    P0 = cmd; //将8位指令通过P0口传给1602
    _nop_();
    _nop_(); //用于产生一个脉冲宽度
    _nop_();
    _nop_();
    LCD_EN = 1;   //高脉冲送入命令
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    LCD_EN = 0;
}

void LCD_Pos(unsigned char pos) //设定LCD数据指针
{
    //数据指针设置 80H + (0~27H, 40~67H)
    LCD_W_Cmd(pos | 0x80);
}

void LCD_W_Data(unsigned char dat)  //写入字符显示数据到LCD
{
    while(LCD_IsBusy());   //等待LCD1602空闲

    LCD_RS = 1;
    LCD_RW = 0;   //写数据操作

    LCD_EN = 0;   //拉低，准备发射高脉冲送入数据

    P0 = dat;  //将8位数据通过P0口传给1602
    _nop_();
    _nop_();   //用于产生一个脉冲宽度
    _nop_();
    _nop_();
    LCD_EN = 1;   //高脉冲送入数据至LCD
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    LCD_EN = 0;
}

void LCD_Init()    //LCD初始化设定
{
    //显示模式设置 0011 1000， 16*2显示，5*7点阵，8位数据接口
    LCD_W_Cmd(0x38);
    Delayms(1);

    //0000 1           D        C       B
    LCD_W_Cmd(0x0c);//显示on，光标off,  光标闪烁off
    Delayms(1);

    //0000 01          N                   S
    LCD_W_Cmd(0x06);//读写操作后指针加/减  写入字符后整屏是否左移
    Delayms(1);

    // 01H 数据指针清零，显示清零        02H 数据指针清零
    LCD_W_Cmd(0x01);     //清除LCD的显示内容
    Delayms(1);
}

void Delayms(unsigned int xms)
{
    unsigned char i;

    while(--xms)
    {
        for(i = 0; i < 110; i++)
            ;
    }
}

void Delay_4nop()  // 4.34us
{
    _nop_();
    _nop_();
    _nop_();
    _nop_();
}
