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

#include <avr/io.h>

#include "InputAndOutput.h"
#include "Fm25040.h"
#include "Delay.h"
#include "att7022.h"

/*
*������������
*/



//���õ�ѹ������Χ
float fnSetUValue(ty_uscl eUScl);
//���õ���������Χ
float fnSetIValue(ty_iscl eIScl);
//���Ա���ʾ������ѹ����Ϣ
void fnAVMessage(unsigned char byMessage);
/*	������ò������û��ɸ���ʵ�����ѡ���ʵ��ı�����ò���	*/
unsigned char fnMenuChoose(unsigned char byMessage);
/*	�������ã������޸Ĺ��Ĳ���	*/
unsigned char fnSaveSet(unsigned char byMessage);
//��֤���룬�������������ȷʱ�ſ��Խ����̡��ں������û�������֤���룬�������������֤
unsigned int fnGetCodePut(unsigned char byMessage);
//�޸����룬������֤�ɹ������޸����롣�ں����У��û�����Ҫ�޸ĵ����룬���������û�������޸�����
unsigned int fnGetCodeSet(unsigned char byMessage);
//���磬ѡ������źŵ��������硣�������ز����źŵ���������ֵ
unsigned int fnGetSetNet(unsigned char byMessage);
//��ѹ��ȣ����õ�ѹ�źű��=1�ο̶�/2�ο̶ȣ�����10KV/100V=100���������ص�ѹ���ֵ
unsigned int fnGetSetPT_U(unsigned char byMessage);
//��ȡ�������ֵ
unsigned int fnGetSetCT_I(unsigned char byMessage);
//��ȡ�����
unsigned int fnGetSetE_CLE(unsigned char byMessage);
//��ȡ��ʾ
unsigned int fnGetDisDISP_E(unsigned char byMessage);
//��ȡ��ʾ��ҳ
unsigned int fnGetDisDIS_P(unsigned char byMessage);
//��ȡ��ʾ����
unsigned char fnGetDisB_LED(unsigned char byMessage);
//��ȡͨѶ������ַ
unsigned int fnGetConnAdd(unsigned char byMessage);
//��ȡͨ�Ų���ͨѶУ��λ
unsigned int fnGetConnData(unsigned char byMessage);
//��ȡͨ�Ų���ͨѶ����
unsigned int fnGetConnBud(unsigned char byMessage);


//���Գ���
void fnDebugAtt7022(void);
//У��
void fnCheckAtt7022(void);
//�������
void fnClearAtt7022(void);

//���Ĵ�����λ��ʮλ����λ��ǧλ�������������һ������
unsigned int fnComposeNum(char * pbyaNum);
//��1�������ַֽ��4��������λ��ʮλ����λ��ǧλ����������
void fnDecomposeNum(unsigned int uNum, unsigned char *pbyaNum);

//���򰴼���Ϣ������ϵͳ״̬������������ȴ�������Ϣ��������Ϣʹ��ϵͳ״̬ת�ƣ��ú������ϵͳ״̬������ʾ����
void fnState(void);

/*ȫ�ֱ�������*/

//ϵͳ״̬:
enum {E_AVMessage,  //������Ϣ��ʾ
	E_MenuChoose,  //�˵�ѡ��״̬
	E_MenuSave,  //���ñ���
	E_SetParam_CodePut,  //��ȡ�˵����룬���Ѿ���E_MenuPassword���棩
	E_SetParam_CodeSet,  //��������
	E_SetParam_SetNet,  //ѡ������źŵ���������
	E_SetParam_SetPtU,  //���õ�ѹ���
	E_SetParam_SetCtI,  //���õ������
	E_SetParam_SetECle,  //�����
	E_SetParam_DisDispE,  //������ʾ
	E_SetParam_DisDisP,  //������ʾ��ҳ
	E_SetParam_DisBLed,  //������ʾ����
	E_SetParam_ConnAdd, //����ͨѶ��ַ
	E_SetParam_ConnData, //����ͨѶУ����
	E_SetParam_ConnBud //����ͨѶ������
} eSystemState = E_AVMessage;

ty_osparameter sOSParameter;	 	//ϵͳ����

unsigned char byMessage = 0;  //���̰�����Ϣ

