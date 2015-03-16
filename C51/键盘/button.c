/***************************************************
 *         Author: Shawn Guo                       *
 *         Date  : 2013/2/12                       *
 *         Last  : 2013/2/12                       *
 *         Notes : Button                          *
 *                 P0 | JP1   P2 | JP5             *
 **************************************************/

#include <reg52.h>

// button
sbit K1 = P2^7;
sbit K2 = P2^6;
sbit K3 = P2^5;
sbit K4 = P2^4;
sbit K5 = P2^3;
sbit K6 = P2^2;
sbit K7 = P2^1;
sbit K8 = P2^0;

void Delayms(unsigned int xms);
void Button_Test();

int main()
{
    while(1)
    {
        if(P2 != 0xff)    // some button was down
        {
            Button_Test();
            Delayms(100);
        }
        P0 = 0xff;
    }

    return 0;
}

void Delayms(unsigned int xms)
{
    unsigned char i;

    while(--xms){
        for(i = 0; i < 110; i++)
            ;
    }
}

void Button_Test()
{
    if(~K1){
        P0 = 0xfe;
    }

    if(~K2) P0 = 0xfd;
    if(!K3) P0 = 0xfb;
    if(~K4) P0 = 0xf7;
    if(~K5) P0 = 0xef;
    if(!K6) P0 = 0XDF;
    if(!K7) P0 = 0XBF;
    if(!K8) P0 = 0X7F;
}
