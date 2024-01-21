/*
 * Att7022.c
 *
 * Created: 2015-11-30 16:56:31
 *  Author: ZhengGaoHuan
 */ 

#include <avr/io.h>
#include "att7022.h"
#include "Delay.h"


//Att7022初始化
void fnAtt7022Init(void)
{
	DDRE |= 1<<DDRE0;
	
	DDRE |= 1<<DDRE1;
	PORTE |= 1<<PORTE1;
	
	DDRE |= 1<<DDRE2;
	PORTE &= ~(1<<PORTE2);	
	
	DDRE |= 1<<DDRE3;
	PORTE &= ~(1<<PORTE3);
	
	DDRE &= ~(1<<DDRE4);
	
	DDRE &= ~(1<<DDRE5);
	
	DDRE |= 1<<DDRE6;
	PORTE |= 1<<PORTE6;
	
	DDRE &= ~(1<<DDRE7); 
}

//设置 三相三线/三相四线，三相三线低电平，三相四线高电平
void fnSetAtt7022Sel(unsigned char byValue)
{
	PORTE = byValue?(PORTE|(1<<PORTE0)):(PORTE&~(1<<PORTE0));
}

//写入校表数据
unsigned char fnWriteCheckParameterToAtt7022(ty_checkmeterparameter sCheckParameter)
{
	
}

//重启Att7022
void fnResterAtt7022(void)
{
}


//写数据到Att7022，包括一个地址和一个24位数据
void fnWriteAtt7022(unsigned char byAddr, unsigned long int ulValue)
{
	byAddr |= 1<<7;
	byAddr &= ~(1<<6);
	
	PORTE |= 1<<PORTE1;  //CS=1
	PORTE &= ~(1<<PORTE2); //SCLK=0;
	PORTE &= ~(1<<PORTE3); //DIN=0;
	fnDelayMs();
	PORTE &= ~(1<<PORTE1); //CS=0;
	fnDelayMs();

	//写地址
	for (int i = 0; i < 8; i ++)
	{
		//SCLK上升沿，MUC写数据
		PORTE |= 1<<PORTE2;
		PORTE = (byAddr&0x80)?(PORTE|(1<<PORTE3)):(PORTE& ~(1<<PORTE3));
		byAddr <<= 1;
		//SCLK下降沿，Att7022读数据
		fnDelayUs();
		PORTE &= ~(1<<PORTE2);
		fnDelayUs();
	}

	//写数据
	for (int i = 0; i < 24; i ++)
	{
		//SCLK上升沿，MUC写数据
		PORTE |= 1<<PORTE2;
		PORTE = (ulValue&0x800000)?(PORTE|(1<<PORTE3)):(PORTE& ~(1<<PORTE3));
		ulValue <<= 1;
		//SCLK下降沿，Att7022读数据
		fnDelayUs();
		PORTE &= ~(1<<PORTE2);
		fnDelayUs();
	}
	
	PORTE &= ~(1<<PORTE3); //DIN=0;
	PORTE &= ~(1<<PORTE2); //SCLK=0;
	fnDelayUs();
	PORTE |= 1<<PORTE1;  //CS=1
}

//根据地址，从Att7022中读取一个24位数据
unsigned long int fnReadAtt7022(unsigned char byAddr)
{
	unsigned long int ulValue = 0;
	byAddr &= ~(1<<7);
	
	PORTE |= 1<<PORTE1;  //CS=1
	PORTE &= ~(1<<PORTE2); //SCLK=0;
	PORTE &= ~(1<<PORTE3); //DIN=0;
	fnDelayMs();
	PORTE &= ~(1<<PORTE1); //CS=0;
	fnDelayMs();

	//写地址
	for (int i = 0; i < 8; i ++)
	{
		//SCLK上升沿，MUC写数据
		PORTE |= 1<<PORTE2;
		PORTE = (byAddr&0x80)?(PORTE|(1<<PORTE3)):(PORTE& ~(1<<PORTE3));
		byAddr <<= 1;
		//SCLK下降沿，Att7022读数据
		fnDelayUs();
		PORTE &= ~(1<<PORTE2);
		fnDelayUs();
	}
	
	//等待一段时间，时间可能还要设置下，注意时间长短，
	PORTE &= ~(1<<PORTE3);//DIN=0;
	fnDelayMs();
	
	//读数据
	for (int i = 0; i < 24; i ++)
	{
		//SCLK上升沿，Att7022写数据
		PORTE |= 1<<PORTE2;
		//SCLK下降沿，MUC读数据
		fnDelayUs();
		PORTE &= ~(1<<PORTE2);
		ulValue <<= 1;
		ulValue |= (PINE&0x10)?1:0;
		fnDelayUs();
	}
	
	//收尾
	PORTE &= ~(1<<PORTE3); //DIN=0;
	PORTE &= ~(1<<PORTE2); //SCLK=0;
	fnDelayUs();
	PORTE |= 1<<PORTE1;  //CS=1

	return ulValue;
}

//写特殊命令到Att7022中
void fnCmdAtt7022(unsigned char byOrder)
{
	PORTE |= 1<<PORTE1;  //CS=1
	PORTE &= ~(1<<PORTE2); //SCLK=0;
	PORTE &= ~(1<<PORTE3); //DIN=0;
	fnDelayMs();
	PORTE &= ~(1<<PORTE1); //CS=0;
	fnDelayMs();
	
	//写地址
	for (int i = 0; i < 8; i ++)
	{
		 //SCLK上升沿，MUC写数据
		PORTE |= 1<<PORTE2;
		PORTE = (byOrder&0x80)?(PORTE|(1<<PORTE3)):(PORTE& ~(1<<PORTE3));
		byOrder <<= 1;
		//SCLK下降沿，Att7022读数据
		fnDelayUs();
		PORTE &= ~(1<<PORTE2);
		fnDelayUs();		
	}
	
	//补齐24个时钟
	PORTE &= ~(1<<PORTE3);//DIN=0;
	for (int i = 0; i < 24; i ++)
	{
		 //SCLK上升沿，MUC写数据
		PORTE |= 1<<PORTE2;
		//SCLK下降沿，Att7022读数据
		fnDelayUs();
		PORTE &= ~(1<<PORTE2);
		fnDelayUs();				
	}
	
	PORTE &= ~(1<<PORTE3); //DIN=0;
	PORTE &= ~(1<<PORTE2); //SCLK=0;
	fnDelayUs();
	PORTE |= 1<<PORTE1;  //CS=1
}


//读取Att7022电压有效值
int fnReadVoltageFromAtt7022(unsigned char byAdd)
{
	unsigned long int ulVoltage = fnReadAtt7022(byAdd);
	ulVoltage = ulVoltage * 1024 / 8388608;
	return ulVoltage;
}