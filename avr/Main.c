/*
* Main.c
*
* Created: 2014-07-13 08:12:59
*  Author: hppc
*/

/*
*�ó������ǰ��̨���ȿ�ܽ��б�д
*ǰ̨����Ϊ�жϵ��ã������ж���λ����ִ�б�־������ִ�б�־�ɶ�ʱ�жϡ�ͨѶ�жϵȸ�����������λ��
*��̨����Ϊ������ѭ��ִ�У�������ִ�е�ʱ�򣬸�������ִ�б�־ִ����Ӧ����
*��־λ����λ������ǰ̨�жϳ�����ΪͨѶ��־λ����λ��ͨ���ж�����λ�����Խ����б�־λ����λ�������жϱ�־�У���̨����ֻ��λ��־λ��
*/

#include <avr/io.h> //�������Ŷ���ͷ�ļ�
#include <avr/interrupt.h>  //�����ж�ͷ�ļ�


/*
*������������
*/

/*
*��ʾ���������ַ���
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
*����ɨ�趨��İ�����Ϣ��
*/
#define ID_KEYMSG_Null 0x00
#define ID_KEYMSG_LeftDown 0xf7
#define ID_KEYMSG_RightDown 0xfB
#define ID_KEYMSG_MenuDown 0xfD
#define ID_KEYMSG_EnterDown 0xfE

//��ʾ�������Ϣ**************************************************
#define FlashTime 60
#define MaxDisplayLightness 120
#define MaxDisplayTime 60
#define ID_Max_GatherCount 50

#define ID_DISCODE_NULL 0xff


//��ʾ���ֵ�����
static const unsigned char g_byaDisplayNumCode[] = {0x50, 0x5f, 0x38, 0x19, 0x17, 0x91, 0x90, 0x55, 0x10, 0x11};
static const unsigned char g_byaDisplayCode[] = {0x50, 0x5f, 0x38, 0x19, 0x17, 0x91, 0x90, 0x55, 0x10, 0x11, 0x14, 0x92, 0xf0, 0x1a, 0xb0, 0xb4, 0xd0, 0x16, 0xf6, 0x5b, 0x94, 0xf2, 0x54, 0x9e, 0x9a, 0x34, 0x15, 0xbe, 0x91, 0xb2, 0x52, 0xda, 0x12, 0x97, 0x13, 0x38, 0xff};
//��ʾ���ȣ���Ϊ6���ȼ������120����С0��
static volatile unsigned char g_byDisplayLightness = 120;
//��ʾ��־��Ϊ1ʱ�������һֱʵ�֣�Ϊ0ʱ�������60S����ʾ
static volatile unsigned char g_byDisplayEnable = 1;
//ʣ����ʾʱ��
static volatile unsigned char g_byDisplayTime = 60;
//��˸��־λ��Ϊ1ʱ���������Ӧλ��˸��ʾ��Ϊ0ʱ������ܲ���˸
static volatile unsigned char g_byFlashEnable = 0;
//��˸λ
static volatile unsigned char g_byFlashDigit = 0;
//��ʾ����
static volatile unsigned char g_byaDisplayContents[3][4] = {{0xff,0xff,0xff,0xff},{0xff,0xff,0xff,0xff},{0xff,0xff,0xff,0xff}};
static volatile unsigned char g_byaIndicator[2] = {0xff,0xff};




/*
*��ʾ
*/
//�����ɨ����ʾ
void fnScanLED();
//�������˸��ʾ
unsigned char fnDisplay(void);



/*
*��ʾ����
*/
//��ʾ�ַ�
void fnOutputChar(unsigned char byDisplayRow, char * byChar );
//��ʾ����
void fnOutputNum(unsigned char byDisplayRow,float iNum);
//��ʾ�����ַ�
void fnOutputNumChar(unsigned char byDisplayRow, unsigned char * pbyValue);
//��ʾָʾ��
void fnOutputIndicator(unsigned int uValue);

/*
*��ʾ����
*/
//������ʾ����
void fnSetDisplayLightness(unsigned char byDisplayLightness);
//��������ܵ���ʾ���Ⱥͷ�������ܵ���ʾʱ���й⣬��ʾʱ��Խ������������ܾ�Խ���������ʾ��ʱʱ��ĳ��Ⱦ����˷�������ܵ�ʵ������
void fnDelayDisplay(unsigned char byDelayTime);



/*
*��ʾʱ��
*/
//��λ��ǰ��ʾʱ�䣬����ǰ��ʾʱ���趨Ϊ���ֵ
void fnResetDisplayTime();
//��ȡ��ǰ��ʾʱ��
unsigned char fnGetDisplayTime();
//����һֱ��ʾ���ܣ�ʹ����ʾ��־λ����ʾ��־λ��1�������һֱ��ʾ
void fnEnableDisplay();
//�ر�һֱ��ʾ���ܣ�ʧ����ʾ��־λ����ʾ��־λ��0���������ʾ60S
void fnDisableDisplay();



