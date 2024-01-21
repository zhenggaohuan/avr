/*
* InputAndOutput.c
*
* Created: 2015-08-24 16:30:30
*  Author: ZhengGaoHuan
*/

#include "InputAndOutput.h"
#include <avr/interrupt.h>
#include <avr/io.h>

//显示定义的消息**************************************************
#define FlashTime 60
#define MaxDisplayLightness 120
#define MaxDisplayTime 60
#define ID_Max_GatherCount 50

#define ID_DISCODE_NULL 0xff

//输出***************************************************************************************************

//显示数字的字码
static const unsigned char g_byaDisplayNumCode[] = {0x50, 0x5f, 0x38, 0x19, 0x17, 0x91, 0x90, 0x55, 0x10, 0x11};
static const unsigned char g_byaDisplayCode[] = {0x50, 0x5f, 0x38, 0x19, 0x17, 0x91, 0x90, 0x55, 0x10, 0x11, 0x14, 0x92, 0xf0, 0x1a, 0xb0, 0xb4, 0xd0, 0x16, 0xf6, 0x5b, 0x94, 0xf2, 0x54, 0x9e, 0x9a, 0x34, 0x15, 0xbe, 0x91, 0xb2, 0x52, 0xda, 0x12, 0x97, 0x13, 0x38, 0xff};
/*
struct
{
	//显示亮度，分为6个等级，最大120，最小0。
	volatile unsigned char byDisplayLightness;
	//显示标志：为1时，数码管一直实现，为0时，数码管60S后不显示
	volatile unsigned char byDisplayEnable;
	//剩余显示时间
	volatile unsigned char byDisplayTime;
	//闪烁标志位，为1时，数码管相应位闪烁显示，为0时，数码管不闪烁
	volatile unsigned char byFlashEnable;
	//闪烁位
	volatile unsigned char byFlashDigit;
	//显示内容
	volatile unsigned char byaDisplayContents[3][4]
}g_stDisplay = {120, 1, 60, 0, 0,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
*/
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

//输出******************************************************************
//**********************初始化****************************************************************************************
//显示初始化
void fnDisplayInit()
{
	PORTA = 0xff;
	DDRA = 0xff;
	PORTC &= ~(1<<7);
	DDRC |= (1<<7);
	PORTG &= ~(1<<2);
	DDRG |= (1<<2);
	DDRB |= 1<<DDRB0;
	PORTB |= 1<<PORTB0;
	
	//中断初始化T/C0
	TIMSK |= (1<<TOIE0);
	TCCR0 |= (1<<CS02)|(1<<CS01)|(1<<CS00);
	
	TCNT0 = 126; 	
	sei();
}
//********************************************************************************************************************


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
//*********************************************************************************************************************


//*输入*************************************************************************************************************
//按键初始化
void fnKeyInit(void)
{
	DDRD &= ~(1<<0);	//PD0的DDR置0，设置PD0输入
	PORTD |= (1<<0);	//PD0的PORT置1，设置PD0上拉电阻使能
	DDRD &= ~(1<<7);	//PD7的DDR置0，设置PD7输入
	PORTD |= (1<<7);	//PD7的PORT置1，设置PD7上拉电阻使能
	DDRG &= ~(1<<1);	//PG1的DDR置0，设置PG1输入
	PORTG |= (1<<1);	//PG1的PORT置1，设置PG1上拉电阻使能
	DDRC &= ~(1<<2);	//PC2的DDR置0，设置PC2输入
	PORTC |= (1<<2);	//PC2的PORT置1，设置PC2上拉电阻使能
}

//获取按键消息
unsigned char fnGetKeyCode(void)
{
	//按键消息码，
	static unsigned char byKeyCode = 0x00;
	//采集按键闭合情况计数器
	static int nGatherCount =0;
	//前次按键闭合，生成的按键码
	static unsigned char byPreviousGather  = 0x00;
	//当前按钮闭合，生成的按键码
	unsigned char byCurrentGather;
	
	/*采集按键闭合情况，如果是单个按键闭合，则所产生的按键码是唯一的，如果两个或两个以上的按键闭合，则产生的按键码会与单个按键闭合产生的按键码不同
	按键对应的按键码为，K4：00001110，K3：00001101，K2：00001011，K1：00000111，无按键：00001111，无效码：00000000*/
	//取当前按键码，并把高4位置0
	byCurrentGather = (PIND&0x01)|(PING&0x02)|((PIND&0x80)>>5)|((PINC&0x04)<<1);
	//如果有按键码或有按键命令码，执行如下程序，（就是说，当扫描到有按键按下，或命令码中还有按键命令，才执行程序。如果无按键按下并且命令码里面也没有按键命令，则表示单品机无任何输入）
	if( ~(byCurrentGather|0xF0) || ~byKeyCode || ~(byPreviousGather|0xF0))
	{
		//如果当前按键码和前按键码相同，按键计数器加1；如果不同，则按键计数器复位为1
		nGatherCount = (byCurrentGather == byPreviousGather)?nGatherCount+1:1;
		//把前面按键码保留起来，等到下个循环的时候在对比
		byPreviousGather = byCurrentGather;
		//如果计数器为7，表示已经采集到一个稳定的按键码，那么就把按键码加入到消息码 中，并发送消息。因为按键码只有4位有效，所以，要将消息码的低4位移到高4位，然后把按键码的低4位放入消息码的低4位，计数器要清0，表示重新开始采集计数；
		if(nGatherCount >= ID_Max_GatherCount)
		{
			//要将消息码的低4位移到高4位
			byKeyCode <<= 4;
			//把按键码的低4位放入消息码的低4位
			byKeyCode |= byCurrentGather;
			//计数器要清0
			nGatherCount = 0;
			//发送消息码
			return  byKeyCode;
		}
	}
	return  0x00;
}

//获取系统消息
unsigned char fnkey( void )
{
	//扫描输入
	unsigned char byKeyMessage = fnGetKeyCode();
	//如果有按键消息，置位剩余显示时间g_byDisplayTime
	//如果剩余显示时间大于0，输出显示，
	return byKeyMessage;
}













