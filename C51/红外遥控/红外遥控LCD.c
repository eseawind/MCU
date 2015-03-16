/***************************************************
 *         Author: Shawn Guo                       *
 *         Date  : 2013/2/13                       *
 *         Last  : 2013/2/13                       *
 *         Notes : 红外遥控                        *
 *     LCD1602, J1短路帽短接                       *
 *    Bug: 与程序无关，LCD连接后，INT1则为低电平  *
 *      导致红外INT0中断无法使用                  *
 **************************************************/

#include<reg52.h>
#include <intrins.h> //_nop_();

//红外接收头
sbit IRIN = P3^2;         //红外接收器数据线
unsigned char IRCOM[7];   //红外线数据接收数组

// LCD1602 接口
sbit LCD_RS = P2^6;  //复位端, 高电平数据操作，低电平命令操作
sbit LCD_RW = P2^5;	 //写数据端，高电平读取，低电平写入
sbit LCD_EN = P2^7;	 //使能端，高电平时能读取，高脉冲写入
//  LCD1602 数据
unsigned char code DISPLAY1[] = {" REMOTE CONTROL "}; // 16 bytes
unsigned char code DISPLAY2[] = {"  IR-CODE: ----H"};

void Delayms(unsigned int xms);
void Delay0_14ms(unsigned int x);    //x*0.14MS

bit LCD_IsBusy();   // 检测LCD是否繁忙
void LCD_W_Cmd(unsigned char cmd);// 写入指令数据到LCD
void LCD_Pos(unsigned char pos); //设定LCD数据指针
void LCD_W_Data(unsigned char dat);  //写入字符显示数据到LCD
void LCD_Init();    //LCD初始化设定

int main()
{
    unsigned char i = 0;

    IE = 0x81; // 1000 0001, EA = 1, EX0 = 1

    // TF1 TR1 TF0 TR0 IE1 IT1 IE0 IT0
    TCON = 0X01;  // IT = 0，低电平触发； IT = 1， 从高到低的跳变沿触发

    IRIN = 1;  // IR 初始化

    //LCD显示数据
    LCD_Init();
    Delayms(10);

    LCD_Pos(0x00);
    i = 0;
    while(DISPLAY1[i] != '\0')   // 显示字符串1
    {
        LCD_W_Data(DISPLAY1[i]);
        i++;
        Delayms(50); //控制显示速度
    }

    LCD_Pos(0x40);
    i = 0;
    while(DISPLAY2[i] != '\0')   // 显示字符串1
    {
        LCD_W_Data(DISPLAY2[i]);
        i++;
        Delayms(50); //控制显示速度
    }

    while(1)   // 等待红外中断
        ;

    return 0;
}


// NEC协议 红外脉冲链： 引导码(9ms高电平，4.5ms低电平), 8字节数据码，8字节数据反码，8字节命令码，8字节命令反码
void IR() interrupt 0 	 //红外中断连接外部中断0，故外部中断0服务程序
{
    unsigned char j, k, N = 0;

    EX0 = 0;  //暂时屏蔽外部中断0允许位

    Delay0_14ms(10);  //防抖延时
    if (IRIN == 1)  //判断是否真正中断，若IRIN为正常高电平，则返回
    {
        EX0 = 1;
        return;
    }

    //IRIN产生中断信号，则继续中断服务程序
    while (!IRIN)   //跳过9ms的前导低电平信号，等IR变为高电平
    {
        Delay0_14ms(1);
    }
    while (IRIN)   //跳过4.5ms的前导高电平信号。等 IR 变为低电平
    {
        Delay0_14ms(1);
    }

    for (j = 0; j < 4; j++)       //收集四组数据，分别为数据码，数据反码，命令码，命令反码
    {
        for (k = 0; k < 8; k++)      //每组数据有8位
        {
            while (!IRIN)          //等 IR 变为高电平
            {
                Delay0_14ms(1);
            }

            while (IRIN) //计算IR高电平时长
            {
                Delay0_14ms(1);
                N++;             // N为计数器
                if (N >= 30)         // 超过30 * 0.14ms，则认为IR是正常高电平信号，退出
                {
                    EX0 = 1;
                    return;
                }
            }                        //高电平计数完毕

            //协议从最低位开始发送，接收也应按顺序
            IRCOM[j] = IRCOM[j] >> 1; //腾出最高位
            if (N >= 8) // 高电平持续时间较长，则可认为是"1"; 数据“0” 脉冲长度1.12ms（0.14 * 8）, 数据“1”脉冲长度2.25ms
            {
                IRCOM[j] = IRCOM[j] | 0x80;   //数据最高位补“1”
            }
            N = 0;
        }//end for k
    }//end for j，数据接收完毕

    //开始数据校验
    if (IRCOM[2] != ~IRCOM[3])  //命令码与命令反码匹配失败
    {
        EX0=1;
        return;
    }

    //处理数据，作于LCD显示之用
    IRCOM[5] = IRCOM[2] & 0x0F;     //取键码的低四位
    IRCOM[6] = IRCOM[2] >> 4;       //右移4次，高四位变为低四位

    if(IRCOM[5] > 9)
        IRCOM[5] = IRCOM[5] + 0x37;
    else
        IRCOM[5] = IRCOM[5] + 0x30;

    if(IRCOM[6] > 9)
        IRCOM[6] = IRCOM[6] + 0x37;
    else
        IRCOM[6] = IRCOM[6] + 0x30;

    LCD_Pos(0x4b);
    LCD_W_Cmd(IRCOM[6]);        //第一位数显示
    LCD_Pos(0x4c);
    LCD_W_Cmd(IRCOM[5]);        //第二位数显示

    EX0 = 1;        //接触屏蔽
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

void Delay0_14ms(unsigned int x)    //x*0.14MS
{
    unsigned char i;

    while(--x)
    {
        for (i = 0; i < 13; i++) {}
    }
}
