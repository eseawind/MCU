/***************************************************
 *         Author: Shawn Guo                       *
 *         Date  : 2013/2/13                       *
 *         Last  : 2013/2/13                       *
 *         Notes : I2C总线驱动                     *
 **************************************************/



void Start_I2C();  //启动I2C总线
void Stop_I2C();   //停止I2C总线
void  Send_Byte(unsigned char  c); //发送一个字节的数据c至I2C总线
unsigned char   Receive_Byte();    // 接受一个字节的数据，并返回
void Answer_I2C(bit flag);   // flag = 0, 应答信号； flag=1,非应答信号


//--------------------AT24C02 EEPROM驱动

/*******************************************************************
                    向有子地址器件发送多字节数据函数
函数原型: bit Send_Str(unsigned char sla, unsigned char suba, unsigned char *s, unsigned char no);
功能:     从启动总线到发送地址，子地址,数据，结束总线的全过程
          从器件地址sla，子地址suba，发送内容是s指向的内容，发送no个字节。
返回：     如果返回1表示操作成功，否则操作有误。
注意：    使用前必须已结束总线。
********************************************************************/
bit Send_Str(unsigned char sla, unsigned char suba, unsigned char *s, unsigned char no);



/*******************************************************************
                    向有子地址器件读取多字节数据函数
函数原型: bit Receive_Str(unsigned char sla,unsigned char suba,unsigned char *s,unsigned char no);
功能:     从启动总线到发送地址，子地址,读数据，结束总线的全过程,从器件
          地址sla，子地址suba，读出的内容放入s指向的存储区，读no个字节。
           如果返回1表示操作成功，否则操作有误。
注意：    使用前必须已结束总线。
********************************************************************/


bit Receive_Str(unsigned char sla,unsigned char suba,unsigned char *s,unsigned char no);
