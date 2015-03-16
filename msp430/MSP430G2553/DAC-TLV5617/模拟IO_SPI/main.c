/**********************************************
 *
 * 		Author	:		Shawn Guo
 * 		Date	:		2013/4/21
 *      Last    :       2013/4/21
 * 		Notes	:       DDS
 * 		Tool    :	    MSP430G2553
 *
 **********************************************/
#include <msp430g2553.h>
#include "DAC.h"

void Config_Clocks();

const int STEP_K = 100;       // DDS 姝ヨ繘閲