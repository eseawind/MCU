/***************************************************
 *         Author: Shawn Guo                       *
 *         Date  : 2013/2/12                       *
 *         Last  : 2013/2/12                       *
 *         Notes :   flash LED                     *
 *                   P0 | JP1                      *
 **************************************************/

#include <reg52.h>

void Flash();
void Delayms(unsigned int xms);

int main()
{
    P0 = 0xfe;      // 1111 1110
    while(1)
    {
        Delayms(500);
        Flash();
    }

    return 0;
}

void Flash()
{
    P0 = (P0 << 1) | 1;   //左移一位，新位置高电平
    if(CY == 0)        //当唯一的低电平溢出至CY，则重新配置P0
        P0 = 0xfe;

}

void Delayms(unsigned int xms)
{
    unsigned char i;

    while(xms--){
        for(i = 0; i < 110; i++)
            ;
    }
}
