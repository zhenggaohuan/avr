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

#include <avr/io.h>

#include "InputAndOutput.h"
#include "Fm25040.h"
#include "Delay.h"
#include "att7022.h"

/*
*函数声明部分
*/



//设置电压测量范围
float fnSetUValue(ty_uscl eUScl);
//设置电流测量范围
float fnSetIValue(ty_iscl eIScl);
//数显表显示电流电压等信息
void fnAVMessage(unsigned char byMessage);
/*	编程设置参数，用户可根据实际情况选择适当的编程设置参数	*/
unsigned char fnMenuChoose(unsigned char byMessage);
/*	保存设置，保存修改过的参数	*/
unsigned char fnSaveSet(unsigned char byMessage);
//验证密码，当输入的密码正确时才可以进入编程。在函数中用户输入验证密码，并对密码进行验证
unsigned int fnGetCodePut(unsigned char byMessage);
//修改密码，密码验证成功才能修改密码。在函数中，用户输入要修改的密码，函数返回用户输入的修改密码
unsigned int fnGetCodeSet(unsigned char byMessage);
//网络，选择测量信号的输入网络。函数返回测量信号的输入网络值
unsigned int fnGetSetNet(unsigned char byMessage);
//电压变比，设置电压信号变比=1次刻度/2次刻度，例：10KV/100V=100，函数返回电压变比值
unsigned int fnGetSetPT_U(unsigned char byMessage);
//获取电流变比值
unsigned int fnGetSetCT_I(unsigned char byMessage);
//获取清电能
unsigned int fnGetSetE_CLE(unsigned char byMessage);
//获取显示
unsigned int fnGetDisDISP_E(unsigned char byMessage);
//获取显示翻页
unsigned int fnGetDisDIS_P(unsigned char byMessage);
//获取显示亮度
unsigned char fnGetDisB_LED(unsigned char byMessage);
//获取通讯参数地址
unsigned int fnGetConnAdd(unsigned char byMessage);
//获取通信参数通讯校验位
unsigned int fnGetConnData(unsigned char byMessage);
//获取通信参数通讯速率
unsigned int fnGetConnBud(unsigned char byMessage);


//调试程序
void fnDebugAtt7022(void);
//校表
void fnCheckAtt7022(void);
//清除参数
void fnClearAtt7022(void);

//把四代表，个位，十位，百位，千位的数字数组组成一个数字
unsigned int fnComposeNum(char * pbyaNum);
//把1个是数字分解成4个代表，个位，十位，百位，千位的数字数组
void fnDecomposeNum(unsigned int uNum, unsigned char *pbyaNum);

//程序按键消息处理，在系统状态处理程序里面先处理按键消息，按键消息使得系统状态转移，让后处理根据系统状态设置显示内容
void fnState(void);

/*全局变量声明*/

//系统状态:
enum {E_AVMessage,  //电流信息显示
	E_MenuChoose,  //菜单选择状态
	E_MenuSave,  //设置保持
	E_SetParam_CodePut,  //获取菜单密码，（已经由E_MenuPassword代替）
	E_SetParam_CodeSet,  //密码设置
	E_SetParam_SetNet,  //选择测试信号的输入网络
	E_SetParam_SetPtU,  //设置电压变比
	E_SetParam_SetCtI,  //设置电流变比
	E_SetParam_SetECle,  //清电能
	E_SetParam_DisDispE,  //设置显示
	E_SetParam_DisDisP,  //设置显示翻页
	E_SetParam_DisBLed,  //设置显示亮度
	E_SetParam_ConnAdd, //设置通讯地址
	E_SetParam_ConnData, //设置通讯校验码
	E_SetParam_ConnBud //设置通讯波特率
} eSystemState = E_AVMessage;

ty_osparameter sOSParameter;	 	//系统参数

unsigned char byMessage = 0;  //键盘按键消息

//定义各种进程运行标志，前台中断程序根据各种情况置位进程运行标志，后台程序根据置位的进程运行标志运行进程，并复位进程运行标志
unsigned char byFlagHalfSecond = 0;  //闪烁变量的处理标志
unsigned char byFlagDisplay = 0; //显示处理标志
unsigned char byFlagKey = 0; //键盘轮询标志
unsigned char byFlagState = 0; //用于系统状态的转移，整个系统是一个状态机的结构，所以每次按键或者时间定时到的时候，会发声状态转移
unsigned char byFlagCom = 0; //通讯状态标志，如果通讯中断程序有数据接收到，则置位通讯状态标志，后台程序处理通讯数据，复位通讯状态标志

