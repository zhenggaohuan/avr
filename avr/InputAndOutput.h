/*
 * InputAndOutput.h
 *
 * Created: 2015-08-24 16:31:11
 *  Author: ZhengGaoHuan
 */ 


#ifndef INPUTANDOUTPUT_H_
#define INPUTANDOUTPUT_H_



/*
*显示输出定义的字符码
*/
#define IDM_NULL 0
#define IDM_Code 1
#define IDM_Set  2
#define IDM_Dis  3
#define IDM_Conn 4
#define IDM_CodePut  5
#define IDM_CodeSet  6
#define IDM_SetNet  7
#define IDM_SetPT_U 8
#define IDM_SetCT_I 9
#define IDM_SetE_CLE 10
#define IDM_DisDIS_E 11
#define IDM_DisDIS_P 12
#define IDM_DisB_LED 13
#define IDM_ConnAdd   14
#define IDM_ConnData  15
#define IDM_ConnBud   16
#define IDM_CLE 17
#define IDM_CHE 18
#define IDM_YES 19
#define IDM_NO 20
#define IDM_SAVE 21
#define IDM_ERR 22


#define IDD_NULL 0xffff
#define IDD_M 0x00ff
#define  IDD_DI 0x017f
#define IDD_W 0x01bf
#define IDD_A 0x01df
#define IDD_TX 0x01ef
#define IDD_V 0x01f7
#define IDD_FXD 0x01fb
#define IDD_K 0x01fd
#define IDD_FXU 0x01fe

/*
*键盘扫描定义的按键消息码
*/
#define ID_KEYMSG_Null 0x00
#define ID_KEYMSG_LeftDown 0xf7
#define ID_KEYMSG_RightDown 0xfB
#define ID_KEYMSG_MenuDown 0xfD
#define ID_KEYMSG_EnterDown 0xfE


/*
*初始化
*/
//显示初始化
void fnDisplayInit();



/*
*显示
*/
//数码管扫描显示
void fnScanLED();
//数码管闪烁显示
unsigned char fnDisplay(void);



/*
*显示内容
*/
//显示字符
void fnOutputChar(unsigned char byDisplayRow, char * byChar );
//显示数字
void fnOutputNum(unsigned char byDisplayRow,float iNum);
//显示数字字符
void fnOutputNumChar(unsigned char byDisplayRow, unsigned char * pbyValue);
//显示指示灯
void fnOutputIndicator(unsigned int uValue);

/*
*显示亮度
*/
//设置显示亮度
void fnSetDisplayLightness(unsigned char byDisplayLightness);
//发光二极管的显示亮度和发光二极管的显示时间有光，显示时间越长，发光二极管就越亮，因此显示延时时间的长度决定了发光二极管的实现亮度
void fnDelayDisplay(unsigned char byDelayTime);



/*
*显示时间
*/
//复位当前显示时间，将当前显示时间设定为最大值
void fnResetDisplayTime();
//获取当前显示时间
unsigned char fnGetDisplayTime();
//开启一直显示功能，使能显示标志位，显示标志位置1，数码管一直显示
void fnEnableDisplay();
//关闭一直显示功能，失能显示标志位，显示标志位置0，数码管显示60S
void fnDisableDisplay();



/*
*闪烁
*/
//关闭闪烁功能，闪烁标志位置0
void fnDisableFlash();
//开启闪烁功能，闪烁标志位置1
void fnSetFlash(unsigned char byFlashDigit);



/*
*按键输入
*/
//按键初始化
void fnKeyInit(void);
//获取按键消息码：扫描按键情况，并生成8位扫描码（低4位有效），多个（数量依具体情况设定）相同的扫描码生成一个8位按键码（低4位有效），同时生成一个8位按键消息码，按键消息码位的低4为当前按键码，高4位为前次按键码
unsigned char fnGetKeyCode(void);
//获取系统消息：函数会根据显示情况，对取得的按键消息进行处理，如果屏幕不显示，则激活屏幕，如果屏幕显示，则把按键消息转换成系统消息，并返回系统消息。
unsigned char fnkey(void);

#endif /* INPUTANDOUTPUT_H_ */