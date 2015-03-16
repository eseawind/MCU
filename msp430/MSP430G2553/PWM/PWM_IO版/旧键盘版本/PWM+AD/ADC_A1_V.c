/**********************************************
 *
 * 		Author	:		Shawn Guo
 * 		Date	:		2013/4/16
 *      Last    :       2013/4/27
 * 		Notes	:		ADC10, Sample A1 and display LCD5110
 * 		Tool    :	    MSP430G2553s
 *
 *
 *
 **********************************************/
#include "msp430g2553.h"
#include "LCD5110.h"

#define ADC_VREF1   3.545
#define ADC_VREF2   0
#define V_OFF_SET 40    //AD杈撳叆涓