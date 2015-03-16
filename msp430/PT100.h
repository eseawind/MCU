/**********************************************
 *
 * 		Author	:		Shawn Guo
 *      Mail    :       iguoxiaopeng@gmail.com
 * 		Date	:       2013/8/9
 *      Last    :
 * 		Notes	:       PT100温度传感器
 * 		Tool    :       MSP430FX2XX PT100
 *
 *      pt100   ---> 惠更斯电桥  --->    差分放大器   --->    AD采样(A6 P6.6)
 *      ---->   计算温度值   --->    LCD显示
 *         _____________________________V-
 *         |
 *         |        VDD
 *         |         |
 *         |        / \
 *         |       /   \                    差分放大    ---> AD
 *         |      /     \
 *         |     R1     R2
 *         |_____/       \ _____________V+
 *               \       /
 *                R3    R4
 *                 \   /
 *                  \ /
 *                   |
 *                  GND
 *
 *      计算公式：R1 = R2 = 1K, R3 = 100欧, R4 = pt100.
 *
 *      R3的电位固定为U3 = VDD / (R1 + R3) * R3.
 *      则R4的电位为 U3 + UT; UT为U3和U4的电位差.
 *      R4 = (U4) / I4;
 *      I4 = (VDD - U4) / R2;
 **********************************************/
#ifndef __PT100_H__
#define __PT100_H__

#define PT100_VDD           3300         //惠更斯电桥的供电电压,放大1000倍, 3.3V
#define PT100_VADC_TIME     10      //差分放大电路的放大倍数
#define PT100_R1            1000        //R1 = 1K
#define PT100_R2            1000        //R2 = 1K
#define PT100_R3            100         //R3 = 100

#define PT100_U3                (PT100_VDD / (PT100_R1 + PT100_R3) * PT100_R3)
#define PT100_U4(VADC)            (PT100_U3 + ((VADC) / (PT100_VADC_TIME)))

#define PT100_MIN_TEMP      10000
#define PT100_MAX_TEMP      13884   //配置PT100的测温范围为0~100度
#define PT100_ROM_NUM       1024     //PT100_ROM表的大小

extern const unsigned int PT100_ROM[PT100_ROM_NUM];

/***************************************************************************//**
 * @brief   根据ADC的采样值计算温度.
 * @param   vadc, ADC采样后换算的电压差值, 单位: mv
 * @return  温度的100倍
 *******************************************************************************/
unsigned int PT100_Temp(unsigned int vadc);



#endif /* __PT100_H__ */
