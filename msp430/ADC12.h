/**********************************************
*
* 		Author	:		Shawn Guo
*       Mail    :       iguoxiaopeng@gmail.com
* 		Date	:       2013/8/10
*       Last    :
* 		Notes	:       ADC12
* 		Tool    :       ADC12 for MSP430X2XX
**********************************************/
#ifndef __ADC12_H__
#define __ADC12_H__

#define VREF    1500         //配置ADC12的参考电压VR+, 放大1000倍，即单位mv.
#define VREF_   0           //配置ADC12的参考电压VR-
#define VREF_DIFF   (VREF - VREF_)

//将ADC的采样值转换为电压值, 单位mv. 使用条件: 2.5V 0V的参考电压，精度高
#define ADC12_Result_2_5(NADC) (((((NADC) * 10) >> 6) * 39) / 10)   //  Vin = (nadc * 10 / 64) * ((2500 - 0) / 64);
#define ADC12_Result_1_5(NADC)  ((NADC * 15 / 41))          // VREF+ = 1.5, VREF- = AVSS时的高精度算法

#define ADC12_ON        (ADC12CTL0 |= ADC12ON)      // ADC12 on
#define ADC12_OVIE      (ADC12CTL0 |= ADC12OVIE)    //ADC12MEMx overflow-interrupt.
#define ADC12_TOVIE     (ADC12CTL0 |= ADC12TOIE)    //ADC12 Conversion-time-overflow
#define ADC12_ENC       (ADC12CTL0 |= ENC)          //Enable Conversion
#define ADC12_SC        (ADC12CTL0 |= ADC12SC)      //Start Conversion.
#define ADC12_INVERT_SH (ADC12CTL0 |= ISSH)     //invert signal sample-and-hold
#define ADC12_ISBUSY    (ADC12CTL1 & ADC12BUSY)

/*********ADC12使用指南*******************
    ADC12_Init(3,1,0,3);            //初始化,选择时钟,分频,模式,采样保持时间
    ADC12_Sample_Source(0, 1);      //选择采样模块的时钟来源, 采样方式(脉冲采样shp = 1, 拓展采样0)

    ADC12_Set_Address(1);           //采样值存放的首地址.
    ADC12_MCTL(15, 0, 1);           //内存单元配置, 在此单元配置后的采样值不一定在此单元存放.
    //没有内存单元配置，则默认通道A0采样存放地址0.
    //没有地址设置,则默认存放在地址0

    ADC12_Enable((BIT0));           //内存单元中断,存放值以后是否中断.
    ADC12_ENC;

    P6DIR &= BIT1;                            // 开启IO的AD采样功能
    P6SEL |= BIT1;                            //
******************************************/