//������ֽ������б�־��ǰ̨�жϳ�����ݸ��������λ�������б�־����̨���������λ�Ľ������б�־���н��̣�����λ�������б�־
unsigned char byFlagHalfSecond = 0;  //��˸�����Ĵ����־
unsigned char byFlagDisplay = 0; //��ʾ�����־
unsigned char byFlagKey = 0; //������ѯ��־
unsigned char byFlagState = 0; //����ϵͳ״̬��ת�ƣ�����ϵͳ��һ��״̬���Ľṹ������ÿ�ΰ�������ʱ�䶨ʱ����ʱ�򣬻ᷢ��״̬ת��
unsigned char byFlagCom = 0; //ͨѶ״̬��־�����ͨѶ�жϳ��������ݽ��յ�������λͨѶ״̬��־����̨������ͨѶ���ݣ���λͨѶ״̬��־

//�������岿��
int main(void)
{
	//��ʼ��
	fnKeyInit();	//������ʼ��
	fnDisplayInit(); //��ʾ��ʼ��

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
		if (byFlagState) //���ϵͳ״̬ת�Ʊ�־��λ
		{
			byFlagState = 0; //ϵͳ״̬��־��λ
			fnState(); //��ϵͳ״̬������������ȴ�������Ϣ��������Ϣʹ��ϵͳ״̬ת�ƣ��ú������ϵͳ״̬������ʾ����
		}
		
	}
}


//��ʱ��0 �жϷ���������
ISR(TIMER0_OVF_vect)
{
	TCNT0 = 126;
	byFlagDisplay = 1; //��ʾ������λ
}


//���򰴼���Ϣ������ϵͳ״̬������������ȴ�������Ϣ��������Ϣʹ��ϵͳ״̬ת�ƣ��ú������ϵͳ״̬������ʾ����
void fnState(void)
{
	//�������Ч������������ʾ����ʾʱ����λ
	switch(eSystemState)//���ݡ�ϵͳ״̬��������Ӧ��������������Ϣ�����������������Ϣ"
	{
		case E_AVMessage:  //������Ϣ��ʾ״̬
		fnAVMessage(byMessage);
		break;
		case E_MenuChoose:  //�˵�ѡ��״̬
		fnMenuChoose(byMessage);
		break;
		case E_MenuSave: //����˵�����
		break;
		case E_SetParam_CodePut: //��֤����
		break;
		case E_SetParam_CodeSet: //��������
		fnGetCodeSet(byMessage);
		break;
		case E_SetParam_SetNet: //ѡ������źŵ���������
		break;
		case E_SetParam_SetPtU: //��ѹ���
		break;
		case E_SetParam_SetCtI: //�������
		break;
		case E_SetParam_SetECle: //�����
		break;
		case E_SetParam_DisDispE: //��ʾ���ã���ʾ
		break;
		case E_SetParam_DisDisP: //��ʾ���ã���ʾ��ҳ
		break;
		case E_SetParam_DisBLed: //��ʾ���ã���ʾ����
		break;
		case E_SetParam_ConnAdd: //ͨѶ��ַ
		break;
		case E_SetParam_ConnData: //ͨѶУ��λ
		break;
		case E_SetParam_ConnBud: //ͨѶ������
		break;
	}
}





