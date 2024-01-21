/*
 * Fm25040.h
 *
 * Created: 2015-11-05 10:06:22
 *  Author: ZhengGaoHuan
 */ 


#ifndef FM25040_H_
#define FM25040_H_

typedef enum{E_N33,E_N34} ty_net;
typedef enum{E_1A,E_5A}ty_iscl;
typedef enum{E_100V,E_220V,E_380V}ty_uscl;
typedef enum{E_4800,E_9600} ty_baud;
		
typedef struct  
{
	unsigned char byEnablePro; //菜单编程是用于确认用户输入的密码是否正确
	unsigned int uCode; //菜单编程密码
	ty_net eNet; //测试信号的输入网络
	ty_iscl eIScl; //电流测试范围
	ty_uscl eUScl; //电压测试范围
	unsigned int uRU; //电流倍率
	unsigned int uRI; //电压倍率
	unsigned char bClearEnergy; //是否清电能
	unsigned char byDisplayEnable; //显示屏是否一直显示
	unsigned char byDisplayPage; //显示翻页
	unsigned char byDisp; //显示亮度
	unsigned char byConnectAdd; //仪表通讯地址
	unsigned char byConnectData; //仪表通讯波特率
	ty_baud eBaud; //通讯波特率
} ty_osparameter;


void fnFm25040Init(void); //fm25040初始化
unsigned int fnReadCodeFromFm25040(void); //从Fm25040中读密码
//往Fm25040中写密码
void fnWriteCodeToFm25040(unsigned int uCode);
//从Fm25040中读输入网络
ty_net fnReadNetFromFm25040(void);
//往Fm25040中写输入网络
void fnWriteNetToFm25040(ty_net eNet);
//从Fm25040中读电流范围
ty_iscl fnReadISclFromFm25040(void);
//往Fm25040中写电流范围
void fnWriteISclToFm25040(ty_iscl eIScl);
//从Fm25040中读电压范围
ty_uscl fnReadVSclFromFm25040(void);
//往Fm25040中写电压范围
void fnWriteVSclToFm25040(ty_uscl eUScl);
//从Fm25040中读电压倍率
unsigned int fnReadRUFromFm25040(void);
//往Fm25040中写电压倍率
void fnWriteRUToFm25040(unsigned int uRU);
//从Fm25040中读电流倍率
unsigned int fnReadRIFromFm25040(void);
//往Fm25040中写电流倍率
void fnWriteRIToFm25040(unsigned int uRI);
//从Fm25040中读波特率
ty_baud fnReadBaudFromFm25040(void);
//往Fm25040中写波特率
void fnWriteBaudToFm25040(ty_baud eBaud);
//从Fm25040中读显示亮度
unsigned char fnReadDispFromFm25040(void);
//往Fm25040中写显示亮度
void fnWriteDispToFm25040(unsigned char byDisp);

//从Fm25040中读全部系统参数
ty_osparameter fnReadOSParameteFromFm25040(void);
//往Fm25040中写全部系统参数
void fnWriteOSParameterToFm25040(ty_osparameter sOSParameter);

//往FM25040中写A相电流校正
void fnWriteCurAToFm25040(unsigned long int ulCurA);
//往FM25040中写B相电流校正
void fnWriteCurBToFm25040(unsigned long int ulCurB);
//往FM25040中写C相电流校正
void fnWriteCurCToFm25040(unsigned long int ulCurC);
//往FM25040中写A相电压校正
void fnWriteVolAToFm25040(unsigned long int ulVolA);
//往FM25040中写B相电压校正
void fnWriteVolBToFm25040(unsigned long int ulVolB);
//往FM25040中写C相电压校正
void fnWriteVolCToFm25040(unsigned long int ulVolC);

//从FM25040中读A相电流校正
unsigned long int fnReadCurAFromFm25040(void);
//从FM25040中读B相电流校正
unsigned long int fnReadCurBFromFm25040(void);
//从FM25040中读C相电流校正
unsigned long int fnReadCurCFromFm25040(void);
//从FM25040中读A相电压校正
unsigned long int fnReadVolAFromFm25040(void);
//从FM25040中读B相电压校正
unsigned long int fnReadVolBFromFm25040(void);
//从FM25040中读C相电压校正
unsigned long int fnReadVolCFromFm25040(void);

//输入单个字节到fm25040
void fnMOSIByteToFm25040(unsigned char byData);
//从fm25040接受单个字节
unsigned char fnMISOByteFromFm25040(void);
//fm25040写使能
void fnWREN(void);
//fm25040写复位，禁止所有的写操作
void fnWRDI(void);
//fm25040读状态寄存器
unsigned char fnRDSR(void);
//fm25040写状态寄存器
void fnWRSR(unsigned char byData);
//fm25040读内存
void fnREAD(unsigned int iAddress, unsigned char * pbyData, int iCount);
//fm25040写内存
void fnWRITE(unsigned int iAddress, unsigned char * pbyData, int iCount);


#endif /* FM25040_H_ */