//函数定义部分
int main(void)
{
	//初始化
	fnKeyInit();	//按键初始化
	fnDisplayInit(); //显示初始化

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
		if (byFlagState) //如果系统状态转移标志复位
		{
			byFlagState = 0; //系统状态标志置位
			fnState(); //在系统状态处理程序里面先处理按键消息，按键消息使得系统状态转移，让后处理根据系统状态设置显示内容
		}
		
	}
}


//定时器0 中断服务器函数
ISR(TIMER0_OVF_vect)
{
	TCNT0 = 126;
	byFlagDisplay = 1; //显示进程置位
}


//程序按键消息处理，在系统状态处理程序里面先处理按键消息，按键消息使得系统状态转移，让后处理根据系统状态设置显示内容
void fnState(void)
{
	//如果是有效按键，就让显示屏显示时间置位
	switch(eSystemState)//根据“系统状态表”进入相应函数处理“按键信息”，并产生“输出信息"
	{
		case E_AVMessage:  //电流信息显示状态
		fnAVMessage(byMessage);
		break;
		case E_MenuChoose:  //菜单选择状态
		fnMenuChoose(byMessage);
		break;
		case E_MenuSave: //保存菜单设置
		break;
		case E_SetParam_CodePut: //验证密码
		break;
		case E_SetParam_CodeSet: //密码设置
		fnGetCodeSet(byMessage);
		break;
		case E_SetParam_SetNet: //选择测试信号的输入网络
		break;
		case E_SetParam_SetPtU: //电压变比
		break;
		case E_SetParam_SetCtI: //电流变比
		break;
		case E_SetParam_SetECle: //清电能
		break;
		case E_SetParam_DisDispE: //显示设置，显示
		break;
		case E_SetParam_DisDisP: //显示设置，显示翻页
		break;
		case E_SetParam_DisBLed: //显示设置，显示亮度
		break;
		case E_SetParam_ConnAdd: //通讯地址
		break;
		case E_SetParam_ConnData: //通讯校验位
		break;
		case E_SetParam_ConnBud: //通讯波特率
		break;
	}
}





