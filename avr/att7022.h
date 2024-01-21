/*
 * Att7022.h
 *
 * Created: 2015-11-30 16:56:51
 *  Author: ZhengGaoHuan
 */ 


#ifndef ATT7022_H_
#define ATT7022_H_


#define ID_ATT7022CMD_STARTCHECK 0x80
#define ID_ATT7022CMD_CLEARCHECK 0xC3
#define ID_ATT7022CMD_RESET 0xD3

typedef struct
{
	unsigned long int hfconst;
	unsigned long int iregchg;
	unsigned long int iregion1;
	unsigned long int iregion2;
	unsigned long int iregion3;
	unsigned long int iregion4;
	unsigned long int pgaina0;
	unsigned long int pgaina1;
	unsigned long int pgainb0;
	unsigned long int pgainb1;
	unsigned long int pgainc0;
	unsigned long int pgainc1;
	unsigned long int phsrega0;
	unsigned long int phsrega1;
	unsigned long int phsrega2;
	unsigned long int phsrega3;
	unsigned long int phsrega4;
	unsigned long int phsregb0;
	unsigned long int phsregb1;
	unsigned long int phsregb2;
	unsigned long int phsregb3;
	unsigned long int phsregb4;
	unsigned long int phsregc0;
	unsigned long int phsregc1;
	unsigned long int phsregc2;
	unsigned long int phsregc3;
	unsigned long int phsregc4;
	unsigned long int ugaina;
	unsigned long int ugainb;
	unsigned long int ugainc;
	unsigned long int igaina;
	unsigned long int igainb;
	unsigned long int igainc;
}ty_checkmeterparameter;

//Att7022初始化
void fnAtt7022Init(void);
//设置 三相三线/三相四线，三相三线低电平，三相四线高电平
void fnSetAtt7022Sel(unsigned char byValue);
//写入校表数据
unsigned char fnWriteCheckParameterToAtt7022(ty_checkmeterparameter sCheckParameter);
//重启Att7022
void fnResterAtt7022(void);
//写数据到Att7022，包括一个地址和一个24位数据
void fnWriteAtt7022(unsigned char byAddr, unsigned long int ulValue);
//根据地址，从Att7022中读取一个24位数据
unsigned long int fnReadAtt7022(unsigned char byAddr);
//写特殊命令到Att7022中
void fnCmdAtt7022(unsigned char byOrder);






#endif /* ATT7022_H_ */