//У�����
/*
void fnDebugAtt7022(void)
{
//�˵��ṹ
const unsigned char byaMenu[2] = {IDM_CHE, IDM_CLE};
unsigned char byMenuColumn = 0;

//�����Ļ
fnCleanOutput(2);
fnCleanOutput(1);
fnOutputChar(0,byaMenu[byMenuColumn]);

//���밴������
while(1)
{
switch(fnGetMessage())
{
case ID_KEYMSG_LeftDown:
byMenuColumn = 0;
fnOutputChar(0,byaMenu[byMenuColumn]);
break;
case ID_KEYMSG_RightDown:
byMenuColumn = 1;
fnOutputChar(0, byaMenu[byMenuColumn]);
break;
case ID_KEYMSG_MenuDown:
fnCleanOutput(2);
fnCleanOutput(1);
fnCleanOutput(0);
return;
break;
case ID_KEYMSG_EnterDown:
switch(byaMenu[byMenuColumn])
{
case IDM_CHE:
fnCheckAtt7022();
fnOutputChar(0,byaMenu[byMenuColumn]);
break;
case IDM_CLE:
fnClearAtt7022();
break;
}
break;
}
}
}

//У��
void fnCheckAtt7022(void)
{
//�˵��ṹ
unsigned char byCheckI = 0, byCheckU = 0;

//��ʾУ�Ե�����ʾ��Ϣ��Ϊд��
//	fnOutput(ID_OUTPUT_TYPE_CHAR,2,IDM_NULL);
//	fnOutput(ID_OUTPUT_TYPE_CHAR,1,IDM_NULL);
//	fnOutput(ID_OUTPUT_TYPE_CHAR,0,IDM_NULL);

//У�Ե���
while(byCheckI==0)
{
switch(fnGetMessage())
{
case ID_KEYMSG_MenuDown:
fnCleanOutput(2);
fnCleanOutput(1);
fnCleanOutput(0);
return;
break;
case ID_KEYMSG_EnterDown:
//У��δд��
byCheckI = 1;
break;
}
}

//��ʾУ�Ե�ѹ��ʾ��Ϣ��δд��
//	fnOutput(ID_OUTPUT_TYPE_CHAR,2,IDM_NULL);
//	fnOutput(ID_OUTPUT_TYPE_CHAR,1,IDM_NULL);
//	fnOutput(ID_OUTPUT_TYPE_CHAR,0,IDM_NULL);

//У�Ե�ѹ
while(byCheckU==0)
{
switch(fnGetMessage())
{
case ID_KEYMSG_MenuDown:
fnCleanOutput(2);
fnCleanOutput(1);
fnCleanOutput(0);
return;
break;
case ID_KEYMSG_EnterDown:
//У��δд��
byCheckU= 1;
break;
}
}

//У����ϣ������ʾ���˳�
fnCleanOutput(2);
fnCleanOutput(1);
fnCleanOutput(0);
}

//�������
void fnClearAtt7022(void)
{
//�˵��ṹ
const unsigned char byaMenu[2] = {IDM_NO, IDM_YES};
unsigned char byMenuColumn = 0;

//�����Ļ
fnCleanOutput(2);
fnOutputChar(1,byaMenu[byMenuColumn]);

//���밴������
while(1)
{
switch(fnGetMessage())
{
case ID_KEYMSG_LeftDown:
byMenuColumn = 0;
fnOutputChar(1,byaMenu[byMenuColumn]);
break;
case ID_KEYMSG_RightDown:
byMenuColumn = 1;
fnOutputChar(1,byaMenu[byMenuColumn]);
break;
case ID_KEYMSG_MenuDown:
fnCleanOutput(2);
fnCleanOutput(1);
return;
break;
case ID_KEYMSG_EnterDown:
switch(byaMenu[byMenuColumn])
{
case IDM_YES:
//���뺯�������Att7022�������
fnCleanOutput(2);
fnCleanOutput(1);
return;
break;
case IDM_NO:
fnCleanOutput(2);
fnCleanOutput(1);
return;
break;
}
break;
}
}
}

*/


