/*
* Main.c
*
* Created: 2014-07-13 08:12:59
*  Author: hppc
*/

/*
*该程序采用前后台调度框架进行编写
*前台程序为中断调用，根据中断置位任务执行标志，任务执行标志由定时中断、通讯中断等根据外界情况置位，
*后台程序为主函数循环执行，主函数执行的时候，根据任务执行标志执行相应任务，
*标志位的置位放置于前台中断程序，因为通讯标志位的置位在通许中断中置位，所以将所有标志位的置位都放于中断标志中，后台程序只复位标志位。
*/

#include <avr/io.h> //包含引脚定义头文件
#include <avr/interrupt.h>  //包含中断头文件


/*
*函数声明部分
*/

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

//显示定义的消息**************************************************
#define FlashTime 60
#define MaxDisplayLightness 120
#define MaxDisplayTime 60
#define ID_Max_GatherCount 50

#define ID_DISCODE_NULL 0xff


//显示数字的字码
static const unsigned char g_byaDisplayNumCode[] = {0x50, 0x5f, 0x38, 0x19, 0x17, 0x91, 0x90, 0x55, 0x10, 0x11};
static const unsigned char g_byaDisplayCode[] = {0x50, 0x5f, 0x38, 0x19, 0x17, 0x91, 0x90, 0x55, 0x10, 0x11, 0x14, 0x92, 0xf0, 0x1a, 0xb0, 0xb4, 0xd0, 0x16, 0xf6, 0x5b, 0x94, 0xf2, 0x54, 0x9e, 0x9a, 0x34, 0x15, 0xbe, 0x91, 0xb2, 0x52, 0xda, 0x12, 0x97, 0x13, 0x38, 0xff};
//显示亮度，分为6个等级，最大120，最小0。
static volatile unsigned char g_byDisplayLightness = 120;
//显示标志：为1时，数码管一直实现，为0时，数码管60S后不显示
static volatile unsigned char g_byDisplayEnable = 1;
//剩余显示时间
static volatile unsigned char g_byDisplayTime = 60;
//闪烁标志位，为1时，数码管相应位闪烁显示，为0时，数码管不闪烁
static volatile unsigned char g_byFlashEnable = 0;
//闪烁位
static volatile unsigned char g_byFlashDigit = 0;
//显示内容
static volatile unsigned char g_byaDisplayContents[3][4] = {{0xff,0xff,0xff,0xff},{0xff,0xff,0xff,0xff},{0xff,0xff,0xff,0xff}};
static volatile unsigned char g_byaIndicator[2] = {0xff,0xff};




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

//把四代表，个位，十位，百位，千位的数字数组组成一个数字
unsigned int fnComposeNum(char * pbyaNum);
//把1个是数字分解成4个代表，个位，十位，百位，千位的数字数组
void fnDecomposeNum(unsigned int uNum, unsigned char *pbyaNum);

//单片机初始化
void fnInit();
//通讯模块处理，用于处理modbus通讯信息，程序调用的时候是在通讯中断中接收到数据后置位通讯标志位
void fnComProcess(); 
//获取按键消息码：扫描按键情况，并生成8位扫描码（低4位有效），多个（数量依具体情况设定）相同的扫描码生成一个8位按键码（低4位有效），同时生成一个8位按键消息码，按键消息码位的低4为当前按键码，高4位为前次按键码
void fnkey();

/*全局变量声明*/
unsigned char byMessage = 0;  //键盘按键消息

//定义各种进程运行标志，前台中断程序根据各种情况置位进程运行标志，后台程序根据置位的进程运行标志运行进程，并复位进程运行标志
unsigned char byFlagHalfSecond = 0;  //闪烁变量的处理标志
unsigned char byFlagDisplay = 0; //显示处理标志
unsigned char byFlagKey = 0; //键盘轮询标志
unsigned char byFlagCom = 0; //通讯状态标志，如果通讯中断程序有数据接收到，则置位通讯状态标志，后台程序处理通讯数据，复位通讯状态标志

//函数定义部分
int main(void)
{
	//单片机初始化
	fnInit();

	//开始后台程序运行
	while(1)
	{
		if (byFlagDisplay)  //如果显示标志位置位
		{
			byFlagDisplay = 0;  //显示标志位复位
			fnDisplay();  //运行显示进程
		}
		if (byFlagKey) //如果键盘轮询标志置位
		{
			byFlagKey = 0; //键盘轮询标志复位
			fnkey(); //扫描键盘，取按键码，确定操作按键
		}
		if (byFlagCom) //如果系统通讯状态标志置位
		{
			byFlagCom = 0; //通讯状态标志位复位
			fnComProcess(); //处理通讯数据
		}
		
	}
}


