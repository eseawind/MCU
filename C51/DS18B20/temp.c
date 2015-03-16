/********************************************************
 *         Author: Shawn Guo                            *
 *         Date  : 2013/2/18                            *
 *         Last  : 2013/2/18                            *
 *         Note:   DS18B20温度传感器 驱动               *
 ********************************************************/

#include <reg52.H>
#include <intrins.h>       // _nop_();
#include "temp.h"

sbit    D18B20=P3^7;


unsigned char idata TIM;  // Time1 中断响应计时器
unsigned char idata *PTR;   // 通用指针，用于快速指向ID缓存或温度缓存
unsigned char idata TEMP_BUFF[9]; //存储读取的字节,读取温度器内部RAM的9字节数据
unsigned char idata ID_BUFF[8];   // read rom ,ID为8字节
bit  FLAG;                         // DS18B20 初始化标志; FLAG = 1表示初始化成功
unsigned int  idata TEMPERATURE;
unsigned char idata CRC_DATA;        // CRC校验数据

// CRC校验表
unsigned char code CRC_TABLE[256] =
{
    0,  94, 188,  226,  97,  63,  221,  131,  194,  156,  126,  32,  163,  253,  31,  65,
    157,  195,  33,  127,  252,  162,  64,  30,  95,  1,  227,  189,  62,  96,  130,  220,
    35,  125,  159,  193,  66,  28,  254,  160,  225,  191,  93,  3,  128,  222,  60,  98,
    190,  224,  2,  92,  223,  129,  99,  61,  124,  34,  192,  158,  29,  67,  161,  255,
    70,  24,  250,  164,  39,  121,  155,  197,  132,  218,  56,  102,  229,  187,  89,  7,
    219,  133, 103,  57,  186,  228,  6,  88,  25,  71,  165,  251,  120,  38,  196,  154,
    101,  59, 217,  135,  4,  90,  184,  230,  167,  249,  27,  69,  198,  152,  122,  36,
    248,  166, 68,  26,  153,  199,  37,  123,  58,  100,  134,  216,  91,  5,  231,  185,
    140,  210, 48,  110,  237,  179,  81,  15,  78,  16,  242,  172,  47,  113,  147,  205,
    17,  79,  173,  243,  112,  46,  204,  146,  211,  141,  111,  49,  178,  236,  14,  80,
    175,  241, 19,  77,  206,  144,  114,  44,  109,  51,  209,  143,  12,  82,  176,  238,
    50,  108,  142,  208,  83,  13,  239,  177,  240,  174,  76,  18,  145,  207,  45,  115,
    202,  148, 118,  40,  171,  245,  23,  73,  8,  86,  180,  234,  105,  55,  213, 139,
    87,  9,  235,  181,  54,  104,  138,  212,  149,  203,  41,  119,  244,  170,  72,  22,
    233,  183,  85,  11,  136,  214,  52,  106,  43,  117,  151,  201,  74,  20,  246,  168,
    116,  42,  200,  150,  21,  75,  169,  247,  182,  232,  10,  84,  215,  137,  107,  53
};


void Init_DS18B20()        // DS18B20初始化
{
    D18B20 = 1;   // 数据线先置高电平1

    _nop_();      // 短暂延时

    D18B20 = 0;   // 拉低

    Delay_Temp(80);   //delay 530 uS//80           延时480~960 us
    _nop_();

    D18B20 = 1;      // 拉高

    Delay_Temp(14);   //delay 100 uS//14            延时15~60us,等待一个由DS18B20返回的低电平0
    _nop_();
    _nop_();
    _nop_();

    if(D18B20 == 0)
        FLAG = 1;   //detect 1820 success!
    else
        FLAG = 0;    //detect 1820 fail!

    Delay_Temp(20);       //片选成功后仍要延时，从上一次拉高算起，至少480us.
    _nop_();
    _nop_();

    D18B20 = 1;     //最后 拉高以结束
}

void Write_Byte (unsigned char idata dat)  //单字节写入
{
    unsigned char idata i;

    for (i = 0; i < 8; i++)  // 每次1位数据
    {
        D18B20 = 0;    //先置低

        _nop_();       //延时15us

        D18B20 = dat & 0x01;    //从低位到高位发送数据

        Delay_Temp(3);   //delay 45 uS //5         延时45us
        _nop_();
        _nop_();

        D18B20 = 1;   // 数据线拉高1
        // 重复
        dat >>= 1;
    }
}