//������ѹ��Ϣ��ʾ
void fnAVMessage(unsigned char byMessage)
{
	unsigned char byMeterDis = 1;
	
	switch(byMessage)
	{
		case ID_KEYMSG_LeftDown:			//����Left
		(byMeterDis>1)?(byMeterDis--):(byMeterDis = 16);
		break;
		case ID_KEYMSG_RightDown:			//����Right
		(byMeterDis<16)?(byMeterDis++):(byMeterDis =1);
		break;
		case ID_KEYMSG_MenuDown:			//����Menu��������״̬
		eSystemState = E_MenuChoose; //��ϵͳ״̬��������������״̬
		break;
		case ID_KEYMSG_EnterDown:			//����Enter
		break;
	}

	switch(byMeterDis)
	{
		case 1: //��һҳ�棬�����ѹ
		fnOutputNum(0,12);
		fnOutputNum(1,0.06);
		fnOutputNum(2,12.34);
		fnOutputIndicator(IDD_V);
		break;
		case 2: //�ڶ�ҳ�棬�������
		fnOutputNum(0,byMeterDis);
		fnOutputNum(1,byMeterDis);
		fnOutputNum(2,byMeterDis);
		fnOutputIndicator(IDD_A);
		break;
		case 3: //����ҳ�棬���๦�ʡ���������
		fnOutputNum(0,byMeterDis);
		fnOutputNum(1,byMeterDis);
		fnOutputNum(2,byMeterDis);
		fnOutputIndicator(IDD_W);
		break;
		case 4: //����ҳ�棬���ڹ��ʡ�Ƶ��
		fnOutputNum(0,byMeterDis);
		fnOutputNum(1,byMeterDis);
		fnOutputNum(2,byMeterDis);
		fnOutputIndicator(IDD_DI);
		break;
		case 5: //����ҳ�棬�����й�����
		fnOutputNum(0,byMeterDis);
		fnOutputNum(1,byMeterDis);
		fnOutputNum(2,byMeterDis);
		fnOutputIndicator(IDD_NULL);
		break;
		case 6: //����ҳ�棬�����й�����
		fnOutputNum(0,byMeterDis);
		fnOutputNum(1,byMeterDis);
		fnOutputNum(2,byMeterDis);
		break;
		case 7: //����ҳ�棬�����й�����
		fnOutputNum(0,byMeterDis);
		fnOutputNum(1,byMeterDis);
		fnOutputNum(2,byMeterDis);
		break;
		case 8: //�ڰ�ҳ�棬�����޹�����
		fnOutputNum(0,byMeterDis);
		fnOutputNum(1,byMeterDis);
		fnOutputNum(2,byMeterDis);
		break;
		case 9: //�ھ�ҳ�棬A���ѹг������
		fnOutputNum(0,byMeterDis);
		fnOutputNum(1,byMeterDis);
		fnOutputNum(2,byMeterDis);
		break;
		case 10: //��ʮҳ�棬B���ѹг������
		fnOutputNum(0,byMeterDis);
		fnOutputNum(1,byMeterDis);
		fnOutputNum(2,byMeterDis);
		break;
		case 11: //��ʮһҳ�棬C���ѹг������
		fnOutputNum(0,byMeterDis);
		fnOutputNum(1,byMeterDis);
		fnOutputNum(2,byMeterDis);
		break;
		case 12: //��ʮ��ҳ�棬A�����г������
		fnOutputNum(0,byMeterDis);
		fnOutputNum(1,byMeterDis);
		fnOutputNum(2,byMeterDis);
		break;
		case 13://��ʮ��ҳ�棬B�����г������
		fnOutputNum(0,byMeterDis);
		fnOutputNum(1,byMeterDis);
		fnOutputNum(2,byMeterDis);
		break;
		case 14://��ʮ��ҳ�棬C�����г������
		fnOutputNum(0,byMeterDis);
		fnOutputNum(1,byMeterDis);
		fnOutputNum(2,byMeterDis);
		break;
		case 15://��ʮ��ҳ�棬�����ѹ�ܲ�ƽ���
		fnOutputNum(0,byMeterDis);
		fnOutputNum(1,byMeterDis);
		fnOutputNum(2,byMeterDis);
		break;
		case 16://��ʮ��ҳ�棬��������ܲ�ƽ���
		fnOutputNum(0,byMeterDis);
		fnOutputNum(1,byMeterDis);
		fnOutputNum(2,byMeterDis);
		break;
	}
}