// 选择时钟, 分频, 方式, 采样保持时间
void ADC12_Init(unsigned char clk, unsigned char div, unsigned char mode, unsigned char shtime); // ADC初始化
void ADC12_Sample_Source(unsigned char shs, unsigned char shp);   // Sample-and-hold source select
//选择内存单元ADC12MCTL0~ADC12MCTL15,选择参考，选择通道, 需另行配置ADC12_Enable(),ENC以及配置必要的IO SEL.
void ADC12_MCTL(unsigned char mctl, unsigned char svref, unsigned char channel);
/***************************************************************************//**
 * @brief  Initialize the ADC sysyem - configure adc clock source, clock divider,
 *          sample signal channel, sample mode, sample and hold time , and set the VREF;
 * @param
 *      parameter
 *-----------------------------------------------------------------------------
 *      clk  =  0       ADC12OSC
 *              1       ACLK
 *              2       MCLK
 *              3       SMCLK
 *-----------------------------------------------------------------------------
 *      div  =  1       /1
 *              2       /2
 *              3       /3
 *              4       /4
 *              5       /5
 *              6       /6
 *              7       /7
 *              8       /8
 *-----------------------------------------------------------------------------
 *      channel 0       A0
 *              1       A1
 *              2       A2
 *              x       Ax
 *              7       A7
 *              8       VeREF+
 *              9       VREF-/VeREF-
 *              10      Temperature
 *              11      (AVcc - AVss) / 2
 *              12      GND
 *-----------------------------------------------------------------------------
 *      shtime= 1       4 ADCCLK cycles
 *              2       8
 *              3       16
 *              4       32
 *              5       64
 *              6       96
 *              7       128
 *              8       192
 *              9       256
 *              10      384
 *              11      512
 *              12      768
 *              13      1024
 *-----------------------------------------------------------------------------
 *      mode =  0       single-channel, single-conversion
 *              1       sequence-of-channels
 *              2       Repeat-single-channel
 *              3       Repeat-sequence-of-channels
 *-----------------------------------------------------------------------------
 *      svref = 0       VR+ = AVCC and VR- = AVSS
 *              1       VR+ = VREF+ and VR- = AVSS
 *              2       VR+ = VeREF+ and VR- = AVSS
 *              3       VR+ = VeREF+ and VR- = AVSS
 *              4       VR+ = AVCC and VR- = VREF-/ VeREF-
 *              5       VR+ = VREF+ and VR- = VREF-/ VeREF-
 *              6       VR+ = VeREF+ and VR- = VREF-/ VeREF-
 *              7       VR+ = VeREF+ and VR- = VREF-/ VeREFINCHx
 *-----------------------------------------------------------------------------
 *      shs =   0       ADC12SC bit
 *              1       Timer_A.OUT1
 *              2       Timer_B.OUT0
 *              3       Timer_B.OUT1
 *-----------------------------------------------------------------------------
 *      shp =   0       采样信号    控制SAMPCON信号
 *              1       采样定时器  控制SAMPCON信号
 *-----------------------------------------------------------------------------
 *-----------------------------------------------------------------------------
 *-----------------------------------------------------------------------------
 *-----------------------------------------------------------------------------
 * @return none
 ******************************************************************************/
void ADC12_Set_VREF2_5(); // 配置ADC Reference generator voltage = 2.5V
void ADC12_Set_VREF1_5(); // 配置ADC REF = 1.5V
// address 0~0FH
void ADC12_Set_Address(unsigned char address);  // 配置ADC Conversion Start address. 用于序列采样情况
void ADC12_Set_Mode(unsigned char mode);
void ADC12_Set_CLK(unsigned char clk, unsigned char div);
//使能ADC12中15个中断源的某个中断
void ADC12_Enable(unsigned int adc12ie);
unsigned int ADC12_T(unsigned int vtemp);  //输入ADC12温度传感器采样出来的毫伏级电压，转化为温度的100倍显示出来.
/*************ADC12中断模板************************

#pragma vector=ADC12_VECTOR
__interrupt void ADC12_ISR (void)
{
    switch(ADC12IV)
    {
    case 2:         //ADC12MEMx overflow
        break;
    case 4:         //Conversion time overflow
        break;
    case 6:         //ADC12MEM0 interrupt flag
        break;
    case 8:         //ADC12MEM1
        break;
    case 10:        //ADC12MEM2
        break;
    case 12:        //ADC12MEM3
        break;
    case 14:        //ADC12MEM4
        break;
    case 16:        //ADC12MEM5
        break;
    case 18:        //ADC12MEM6
        break;
    case 20:        //ADC12MEM7
        break;
    case 22:        //ADC12MEM8
        break;
    case 24:        //ADC12MEM9
        break;
    case 26:        //ADC12MEM10
        break;
    case 28:        //ADC12MEM11
        break;
    case 30:        //ADC12MEM12
        break;
    case 32:        //ADC12MEM13
        break;
    case 34:        //ADC12MEM14
        break;
    case 36:        //ADC12MEM15
        break;
    }
}

**************************************************/
#endif /* __ADC12_H__ */