//定时器T0溢出中断
ISR(TIMER0_OVF_vect)
{
	TCNT0 = 126;
	byFlagDisplay = 1; //显示进程置位
	byFlagKey = 1; //键盘轮询置位
}

//单片机初始化
void fnInit()
{
	//键盘初始化
	DDRD &= ~(1<<0);	//PD0的DDR置0，设置PD0输入
	PORTD |= (1<<0);	//PD0的PORT置1，设置PD0上拉电阻使能
	DDRD &= ~(1<<7);	//PD7的DDR置0，设置PD7输入
	PORTD |= (1<<7);	//PD7的PORT置1，设置PD7上拉电阻使能
	DDRG &= ~(1<<1);	//PG1的DDR置0，设置PG1输入
	PORTG |= (1<<1);	//PG1的PORT置1，设置PG1上拉电阻使能
	DDRC &= ~(1<<2);	//PC2的DDR置0，设置PC2输入
	PORTC |= (1<<2);	//PC2的PORT置1，设置PC2上拉电阻使能
	
	//显示初始化
	PORTA = 0xff;
	DDRA = 0xff;
	PORTC &= ~(1<<7);
	DDRC |= (1<<7);
	PORTG &= ~(1<<2);
	DDRG |= (1<<2);
	DDRB |= 1<<DDRB0;
	PORTB |= 1<<PORTB0;
	
	//定时器 计时器中断初始化T/C0
	TIMSK |= (1<<TOIE0);
	TCCR0 |= (1<<CS02)|(1<<CS01)|(1<<CS00);
	TCNT0 = 126; //设置计数器的值

	//开总中断 	
	asm("sei"); //这个是用汇编插入的方式开总中断 
}

void fnComProcess()
{
	
}

//获取按键消息
void fnkey()
{
	static unsigned char byKeyCode = 0x00;	//按键消息码，
	static int nGatherCount =0;	//采集按键闭合情况计数器
	static unsigned char byPreviousGather  = 0x00;	//前次按键闭合，生成的按键码
	unsigned char byCurrentGather;	//当前按钮闭合，生成的按键码

	//采集按键闭合情况，如果是单个按键闭合，则所产生的按键码是唯一的，如果两个或两个以上的按键闭合，则产生的按键码会与单个按键闭合产生的按键码不同，按键对应的按键码为，K4：00001110，K3：00001101，K2：00001011，K1：00000111，无按键：00001111，无效码：00000000
	byCurrentGather = (PIND&0x01)|(PING&0x02)|((PIND&0x80)>>5)|((PINC&0x04)<<1);	//取当前按键码，并把高4位置0
	if( ~(byCurrentGather|0xF0) || ~byKeyCode || ~(byPreviousGather|0xF0))	//如果有按键码或有按键命令码，执行如下程序，（就是说，当扫描到有按键按下，或命令码中还有按键命令，才执行程序。如果无按键按下并且命令码里面也没有按键命令，则表示单品机无任何输入）
	{
		nGatherCount = (byCurrentGather == byPreviousGather)?nGatherCount+1:1;		//如果当前按键码和前按键码相同，按键计数器加1；如果不同，则按键计数器复位为1
		byPreviousGather = byCurrentGather;		//把前面按键码保留起来，等到下个循环的时候在对比
		if(nGatherCount >= ID_Max_GatherCount)	   //如果计数器为7，表示已经采集到一个稳定的按键码，那么就把按键码加入到消息码 中，并发送消息。因为按键码只有4位有效，所以，要将消息码的低4位移到高4位，然后把按键码的低4位放入消息码的低4位，计数器要清0，表示重新开始采集计数；
		{
			byKeyCode <<= 4;    //要将消息码的低4位移到高4位
			byKeyCode |= byCurrentGather;    //把按键码的低4位放入消息码的低4位
			nGatherCount = 0;    //计数器要清0
			byMessage =  byKeyCode;    //发送消息码
		}
	}
}