//������ò���
unsigned char fnMenuChoose(unsigned char byMessage)
{

	//�˵��ṹ
	const unsigned char byaMenu[5][5] = {
		{IDM_Code, IDM_Set, IDM_Dis, IDM_Conn, IDM_NULL},
		{IDM_CodePut, IDM_CodeSet, IDM_NULL, IDM_NULL, IDM_NULL},
		{IDM_SetNet, IDM_SetPT_U, IDM_SetCT_I, IDM_SetE_CLE, IDM_NULL},
		{IDM_DisDIS_E, IDM_DisDIS_P, IDM_DisB_LED,IDM_NULL, IDM_NULL},
		{IDM_ConnAdd, IDM_ConnData, IDM_ConnBud, IDM_NULL, IDM_NULL}
	};

	//�˵�λ�ã�g_byaMenu[5][5]�˵���λ��
	static unsigned char byMenuRow = 0;
	static unsigned char byMenuColumn = 0;

	//��ϵͳ�������б���
	//ty_osparameter sNewOSParameter = *psOSParameter;

	switch(byMessage)//�԰�����Ϣ�������ı�˵���λ�á�
	{
		case ID_KEYMSG_LeftDown://����Left������ͬ����˵�
		if(byMenuColumn)
		{
			byMenuColumn--;
		}
		break;
		case ID_KEYMSG_RightDown://����Right������ͬ���Ҳ˵�
		if(byaMenu[byMenuRow][byMenuColumn+1])
		{
			byMenuColumn ++;
		}
		break;
		case ID_KEYMSG_MenuDown:	//����Menu�������ϼ��˵�
		if(byMenuRow) //����ڶ���˵����򷵻�һ��˵�
		{
			byMenuColumn = byMenuRow - 1;
			byMenuRow = 0;
		}
		else //��һ��˵����˳����棬***ע�⣬Ҫ��ʵ��3��menu�������˵��˵�����ҳ��
		{
			
			eSystemState = E_MenuSave;
		}
		break;
		case ID_KEYMSG_EnterDown: //����Enter�������¼��˵�
		if(byMenuRow) //����ڶ���˵����������Ӧ�����������ʾ��һ��˵���������Ӧ����˵�
		{
			switch(byaMenu[byMenuRow][byMenuColumn])
			{
				case IDM_CodeSet: //�����޸���������
				eSystemState = E_SetParam_CodeSet;
				break;
				case IDM_SetNet: //������������
				eSystemState = E_SetParam_SetNet;
				break;
				case IDM_SetPT_U: // �����ѹ�������
				eSystemState = E_SetParam_SetPtU;
				break;
				case IDM_SetCT_I: //��������������
				eSystemState = E_SetParam_SetCtI;
				break;
				case IDM_SetE_CLE: //�������������
				eSystemState = E_SetParam_SetECle;
				break;
				case IDM_DisDIS_E: //������ʾ����
				eSystemState = E_SetParam_DisDispE;
				break;
				case IDM_DisDIS_P: //������ʾ��ҳ����
				eSystemState = E_SetParam_DisDisP;
				break;
				case IDM_DisB_LED: //������ʾ��������
				eSystemState = E_SetParam_DisBLed;
				break;
				case IDM_ConnAdd: //����ͨ�ŵ�ַ����
				eSystemState = E_SetParam_ConnAdd;
				break;
				case IDM_ConnData: //����ͨѶУ��λ����
				eSystemState = E_SetParam_ConnData;
				break;
				case IDM_ConnBud: //����ͨѶ����������
				eSystemState = E_SetParam_ConnBud;
				break;
			}
		}
		else //��ʾ��1��˵����߽���2��˵���
		{
			byMenuRow = byMenuColumn + 1;
			byMenuColumn = 0;
		}
		break;
	}
	
	switch (byaMenu[byMenuRow][byMenuColumn])	 //���ݵ�ǰ�˵�ָ��������ʾ����
	{
		case IDM_Code:
		fnOutputChar(0, "code");
		fnOutputChar(1, "    ");
		fnOutputChar(2, "    ");
		break;
		case IDM_Set:
		fnOutputChar(0, "set");
		fnOutputChar(1, "    ");
		fnOutputChar(2, "    ");
		break;
		case IDM_Dis:
		fnOutputChar(0, "dis");
		fnOutputChar(1, "    ");
		fnOutputChar(2, "    ");
		break;
		case IDM_Conn:
		fnOutputChar(0, "conn");
		fnOutputChar(1, "    ");
		fnOutputChar(2, "    ");
		break;
		case IDM_CodeSet:
		fnOutputChar(0, "code");
		fnOutputChar(1, "set");
		fnOutputChar(2, "    ");
		break;
		case IDM_SetNet:
		fnOutputChar(0, "set");
		fnOutputChar(1, "net");
		fnOutputChar(2, "    ");
		break;
		case IDM_SetPT_U:
		fnOutputChar(0, "set");
		fnOutputChar(1, "pt .u");
		fnOutputChar(2, "    ");
		break;
		case IDM_SetCT_I:
		fnOutputChar(0, "set");
		fnOutputChar(1, "ct .i");
		fnOutputChar(2, "    ");
		break;
		case IDM_SetE_CLE:
		fnOutputChar(0, "set");
		fnOutputChar(1, "e.cle");
		fnOutputChar(2, "    ");
		break;
		case IDM_DisDIS_E:
		fnOutputChar(0, "dis");
		fnOutputChar(1, "dis.e");
		fnOutputChar(2, "    ");
		break;
		case IDM_DisDIS_P:
		fnOutputChar(0, "dis");
		fnOutputChar(1, "dis.p");
		fnOutputChar(2, "    ");
		break;
		case IDM_DisB_LED:
		fnOutputChar(0, "dis");
		fnOutputChar(1, "b.led");
		fnOutputChar(2, "    ");
		break;
		case IDM_ConnAdd:
		fnOutputChar(0, "conn");
		fnOutputChar(1, "add");
		fnOutputChar(2, "    ");
		break;
		case IDM_ConnData:
		fnOutputChar(0, "conn");
		fnOutputChar(1, "data");
		fnOutputChar(2, "    ");
		break;
		case IDM_ConnBud:
		fnOutputChar(0, "dis");
		fnOutputChar(1, "bud");
		fnOutputChar(2, "    ");
		break;
	}
}