/*
*��˸
*/
//�ر���˸���ܣ���˸��־λ��0
void fnDisableFlash();
//������˸���ܣ���˸��־λ��1
void fnSetFlash(unsigned char byFlashDigit);

//���Ĵ�����λ��ʮλ����λ��ǧλ�������������һ������
unsigned int fnComposeNum(char * pbyaNum);
//��1�������ַֽ��4��������λ��ʮλ����λ��ǧλ����������
void fnDecomposeNum(unsigned int uNum, unsigned char *pbyaNum);

//��Ƭ����ʼ��
void fnInit();
//ͨѶģ�鴦�����ڴ���modbusͨѶ��Ϣ��������õ�ʱ������ͨѶ�ж��н��յ����ݺ���λͨѶ��־λ
void fnComProcess(); 
//��ȡ������Ϣ�룺ɨ�谴�������������8λɨ���루��4λ��Ч�����������������������趨����ͬ��ɨ��������һ��8λ�����루��4λ��Ч����ͬʱ����һ��8λ������Ϣ�룬������Ϣ��λ�ĵ�4Ϊ��ǰ�����룬��4λΪǰ�ΰ�����
void fnkey();

/*ȫ�ֱ�������*/
unsigned char byMessage = 0;  //���̰�����Ϣ

//������ֽ������б�־��ǰ̨�жϳ�����ݸ��������λ�������б�־����̨���������λ�Ľ������б�־���н��̣�����λ�������б�־
unsigned char byFlagHalfSecond = 0;  //��˸�����Ĵ����־
unsigned char byFlagDisplay = 0; //��ʾ�����־
unsigned char byFlagKey = 0; //������ѯ��־
unsigned char byFlagCom = 0; //ͨѶ״̬��־�����ͨѶ�жϳ��������ݽ��յ�������λͨѶ״̬��־����̨������ͨѶ���ݣ���λͨѶ״̬��־

//�������岿��
int main(void)
{
	//��Ƭ����ʼ��
	fnInit();

	//��ʼ��̨��������
	while(1)
	{
		if (byFlagDisplay)  //�����ʾ��־λ��λ
		{
			byFlagDisplay = 0;  //��ʾ��־λ��λ
			fnDisplay();  //������ʾ����
		}
		if (byFlagKey) //���������ѯ��־��λ
		{
			byFlagKey = 0; //������ѯ��־��λ
			fnkey(); //ɨ����̣�ȡ�����룬ȷ����������
		}
		if (byFlagCom) //���ϵͳͨѶ״̬��־��λ
		{
			byFlagCom = 0; //ͨѶ״̬��־λ��λ
			fnComProcess(); //����ͨѶ����
		}
		
	}
}


//��ʱ��T0����ж�
ISR(TIMER0_OVF_vect)
{
	TCNT0 = 126;
	byFlagDisplay = 1; //��ʾ������λ
	byFlagKey = 1; //������ѯ��λ
}

//��Ƭ����ʼ��
void fnInit()
{
	//���̳�ʼ��
	DDRD &= ~(1<<0);	//PD0��DDR��0������PD0����
	PORTD |= (1<<0);	//PD0��PORT��1������PD0��������ʹ��
	DDRD &= ~(1<<7);	//PD7��DDR��0������PD7����
	PORTD |= (1<<7);	//PD7��PORT��1������PD7��������ʹ��
	DDRG &= ~(1<<1);	//PG1��DDR��0������PG1����
	PORTG |= (1<<1);	//PG1��PORT��1������PG1��������ʹ��
	DDRC &= ~(1<<2);	//PC2��DDR��0������PC2����
	PORTC |= (1<<2);	//PC2��PORT��1������PC2��������ʹ��
	
	//��ʾ��ʼ��
	PORTA = 0xff;
	DDRA = 0xff;
	PORTC &= ~(1<<7);
	DDRC |= (1<<7);
	PORTG &= ~(1<<2);
	DDRG |= (1<<2);
	DDRB |= 1<<DDRB0;
	PORTB |= 1<<PORTB0;
	
	//��ʱ�� ��ʱ���жϳ�ʼ��T/C0
	TIMSK |= (1<<TOIE0);
	TCCR0 |= (1<<CS02)|(1<<CS01)|(1<<CS00);
	TCNT0 = 126; //���ü�������ֵ

	//�����ж� 	
	asm("sei"); //������û�����ķ�ʽ�����ж� 
}

void fnComProcess()
{
	
}