//把4个代表，个位，十位，百位，千位的数字数组组成一个数字
unsigned int fnComposeNum(char * pbyaNum)
{
	unsigned int uNum = 0;
	for (int i=0; i<4; i++)
	{
		uNum *= 10;
		uNum += (pbyaNum[i] - 48);
	}
	return uNum;
}
//把1个是数字分解成4个代表，个位，十位，百位，千位的数字数组
void fnDecomposeNum(unsigned int uNum, unsigned char *pbyaNum)
{
	for (int i=3; i>=0; i--)
	{
		pbyaNum[i] = uNum%10;
		uNum /= 10;
	}
}


//***********************显示*****************************************************************************************
//数码管显示
unsigned char fnDisplay()
{
	static volatile unsigned int wFlashTime = 0;
	static volatile unsigned char byIsFlash = 0;
	unsigned char byFlashContent;
	
	if(!(g_byDisplayEnable||g_byDisplayTime))
	return 1;
	
	if(g_byFlashEnable)
	{
		wFlashTime++;
		wFlashTime %= FlashTime;
		if(!wFlashTime)
		{
			byIsFlash ++;
			byIsFlash %= 2;
		}
	}
	if(g_byFlashEnable&&byIsFlash)
	{
		byFlashContent = g_byaDisplayContents[2][g_byFlashDigit];
		g_byaDisplayContents[2][g_byFlashDigit] = 0xff;
	}
	fnScanLED();
	if(g_byFlashEnable&&byIsFlash)
	g_byaDisplayContents[2][g_byFlashDigit] = byFlashContent;
	
	return 1;
}

//数码管扫描显示
void fnScanLED()
{
	unsigned char byCnt;   //要显示的内容位置

	PORTA = 0xff;
	PORTG &= ~(1<<2);  	   //PG2 = 0
	PORTC &= ~(1<<7);	   //PC7 = 0
	PORTG  |= (1<<2);	   //PG2 = 1
	
	PORTC |= (1<<7);	   //PC7 = 1
	
	//显示第2行内容
	for(byCnt = 0;byCnt < 4;byCnt ++)
	{
		PORTG &= ~(1<<2);
		PORTA = g_byaDisplayContents[1][byCnt];
		fnDelayDisplay( g_byDisplayLightness);
		PORTA = 0xff;
		fnDelayDisplay( MaxDisplayLightness - g_byDisplayLightness);
		PORTG  |= (1<<2);
	}
	//显示第3行内容
	for(byCnt = 0;byCnt < 4;byCnt ++)
	{
		PORTG &= ~(1<<2);
		PORTA = g_byaDisplayContents[2][byCnt];
		fnDelayDisplay( g_byDisplayLightness);
		PORTA = 0xff;
		fnDelayDisplay( MaxDisplayLightness - g_byDisplayLightness);
		PORTG  |= (1<<2);
	}
	//显示第1行内容
	for(byCnt = 0;byCnt < 4;byCnt ++)
	{
		PORTG &= ~(1<<2);
		PORTA = g_byaDisplayContents[0][byCnt];
		fnDelayDisplay( g_byDisplayLightness);
		PORTA = 0xff;
		fnDelayDisplay( MaxDisplayLightness - g_byDisplayLightness);
		PORTG  |= (1<<2);
	}
	//显示指示灯
	PORTG &= ~(1<<2);
	PORTA = g_byaIndicator[0];
	PORTB = (g_byaIndicator[1]&0x01)?(PORTB|0x01):(PORTB&0xfe); 
	fnDelayDisplay( g_byDisplayLightness);
	PORTA = 0xff;
	PORTB |= 0x01; 
	fnDelayDisplay( MaxDisplayLightness - g_byDisplayLightness);
	PORTG  |= (1<<2);
	
}
//********************************************************************************************************************