//��������
unsigned char fnSaveSet(unsigned char byMessage)
{
	switch(byMessage)
	{
		case ID_KEYMSG_EnterDown:
		return 1;
		break;
		case ID_KEYMSG_MenuDown:
		return 0;
		break;
	}
	return 0;
}


//���õ�ѹ������Χ
float fnSetUValue(ty_uscl eUScl)
{
	switch(eUScl)
	{
		case E_100V:
		return 100.0;
		break;
		case E_220V:
		return 220.0;
		break;
		case E_380V:
		return 380.0;
		break;
		default:
		return 380.0;
		break;
	}
}
//���õ���������Χ
float fnSetIValue(ty_iscl eIScl)
{
	switch(eIScl)
	{
		case E_1A:
		return 1.0;
		break;
		case E_5A:
		return 5.0;
		break;
		default:
		return 1.0;
		break;
	}
}


unsigned int fnGetCodePut(unsigned char byMessage) //��ȡ��֤����
{
	//��ʼ��
	static char byConfirmPassword[4] = {'0', '0', '0', '0'};
	static unsigned char byEditDigit = 0;
	
	fnSetFlash(byEditDigit);
	switch(byMessage)
	{
		
		case ID_KEYMSG_LeftDown: //����Left,��֤������Ӧ�༭λ����
		byConfirmPassword[byEditDigit] = byConfirmPassword[byEditDigit] == '0'?'9':(byConfirmPassword[byEditDigit] - 1);
		break;
		case ID_KEYMSG_RightDown: //����Right,��֤������Ӧ�༭λ����
		byConfirmPassword[byEditDigit] = byConfirmPassword[byEditDigit] == '9'?'0':(byConfirmPassword[byEditDigit] + 1);
		break;
		case ID_KEYMSG_MenuDown: //����Menu����������
		fnDisableFlash();
		byConfirmPassword[0] = '0';
		byConfirmPassword[1] = '0';
		byConfirmPassword[2] = '0';
		byConfirmPassword[3] = '0';
		byEditDigit = 0;
		if (sOSParameter.uCode == fnComposeNum(byConfirmPassword))
		{
			sOSParameter.byEnablePro = 1;
		}
		else
		{
			sOSParameter.byEnablePro = 0;
		}
		eSystemState = E_MenuChoose;
		break;
		case ID_KEYMSG_EnterDown: //����Enter���ı�����༭λ
		byEditDigit ++;
		byEditDigit %= 4;
		fnSetFlash(byEditDigit);
		break;
	}
	
	fnOutputChar(0, "code");
	fnOutputChar(1, "put ");
	fnOutputChar(2, byConfirmPassword);
}