//��ȡ������Ϣ
void fnkey()
{
	static unsigned char byKeyCode = 0x00;	//������Ϣ�룬
	static int nGatherCount =0;	//�ɼ������պ����������
	static unsigned char byPreviousGather  = 0x00;	//ǰ�ΰ����պϣ����ɵİ�����
	unsigned char byCurrentGather;	//��ǰ��ť�պϣ����ɵİ�����

	//�ɼ������պ����������ǵ��������պϣ����������İ�������Ψһ�ģ�����������������ϵİ����պϣ�������İ�������뵥�������պϲ����İ����벻ͬ��������Ӧ�İ�����Ϊ��K4��00001110��K3��00001101��K2��00001011��K1��00000111���ް�����00001111����Ч�룺00000000
	byCurrentGather = (PIND&0x01)|(PING&0x02)|((PIND&0x80)>>5)|((PINC&0x04)<<1);	//ȡ��ǰ�����룬���Ѹ�4λ��0
	if( ~(byCurrentGather|0xF0) || ~byKeyCode || ~(byPreviousGather|0xF0))	//����а�������а��������룬ִ�����³��򣬣�����˵����ɨ�赽�а������£����������л��а��������ִ�г�������ް������²�������������Ҳû�а���������ʾ��Ʒ�����κ����룩
	{
		nGatherCount = (byCurrentGather == byPreviousGather)?nGatherCount+1:1;		//�����ǰ�������ǰ��������ͬ��������������1�������ͬ���򰴼���������λΪ1
		byPreviousGather = byCurrentGather;		//��ǰ�水���뱣���������ȵ��¸�ѭ����ʱ���ڶԱ�
		if(nGatherCount >= ID_Max_GatherCount)	   //���������Ϊ7����ʾ�Ѿ��ɼ���һ���ȶ��İ����룬��ô�ͰѰ�������뵽��Ϣ�� �У���������Ϣ����Ϊ������ֻ��4λ��Ч�����ԣ�Ҫ����Ϣ��ĵ�4λ�Ƶ���4λ��Ȼ��Ѱ�����ĵ�4λ������Ϣ��ĵ�4λ��������Ҫ��0����ʾ���¿�ʼ�ɼ�������
		{
			byKeyCode <<= 4;    //Ҫ����Ϣ��ĵ�4λ�Ƶ���4λ
			byKeyCode |= byCurrentGather;    //�Ѱ�����ĵ�4λ������Ϣ��ĵ�4λ
			nGatherCount = 0;    //������Ҫ��0
			byMessage =  byKeyCode;    //������Ϣ��
		}
	}
}


//��4��������λ��ʮλ����λ��ǧλ�������������һ������
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
//��1�������ַֽ��4��������λ��ʮλ����λ��ǧλ����������
void fnDecomposeNum(unsigned int uNum, unsigned char *pbyaNum)
{
	for (int i=3; i>=0; i--)
	{
		pbyaNum[i] = uNum%10;
		uNum /= 10;
	}
}


//***********************��ʾ*****************************************************************************************
//�������ʾ
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

