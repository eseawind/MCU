/**********************************************
 *
 * 		Author	:		Shawn Guo
 *      Mail    :       iguoxiaopeng@gmail.com
 * 		Date	:		2013/5/26
 *      Last    :       2013/5/26
 * 		Notes	:       利用多路开关CD4051实现的程控增益放大器
 * 		Tool    :	    MSP430G2553
 *
 *
 *      cd4051:         地址CBA
 *      0dB    ---->        000
 *      10dB   ---->        001
 *      20dB   ---->        010
 *      30dB   ---->        011
 *      40dB   ---->        101
 *      50dB   ---->        111
 *
 *********************************************
 *      按键A(P2.2)端口控制放大倍数的步进
 *      P1.3   ---->    C
 *      P1.4   ---->    B
 *      P1.5   ---->    A
 *
 **********************************************/
#include <msp430g2553.h>

// ===============================精确延时宏定义=========================
#define CPU_F ((double)1000000)        // 这里的CPU_F需要自己配置，这里配置为1MHZ
#define DELAY_US(x) __delay_cycles((long)(CPU_F*(double)x/1000000.0))
#define DELAY_MS(x) __delay_cycles((long)(CPU_F*(double)x/1000.0))

#define CD4051_A1   (P1OUT |= BIT3)
#define CD4051_A0   (P1OUT &= ~BIT3)
#define CD4051_B1   (P1OUT |= BIT4)
#define CD4051_B0   (P1OUT &= ~BIT4)
#define CD4051_C1   (P1OUT |= BIT5)
#define CD4051_C0   (P1OUT &= ~BIT5)

void Config_Clocks();
void Config_Port();
void CD4051_Change(); //根据CD4051_FLAG的值配置多路开关的闭合
void CD4051_Init();        //多路开关的初始化
unsigned char CD4051_FLAG = 0;

void main()
{
    WDTCTL = WDTPW + WDTHOLD; // Stop watchdog timer

    Config_Clocks();
    Config_Port();
    CD4051_Init();
    _EINT();
    while(1);
}

void CD4051_Change() //根据flag的值配置多路开关的闭合
{
    switch(CD4051_FLAG)//切换到下一通道
    {
    case 0:
        CD4051_FLAG = 1;
        break;
    case 1:
        CD4051_FLAG = 2;
        break;
    case 2:
        CD4051_FLAG = 3;
        break;
    case 3:
        CD4051_FLAG = 5;
        break;
    case 4:
        CD4051_FLAG = 0;
        break;
    case 5:
        CD4051_FLAG = 7;
        break;
    case 6:
        CD4051_FLAG = 0;
        break;
    case 7:
        CD4051_FLAG = 0;
        break;
    }

    switch(CD4051_FLAG) //根据状态切换制定通道
    {
    case 0:
        CD4051_C0;
        CD4051_B0;
        CD4051_A0;
        break;
    case 1:
        CD4051_C0;
        CD4051_B0;
        CD4051_A1;
        break;
    case 2:
        CD4051_C0;
        CD4051_B1;
        CD4051_A0;
        break;
    case 3:
        CD4051_C0;
        CD4051_B1;
        CD4051_A1;
        break;
    case 4:
        CD4051_C1;
        CD4051_B0;
        CD4051_A0;
        break;
    case 5:
        CD4051_C1;
        CD4051_B0;
        CD4051_A1;
        break;
    case 6:
        CD4051_C1;
        CD4051_B1;
        CD4051_A0;
        break;
    case 7:
        CD4051_C1;
        CD4051_B1;
        CD4051_A1;
        break;
    }
}

void CD4051_Init()        //多路开关的初始化
{
    // IO初始化
    P1DIR |= (BIT3 + BIT4 + BIT5);
    P1OUT &= ~(BIT3 + BIT4 + BIT5);     //默认0通道
}

#pragma vector=PORT2_VECTOR
__interrupt void P2_ISR (void)
{
    if(P2IFG & BIT2)
    {
        CD4051_Change();
        DELAY_MS(100);
    }

    P2IFG = 0;
}

void Config_Port()   // Set Interrupt IO
{
    P2DIR &= ~BIT2; // 按键A(P2.2)端口控制放大倍数的步进
    P2IES &= ~BIT2;
    P2IE |= BIT2;
    P2REN |= BIT2;
    P2OUT &= ~BIT2;
}

void Config_Clocks()
{
    BCSCTL1 = CALBC1_1MHZ; // Set range
    DCOCTL = CALDCO_1MHZ; // Set DCO step + modulation
    BCSCTL3 |= LFXT1S_2;
    IFG1 &= ~OFIFG;
    BCSCTL2 |= SELM_0 + DIVM_0 + DIVS_0; // SMCLK = DCO
}