//�޸����룬������֤�ɹ������޸����롣
unsigned int fnGetCodeSet(unsigned char byMessage)
{
	static char byConfirmPassword[4] = {'0', '0', '0', '0'};
	static unsigned char byEditDigit = 0;
	unsigned int uCode;
	
	fnSetFlash(byEditDigit);
	switch(byMessage)
	{
		//����Left,��֤������Ӧ�༭λ����
		case ID_KEYMSG_LeftDown:
		byConfirmPassword[byEditDigit] = byConfirmPassword[byEditDigit]=='0'?'9':(byConfirmPassword[byEditDigit]-1);
		break;
		//����Right,��֤������Ӧ�༭λ����
		case ID_KEYMSG_RightDown:
		byConfirmPassword[byEditDigit] = byConfirmPassword[byEditDigit]=='9'?'0':(byConfirmPassword[byEditDigit]+1);
		break;
		//����Menu����������
		case ID_KEYMSG_MenuDown:
		fnDisableFlash();
		uCode = fnComposeNum(byConfirmPassword);
		if (uCode)
		{
			sOSParameter.uCode = uCode;
		}
		byEditDigit = 0;
		byConfirmPassword[0] = '0';
		byConfirmPassword[1] = '0';
		byConfirmPassword[2] = '0';
		byConfirmPassword[3] = '0';
		eSystemState = E_MenuChoose;
		break;
		//����Enter���ı�����༭λ
		case ID_KEYMSG_EnterDown:
		byEditDigit ++;
		byEditDigit %= 4;
		fnSetFlash(byEditDigit);
		break;
	}
	
	fnOutputChar(0, "code");
	fnOutputChar(1, "set ");
	fnOutputChar(2, byConfirmPassword);
}

//���磬ѡ������źŵ��������硣�������ز����źŵ���������ֵ
unsigned int fnGetSetNet(unsigned char byMessage)
{
	switch(byMessage)
	{
		//����Left,��֤������Ӧ�༭λ����
		case ID_KEYMSG_LeftDown:
		if (sOSParameter.eNet == E_N33)
		{
			sOSParameter.eNet = E_N34;
		}
		else
		{
			sOSParameter.eNet = E_N33;
		}
		break;
		//����Right,��֤������Ӧ�༭λ����
		case ID_KEYMSG_RightDown:
		if (sOSParameter.eNet == E_N33)
		{
			sOSParameter.eNet = E_N34;
		}
		else
		{
			sOSParameter.eNet = E_N33;
		}
		break;
		//����Menu����������
		case ID_KEYMSG_MenuDown:
		eSystemState = E_MenuChoose;
		break;
		//����Enter���ı�����༭λ
		case ID_KEYMSG_EnterDown:
		break;
	}
	
	fnOutputChar(0, "set ");
	fnOutputChar(1, "net ");
	if (sOSParameter.eNet == E_N33)
	{
		fnOutputChar(2, "n.33 ");
	}
	else
	{
		fnOutputChar(2, "n.34 ");
	}
}


unsigned int fnGetSetPT_U(unsigned char byMessage) //��ѹ��ȣ����õ�ѹ�źű��=1�ο̶�/2�ο̶ȣ�����10KV/100V=100���������ص�ѹ���ֵ
{
	static char cTemp[4] = {'0', '0', '0', '0'};
	static unsigned char byEditDigit = 0;
	unsigned int uTemp;
	
	fnSetFlash(byEditDigit);
	switch(byMessage)
	{
		//����Left,��֤������Ӧ�༭λ����
		case ID_KEYMSG_LeftDown:
		cTemp[byEditDigit] = cTemp[byEditDigit]=='0'?'9':(cTemp[byEditDigit]-1);
		break;
		//����Right,��֤������Ӧ�༭λ����
		case ID_KEYMSG_RightDown:
		cTemp[byEditDigit] = cTemp[byEditDigit]=='9'?'0':(cTemp[byEditDigit]+1);
		break;
		//����Menu����������
		case ID_KEYMSG_MenuDown:
		fnDisableFlash();
		uTemp = fnComposeNum(cTemp);
		if (uTemp > 5000)
		{
			sOSParameter.uRU = 5000;
		}
		else if (uTemp > 0 && uTemp <= 5000)
		{
			sOSParameter.uRU = uTemp;
		}
		byEditDigit = 0;
		cTemp[0] = '0';
		cTemp[1] = '0';
		cTemp[2] = '0';
		cTemp[3] = '0';
		eSystemState = E_MenuChoose;
		break;
		//����Enter���ı�����༭λ
		case ID_KEYMSG_EnterDown:
		byEditDigit ++;
		byEditDigit %= 4;
		fnSetFlash(byEditDigit);
		break;
	}
	
	fnOutputChar(0, "set ");
	fnOutputChar(1, "pt .u ");
	fnOutputChar(2, cTemp);
}


