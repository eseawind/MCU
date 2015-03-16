


#ifndef _CJMUC_H_
#define _CJMCU_H_


/***********************************************************************************************
 *                                           CJMCU
 * CJMCU_TX(CJMCU) ---> P3.7(RX)
 * RX(CJMCU) ---> P3.6(TX)
 * DTR       ---> NULL
 * VCC       ---> 3.3V/5V
 * GND       ---> 0
 ***********************************************************************************************/

//CJMCU IO配置
#define UARTA0_RX   (P3SEL |= BIT7, P3DIR &= ~BIT7)
#define UARTA0_TX   (P3SEL |= BIT6, P3DIR |=  BIT6)

//CJMCU BUF length
#define CJMCU_BUF_LEN     25       //接收缓存的大小

//传感器校准次数, 取CJMCU_ALINE_NUM次数据取平均值作为静止偏差
#define CJMCU_ALINE_NUM   10

#define CJMCU_BAUDRATE      ((long)CPU_F / 57600)

/*****************CJMCU配置***********************/
//连续输出选择
#define  CJMCU_E_CON        ("#o1")    //允许连续数据模式
#define  CJMCU_D_CON        ("#o0")    //禁止连续数据模式(CJMCU停止发送数据,当允许连续数据模式时将继续发送数据)

//输出数据格式
#define  CJMCU_F_BIN        ("#ob")    //二进制
#define  CJMCU_F_TXT        ("#ot")    //字符串

//错误信息输出
#define  CJMCU_E_ERR        ("#oe1")   //允许输出错误信息
#define  CJMCU_D_ERR        ("#oe0")   //禁止输出错误信息

//传感器校正
#define  CJMCU_MODE_CHECK   ("#oc")    //进入校正模式
#define  CJMCU_CHECK_NEXT   ("#on")    //进入下一个传感器校正
#define  CJMCU_CHECK_TXT    ("#osct")  //输出校正后的数据(文本)
#define  CJMCU_ORIGN_TXT    ("#osrt")  //输出原始数据(文本)
#define  CJMCU_CHOR_TXT     ("#osbt")  //原始数据 + 校正后的数据(文本)
#define  CJMCU_CHECK_BIN    ("#oscb")  //输出校正后的数据(二进制)
#define  CJMCU_ORIGN_BIN    ("#osrb")  //输出原始数据(二进制)
#define  CJMCU_CHOR_BIN     ("#osbb")  //原始数据 + 校正后的数据(二进制)


/******************IO函数定义*********************/
//初始化UART通信
void CJMCU_INIT();

//DCO : 1MHZ 配置
void CJMCU_CLK_INIT();

//CJMCU配置
void CJMCU_CONFIG(const unsigned char *pdata);
//开启UART
void CJMCU_START();

//关闭UART
void CJMCU_STOP();

//获取姿态角
void CJMCU_GET_VALUE(int *x, int *y, int *z);

//中断接收数据
void USCIAB0RX_Interrupt();

//UART发送数据
void CJMCU_SEND(const unsigned char data);

//使九轴姿态传感器静止, 读取数据十次, 取平均值作为静止偏差
void CJMCU_Aline();


/*********************    全局变量   ****************************/

extern int CJMCU_YAW_OFFSET, CJMCU_PITCH_OFFSET, CJMCU_ROLL_OFFSET;
extern unsigned char CJMCU_RCV_FLAG;                       //读取数据标志


#endif
