/**********************************************
 *
 *      Author  :       Shawn Guo
 *      Mail    :       iguoxiaopeng@gmail.com
 *      Date    :       2013/8/3
 *      Last    :
 *      Notes   :       DAC12
 *      Tool    :       MSP430X2XX
 **********************************************/
#ifndef __DAC12_H__
#define __DAC12_H__

#define VOLTAGE_REF 250    // 宏定义参考电压(放大100倍)

/***************************************************************************//**
 * @brief   Initialize the DAC12 sysyem - configure ...
 * @param
 *------------------------------------------------------------------------------
 *          ops =   0,      DAC12_0 ---> P6.6   DAC12_1 ---> P6.7
 *                  1,      DAC12_0 ---> VeREF+ DAC12_1 ---> P6.5
 *------------------------------------------------------------------------------
 *          vref=   0,      reference voltage = VREF+
 *                  2,      reference voltage = VeREF+
 *------------------------------------------------------------------------------
 *          dac =   0,      Configure DAC12_0CTL
 *                  1,                DAC12_1CTL
 *------------------------------------------------------------------------------
 *          bits=   0,      Set 12-bit resolution
 *                  1,           8-bit
 *------------------------------------------------------------------------------
 *          lsel =  0,      写入DAC12_xDAT的数据直接载入(无视DAC12ENC)
 *                  1,                           直接载入
 *                  2,    Rising edge of Timer_A.OUT1
 *                  3,    Rising edge of Timer_B.OUT2
 *------------------------------------------------------------------------------
 *          range=  0,     full-scale output = 3x vref.
 *                  1,                         1x vref.
 *------------------------------------------------------------------------------
 *          amp =   0,      Input Buffer(off),  DAC12(off), output(high Z);
 *                  1,          off,            off,            0V
 *                  2,      Low speed           Output Buffer(Low speed)
 *                  3,      low                 medium
 *                  4,      low                 high
 *                  5,      medium              medium
 *                  6,      medium              high
 *                  7,      high                high
 *------------------------------------------------------------------------------
 *          df =    0,      data format (straigh binary)
 *                  1,                  2s complement
 *------------------------------------------------------------------------------
 *          group = 0,      Not group.
 *                  1,      Group.
 *------------------------------------------------------------------------------
 * @return none
 *******************************************************************************/
// DAC12_Init()函数采用直接配置.DAC12_V2Int_121(unsigned int v100);每次使用前需要配置
void DAC12_Init();

void DAC12_Sent_A(unsigned int dat);
void DAC12_Sent_B(unsigned int dat);

//Basic Operator
void DAC12_SetOutput(unsigned char ops);
void DAC12_SetRef(unsigned char dac, unsigned char vref);
void DAC12_SetRes(unsigned char dac, unsigned char bits);
void DAC12_SetLsel(unsigned char dac, unsigned char lsel);
void DAC12_Calon(unsigned char dac);
void DAC12_SetIR(unsigned char dac, unsigned char range);
void DAC12_SetAMP(unsigned char dac, unsigned char amp);
void DAC12_SetDF(unsigned char dac, unsigned char df);
void DAC12_Enabled(unsigned char dac);
void DAC12_Disabled(unsigned char dac);
void DAC12_ENC(unsigned char dac);
void DAC12_Group(unsigned char group);

//输入参数v*100,返回电压v对应的DAC12 code, 电压范围 1x, 12-bit
unsigned int DAC12_V2Int_121(unsigned long int v100);
//输入参数v*100,返回电压v对应的DAC12 code, 电压范围 3x, 12-bit
unsigned int DAC12_V2Int_123(unsigned long int v100);
//输入参数v*100,返回电压v对应的DAC12 code, 电压范围 1x, 8-bit
unsigned int DAC12_V2Int_81(unsigned long int v100);
//输入参数v*100,返回电压v对应的DAC12 code, 电压范围 3x, 8-bit
unsigned int DAC12_V2Int_83(unsigned long int v100);

#endif /* __DAC12_H__ */