//校表程序
/*
void fnDebugAtt7022(void)
{
//菜单结构
const unsigned char byaMenu[2] = {IDM_CHE, IDM_CLE};
unsigned char byMenuColumn = 0;

//清空屏幕
fnCleanOutput(2);
fnCleanOutput(1);
fnOutputChar(0,byaMenu[byMenuColumn]);

//进入按键处理
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

//校表
void fnCheckAtt7022(void)
{
//菜单结构
unsigned char byCheckI = 0, byCheckU = 0;

//显示校对电流提示信息，为写完
//	fnOutput(ID_OUTPUT_TYPE_CHAR,2,IDM_NULL);
//	fnOutput(ID_OUTPUT_TYPE_CHAR,1,IDM_NULL);
//	fnOutput(ID_OUTPUT_TYPE_CHAR,0,IDM_NULL);

//校对电流
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
//校表，未写完
byCheckI = 1;
break;
}
}

//显示校对电压提示信息，未写完
//	fnOutput(ID_OUTPUT_TYPE_CHAR,2,IDM_NULL);
//	fnOutput(ID_OUTPUT_TYPE_CHAR,1,IDM_NULL);
//	fnOutput(ID_OUTPUT_TYPE_CHAR,0,IDM_NULL);

//校对电压
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
//校表，未写完
byCheckU= 1;
break;
}
}

//校表完毕，清空显示，退出
fnCleanOutput(2);
fnCleanOutput(1);
fnCleanOutput(0);
}

//清除参数
void fnClearAtt7022(void)
{
//菜单结构
const unsigned char byaMenu[2] = {IDM_NO, IDM_YES};
unsigned char byMenuColumn = 0;

//清空屏幕
fnCleanOutput(2);
fnOutputChar(1,byaMenu[byMenuColumn]);

//进入按键处理
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
//插入函数，清除Att7022表的内容
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


//电流电压信息显示
void fnAVMessage(unsigned char byMessage)
{
	unsigned char byMeterDis = 1;
	
	switch(byMessage)
	{
		case ID_KEYMSG_LeftDown:			//按下Left
		(byMeterDis>1)?(byMeterDis--):(byMeterDis = 16);
		break;
		case ID_KEYMSG_RightDown:			//按下Right
		(byMeterDis<16)?(byMeterDis++):(byMeterDis =1);
		break;
		case ID_KEYMSG_MenuDown:			//按下Menu，进入编程状态
		eSystemState = E_MenuChoose; //将系统状态调整到密码输入状态
		break;
		case ID_KEYMSG_EnterDown:			//按下Enter
		break;
	}

	switch(byMeterDis)
	{
		case 1: //第一页面，三相电压
		fnOutputNum(0,12);
		fnOutputNum(1,0.06);
		fnOutputNum(2,12.34);
		fnOutputIndicator(IDD_V);
		break;
		case 2: //第二页面，三相电流
		fnOutputNum(0,byMeterDis);
		fnOutputNum(1,byMeterDis);
		fnOutputNum(2,byMeterDis);
		fnOutputIndicator(IDD_A);
		break;
		case 3: //第三页面，三相功率、功率因素
		fnOutputNum(0,byMeterDis);
		fnOutputNum(1,byMeterDis);
		fnOutputNum(2,byMeterDis);
		fnOutputIndicator(IDD_W);
		break;
		case 4: //第四页面，视在功率、频率
		fnOutputNum(0,byMeterDis);
		fnOutputNum(1,byMeterDis);
		fnOutputNum(2,byMeterDis);
		fnOutputIndicator(IDD_DI);
		break;
		case 5: //第五页面，正向有功电能
		fnOutputNum(0,byMeterDis);
		fnOutputNum(1,byMeterDis);
		fnOutputNum(2,byMeterDis);
		fnOutputIndicator(IDD_NULL);
		break;
		case 6: //第六页面，反向有功电能
		fnOutputNum(0,byMeterDis);
		fnOutputNum(1,byMeterDis);
		fnOutputNum(2,byMeterDis);
		break;
		case 7: //第七页面，正向有功电能
		fnOutputNum(0,byMeterDis);
		fnOutputNum(1,byMeterDis);
		fnOutputNum(2,byMeterDis);
		break;
		case 8: //第八页面，反向无功电能
		fnOutputNum(0,byMeterDis);
		fnOutputNum(1,byMeterDis);
		fnOutputNum(2,byMeterDis);
		break;
		case 9: //第九页面，A相电压谐波含量
		fnOutputNum(0,byMeterDis);
		fnOutputNum(1,byMeterDis);
		fnOutputNum(2,byMeterDis);
		break;
		case 10: //第十页面，B相电压谐波含量
		fnOutputNum(0,byMeterDis);
		fnOutputNum(1,byMeterDis);
		fnOutputNum(2,byMeterDis);
		break;
		case 11: //第十一页面，C相电压谐波含量
		fnOutputNum(0,byMeterDis);
		fnOutputNum(1,byMeterDis);
		fnOutputNum(2,byMeterDis);
		break;
		case 12: //第十二页面，A相电流谐波含量
		fnOutputNum(0,byMeterDis);
		fnOutputNum(1,byMeterDis);
		fnOutputNum(2,byMeterDis);
		break;
		case 13://第十三页面，B相电流谐波含量
		fnOutputNum(0,byMeterDis);
		fnOutputNum(1,byMeterDis);
		fnOutputNum(2,byMeterDis);
		break;
		case 14://第十四页面，C相电流谐波含量
		fnOutputNum(0,byMeterDis);
		fnOutputNum(1,byMeterDis);
		fnOutputNum(2,byMeterDis);
		break;
		case 15://第十五页面，三相电压总不平衡度
		fnOutputNum(0,byMeterDis);
		fnOutputNum(1,byMeterDis);
		fnOutputNum(2,byMeterDis);
		break;
		case 16://第十六页面，三相电流总不平衡度
		fnOutputNum(0,byMeterDis);
		fnOutputNum(1,byMeterDis);
		fnOutputNum(2,byMeterDis);
		break;
	}
}


//编程设置参数
unsigned char fnMenuChoose(unsigned char byMessage)
{

	//菜单结构
	const unsigned char byaMenu[5][5] = {
		{IDM_Code, IDM_Set, IDM_Dis, IDM_Conn, IDM_NULL},
		{IDM_CodePut, IDM_CodeSet, IDM_NULL, IDM_NULL, IDM_NULL},
		{IDM_SetNet, IDM_SetPT_U, IDM_SetCT_I, IDM_SetE_CLE, IDM_NULL},
		{IDM_DisDIS_E, IDM_DisDIS_P, IDM_DisB_LED,IDM_NULL, IDM_NULL},
		{IDM_ConnAdd, IDM_ConnData, IDM_ConnBud, IDM_NULL, IDM_NULL}
	};

	//菜单位置，g_byaMenu[5][5]菜单的位置
	static unsigned char byMenuRow = 0;
	static unsigned char byMenuColumn = 0;

	//对系统参数进行备份
	//ty_osparameter sNewOSParameter = *psOSParameter;

	switch(byMessage)//对按键消息做处理，改变菜单的位置。
	{
		case ID_KEYMSG_LeftDown://按下Left，进入同级左菜单
		if(byMenuColumn)
		{
			byMenuColumn--;
		}
		break;
		case ID_KEYMSG_RightDown://按下Right，进入同级右菜单
		if(byaMenu[byMenuRow][byMenuColumn+1])
		{
			byMenuColumn ++;
		}
		break;
		case ID_KEYMSG_MenuDown:	//按下Menu，返回上级菜单
		if(byMenuRow) //如果在二层菜单，则返回一层菜单
		{
			byMenuColumn = byMenuRow - 1;
			byMenuRow = 0;
		}
		else //在一层菜单，退出保存，***注意，要现实按3次menu健才能退到菜单保存页面
		{
			
			eSystemState = E_MenuSave;
		}
		break;
		case ID_KEYMSG_EnterDown: //按下Enter，进入下级菜单
		if(byMenuRow) //如果在二层菜单，则进入相应函数，否则表示在一层菜单，进入相应二层菜单
		{
			switch(byaMenu[byMenuRow][byMenuColumn])
			{
				case IDM_CodeSet: //进入修改密码设置
				eSystemState = E_SetParam_CodeSet;
				break;
				case IDM_SetNet: //进入网络设置
				eSystemState = E_SetParam_SetNet;
				break;
				case IDM_SetPT_U: // 进入电压变比设置
				eSystemState = E_SetParam_SetPtU;
				break;
				case IDM_SetCT_I: //进入电流变比设置
				eSystemState = E_SetParam_SetCtI;
				break;
				case IDM_SetE_CLE: //进入清电能设置
				eSystemState = E_SetParam_SetECle;
				break;
				case IDM_DisDIS_E: //进入显示设置
				eSystemState = E_SetParam_DisDispE;
				break;
				case IDM_DisDIS_P: //进入显示翻页设置
				eSystemState = E_SetParam_DisDisP;
				break;
				case IDM_DisB_LED: //进入显示亮度设置
				eSystemState = E_SetParam_DisBLed;
				break;
				case IDM_ConnAdd: //进入通信地址设置
				eSystemState = E_SetParam_ConnAdd;
				break;
				case IDM_ConnData: //进入通讯校验位设置
				eSystemState = E_SetParam_ConnData;
				break;
				case IDM_ConnBud: //进入通讯波特率设置
				eSystemState = E_SetParam_ConnBud;
				break;
			}
		}
		else //表示在1层菜单，者进入2层菜单。
		{
			byMenuRow = byMenuColumn + 1;
			byMenuColumn = 0;
		}
		break;
	}
	
	switch (byaMenu[byMenuRow][byMenuColumn])	 //根据当前菜单指针设置显示界面
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

//保存设置
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


//设置电压测量范围
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
//设置电流测量范围
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


unsigned int fnGetCodePut(unsigned char byMessage) //获取验证密码
{
	//初始化
	static char byConfirmPassword[4] = {'0', '0', '0', '0'};
	static unsigned char byEditDigit = 0;
	
	fnSetFlash(byEditDigit);
	switch(byMessage)
	{
		
		case ID_KEYMSG_LeftDown: //按下Left,验证密码相应编辑位减少
		byConfirmPassword[byEditDigit] = byConfirmPassword[byEditDigit] == '0'?'9':(byConfirmPassword[byEditDigit] - 1);
		break;
		case ID_KEYMSG_RightDown: //按下Right,验证密码相应编辑位增加
		byConfirmPassword[byEditDigit] = byConfirmPassword[byEditDigit] == '9'?'0':(byConfirmPassword[byEditDigit] + 1);
		break;
		case ID_KEYMSG_MenuDown: //按下Menu，返回密码
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
		case ID_KEYMSG_EnterDown: //按下Enter，改变参数编辑位
		byEditDigit ++;
		byEditDigit %= 4;
		fnSetFlash(byEditDigit);
		break;
	}
	
	fnOutputChar(0, "code");
	fnOutputChar(1, "put ");
	fnOutputChar(2, byConfirmPassword);
}

//修改密码，密码验证成功才能修改密码。
unsigned int fnGetCodeSet(unsigned char byMessage)
{
	static char byConfirmPassword[4] = {'0', '0', '0', '0'};
	static unsigned char byEditDigit = 0;
	unsigned int uCode;
	
	fnSetFlash(byEditDigit);
	switch(byMessage)
	{
		//按下Left,验证密码相应编辑位减少
		case ID_KEYMSG_LeftDown:
		byConfirmPassword[byEditDigit] = byConfirmPassword[byEditDigit]=='0'?'9':(byConfirmPassword[byEditDigit]-1);
		break;
		//按下Right,验证密码相应编辑位增加
		case ID_KEYMSG_RightDown:
		byConfirmPassword[byEditDigit] = byConfirmPassword[byEditDigit]=='9'?'0':(byConfirmPassword[byEditDigit]+1);
		break;
		//按下Menu，返回密码
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
		//按下Enter，改变参数编辑位
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

//网络，选择测量信号的输入网络。函数返回测量信号的输入网络值
unsigned int fnGetSetNet(unsigned char byMessage)
{
	switch(byMessage)
	{
		//按下Left,验证密码相应编辑位减少
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
		//按下Right,验证密码相应编辑位增加
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
		//按下Menu，返回密码
		case ID_KEYMSG_MenuDown:
		eSystemState = E_MenuChoose;
		break;
		//按下Enter，改变参数编辑位
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


unsigned int fnGetSetPT_U(unsigned char byMessage) //电压变比，设置电压信号变比=1次刻度/2次刻度，例：10KV/100V=100，函数返回电压变比值
{
	static char cTemp[4] = {'0', '0', '0', '0'};
	static unsigned char byEditDigit = 0;
	unsigned int uTemp;
	
	fnSetFlash(byEditDigit);
	switch(byMessage)
	{
		//按下Left,验证密码相应编辑位减少
		case ID_KEYMSG_LeftDown:
		cTemp[byEditDigit] = cTemp[byEditDigit]=='0'?'9':(cTemp[byEditDigit]-1);
		break;
		//按下Right,验证密码相应编辑位增加
		case ID_KEYMSG_RightDown:
		cTemp[byEditDigit] = cTemp[byEditDigit]=='9'?'0':(cTemp[byEditDigit]+1);
		break;
		//按下Menu，返回密码
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
		//按下Enter，改变参数编辑位
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


unsigned int fnGetSetCT_I(unsigned char byMessage) //获取电流变比值
{
	static char cTemp[4] = {'0', '0', '0', '0'};
	static unsigned char byEditDigit = 0;
	unsigned int uTemp;
	
	fnSetFlash(byEditDigit);
	switch(byMessage)
	{
		//按下Left,验证密码相应编辑位减少
		case ID_KEYMSG_LeftDown:
		cTemp[byEditDigit] = cTemp[byEditDigit]=='0'?'9':(cTemp[byEditDigit]-1);
		break;
		//按下Right,验证密码相应编辑位增加
		case ID_KEYMSG_RightDown:
		cTemp[byEditDigit] = cTemp[byEditDigit]=='9'?'0':(cTemp[byEditDigit]+1);
		break;
		//按下Menu，返回密码
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
		//按下Enter，改变参数编辑位
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


unsigned int fnGetSetE_CLE(unsigned char byMessage) //获取清电能
{
	switch(byMessage)
	{
		//按下Left,验证密码相应编辑位减少
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
		//按下Right,验证密码相应编辑位增加
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
		//按下Menu，返回密码
		case ID_KEYMSG_MenuDown:
		eSystemState = E_MenuChoose;
		break;
		//按下Enter，改变参数编辑位
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


unsigned char fnGetDisB_LED(unsigned char byMessage) //设置亮度显示
{
	switch(byMessage)
	{
		case ID_KEYMSG_LeftDown: //按下Left,验证密码相应编辑位减少，！！未写完！！
		sOSParameter.byDisp --;
		break;
		case ID_KEYMSG_RightDown://按下Right,验证密码相应编辑位增加，！！未写完！！
		sOSParameter.byDisp ++;
		break;
		case ID_KEYMSG_MenuDown://按下Menu，返回密码
		break;
		case ID_KEYMSG_EnterDown://按下Enter，改变参数编辑位
		break;
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
