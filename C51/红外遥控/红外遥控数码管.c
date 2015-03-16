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


// 74HC138        数码管位选控制
sbit HC138A = P2^2;
sbit HC138B = P2^3;
sbit HC138C = P2^4;
// Nixie Tube(-)     低电平位选数码管， 高电平段选数码条， 因此数码表取自Tube(-) ，并取反
// a b ... g dp (数码管段选接口)
// 8 7 ... 2 1  (对应电平接口)
//                                  0      1       2      3     4      5      6      7       8     9      A      b      C      d      E      F
unsigned char code NUM_TABLE[] = {~0x03, ~0x9f, ~0x25, ~0x0d, ~0x99, ~0x49, ~0x41, ~0x1f, ~0x01, ~0x09, ~0x11, ~0xc1, ~0x63, ~0x85, ~0x61, ~0x71};
unsigned char LED[8];

void Delayms(unsigned int xms);
void Delay0_14ms(unsigned int x);    //x*0.14MS
void Display(unsigned int NumVal);   //将数字显示至数码管0~3

int main()
{
    IE = 0x81; // 1000 0001, EA = 1, EX0 = 1

    // TF1 TR1 TF0 TR0 IE1 IT1 IE0 IT0
    TCON = 0X01;  // IT = 0，低电平触发； IT = 1， 从高到低的跳变沿触发

    IRIN = 1;  // IR 初始化

    while(1)   //将接收到的命令码IRCOM[2]发送至数码管上
    {
        Display(IRCOM[2]);
    }

    return 0;
}

void Display(unsigned int NumVal)   //将数字显示至数码管0~3
{
    unsigned char i = 0;

    P0 = 0XFF;          // 消影
    LED[0] = NUM_TABLE[NumVal % 10000 / 1000];
    LED[1] = NUM_TABLE[NumVal % 1000 / 100];
    LED[2] = NUM_TABLE[NumVal % 100 / 10];
    LED[3] = NUM_TABLE[NumVal % 10];

    for(i = 0; i < 4; i++)
    {
        switch(i)   // 数码管位选  HC138
        {
        case 0:
            HC138C = 0;
            HC138B = 0;
            HC138A = 0;
            break;
        case 1:
            HC138C = 0;
            HC138B = 0;
            HC138A = 1;
            break;
        case 2:
            HC138C = 0;
            HC138B = 1;
            HC138A = 0;
            break;
        case 3:
            HC138C = 0;
            HC138B = 1;
            HC138A = 1;
            break;
        }

        P0 = LED[i];
        Delayms(6);   //调节延迟以取得最佳数码扫描效果
    }
}


// NEC协议 红外脉冲链： 引导码(9ms高电平，4.5ms低电平), 8字节数据码，8字节数据反码，8字节命令码，8字节命令反码
void IR() interrupt 0 	 //红外中断连接外部中断0，故外部中断0服务程序
{
    unsigned char j, k, N = 0;

    EX0 = 0;  //暂时屏蔽外部中断0允许位

    Delay0_14ms(5);  //防抖延时
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

    for (j = 0; j < 4; j++)       //收集四组数据，分别为数据码，数据反码，命令码，命令反码
    {
        for (k = 0; k < 8; k++)      //每组数据有8位
        {
            while (IRIN)   //跳过4.5ms的前导高电平信号。等 IR 变为低电平
            {
                Delay0_14ms(1);
            }
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
        EX0 = 1;
        return;
    }

    EX0 = 1;        //接触屏蔽
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

    while(x--)   //  注： 此处考虑到--X比X--更精简，故用--x,但程序中有用Delay0_14ms(1)计算脉冲长度的块，因此会出现BUG
    {
        for (i = 0; i < 13; i++) {}
    }
}
