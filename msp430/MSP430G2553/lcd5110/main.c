/**********************************************
 *
 * 		Author	:		Shawn Guo
 * 		Date	:		2013/4/14
 *      Last    :       2013/4/14
 * 		Notes	:
 * 		Tool    :	MSP430G2553
 *
 **********************************************/
#include <msp430g2553.h>
#include<math.h>
#include <stdio.h>
#include "LCD5110.h"


//======================================================================
void Config_Clocks(); //配置时钟

void main()
{
    char a[15] = "abc19940225";

    WDTCTL = WDTPW + WDTHOLD; // Stop watchdog timer
    Config_Clocks(); //配置时钟

    LCD_Init(); //初始化液晶
    LCD_Write_String(0,0,a);
    while(1);
}


void Config_Clocks() //配置时钟
{
	BCSCTL1 = CALBC1_8MHZ; // Set range
	DCOCTL = CALDCO_8MHZ; // Set DCO step + modulation
	BCSCTL3 |= LFXT1S_0;
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

		//    MCLK=DC0       不分频
	BCSCTL2 |= SELM_0 + DIVM_0 + DIVS_0; // SMCLK = DCO
}