//�����ɨ����ʾ
void fnScanLED()
{
	unsigned char byCnt;   //Ҫ��ʾ������λ��

	PORTA = 0xff;
	PORTG &= ~(1<<2);  	   //PG2 = 0
	PORTC &= ~(1<<7);	   //PC7 = 0
	PORTG  |= (1<<2);	   //PG2 = 1
	
	PORTC |= (1<<7);	   //PC7 = 1
	
	//��ʾ��2������
	for(byCnt = 0;byCnt < 4;byCnt ++)
	{
		PORTG &= ~(1<<2);
		PORTA = g_byaDisplayContents[1][byCnt];
		fnDelayDisplay( g_byDisplayLightness);
		PORTA = 0xff;
		fnDelayDisplay( MaxDisplayLightness - g_byDisplayLightness);
		PORTG  |= (1<<2);
	}
	//��ʾ��3������
	for(byCnt = 0;byCnt < 4;byCnt ++)
	{
		PORTG &= ~(1<<2);
		PORTA = g_byaDisplayContents[2][byCnt];
		fnDelayDisplay( g_byDisplayLightness);
		PORTA = 0xff;
		fnDelayDisplay( MaxDisplayLightness - g_byDisplayLightness);
		PORTG  |= (1<<2);
	}
	//��ʾ��1������
	for(byCnt = 0;byCnt < 4;byCnt ++)
	{
		PORTG &= ~(1<<2);
		PORTA = g_byaDisplayContents[0][byCnt];
		fnDelayDisplay( g_byDisplayLightness);
		PORTA = 0xff;
		fnDelayDisplay( MaxDisplayLightness - g_byDisplayLightness);
		PORTG  |= (1<<2);
	}
	//��ʾָʾ��
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


//************************��ʾ����************************************************************************************
//��ʾ�ַ��������ַ�������ĸ����Ӧ��ASCII�ַ������ִ����㣬ֱ������ʾ�ַ�����һһ��Ӧ��
void fnOutputChar(unsigned char byDisplayRow, char * byChar )
{
	unsigned char i = 0;
	if(byDisplayRow < 3 && byChar)
	{
		for (unsigned char j=0; j<4; j++ )
		{
			//��д��ĸ��ASCII��ֵ��65-90֮�䣬��ֵ��ȥ55����Ӧ����ʾ�ַ������10-35֮�䡣
			if((*(byChar+i)>64) && (*(byChar+i) < 91))
			{
				g_byaDisplayContents[byDisplayRow][j] = g_byaDisplayCode[*(byChar+i) - 55];
			}
			//Сд��ĸ��ASCII��ֵ��97-122֮�䣬��ֵ��ȥ87����Ӧ����ʾ�ַ������10-35֮�䡣
			else if ( (*(byChar+i)>96 ) && (*(byChar+i)<123 ) )
			{
				g_byaDisplayContents[byDisplayRow][j] = g_byaDisplayCode[*(byChar+i) - 87];
			}
			////������ĸ��ASCII��ֵ��48-57֮�䣬��ֵ��ȥ48����Ӧ����ʾ�ַ������0-9֮�䡣
			else if ( (*(byChar+i)>47 ) && (*(byChar+i)<58 ) )
			{
				g_byaDisplayContents[byDisplayRow][j] = g_byaDisplayCode[*(byChar+i) - 48];
			}
			//���Ϊ�����ַ���һ��Ϊ��ʾ�ַ������36����ʾ��
			else
			{
				g_byaDisplayContents[byDisplayRow][j] = g_byaDisplayCode[36];
			}
			
			//�����ǰ�ַ���Ϊ�գ�ASCΪ0��������Ҳ��Ϊ"."��ASCΪ46����iָ����һ���ַ���
			if ((*(byChar+i)!=0)&&(*(byChar+i)!=46))
			{
				i++;
			}			
			//����ǡ�.�� �ַ�������ʾ�㲢���ƶ� i ȥ������һλ�ַ�
			if (*(byChar+i) == 46)
			{
				g_byaDisplayContents[byDisplayRow][j] &= 0xef;
				i++;
			}
		}
	}
}
//��ʾ����
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

//��ʾ�����ַ�
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

//��ʾָʾ��
void fnOutputIndicator(unsigned int uValue)
{
	g_byaIndicator[0] = (unsigned char)uValue;
	g_byaIndicator[1] = (unsigned char)(uValue>>8);
}


//****************************��ʾ����********************************************************************************
//������ʾ����
void fnSetDisplayLightness(unsigned char byDisplayLightness)
{
	if(byDisplayLightness < 7 )
	g_byDisplayLightness = MaxDisplayLightness/6 * byDisplayLightness;
}

//��������ܵ���ʾ���Ⱥͷ�������ܵ���ʾʱ���й⣬��ʾʱ��Խ������������ܾ�Խ���������ʾ��ʱʱ��ĳ��Ⱦ����˷�������ܵ�ʵ������
void fnDelayDisplay(unsigned char byDelayTime)
{
	unsigned char byCnt;
	for(byCnt = 0; byCnt <byDelayTime; byCnt++);
}
//********************************************************************************************************************


//*****************************��ʾʱ��********************************************************************************
//��λ��ǰ��ʾʱ�䣬����ǰ��ʾʱ���趨Ϊ���ֵ
void fnResetDisplayTime()
{
	g_byDisplayTime = MaxDisplayTime;
	if(!g_byDisplayEnable)
	//������ʱ�ж�
	;
}

//��ȡ��ǰ��ʾʱ��
unsigned char fnGetDisplayTime()
{
	return g_byDisplayTime;
}

//ʹ����ʾ��־λ����ʾ��־λ��1�������һֱ��ʾ
void fnEnableDisplay()
{
	g_byDisplayEnable = 1;
}
//ʧ����ʾ��־λ����ʾ��־λ��0���������ʾ60S
void fnDisableDisplay()
{
	g_byDisplayEnable = 0;
}
//*********************************************************************************************************************


//****************************��˸*************************************************************************************
//�ر���˸���ܣ���˸��־λ��0
void fnDisableFlash()
{
	g_byFlashEnable = 0;
}
//������˸���ܣ���˸��־λ��1
void fnSetFlash(unsigned char byFlashDigit)
{
	g_byFlashEnable = 1;
	if(byFlashDigit < 4)
	g_byFlashDigit = byFlashDigit;
}