unsigned int fnGetSetCT_I(unsigned char byMessage) //��ȡ�������ֵ
{
	static char cTemp[4] = {'0', '0', '0', '0'};
	static unsigned char byEditDigit = 0;
	unsigned int uTemp;
	
	fnSetFlash(byEditDigit);
	switch(byMessage)
	{
		//����Left,��֤������Ӧ�༭λ����
		case ID_KEYMSG_LeftDown:
		cTemp[byEditDigit] = cTemp[byEditDigit]=='0'?'9':(cTemp[byEditDigit]-1);
		break;
		//����Right,��֤������Ӧ�༭λ����
		case ID_KEYMSG_RightDown:
		cTemp[byEditDigit] = cTemp[byEditDigit]=='9'?'0':(cTemp[byEditDigit]+1);
		break;
		//����Menu����������
		case ID_KEYMSG_MenuDown:
		fnDisableFlash();
		uTemp = fnComposeNum(cTemp);
		if (uTemp > 5000)
		{
			sOSParameter.uRI = 5000;
		}
		else if (uTemp > 0 && uTemp <= 5000)
		{
			sOSParameter.uRI = uTemp;
		}
		byEditDigit = 0;
		cTemp[0] = '0';
		cTemp[1] = '0';
		cTemp[2] = '0';
		cTemp[3] = '0';
		eSystemState = E_MenuChoose;
		break;
		//����Enter���ı�����༭λ
		case ID_KEYMSG_EnterDown:
		byEditDigit ++;
		byEditDigit %= 4;
		fnSetFlash(byEditDigit);
		break;
	}
	
	fnOutputChar(0, "set ");
	fnOutputChar(1, "ct .i ");
	fnOutputChar(2, cTemp);
}


unsigned int fnGetSetE_CLE(unsigned char byMessage) //��ȡ�����
{
	switch(byMessage)
	{
		//����Left,��֤������Ӧ�༭λ����
		case ID_KEYMSG_LeftDown:
		if (sOSParameter.bClearEnergy == 0)
		{
			sOSParameter.bClearEnergy = 1;
		}
		else
		{
			sOSParameter.bClearEnergy = 0;
		}
		break;
		//����Right,��֤������Ӧ�༭λ����
		case ID_KEYMSG_RightDown:
		if (sOSParameter.bClearEnergy == 0)
		{
			sOSParameter.bClearEnergy = 1;
		}
		else
		{
			sOSParameter.bClearEnergy = 0;
		}
		break;
		//����Menu����������
		case ID_KEYMSG_MenuDown:
		eSystemState = E_MenuChoose;
		break;
		//����Enter���ı�����༭λ
		case ID_KEYMSG_EnterDown:
		break;
	}
	
	fnOutputChar(0, "set ");
	fnOutputChar(1, "net ");
	if (sOSParameter.bClearEnergy == 1)
	{
		fnOutputChar(2, "yes ");
	}
	else
	{
		fnOutputChar(2, "no  ");
	}
}


unsigned char fnGetDisB_LED(unsigned char byMessage) //����������ʾ
{
	switch(byMessage)
	{
		case ID_KEYMSG_LeftDown: //����Left,��֤������Ӧ�༭λ���٣�����δд�꣡��
		sOSParameter.byDisp --;
		break;
		case ID_KEYMSG_RightDown://����Right,��֤������Ӧ�༭λ���ӣ�����δд�꣡��
		sOSParameter.byDisp ++;
		break;
		case ID_KEYMSG_MenuDown://����Menu����������
		break;
		case ID_KEYMSG_EnterDown://����Enter���ı�����༭λ
		break;
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
