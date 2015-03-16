/**********************************************
 *
 * 		Author	:		Shawn Guo
 *      Mail    :       iguoxiaopeng@gmail.com
 * 		Date	:		2013/7/17
 *      Last    :       2013/7/17
 * 		Notes	:       DDS
 * 		Tool    :	    MSP430G2553 + TLV5618
 *
 *
 *      DDS
 *
 *      TLV5618 OUTB通道输出波形数据
 *
 *      键盘A控制步进频率，步进量100HZ, 100HZ~10khz
 **********************************************/
#include <msp430g2553.h>
#include "DAC.h"
#include "data.h"


// ===============================精确延时宏定义=========================
#define CPU_F ((double)16000000)        // 这里的CPU_F需要自己配置，这里配置为1MHZ
#define DELAY_US(x) __delay_cycles((long)(CPU_F*(double)x/1000000.0))
#define DELAY_MS(x) __delay_cycles((long)(CPU_F*(double)x/1000.0))


void Config_Port(); //设置IO中断端口
void Config_Timer1(); //设置定时器1
void Config_Clocks(); //时钟配置

//------------------------------DDS全局变量------------------------
int STEP_K = 2 * 2;         // DDS步进量
const int SSTEP_K = 4;        // 步进量的固定步进量，这里取4代表频率步进100HZ
const int NUM = 2 * 4096;       // 波形ROM数组， 4096个数据，分为高低位，ROM的大小决定DAC的输出精度；
int TIME_NUM = 76;          //定时器溢出周期数，表示每次给DAC发送数据的间隔，定时过长会影响DDS的最高输出频率
extern const unsigned char WAVE_ROM[];  //波形数据，存放在data.h文件里
unsigned int STEP = 0;


// 16M时钟，TIME_NUM = 96, STEP_K = 4, F = 80HZ
// 16M时钟，TIME_NUM = 96, STEP_K = 40, F = 800HZ
// 16M时钟，TIME_NUM = 76, STEP_K = 4, F = 100HZ
// 16M时钟，TIME_NUM = 76, STEP_K = 40, F = 100HZ
void main()
{
    // Stop watchdog timer to prevent time out reset
    WDTCTL = WDTPW + WDTHOLD;
    Config_Clocks();
    Config_Port();

    //定时器0配置;用作DDS，定时中断来给DAC发送数据
    TACTL |= TASSEL_2 + ID_0 + MC_1 + TACLR;
    CCTL0 = CCIE;
    TACCR0 = TIME_NUM - 1;

    //DAC初始化配置
    DAC_Init();

    //开启全局中断允许
    _EINT();

    //数据放在定时中断中发送
    while(1);
}


#pragma vector=TIMER0_A0_VECTOR
__interrupt void TIMER0_A0(void)
{
    //定时器CCR0中断，用法给DAC发送数据
    DAC_CE_0;
    //IFG2 &= ~UCA0RXIFG;
    UCA0TXBUF = WAVE_ROM[STEP];
    // while(!(IFG2 & UCA0TXIFG));
    UCA0TXBUF = WAVE_ROM[STEP + 1];
    STEP += STEP_K;                //模拟DDS加法器，每次递增一个步进量
    if(STEP >= NUM) // 溢出回零
        STEP = 0;
    //while(!(IFG2 & UCA0RXIFG));    LCD_Point(17 + POINT_NUM, POINT_NUM++);
    DAC_CE_1; //拉高使能信号，使得转移寄存器中的数据移入目标锁存
}


void Config_Clocks()
{
    BCSCTL1 = CALBC1_16MHZ; // Set range
    DCOCTL = CALDCO_16MHZ; // Set DCO step + modulation
    BCSCTL3 |= LFXT1S_2;
// BCSCTL2, Basic Clock System Control Register 2

    // SELMx , Select MCLK. These bits select the MCLK source.
    // 00 DCOCLK    01 DCOCLK  11 LFXT1CLK or VLOCLK
    // 10 XT2CLK when XT2 oscillator present on-chip.
    //LFXT1CLK or VLOCLK when XT2 oscillator not presenton-chip.

    //SELS , Select SMCLK. This bit selects the SMCLK source.
    // 0 DCOCLK
    //1 XT2CLK when XT2 oscillator present.
    //LFXT1CLK or VLOCLK when XT2 oscillator not present

    //DIVMx , Divider for MCLK
    // 00 /1	01 /2	10 /4	11 /8
    //DIVSx , Divider for SMCLK
    //00 /1		01 /2	10 /4	11 /8

    //DCOR , DCO resistor select. Not available in all devices. See the device-specific data sheet.
    //0 Internal resistor
    //1 External resistor

    //    MCLK=DC0
    BCSCTL2 |= SELM_0 + DIVM_0 + DIVS_0; // SMCLK = DCO
}


void Config_Port() //设置IO中断端口，键盘控制
{
    P2DIR &= ~BIT2; //  P2.2设为输入
    P2IES &= ~BIT2; //  P2.2中断触发沿：上升沿；
    P2IE |= BIT2; //    开启P2.2 I/O中断
    P2REN |= BIT2;
    P2OUT &= ~BIT2;
}


//  I/O中断程序，键盘控制响应事件的发生
#pragma vector=PORT2_VECTOR
__interrupt void P2_ISR (void)
{
    if(P2IFG & BIT2) //键盘控制频率步进量
    {
        STEP_K += SSTEP_K;
        STEP = 0;
        DELAY_MS(100);
    }
    P2IFG = 0;         // P1端口的中断标志位清零，等待下一次中断
}