//************************显示内容************************************************************************************
//显示字符，程序将字符串中字母所对应的ASCII字符按数字处理换算，直接与显示字符数组一一对应。
void fnOutputChar(unsigned char byDisplayRow, char * byChar )
{
	unsigned char i = 0;
	if(byDisplayRow < 3 && byChar)
	{
		for (unsigned char j=0; j<4; j++ )
		{
			//大写字母：ASCII的值在65-90之间，将值减去55，对应到显示字符数组的10-35之间。
			if((*(byChar+i)>64) && (*(byChar+i) < 91))
			{
				g_byaDisplayContents[byDisplayRow][j] = g_byaDisplayCode[*(byChar+i) - 55];
			}
			//小写字母：ASCII的值在97-122之间，将值减去87，对应到显示字符数组的10-35之间。
			else if ( (*(byChar+i)>96 ) && (*(byChar+i)<123 ) )
			{
				g_byaDisplayContents[byDisplayRow][j] = g_byaDisplayCode[*(byChar+i) - 87];
			}
			////数字字母：ASCII的值在48-57之间，将值减去48，对应到显示字符数组的0-9之间。
			else if ( (*(byChar+i)>47 ) && (*(byChar+i)<58 ) )
			{
				g_byaDisplayContents[byDisplayRow][j] = g_byaDisplayCode[*(byChar+i) - 48];
			}
			//如果为其他字符，一律为显示字符数组的36不显示。
			else
			{
				g_byaDisplayContents[byDisplayRow][j] = g_byaDisplayCode[36];
			}
			
			//如果当前字符不为空（ASC为0），并且也不为"."（ASC为46），i指向下一个字符。
			if ((*(byChar+i)!=0)&&(*(byChar+i)!=46))
			{
				i++;
			}			
			//如果是“.” 字符，就显示点并且移动 i 去处理下一位字符
			if (*(byChar+i) == 46)
			{
				g_byaDisplayContents[byDisplayRow][j] &= 0xef;
				i++;
			}
		}
	}
}
//显示数字
void fnOutputNum(unsigned char byDisplayRow,float iNum)
{
	short int i,byFlag;
	long iValue;
	iValue = (long)(iNum*1000);
	if (byDisplayRow < 3)
	{
		for (i= -3; i<0&&!(iValue%10); i++)
		{
			iValue /= 10;
		}
		for (byFlag = 3;byFlag>=0;byFlag --)
		{
			g_byaDisplayContents[byDisplayRow][byFlag] = ((i<=0)||iValue)?g_byaDisplayNumCode[iValue%10]:ID_DISCODE_NULL;
			if(!i&&byFlag<3)
			{
				g_byaDisplayContents[byDisplayRow][byFlag] &= 0xef;			
			}
			iValue /= 10;
			i++;
		}
	}
}

//显示数字字符
void fnOutputNumChar(unsigned char byDisplayRow, unsigned char * pbyValue)
{
	if (byDisplayRow < 3)
	{
		g_byaDisplayContents[byDisplayRow][3] = g_byaDisplayNumCode[pbyValue[3]];
		g_byaDisplayContents[byDisplayRow][2] = g_byaDisplayNumCode[pbyValue[2]];
		g_byaDisplayContents[byDisplayRow][1] = g_byaDisplayNumCode[pbyValue[1]];
		g_byaDisplayContents[byDisplayRow][0] = g_byaDisplayNumCode[pbyValue[0]];
	}
}

//显示指示灯
void fnOutputIndicator(unsigned int uValue)
{
	g_byaIndicator[0] = (unsigned char)uValue;
	g_byaIndicator[1] = (unsigned char)(uValue>>8);
}


//****************************显示亮度********************************************************************************
//设置显示亮度
void fnSetDisplayLightness(unsigned char byDisplayLightness)
{
	if(byDisplayLightness < 7 )
	g_byDisplayLightness = MaxDisplayLightness/6 * byDisplayLightness;
}

//发光二极管的显示亮度和发光二极管的显示时间有光，显示时间越长，发光二极管就越亮，因此显示延时时间的长度决定了发光二极管的实现亮度
void fnDelayDisplay(unsigned char byDelayTime)
{
	unsigned char byCnt;
	for(byCnt = 0; byCnt <byDelayTime; byCnt++);
}
//********************************************************************************************************************


//*****************************显示时间********************************************************************************
//复位当前显示时间，将当前显示时间设定为最大值
void fnResetDisplayTime()
{
	g_byDisplayTime = MaxDisplayTime;
	if(!g_byDisplayEnable)
	//启动定时中断
	;
}

//获取当前显示时间
unsigned char fnGetDisplayTime()
{
	return g_byDisplayTime;
}

//使能显示标志位，显示标志位置1，数码管一直显示
void fnEnableDisplay()
{
	g_byDisplayEnable = 1;
}
//失能显示标志位，显示标志位置0，数码管显示60S
void fnDisableDisplay()
{
	g_byDisplayEnable = 0;
}
//*********************************************************************************************************************


//****************************闪烁*************************************************************************************
//关闭闪烁功能，闪烁标志位置0
void fnDisableFlash()
{
	g_byFlashEnable = 0;
}
//开启闪烁功能，闪烁标志位置1
void fnSetFlash(unsigned char byFlashDigit)
{
	g_byFlashEnable = 1;
	if(byFlashDigit < 4)
	g_byFlashDigit = byFlashDigit;
}


