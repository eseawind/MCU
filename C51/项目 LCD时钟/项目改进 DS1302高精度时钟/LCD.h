/********************************************************
 *         Author: Shawn Guo                            *
 *         Date  : 2013/2/15                            *
 *         Last  : 2013/2/15                            *
 *         Project:   LCD时钟                           *
 *         Notes :  LCD驱动                             *
 ********************************************************/

bit LCD_IsBusy();   // 检测LCD是否繁忙
void LCD_W_Cmd(unsigned char cmd);// 写入指令数据到LCD
void LCD_Pos(unsigned char pos); //设定LCD数据指针
void LCD_W_Data(unsigned char dat);  //写入字符显示数据到LCD
void LCD_Display();        // 显示数据
void LCD_Init();    //LCD初始化设定
void LCD_Set(unsigned char pos, unsigned char dat);
void LCD_Set_Week(unsigned char week);  // 在固定位置设置星期 // 在固定位置显示数据,数据必须为0~99之间
void LCD_Op1();     // 功能键操作
void LCD_Op2();     // 增加键操作
void LCD_Op3();     // 闹铃键操作
void LCD_Op5();     // 返回键操作
void LCD_Op6();     // 减少键操作