unsigned char Read_Byte()     //读取单字节
{
    unsigned char idata i, dat = 0;

    for(i = 0; i < 8; i++)
    {
        //拉高1
        //延时2us           考虑到前面的数据线一定已经拉高至2us以上，故这里省略

        D18B20 = 0;   //拉低

        dat >>= 1;   // 右移一位，腾出最高位接受数据， 延时6us

        D18B20 = 1;//拉高，延时4us

        if(D18B20 == 1)          //读取数据线数据
            dat |= 0x80;

        Delay_Temp(2); //   延时30us
        _nop_();
    }

    return dat;
}


void Read_Str(unsigned char idata n)  //依次读取n个字节放入从PTR开始的内存中,PTR必须是以配置好的ID缓存或温度缓存
{
    unsigned char idata i;

    for(i = 0; i < n; i++)
    {
        *PTR = Read_Byte();
        PTR++;
    }
}

unsigned char CRC(unsigned char n)      // CRC校验温度缓存中的n位数据,并返回结果
{
    unsigned char idata i, dat = 0;

    for(i = 0; i < n; i++) //查表校验
        dat = CRC_TABLE[dat ^ TEMP_BUFF[i]];

    return dat;
}

void Get_Temp()             // 读取温度至温度缓存数组TEMP_BUFF中,需要PTR提前指向温度缓存，最后的数据在TEMPERATURE中
{
    Read_Str(9);

    if (CRC(9) == 0) //校验正确，进行数据处理
    {
        TEMPERATURE = TEMP_BUFF[1] * 0x100 + TEMP_BUFF[0];
//	    Temperature *= 0.625;
        TEMPERATURE /= 16;

        Delay_Temp(1);
    }
}

void Config_DS18B20()  //重新配置报警限定值和分辨率
{
    Init_DS18B20();
    Write_Byte(0xcc);  // 0XCC, 跳过ROM，忽略64位ROM地址，直接发送指令(适用于一个从机工作)
    Write_Byte(0x4e);  // 0X4E, 写暂存器。在内部RAM中的2,3字节写入上、下限温度数据
    Write_Byte(0x19);  //上限
    Write_Byte(0x1a);  //下限
    Write_Byte(0x7f);  //set 12 bit (0.125)

    Init_DS18B20();
    Write_Byte(0xcc);  //skip rom
    Write_Byte(0x48);  // 0X48，复制暂存器。将RAM中的2,3字节TH,TL的内容复制到EEPROOM中的TH,TL中以保存设定置

    Init_DS18B20();
    Write_Byte(0xcc);  //skip rom
    Write_Byte(0xb8);  // 0XB8，重调EEPROM，将EEPROM中的TH,TL的内容到RAM的TH,TL中
}

void Read_ID() //读取器件 id，需要PTR提前设置位ID缓存
{
    Init_DS18B20();
    Write_Byte(0x33);  // 0X33,read rom.读DS18B20 ROM中的编码(64位地址)
    Read_Str(8);     // 连续读8个字节，共64位
}


void Temperatuer_Result() // DS18B20 温度处理:读ID，配置温度计，转换温度，读取温度至温度缓存
{
    PTR = ID_BUFF;     // 配置ID缓存指针
    Read_ID();       // 读取ID至ID缓存

    Config_DS18B20();// 设置温度器配置

    Init_DS18B20();
    Write_Byte(0xcc);   //skip rom
    Write_Byte(0x44);   //0X44, 温度转换指令， 启动DS18B20进行温度转换,数据存入内部9字节的RAM中

    Init_DS18B20();
    Write_Byte(0xcc);   //skip rom
    Write_Byte(0xbe);   //0XBE, 读暂存器

    PTR = TEMP_BUFF;  //设置温度缓存数组，存放将要读取的数据
    Get_Temp();
}


void GetTemp()  //每间隔一段时间，读取一次温度
{
    if(TIM == 100) //每隔 1000ms 读取温度
    {
        TIM = 0;
        Temperatuer_Result();
    }

}

void Delay_Temp(unsigned char idata xus)
{
    while(xus--);
}
